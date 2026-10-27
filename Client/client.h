#ifndef CLIENT_H
#define CLIENT_H

#include <QTcpSocket>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class Client; }
QT_END_NAMESPACE

class Client : public QWidget
{
    Q_OBJECT

public:
    ~Client();
    void loadConfig();
    QString m_strIp;
    quint16 m_usPort;
    QTcpSocket m_socket;
    static Client& getInstance();
public slots:
    void showConnect();
private:
    Ui::Client *ui;
    Client(QWidget *parent = nullptr);
    Client(const Client& instance) = delete;
    Client& operator=(const Client&) = delete;
};
#endif // CLIENT_H
