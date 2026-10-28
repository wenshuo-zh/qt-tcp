#include "mytcpsocket.h"
#include <QDebug>
#include "protocol.h"

MyTcpSocket::MyTcpSocket()
{
    connect(this, &QTcpSocket::readyRead, this, &MyTcpSocket::recvMsg);
}

void MyTcpSocket::recvMsg()
{
    qDebug() << "recvMsg接收数组总长度" << this->bytesAvailable();
    uint uiPDULen = 0;
    this->read((char*)&uiPDULen, sizeof (uint));
    uint uiMsgLen = uiPDULen - sizeof (PDU);
    PDU *pdu = mkPDU(uiMsgLen);
    this->read((char*)pdu+sizeof(uint), uiPDULen - sizeof(uint));
    qDebug() << "send msg pdu->uiTotalLen" << pdu->uiTotalLen
             << "pdu->uiMsgLen" << pdu->uiMsgLen
             << "pdu->uiType" << pdu->uiType
             << "pdu->caData" << pdu->caData
             << "pdu->caMsg" << pdu->caMsg;
    free(pdu);
    pdu = NULL;
}
