#include "mytcpsocket.h"
#include <QDebug>
#include "protocol.h"
#include "operatedb.h"
#include "mytcpserver.h"

MyTcpSocket::MyTcpSocket() : m_pmh(new MsgHandler)
{
    connect(this, &QTcpSocket::readyRead, this, &MyTcpSocket::recvMsg);
    //关联断开连接
    connect(this, &QTcpSocket::disconnected, this, &MyTcpSocket::clientOffline);
}

MyTcpSocket::~MyTcpSocket()
{
    delete m_pmh;
    m_pmh = NULL;
}



void MyTcpSocket::clientOffline()
{
    OperateDB::getInstance().handleOffline(m_strLoginName.toStdString().c_str());
    MyTcpServer::getInstance().removeSocket(this);
}

PDU *MyTcpSocket::readMsg()
{
    qDebug() << "recvMsg接收数组总长度" << this->bytesAvailable();
    uint uiPDULen = 0;
    this->read((char*)&uiPDULen, sizeof(uint));//读取总长度到uiPDULen
    uint uiMsgLen = uiPDULen - sizeof(PDU);
    PDU *pdu = mkPDU(uiMsgLen);
    this->read((char*)pdu+sizeof(uint), uiPDULen - sizeof(uint));
    return pdu;
}

PDU *MyTcpSocket::handleMsg(PDU *pdu)
{
    PDU* respdu = NULL;
    m_pmh->pdu = pdu;
    //case语句判断类型
    switch (pdu->uiType) {

    case ENUM_TYPE_REGIST_REQUEST:{
        respdu = m_pmh->regist();
        break;
    }
    case ENUM_TYPE_LOGIN_REQUEST:{
        respdu = m_pmh->login(m_strLoginName);
        break;
    }
    case ENUM_TYPE_FIND_USER_REQUEST:{
        respdu = m_pmh->findUser();
        break;
    }
    case ENUM_TYPE_ONLINE_USER_REQUEST:{
        respdu = m_pmh->onlineUser();
        break;
    }
    case ENUM_TYPE_ADD_FRIEND_REQUEST:{
        respdu = m_pmh->addFriend();
        break;
    }
    case ENUM_TYPE_ADD_FRIEND_AGREE_REQUEST:{
        respdu = m_pmh->addFriendAgree();
        break;
    }
    case ENUM_TYPE_ADD_FRIEND_REFUSE_REQUEST:{
        respdu = m_pmh->addFriendRefuse();
        break;
    }
    case ENUM_TYPE_FLUSH_FRIEND_REQUEST:{
        respdu = m_pmh->flushFriend();
        break;
    }
    case ENUM_TYPE_DELETE_FRIEND_REQUEST:{
        respdu = m_pmh->deleteFriend();
        break;
    }
    case ENUM_TYPE_CHAT_REQUEST:{
        respdu = m_pmh->chat();
        break;
    }
    case ENUM_TYPE_MKDIR_REQUEST:{
        respdu = m_pmh->mkdir();
        break;
    }
    case ENUM_TYPE_FLUSH_FILE_REQUEST:{
        respdu = m_pmh->flushFile();
        break;
    }
    case ENUM_TYPE_DELETE_FILE_REQUEST:{
        respdu = m_pmh->deleteFile();
        break;
    }
    case ENUM_TYPE_RENAME_FILE_REQUEST:{
        respdu = m_pmh->renameFile();
        break;
    }
    case ENUM_TYPE_MOVE_FILE_REQUEST:{
        respdu = m_pmh->moveFile();
        break;
    }
    case ENUM_TYPE_UPLOAD_FILE_INIT_REQUEST:{
        respdu = m_pmh->uploadFileInit();
        break;
    }
    case ENUM_TYPE_UPLOAD_FILE_DATA_REQUEST:{
        respdu = m_pmh->uploadFileData();
        break;
    }
    case ENUM_TYPE_DOWNLOAD_FILE_INIT_REQUEST:{
        respdu = m_pmh->downloadFileInit();
        break;
    }
    case ENUM_TYPE_DOWNLOAD_FILE_DATA_REQUEST:{
        respdu = m_pmh->downloadFileData();
        break;
    }
    default:
        break;
    }
    return respdu;
}

void MyTcpSocket::sendMsg(PDU* respdu)
{
    if(respdu == NULL)return ;
    this->write((char*)respdu, respdu->uiTotalLen);
    qDebug() << "send msg respdu->uiTotalLen" << respdu->uiTotalLen
             << "respdu->uiMsgLen" << respdu->uiMsgLen
             << "respdu->uiType" << respdu->uiType
             << "respdu->caData" << respdu->caData
             << "respdu->caData+32" << respdu->caData+32;
    free(respdu);
    respdu = NULL;
}

void MyTcpSocket::recvMsg()
{
    qDebug() << "recvMsg 接受消息长度" << this->bytesAvailable();
    QByteArray data = this->readAll();
    buffer.append(data);//追加缓存
    while(buffer.size() >= int(sizeof(PDU))){//判断是否至少收到一个PDU头部
        PDU* pdu = (PDU*)buffer.data();
        if(buffer.size() < int(pdu->uiTotalLen)){//判断PDU是否完整
            break;
        }
        PDU* respdu = handleMsg(pdu);
        sendMsg(respdu);
        buffer.remove(0, pdu->uiTotalLen);//删除处理完的一个PDU
    }
}
