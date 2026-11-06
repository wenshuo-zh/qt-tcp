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
    //处理请求
    bool handleRegist(const char* caName, const char* caPwd);
    ~OperateDB();
private:
    explicit OperateDB(QObject *parent = nullptr);
    OperateDB(const OperateDB& instance) = delete;
    OperateDB& operator=(const OperateDB&) = delete;
    QSqlDatabase m_db;//数据库连接对象

signals:

};

#endif // OPERATEDB_H
