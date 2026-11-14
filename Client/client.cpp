#include "client.h"
#include "index.h"
#include "protocol.h"
#include "ui_client.h"

#include <QFile>
#include <QDebug>
#include <QHostAddress>
#include <QMessageBox>

Client::Client(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Client)
    , m_prh(new ResHandler)
{
    ui->setupUi(this);
    loadConfig();
    connect(&m_socket, &QTcpSocket::connected, this, &Client::showConnect);
    //connect参数(信号发送者, 信号, 接收对象, 要执行的槽函数) 都是指针
    connect(&m_socket, &QTcpSocket::readyRead, this, &Client::recvMsg);
    m_socket.connectToHost(QHostAddress(m_strIp), m_usPort);
}

Client::~Client()
{
    delete m_prh;
    m_prh = NULL;
    delete ui;
}

void Client::loadConfig()
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

Client &Client::getInstance()
{
    static Client instance;
    return instance;
}


void Client::handleMsg(PDU *pdu)
{
    switch(pdu->uiType){
    case ENUM_TYPE_REGIST_RESPOND:{
        m_prh->regist(pdu);
        break;
    }
    case ENUM_TYPE_LOGIN_RESPOND:{
        m_prh->login(pdu);
        break;
    }
    case ENUM_TYPE_FIND_USER_RESPOND:{
        m_prh->findUser(pdu);
        break;
    }
    case ENUM_TYPE_ONLINE_USER_RESPOND:{
        m_prh->onlineUser(pdu);
        break;
    }
    case ENUM_TYPE_ADD_FRIEND_RESPOND:{
        m_prh->addFriend(pdu);
        break;
    }
    case ENUM_TYPE_ADD_FRIEND_RESEND:{
        m_prh->addFriendResend(pdu);
        break;
    }
    case ENUM_TYPE_ADD_FRIEND_AGREE_RESPOND:{
        m_prh->addFriendAgree(pdu);
        break;
    }
    case ENUM_TYPE_ADD_FRIEND_REFUSE_RESPOND:{
        m_prh->addFriendRefuse();
        break;
    }
    case ENUM_TYPE_FLUSH_FRIEND_RESPOND:{
        m_prh->flushFriend(pdu);
        break;
    }
    case ENUM_TYPE_DELETE_FRIEND_RESPOND:{
        m_prh->deleteFriend(pdu);
        break;
    }
    case ENUM_TYPE_CHAT_RESEND:{
        m_prh->chat(pdu);
        break;
    }
    default:{
        break;
    }
    }
    free(pdu);
    pdu = NULL;
}

void Client::sendMsg(PDU *pdu)
{
    m_socket.write((char*)pdu, pdu->uiTotalLen);
    qDebug() << "send msg pdu->uiTotalLen" << pdu->uiTotalLen
             << "pdu->uiMsgLen" << pdu->uiMsgLen
             << "pdu->uiType" << pdu->uiType
             << "pdu->caData" << pdu->caData
             << "pdu->caData" << pdu->caData+32;
    free(pdu);
    pdu = NULL;
}

PDU *Client::readMsg()
{
    qDebug() << "recvMsg 接收消息长度" << m_socket.bytesAvailable();
    uint uiPDULen = 0;
    //read参数(读取的数据存到内存的地址, 读取的字节数);
    //第一次读为了知道长度
    m_socket.read((char*)&uiPDULen, sizeof (uint));
    uint uiMsgLen = uiPDULen - sizeof (PDU);
    PDU* pdu = mkPDU(uiMsgLen);
    //第二次读为了接收剩余内容
    m_socket.read((char*)pdu + sizeof(uint), uiPDULen - sizeof(uint));
    return pdu;
}

void Client::showConnect()
{
    qDebug() << "连接成功";
}



//void Client::on_send_PB_clicked()
//{
//    QString strMsg = ui->input_LE->text();
//    PDU* pdu = mkPDU(strMsg.toStdString().size()+1);
//    pdu->uiType = ENUM_TYPE_REGIST_REQUEST;
//    //目标地址，源地址，字节数
//    memcpy(pdu->caMsg, strMsg.toStdString().c_str(), strMsg.toStdString().size());
//    m_socket.write((char*)pdu, pdu->uiTotalLen);
//    qDebug() << "send msg pdu->uiTotalLen" << pdu->uiTotalLen
//             << "pdu->uiMsgLen" << pdu->uiMsgLen
//             << "pdu->uiType" << pdu->uiType
//             << "pdu->caData" << pdu->caData
//             << "pdu->caMsg" << pdu->caMsg;
//    free(pdu);
//    pdu = NULL;
//}

void Client::on_regist_PB_clicked()
{
//客户端给服务器发送请求
    //从ui的输入框控件取出用户名和密码
    QString strName = ui->name_LE->text();
    QString strPwd = ui->pwd_LE->text();
    if(strName.isEmpty() || strPwd.isEmpty() || strName.toStdString().size() > 32 || strPwd.toStdString().size() > 32){
        //参数：父窗口, 标题, 内容
        QMessageBox::critical(this, "注册失败", "用户名或密码长度非法");
        return;
    }
    //构建pdu，参数为0，用户名密码放到caData
    PDU* pdu = mkPDU(0);
    //memcpy的参数(目标首地址, 原首地址, 长度)
    memcpy(pdu->caData, strName.toStdString().c_str(), 32);
    memcpy(pdu->caData+32, strPwd.toStdString().c_str(), 32);
    pdu->uiType = ENUM_TYPE_REGIST_REQUEST;
    //通过socket发送pdu
    Client::getInstance().sendMsg(pdu);
}

void Client::recvMsg()
{
    PDU* pdu = readMsg();
    handleMsg(pdu);
}

void Client::on_login_PB_clicked()
{
    QString strName = ui->name_LE->text();
    QString strPwd = ui->pwd_LE->text();
    if(strName.isEmpty() || strPwd.isEmpty() || strName.toStdString().size() > 32 || strPwd.toStdString().size() > 32){
        QMessageBox::critical(this, "登录失败", "用户名或密码长度非法");
        return;
    }
    m_strLoginName = strName;
    PDU* pdu = mkPDU(0);
    memcpy(pdu->caData, strName.toStdString().c_str(), 32);
    memcpy(pdu->caData+32, strPwd.toStdString().c_str(), 32);
    pdu->uiType = ENUM_TYPE_LOGIN_REQUEST;
    Client::getInstance().sendMsg(pdu);
}
