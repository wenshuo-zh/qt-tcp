#ifndef OPERATEDB_H
#define OPERATEDB_H

#include <QObject>
#include <QSqlDatabase>

class OperateDB : public QObject
{
    Q_OBJECT
public:
    static OperateDB&getInstance();
    void connectSQL();
    //处理注册请求
    bool handleRegist(const char* caName, const char* caPwd);
    //处理登录请求
    bool handleLogin(const char* caName, const char* caPwd);
    //处理离线
    void handleOffline(const char* caName);
    //处理查询用户
    int handleFindUser(const char* caName);
    ~OperateDB();
private:
    explicit OperateDB(QObject *parent = nullptr);
    OperateDB(const OperateDB& instance) = delete;
    OperateDB& operator=(const OperateDB&) = delete;
    QSqlDatabase m_db;//数据库连接对象

signals:

};

#endif // OPERATEDB_H
