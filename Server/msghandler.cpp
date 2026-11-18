#include "msghandler.h"
#include "mytcpserver.h"
#include "mytcpsocket.h"
#include "operatedb.h"
#include "server.h"
#include <QDebug>
#include <QDir>
#include<string>
MsgHandler::MsgHandler()
{

}

PDU *MsgHandler::regist()
{
    //从caData取出账号密码
    char caName[32] = {'\0'};
    memcpy(caName, pdu->caData, 32);
    char caPwd[32] = {'\0'};
    memcpy(caPwd, pdu->caData+32, 32);
    qDebug() << "regist caName" << caName << "regist caPwd" << caPwd;
    //数据库操作类处理注册函数
    bool ret = OperateDB::getInstance().handleRegist(caName, caPwd);
    qDebug() << "regist ret" << ret;
    //注册时创建用户文件夹
    if(ret){
        QDir dir;
        bool res = dir.mkdir(QString("%1/%2").arg(Server::getInstance().m_strRootPath).arg(caName));
        qDebug() << "创建用户文件夹 res" << res;
    }
    //构建respdu，处理结果放进caData，注册响应类型赋值给消息类型
    PDU* respdu = mkPDU(0);
    memcpy(respdu->caData, &ret, sizeof (bool));
    respdu->uiType = ENUM_TYPE_REGIST_RESPOND;
    return respdu;
}

PDU *MsgHandler::findUser()
{
    char caName[32] = {'\0'};
    memcpy(caName, pdu->caData, 32);
    int ret = OperateDB::getInstance().handleFindUser(caName);
    qDebug() << "findUser ret" << ret;
    PDU* respdu = mkPDU();
    memcpy(respdu->caData, &ret, sizeof (int));
    respdu->uiType = ENUM_TYPE_FIND_USER_RESPOND;
    return respdu;
}

PDU *MsgHandler::onlineUser()
{
    QStringList ret = OperateDB::getInstance().handleOnlineUser();
    //在线用户列表放在柔性数组里
    PDU* respdu = mkPDU(ret.size()*32);
    for(int i = 0; i < ret.size(); i++){
        memcpy(respdu->caMsg + i*32,ret[i].toStdString().c_str(), 32);
    }
    respdu->uiType = ENUM_TYPE_ONLINE_USER_RESPOND;
    return respdu;
}

PDU *MsgHandler::login(QString& m_strLoginName)
{
    char caName[32] = {'\0'};
    memcpy(caName, pdu->caData, 32);
    char caPwd[32] = {'\0'};
    memcpy(caPwd, pdu->caData+32, 32);
    bool ret = OperateDB::getInstance().handleLogin(caName, caPwd);
    qDebug() << "login ret" << ret;
    if(ret){
        m_strLoginName = caName;
    }
    PDU* respdu = mkPDU(0);
    memcpy(respdu->caData, &ret, sizeof (bool));
    respdu->uiType = ENUM_TYPE_LOGIN_RESPOND;
    return respdu;
}

PDU *MsgHandler::addFriend()
{
    char caCurName[32] = {'\0'};
    memcpy(caCurName, pdu->caData, 32);
    char caTarName[32] = {'\0'};
    memcpy(caTarName, pdu->caData+32, 32);
    int ret = OperateDB::getInstance().handleAddFriend(caCurName, caTarName);
    qDebug() << "addFriend ret " << ret;
    if(ret == 1){//转发
        pdu->uiType = ENUM_TYPE_ADD_FRIEND_RESEND;
        MyTcpServer::getInstance().resendSocket(caTarName, pdu);
        return NULL;
    }
    PDU* respdu = mkPDU();
    respdu->uiType = ENUM_TYPE_ADD_FRIEND_RESPOND;
    memcpy(respdu->caData, &ret, sizeof (int));
    return respdu;
}

PDU *MsgHandler::addFriendAgree()
{
    char caCurName[32] = {'\0'};
    memcpy(caCurName, pdu->caData, 32);
    char caTarName[32] = {'\0'};
    memcpy(caTarName, pdu->caData+32, 32);
    bool ret = OperateDB::getInstance().handleAddFriendAgree(caCurName, caTarName);
    qDebug() << "addFriendAgerr ret" << ret;
    PDU* respdu = mkPDU();
    respdu->uiType = ENUM_TYPE_ADD_FRIEND_AGREE_RESPOND;
    memcpy(respdu->caData, &ret, sizeof (bool));
    MyTcpServer::getInstance().resendSocket(caCurName, respdu);
    return respdu;
}

PDU *MsgHandler::addFriendRefuse()
{
    char caCurName[32] = {'\0'};
    memcpy(caCurName, pdu->caData, 32);
    PDU* respdu = mkPDU();
    respdu->uiType = ENUM_TYPE_ADD_FRIEND_REFUSE_RESPOND;
    memcpy(respdu->caData, caCurName, 32);
    MyTcpServer::getInstance().resendSocket(caCurName, respdu);
    return NULL;
}

PDU *MsgHandler::flushFriend()
{
    QStringList res = OperateDB::getInstance().handleFlushFriend(pdu->caData);
    PDU* respdu = mkPDU(res.size()*32);
    respdu->uiType = ENUM_TYPE_FLUSH_FRIEND_RESPOND;
    for(int i = 0; i < res.size(); i++){
        memcpy(respdu->caMsg + i*32, res[i].toStdString().c_str(), 32);
    }
    return respdu;
}

PDU *MsgHandler::deleteFriend()
{
    char caCurName[32] = {'\0'};
    memcpy(caCurName, pdu->caData, 32);
    char caTarName[32] = {'\0'};
    memcpy(caTarName, pdu->caData+32, 32);
    bool ret = OperateDB::getInstance().handleDeleteFriend(caCurName, caTarName);
    qDebug() << "deleteFriend ret" << ret;
    PDU* respdu = mkPDU();
    respdu->uiType = ENUM_TYPE_DELETE_FRIEND_RESPOND;
    memcpy(respdu->caData, &ret, sizeof (bool));
    return respdu;
}

PDU *MsgHandler::chat()
{
    char caCurName[33] = {'\0'};
    memcpy(caCurName, pdu->caData, 32);
    char caTarName[33] = {'\0'};
    memcpy(caTarName, pdu->caData+32, 32);
    pdu->uiType = ENUM_TYPE_CHAT_RESEND;
    //发送给目标方
    MyTcpServer::getInstance().resendSocket(caTarName, pdu);
    //回发给发送方
    MyTcpServer::getInstance().resendSocket(caCurName, pdu);
    return NULL;
}

PDU *MsgHandler::mkdir()
{
    QString strPath = QString("%1/%2").arg(pdu->caMsg).arg(pdu->caData);
    qDebug() << "mkdir strPath" << strPath;
    QDir dir;
    bool ret = dir.mkdir(strPath);
    qDebug() << "mkdir ret" << ret;
    PDU* respdu = mkPDU();
    memcpy(respdu->caData, &ret, sizeof(bool));
    respdu->uiType = ENUM_TYPE_MKDIR_RESPOND;
    return respdu;
}

PDU *MsgHandler::flushFile()
{
    QDir dir(pdu->caMsg);
    QFileInfoList fileInfoList = dir.entryInfoList();
    PDU* respdu = mkPDU((fileInfoList.size()-2) * sizeof(FileInfo));
    respdu->uiType = ENUM_TYPE_FLUSH_FILE_RESPOND;
    for(int i = 0, j = 0; i < fileInfoList.size(); i++){
        if(fileInfoList[i].fileName() == "." || fileInfoList[i].fileName() == ".."){
          continue;
        };
        FileInfo* pFileInfo = (FileInfo*)respdu->caMsg + j++;//指针加一直接加一个结构体大小的字节
        if(fileInfoList[i].isDir()){
            pFileInfo->iFileType = 0;
        }else{
            pFileInfo->iFileType = 1;
        }
        memcpy(pFileInfo->caName, fileInfoList[i].fileName().toStdString().c_str(), 32);
        qDebug() << "pFileInfo caName" << pFileInfo->caName;
    }
    return respdu;
}

PDU *MsgHandler::deleteFile()
{
    QFileInfo fileInfo(pdu->caMsg);
    bool ret = false;
    if(fileInfo.isDir()){
        QDir dir(pdu->caMsg);
        ret = dir.removeRecursively();
    }else{
        QFile file(pdu->caMsg);
        ret = file.remove();
    }
    PDU* respdu = mkPDU();
    memcpy(respdu->caData, &ret, sizeof(bool));
    respdu->uiType = ENUM_TYPE_DELETE_FILE_RESPOND;
    return respdu;
}

PDU *MsgHandler::renameFile()
{
    char caOldName[33] = {'\0'};
    char caNewName[33] = {'\0'};
    memcpy(&caOldName, pdu->caData, 32);
    memcpy(&caNewName, pdu->caData+32, 32);
    QString strOldPath = QString("%1/%2").arg(pdu->caMsg).arg(caOldName);
    QString strNewPath = QString("%1/%2").arg(pdu->caMsg).arg(caNewName);
    QDir dir;
    bool ret = dir.rename(strOldPath, strNewPath);
    PDU* respdu = mkPDU();
    memcpy(respdu->caData, &ret, sizeof(bool));
    respdu->uiType = ENUM_TYPE_RENAME_FILE_RESPOND;
    return respdu;
}

PDU *MsgHandler::moveFile()
{
    uint uiOldPathLen = 0;
    uint uiNewPathLen = 0;
    memcpy(&uiOldPathLen, pdu->caData+32, sizeof(uint));
    memcpy(&uiNewPathLen, pdu->caData+32+sizeof(uint), sizeof(uint));

    bool ret = false;
    if(uiOldPathLen > 0 && uiNewPathLen > 0 &&
       uiOldPathLen <= pdu->uiMsgLen &&
       uiNewPathLen == pdu->uiMsgLen - uiOldPathLen){
        char caName[33] = {'\0'};
        memcpy(caName, pdu->caData, 32);
        QString strOldDir = QString::fromStdString(
            std::string(pdu->caMsg, uiOldPathLen));
        QString strNewDir = QString::fromStdString(
            std::string(pdu->caMsg+uiOldPathLen, uiNewPathLen));
        QString strOldPath = QString("%1/%2").arg(strOldDir).arg(caName);
        QString strNewPath = QString("%1/%2").arg(strNewDir).arg(caName);
        QDir dir;
        ret = dir.rename(strOldPath, strNewPath);
    }
    PDU* respdu = mkPDU();
    memcpy(respdu->caData, &ret, sizeof(bool));
    respdu->uiType = ENUM_TYPE_MOVE_FILE_RESPOND;
    return respdu;
}

PDU *MsgHandler::uploadFileInit()
{
    char caFileName[32] = {'\0'};
    memcpy(&caFileName, pdu->caData, 32);
    m_iUploadFileReceivedSize = 0;
    memcpy(&m_iUploadFileSize, pdu->caData+32, sizeof (qint64));
    QString strPath = QString("%1/%2").arg(pdu->caMsg).arg(caFileName);
    m_fUploadFile.setFileName(strPath);
    bool ret = m_fUploadFile.open(QIODevice::WriteOnly);
    bool isFinished = false;
    if(ret && m_iUploadFileSize == 0){
        m_fUploadFile.close();
        isFinished = true;
    }
    PDU* respdu = mkPDU();
    memcpy(respdu->caData, &ret, sizeof(bool));
    memcpy(respdu->caData + sizeof(bool), &isFinished, sizeof(bool));
    respdu->uiType = ENUM_TYPE_UPLOAD_FILE_INIT_RESPOND;
    return respdu;
}

PDU *MsgHandler::uploadFileData()
{
    PDU* respdu = mkPDU();
    respdu->uiType = ENUM_TYPE_UPLOAD_FILE_DATA_RESPOND;
    bool success = false;
    if(m_iUploadFileReceivedSize+pdu->uiMsgLen > m_iUploadFileSize){//超量判断
        m_fUploadFile.close();
        m_fUploadFile.remove();
        memcpy(respdu->caData, &success, sizeof (bool));
        return respdu;
    }
    qint64 ret = m_fUploadFile.write(pdu->caMsg, pdu->uiMsgLen);//获取实际写入的字节数

    if(ret != pdu->uiMsgLen){//响应失败pdu
        m_fUploadFile.close();
        m_fUploadFile.remove();
        memcpy(respdu->caData, &success, sizeof (bool));
        return respdu;
    }

    m_iUploadFileReceivedSize += ret;
    if(m_iUploadFileReceivedSize < m_iUploadFileSize){
        free(respdu);
        return NULL;
    }
    m_fUploadFile.close();
    success = true;
    memcpy(respdu->caData, &success, sizeof (bool));
    return respdu;
}
