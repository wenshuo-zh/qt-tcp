#ifndef SERVER_H
#define SERVER_H

#include <QWidget>

class Server : public QWidget
{
    Q_OBJECT

public:
    static Server& getInstance();
    ~Server();
    QString m_strIp;
    quint16 m_usPort;
    QString m_strRootPath;
    void loadConfig();
private:
     Server(QWidget *parent = nullptr);
     Server(const Server& instance) = delete;
     Server& operator= (const Server&) = delete;
};
#endif // SERVER_H
