#ifndef CHAT_H
#define CHAT_H

#include <QMap>
#include <QWidget>

namespace Ui {
class Chat;
}

struct ChatMessage
{
    QString sender;//发送者
    QString content;//发送内容
    bool isMine;//是否自己发送
};

class Chat : public QWidget
{
    Q_OBJECT

public:
    QString m_strChatName;
    QMap<QString, QList<ChatMessage>> m_chatHistory;//key存好友名，val存聊天记录列表
    explicit Chat(QWidget *parent = nullptr);
    void updateShow_TE(QString strMsg);
    void addChatMessage(QString strChatName, ChatMessage msg);
    void switchChat(QString strChatName);
    void appendMessage(const ChatMessage& msg);
    ~Chat();

private slots:

    void on_send_PB_clicked();

private:
    Ui::Chat *ui;
};

#endif // CHAT_H
