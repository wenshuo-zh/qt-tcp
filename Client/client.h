#ifndef CLIENT_H
#define CLIENT_H

#include "protocol.h"
#include "reshandler.h"

#include <QTcpSocket>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class Client; }
QT_END_NAMESPACE

class Client : public QWidget
{
    Q_OBJECT

public:
    QString m_strIp;
    quint16 m_usPort;
    QTcpSocket m_socket;
    QString m_strLoginName;
    ResHandler* m_prh;
    static Client& getInstance();
    void loadConfig();
    PDU* readMsg();
    void handleMsg(PDU* pdu);
    void sendMsg(PDU* pdu);

    ~Client();

public slots:
    void showConnect();


private slots:
    //void on_send_PB_clicked();

    void on_regist_PB_clicked();
    void recvMsg();
    void on_login_PB_clicked();

private:
    Ui::Client *ui;
    Client(QWidget *parent = nullptr);
    Client(const Client& instance) = delete;
    Client& operator=(const Client&) = delete;
};
#endif // CLIENT_H
