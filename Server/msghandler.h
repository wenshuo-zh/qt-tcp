#ifndef MSGHANDLER_H
#define MSGHANDLER_H

#include "protocol.h"
#include <QString>



class MsgHandler
{
public:
    MsgHandler();
    PDU* pdu;
    PDU* regist();
    PDU* findUser();
    PDU* onlineUser();
    PDU* login(QString& strLoginName);
};

#endif // MSGHANDLER_H
