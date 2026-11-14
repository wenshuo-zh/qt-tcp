#include "client.h"
#include "index.h"
#include "reshandler.h"
#include "string"
#include <QDebug>
#include <qmessagebox.h>
ResHandler::ResHandler()
{

}

void ResHandler::regist(PDU *pdu)
{
    bool ret;
    memcpy(&ret, pdu->caData, sizeof (bool));
    qDebug() << "regist ret" << ret;
    if(ret){
        QMessageBox::information(&Client::getInstance(), "提示", "注册成功");
    }
    else{
        QMessageBox::information(&Client::getInstance(), "提示", "注册失败");
    }
}

void ResHandler::login(PDU *pdu)
{
    bool ret;
    memcpy(&ret, pdu->caData, sizeof (bool));
    qDebug() << "login ret" << ret;
    if(ret){
        Index::getInstance().show();
        Client::getInstance().hide();
    }
    else{
        QMessageBox::information(&Client::getInstance(), "提示", "登录失败");
    }
}

void ResHandler::findUser(PDU *pdu)
{
    int ret;
    memcpy(&ret, pdu->caData, sizeof (int));
    qDebug() << "findUser ret" << ret;
    if(ret == 0){
        QMessageBox::information(&Index::getInstance(), "查找在线用户", "用户不在线");
    }else if(ret == 1){
        QMessageBox::information(&Index::getInstance(), "查找在线用户", "用户在线");
    }else if(ret == 2){
        QMessageBox::information(&Index::getInstance(), "查找在线用户", "用户不存在");
    }else if(ret == -1){
        QMessageBox::information(&Index::getInstance(), "查找在线用户", "查找失败");
    }
}

void ResHandler::onlineUser(PDU *pdu)
{
    //用户名数量
    uint uiSize = pdu->uiMsgLen / 32;
    char caTmp[32] = {'\0'};
    QStringList userList;
    for(uint i = 0; i < uiSize; i++){
        memcpy(caTmp, pdu->caMsg + i*32, 32);
        userList.append(caTmp);
    }
    Index::getInstance().getFriend()->m_pOnlineUser->updateLW(userList);
}

void ResHandler::addFriend(PDU* pdu)
{
    int ret;
    memcpy(&ret, pdu->caData, sizeof (int));
    qDebug() << "addFriend ret" << ret;
    if(ret == 0){
        QMessageBox::information(&Index::getInstance(), "添加好友", "用户不在线");
    }else if(ret == -2){
        QMessageBox::information(&Index::getInstance(), "添加好友", "该用户已经是好友");
    }else if(ret == -3){
        QMessageBox::warning(&Index::getInstance(), "添加好友", "不能添加自己为好友！");
    }else if(ret == -1){
        QMessageBox::information(&Index::getInstance(), "添加好友", "服务器错误：请联系开发人员");
    }
}

void ResHandler::addFriendResend(PDU *pdu)
{
    char caName[32] = {'\0'};
    memcpy(caName, pdu->caData, 32);
    int ret = QMessageBox::question(&Index::getInstance(), "添加好友", QString("是否同意'%1'添加好友的请求？").arg(caName));
    PDU* respdu = mkPDU();
    memcpy(respdu->caData, pdu->caData, 64);
    if(ret != QMessageBox::Yes){
        respdu->uiType = ENUM_TYPE_ADD_FRIEND_REFUSE_REQUEST;
    }else {
        respdu->uiType = ENUM_TYPE_ADD_FRIEND_AGREE_REQUEST;
    }
    Client::getInstance().sendMsg(respdu);
}



void ResHandler::addFriendAgree(PDU* pdu)
{
    bool ret;
    memcpy(&ret, pdu->caData, sizeof (bool));
    if(ret){
        QMessageBox::information(&Index::getInstance(), "添加好友", "添加好友成功");
        Index::getInstance().getFriend()->flushFriend();
    }else{
        QMessageBox::information(&Index::getInstance(), "添加好友", "添加好友失败");
    }
}

void ResHandler::addFriendRefuse()
{
    QMessageBox::information(&Index::getInstance(), "添加好友失败", "对方拒绝你的好友申请");
}

void ResHandler::flushFriend(PDU* pdu)
{
    QStringList friendList;
    uint iSize = pdu->uiMsgLen / 32;
    char caTmp[32] = {'\0'};
    for(uint i = 0; i < iSize; i++){
        memcpy(caTmp, pdu->caMsg + i*32, 32);
        friendList.append(caTmp);
    }
    Index::getInstance().getFriend()->update_LW(friendList);
}

void ResHandler::deleteFriend(PDU *pdu)
{
    bool ret;
    memcpy(&ret, pdu->caData, sizeof (bool));
    if(ret){
        QMessageBox::information(&Index::getInstance(), "删除好友", "删除好友成功");
        Index::getInstance().getFriend()->flushFriend();
    }else{
        QMessageBox::information(&Index::getInstance(), "删除好友", "删除好友失败");
    }
}

void ResHandler::chat(PDU *pdu)
{
    Chat* c = Index::getInstance().getFriend()->m_pChat;
    if(c->isHidden()){
        c->show();
    }
    char caChatName[32] = {'\0'};
    memcpy(caChatName, pdu->caData, 32);
    c->updateShow_TE(QString("%1 : %2").arg(caChatName).arg(pdu->caMsg));
    c->m_strChatName = caChatName;
}
