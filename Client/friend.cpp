#include "client.h"
#include "friend.h"
#include "protocol.h"
#include "ui_friend.h"

#include <QInputDialog>
#include <QMessageBox>

Friend::Friend(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Friend)
{
    ui->setupUi(this);
}

Friend::~Friend()
{
    delete ui;
}

void Friend::on_findUser_PB_clicked()
{
    QString strName = QInputDialog::getText(this, "查询用户", "用户名：");
    if(strName.isEmpty() || strName.toStdString().size() > 32){
        QMessageBox::critical(this, "查询失败", "长度非法");
        return;
    }
    PDU* pdu = mkPDU();
    memcpy(pdu->caData, strName.toStdString().c_str(), 32);
    pdu->uiType = ENUM_TYPE_FIND_USER_REQUEST;
    Client::getInstance().sendMsg(pdu);
}
