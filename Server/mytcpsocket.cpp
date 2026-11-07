#include "mytcpsocket.h"
#include <QDebug>
#include "protocol.h"
#include "operatedb.h"

MyTcpSocket::MyTcpSocket()
{
    connect(this, &QTcpSocket::readyRead, this, &MyTcpSocket::recvMsg);
}

void MyTcpSocket::recvMsg()
{
    qDebug() << "recvMsg接收数组总长度" << this->bytesAvailable();
    uint uiPDULen = 0;
    this->read((char*)&uiPDULen, sizeof(uint));//读取总长度到uiPDULen
    uint uiMsgLen = uiPDULen - sizeof(PDU);
    PDU *pdu = mkPDU(uiMsgLen);
    this->read((char*)pdu+sizeof(uint), uiPDULen - sizeof(uint));
    qDebug() << "send msg pdu->uiTotalLen" << pdu->uiTotalLen
             << "pdu->uiMsgLen" << pdu->uiMsgLen
             << "pdu->uiType" << pdu->uiType
             << "pdu->caData" << pdu->caData
             << "pdu->caMsg" << pdu->caMsg;

    //case语句判断类型
    switch (pdu->uiType) {

    //服务器处理注册并将结果相应给服务端（后期封装成函数）
    case ENUM_TYPE_REGIST_REQUEST:{
        //从caData取出账号密码
        char caName[32] = {'\0'};
        memcpy(caName, pdu->caData, 32);
        char caPwd[32] = {'\0'};
        memcpy(caPwd, pdu->caData+32, 32);
        qDebug() << "regist caName" << caName << "regist caPwd" << caPwd;
        //数据库操作类处理注册函数
        bool res = OperateDB::getInstance().handleRegist(caName, caPwd);
        qDebug() << "regist res" << res;
        //构建respdu，处理结果放进caData，注册响应类型赋值给消息类型
        PDU* respdu = mkPDU(0);
        memcpy(respdu->caData, &res, sizeof (bool));
        respdu->uiType = ENUM_TYPE_REGIST_RESPOND;
        //调用父类socket发送respdu
        this->write((char*)respdu, respdu->uiTotalLen);
        qDebug() << "send msg respdu->uiTotalLen" << respdu->uiTotalLen
                 << "respdu->uiMsgLen" << respdu->uiMsgLen
                 << "respdu->uiType" << respdu->uiType
                 << "respdu->caData" << respdu->caData
                 << "respdu->caData+32" << respdu->caData+32
                 << "respdu->caMsg" << respdu->caMsg;
        free(respdu);
        respdu = NULL;
        break;
    }

    case ENUM_TYPE_LOGIN_REQUEST:{
        char caName[32] = {'\0'};
        memcpy(caName, pdu->caData, 32);
        char caPwd[32] = {'\0'};
        memcpy(caPwd, pdu->caData+32, 32);
        bool res = OperateDB::getInstance().handleLogin(caName, caPwd);
        qDebug() << "login res" << res;
        if(res){
            m_strLoginName = caName;
        }
        PDU* respdu = mkPDU(0);
        memcpy(respdu->caData, &res, sizeof (bool));
        respdu->uiType = ENUM_TYPE_LOGIN_RESPOND;
        this->write((char*)respdu, respdu->uiTotalLen);
        qDebug() << "send msg respdu->uiTotalLen" << respdu->uiTotalLen
                 << "respdu->uiMsgLen" << respdu->uiMsgLen
                 << "respdu->uiType" << respdu->uiType
                 << "respdu->caData" << respdu->caData
                 << "respdu->caData+32" << respdu->caData+32
                 << "respdu->caMsg" << respdu->caMsg;
        free(respdu);
        respdu = NULL;
        break;
    }
    default:
        break;
    }
    free(pdu);
    pdu = NULL;
}
