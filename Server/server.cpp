#include "mytcpserver.h"
#include "server.h"

#include <qfile.h>
#include <QDebug>
Server::Server(QWidget *parent)
    : QWidget(parent)
{
    loadConfig();
    MyTcpServer::getInstance().listen(QHostAddress(m_strIp), m_usPort);
}

Server::~Server()
{
}

void Server::loadConfig()
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
