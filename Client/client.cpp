#include "client.h"
#include "protocol.h"
#include "ui_client.h"

#include <QFile>
#include <QDebug>
#include <QHostAddress>
#include <QMessageBox>

Client::Client(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Client)
{
    ui->setupUi(this);
    loadConfig();
    connect(&m_socket, &QTcpSocket::connected, this, &Client::showConnect);
    m_socket.connectToHost(QHostAddress(m_strIp), m_usPort);
}

Client::~Client()
{
    delete ui;
}

void Client::loadConfig()
{
    QFile file(":/connect.config");
    if(!file.open(QIODevice::ReadOnly)){
        qDebug() << "打开文件失败";
        return;
    }
    QByteArray baData = file.readAll();
    QString strData = QString(baData);
    //qDebug() << "strData" << strData;
    QStringList strList = strData.split("\r\n");
    m_strIp = strList[0];
    m_usPort = strList[1].toUShort();
    qDebug() << "ip" << m_strIp << "port" << m_usPort;
    file.close();
}

Client &Client::getInstance()
{
    static Client instance;
    return instance;
}

void Client::showConnect()
{
    qDebug() << "连接成功";
}



//void Client::on_send_PB_clicked()
//{
//    QString strMsg = ui->input_LE->text();
//    PDU* pdu = mkPDU(strMsg.toStdString().size()+1);
//    pdu->uiType = ENUM_TYPE_REGIST_REQUEST;
//    //目标地址，源地址，字节数
//    memcpy(pdu->caMsg, strMsg.toStdString().c_str(), strMsg.toStdString().size());
//    m_socket.write((char*)pdu, pdu->uiTotalLen);
//    qDebug() << "send msg pdu->uiTotalLen" << pdu->uiTotalLen
//             << "pdu->uiMsgLen" << pdu->uiMsgLen
//             << "pdu->uiType" << pdu->uiType
//             << "pdu->caData" << pdu->caData
//             << "pdu->caMsg" << pdu->caMsg;
//    free(pdu);
//    pdu = NULL;
//}

void Client::on_regist_PB_clicked()
{
//客户端给服务器发送请求
    //从ui的输入框控件取出用户名和密码
    QString strName = ui->name_LE->text();
    QString strPwd = ui->pwd_LE->text();
    if(strName.isEmpty() || strPwd.isEmpty() || strName.toStdString().size() > 32 || strPwd.toStdString().size() > 32){
        //参数：父窗口, 标题, 内容
        QMessageBox::critical(this, "注册失败", "用户名或密码长度非法");
        return;
    }
    //构建pdu，参数为0，用户名密码放到caData
    PDU* pdu = mkPDU(0);
    //memcpy的参数(目标首地址, 原首地址, 长度)
    memcpy(pdu->caData, strName.toStdString().c_str(), 32);
    memcpy(pdu->caData+32, strPwd.toStdString().c_str(), 32);
    pdu->uiType = ENUM_TYPE_REGIST_REQUEST;
    //通过socket发送pdu
    m_socket.write((char*)pdu, pdu->uiTotalLen);
    qDebug() << "send msg pdu->uiTotalLen" << pdu->uiTotalLen
             << "pdu->uiMsgLen" << pdu->uiMsgLen
             << "pdu->uiType" << pdu->uiType
             << "pdu->caData" << pdu->caData
             << "pdu->caData" << pdu->caData+32
             << "pdu->caMsg" << pdu->caMsg;

    free(pdu);
    pdu = NULL;
}
