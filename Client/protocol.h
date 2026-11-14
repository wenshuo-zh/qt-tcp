#ifndef PROTOCOL_H
#define PROTOCOL_H

typedef unsigned int uint;

//协议类型的枚举值
enum ENUM_TYPE{
    ENUM_TYPE_MIN = 0,
    ENUM_TYPE_REGIST_REQUEST,   //注册请求
    ENUM_TYPE_REGIST_RESPOND,   //注册响应
    ENUM_TYPE_LOGIN_REQUEST,    //登录
    ENUM_TYPE_LOGIN_RESPOND,
    ENUM_TYPE_FIND_USER_REQUEST, //查询用户
    ENUM_TYPE_FIND_USER_RESPOND,
    ENUM_TYPE_ONLINE_USER_REQUEST, //在线用户
    ENUM_TYPE_ONLINE_USER_RESPOND,
    ENUM_TYPE_ADD_FRIEND_REQUEST,//添加好友
    ENUM_TYPE_ADD_FRIEND_RESPOND,
    ENUM_TYPE_ADD_FRIEND_RESEND,//添加好友转发
    ENUM_TYPE_ADD_FRIEND_AGREE_REQUEST,//同意添加好友
    ENUM_TYPE_ADD_FRIEND_AGREE_RESPOND,
    ENUM_TYPE_ADD_FRIEND_REFUSE_REQUEST,//拒绝添加好友
    ENUM_TYPE_ADD_FRIEND_REFUSE_RESPOND,
    ENUM_TYPE_FLUSH_FRIEND_REQUEST,//刷新好友
    ENUM_TYPE_FLUSH_FRIEND_RESPOND,
    ENUM_TYPE_DELETE_FRIEND_REQUEST,//删除好友
    ENUM_TYPE_DELETE_FRIEND_RESPOND,
    ENUM_TYPE_CHAT_REQUEST,//聊天
    ENUM_TYPE_CHAT_RESEND,
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
