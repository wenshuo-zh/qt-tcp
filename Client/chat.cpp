#include "chat.h"
#include "client.h"
#include "index.h"
#include "protocol.h"
#include "ui_chat.h"

#include <QColor>
#include <QFont>
#include <QTextBlockFormat>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QTextDocument>

Chat::Chat(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Chat)
{
    ui->setupUi(this);
}

void Chat::updateShow_TE(QString strMsg)
{
    ui->show_TE->append(strMsg);
}

void Chat::addChatMessage(QString strChatName, ChatMessage msg)
{
    m_chatHistory[strChatName].append(msg);
}

void Chat::switchChat(QString strChatName)
{
    //更新当前聊天对象
    m_strChatName = strChatName;
    setWindowTitle(QStringLiteral("正在与 %1 聊天").arg(m_strChatName));
    ui->show_TE->clear();
    //获取该好友的聊天记录
    QList<ChatMessage> msgList = m_chatHistory[m_strChatName];
    //遍历聊天记录，逐条显示
    for(const ChatMessage& msg : msgList)
    {
        // 实现消息显示
        Chat::appendMessage(msg);
    }
}

void Chat::appendMessage(const ChatMessage &msg)
{
    QTextDocument* document = ui->show_TE->document();
    QTextCursor cursor(document);
    cursor.movePosition(QTextCursor::End);

    QTextBlockFormat blockFormat;
    blockFormat.setAlignment(Qt::AlignLeft);
    blockFormat.setTopMargin(document->isEmpty() ? 0 : 10);
    blockFormat.setBottomMargin(2);
    if(document->isEmpty()){
        cursor.setBlockFormat(blockFormat);
    }else{
        cursor.insertBlock(blockFormat);
    }

    QTextCharFormat messageFormat;
    messageFormat.setForeground(msg.isMine ? QColor("#86d99b") : QColor("#8bbcff"));

    QTextCharFormat senderFormat = messageFormat;
    senderFormat.setFontWeight(QFont::DemiBold);
    QString strSender = msg.isMine ? QStringLiteral("我") : msg.sender;
    cursor.insertText(QStringLiteral("%1：").arg(strSender), senderFormat);
    cursor.insertText(msg.content, messageFormat);

    ui->show_TE->setTextCursor(cursor);
    ui->show_TE->ensureCursorVisible();
}

Chat::~Chat()
{
    delete ui;
}

void Chat::on_send_PB_clicked()
{
    QString strMsg = ui->input_LE->text();
    if(strMsg.isEmpty()){
        return;
    }
    ui->input_LE->clear();
    PDU* pdu = mkPDU(strMsg.toStdString().size()+1);
    pdu->uiType = ENUM_TYPE_CHAT_REQUEST;
    memcpy(pdu->caData, Client::getInstance().m_strLoginName.toStdString().c_str(), 32);
    memcpy(pdu->caData+32, m_strChatName.toStdString().c_str(), 32);
    memcpy(pdu->caMsg, strMsg.toStdString().c_str(), strMsg.toStdString().size());
    Client::getInstance().sendMsg(pdu);
    //构造结构体
    ChatMessage msg;
    msg.sender = Client::getInstance().m_strLoginName;
    msg.content = strMsg;
    msg.isMine = true;
    addChatMessage(m_strChatName, msg);
    appendMessage(msg);
}
