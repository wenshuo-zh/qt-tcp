#include "msghandler.h"
#include "mytcpserver.h"
#include "mytcpsocket.h"
#include "operatedb.h"
#include <QDebug>
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
    char caTarName[32] = {'\0'};
    memcpy(caTarName, pdu->caData+32, 32);
    pdu->uiType = ENUM_TYPE_CHAT_RESEND;
    MyTcpServer::getInstance().resendSocket(caTarName, pdu);
    return NULL;
}
