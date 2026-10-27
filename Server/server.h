#ifndef SERVER_H
#define SERVER_H

#include <QWidget>

class Server : public QWidget
{
    Q_OBJECT

public:
    Server(QWidget *parent = nullptr);
    ~Server();
    QString m_strIp;
    quint16 m_usPort;
    void loadConfig();
};
#endif // SERVER_H
