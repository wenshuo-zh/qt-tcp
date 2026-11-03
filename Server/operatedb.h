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
    ~OperateDB();
private:
    explicit OperateDB(QObject *parent = nullptr);
    OperateDB(const OperateDB& instance) = delete;
    OperateDB& operator=(const OperateDB&) = delete;
    QSqlDatabase m_db;//数据库连接对象

signals:

};

#endif // OPERATEDB_H
