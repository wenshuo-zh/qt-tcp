#include "client.h"
#include "onlineuser.h"
#include "ui_onlineuser.h"

OnlineUser::OnlineUser(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OnlineUser)
{
    ui->setupUi(this);
}

void OnlineUser::updateLW(QStringList userList)
{
    ui->listWidget->clear();
    ui->listWidget->addItems(userList);
}

OnlineUser::~OnlineUser()
{
    delete ui;
}

void OnlineUser::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    QString strCurName = Client::getInstance().m_strLoginName;
    QString strTarName = item->text();
    PDU* pdu = mkPDU();
    memcpy(pdu->caData, strCurName.toStdString().c_str(), 32);
    memcpy(pdu->caData+32, strTarName.toStdString().c_str(), 32);
    pdu->uiType = ENUM_TYPE_ADD_FRIEND_REQUEST;
    Client::getInstance().sendMsg(pdu);
}
