#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H

#include "protocol.h"

#include <QObject>
#include <qtcpsocket.h>

class MyTcpSocket : public QTcpSocket
{
    Q_OBJECT
public:
    MyTcpSocket();
    QString m_strLoginName;
public slots:
    void recvMsg();
    void clientOffline();
    void sendMsg(PDU* pdu);
};

#endif // MYTCPSOCKET_H
