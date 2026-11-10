#include "msghandler.h"
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
    bool res = OperateDB::getInstance().handleRegist(caName, caPwd);
    qDebug() << "regist res" << res;
    //构建respdu，处理结果放进caData，注册响应类型赋值给消息类型
    PDU* respdu = mkPDU(0);
    memcpy(respdu->caData, &res, sizeof (bool));
    respdu->uiType = ENUM_TYPE_REGIST_RESPOND;
    return respdu;
}

PDU *MsgHandler::findUser()
{
    char caName[32] = {'\0'};
    memcpy(caName, pdu->caData, 32);
    int res = OperateDB::getInstance().handleFindUser(caName);
    qDebug() << "findUser res" << res;
    PDU* respdu = mkPDU();
    memcpy(respdu->caData, &res, sizeof (int));
    respdu->uiType = ENUM_TYPE_FIND_USER_RESPOND;
    return respdu;
}

PDU *MsgHandler::onlineUser()
{
    QStringList res = OperateDB::getInstance().handleOnlineUser();
    //在线用户列表放在柔性数组里
    PDU* respdu = mkPDU(res.size()*32);
    for(int i = 0; i < res.size(); i++){
        memcpy(respdu->caMsg + i*32,res[i].toStdString().c_str(), 32);
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
    bool res = OperateDB::getInstance().handleLogin(caName, caPwd);
    qDebug() << "login res" << res;
    if(res){
        m_strLoginName = caName;
    }
    PDU* respdu = mkPDU(0);
    memcpy(respdu->caData, &res, sizeof (bool));
    respdu->uiType = ENUM_TYPE_LOGIN_RESPOND;
    return respdu;
}
