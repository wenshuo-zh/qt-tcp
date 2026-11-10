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
        QMessageBox::information(&Index::getInstance(), "提示", "用户不在线");
    }else if(ret == 1){
        QMessageBox::information(&Index::getInstance(), "提示", "用户在线");
    }else if(ret == 2){
        QMessageBox::information(&Index::getInstance(), "提示", "用户不存在");
    }else if(ret == -1){
        QMessageBox::information(&Index::getInstance(), "提示", "查找失败");
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
