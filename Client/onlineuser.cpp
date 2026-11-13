#include "client.h"
#include "onlineuser.h"
#include "ui_onlineuser.h"

#include <qmessagebox.h>

OnlineUser::OnlineUser(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OnlineUser)
{
    ui->setupUi(this);
}

OnlineUser::~OnlineUser()
{
    delete ui;
}

void OnlineUser::updateLW(QStringList userList)
{
    ui->listWidget->clear();
    ui->listWidget->addItems(userList);
}

void OnlineUser::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    QString strCurName = Client::getInstance().m_strLoginName;
    QString strTarName = item->text();
    if(strCurName == strTarName){
        QMessageBox::warning(this, "添加好友", "不能添加自己为好友");
        return;
    }
    PDU* pdu = mkPDU();
    memcpy(pdu->caData, strCurName.toStdString().c_str(), 32);
    memcpy(pdu->caData+32, strTarName.toStdString().c_str(), 32);
    pdu->uiType = ENUM_TYPE_ADD_FRIEND_REQUEST;
    Client::getInstance().sendMsg(pdu);
}
