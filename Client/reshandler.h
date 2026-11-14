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

    //cur客户端的响应
    void addFriend(PDU* pdu);
    //tar客户端的响应
    void addFriendResend(PDU* pdu);
    //返回给cur同意/拒绝添加好友的响应
    void addFriendAgree(PDU* pdu);
    void addFriendRefuse();

    void flushFriend(PDU* pdu);
    void deleteFriend(PDU* pdu);
    void chat(PDU* pdu);
};

#endif // RESHANDLER_H
