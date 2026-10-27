#ifndef PROTOCOL_H
#define PROTOCOL_H

typedef unsigned int uint;

//协议类型的枚举值
enum ENUM_TYPE{
    ENUM_TYPE_MIN = 0,
    ENUM_TYPE_REGIST_REQUEST,   //注册请求
    ENUM_TYPE_REGIST_RESPOND,   //注册响应
    ENUM_TYPE_MAX = 9999,
};

//协议数据单元
struct PDU{
    uint uiTotalLen;    //总长度
    uint uiMsgLen;      //柔性数组长度
    uint uiType;        //消息类型
    char caData[64];    //参数
    char caMsg[];       //实际消息（柔性数组）
};

PDU *mkPDU(uint uiMsLen = 0);

#endif // PROTOCOL_H
