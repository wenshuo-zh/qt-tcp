#include "mytcpserver.h"
#include "mytcpsocket.h"

MyTcpServer &MyTcpServer::getInstance()
{
    static MyTcpServer instance;
    return instance;
}

void MyTcpServer::incomingConnection(qintptr handle)
{
    qDebug() << "新客户端连接";
    MyTcpSocket* pTcpSocket = new MyTcpSocket;
    pTcpSocket->setSocketDescriptor(handle);
    m_tcpSocketList.append(pTcpSocket);

    for(int i = 0; i < m_tcpSocketList.size(); i++)qDebug() << m_tcpSocketList[i];
}

MyTcpServer::MyTcpServer()
{

}
