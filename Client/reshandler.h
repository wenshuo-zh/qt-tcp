#ifndef RESHANDLER_H
#define RESHANDLER_H

#include "protocol.h"



class ResHandler
{
public:
    ResHandler();
    PDU* pdu;
    void regist(PDU* pdu);
    void login(PDU* pdu);
    void findUser(PDU* pdu);
    void onlineUser(PDU* pdu);
};

#endif // RESHANDLER_H
