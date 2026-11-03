#include "operatedb.h"
#include <QSqlDatabase>
#include <QDebug>
#include<QSqlError>

OperateDB &OperateDB::getInstance()
{
    static OperateDB instance;
    return instance;
}

void OperateDB::connectSQL()
{
    m_db.setHostName("localhost");
    m_db.setPort(3306);
    m_db.setUserName("root");
    m_db.setPassword(qEnvironmentVariable("QT_DB_PASSWORD"));
    m_db.setDatabaseName("qt_project");
    if(m_db.open()){
        qDebug() << "数据库连接成功";
    }else{
        qDebug() << "数据库连接失败" << m_db.lastError().text();
    }
}

OperateDB::~OperateDB()
{
    m_db.close();
}

OperateDB::OperateDB(QObject *parent) : QObject(parent)
{
    m_db = QSqlDatabase::addDatabase("QMYSQL");
}
