#ifndef MSGHANDLER_H
#define MSGHANDLER_H

#include "protocol.h"
#include <QString>
#include <qfile.h>



class MsgHandler
{
public:
    MsgHandler();
    PDU* pdu;
    qint64 m_iUploadFileSize;           //总大小
    qint64 m_iUploadFileReceivedSize;   //已接收大小
    QFile m_fUploadFile;
    PDU* regist();
    PDU* findUser();
    PDU* onlineUser();
    PDU* login(QString& strLoginName);
    PDU* addFriend();
    PDU* addFriendAgree();
    PDU* addFriendRefuse();
    PDU* flushFriend();
    PDU* deleteFriend();
    PDU* chat();
    PDU* mkdir();
    PDU* flushFile();
    PDU* deleteFile();
    PDU* renameFile();
    PDU* moveFile();
    PDU* uploadFileInit();
    PDU* uploadFileData();
};

#endif // MSGHANDLER_H
