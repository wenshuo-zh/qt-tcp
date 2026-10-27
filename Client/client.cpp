#include "client.h"
#include "ui_client.h"

#include <QFile>
#include <QDebug>
#include <QHostAddress>

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
