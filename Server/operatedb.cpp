#include "operatedb.h"
#include <QSqlDatabase>
#include <QDebug>
#include <QSqlError>
#include <QCoreApplication>
#include <QFile>
#include <QSettings>
#include <QSqlQuery>

OperateDB &OperateDB::getInstance()
{
    static OperateDB instance;
    return instance;
}

void OperateDB::connectSQL()
{
    const QString configPath = QCoreApplication::applicationDirPath() + "/database.ini";
    if (!QFile::exists(configPath)) {
        qDebug() << "数据库配置文件不存在:" << configPath
                 << "请复制 database.ini.example 并填写本地配置";
        return;
    }

    QSettings settings(configPath, QSettings::IniFormat);
    m_db.setHostName(settings.value("database/host").toString());
    m_db.setPort(settings.value("database/port", 3306).toInt());
    m_db.setUserName(settings.value("database/username").toString());
    m_db.setPassword(settings.value("database/password").toString());
    m_db.setDatabaseName(settings.value("database/name").toString());
    if(m_db.open()){
        qDebug() << "数据库连接成功";
    }else{
        qDebug() << "数据库连接失败" << m_db.lastError().text();
    }
}

bool OperateDB::handleRegist(const char *caName, const char *caPwd)
{
    //用户名密码判空
    if(caName == NULL || caPwd == NULL){
        return false;
    }
    //sql语句，判断用户名是否已存在
    QString sql = QString("select * from user_info where name = '%1'").arg(caName);
    qDebug() << "查找用户名是否存在 sql" << sql;
    QSqlQuery q;
    if(!q.exec(sql) || q.next()){
        return false;
    }
    sql = QString("insert into user_info(name, pwd) values('%1', '%2')").arg(caName).arg(caPwd);
    qDebug() << "数据库插入姓名和密码 sql" << sql;
    return q.exec(sql);
}

bool OperateDB::handleLogin(const char *caName, const char *caPwd)
{
    if(caName == NULL || caPwd == NULL){
        return false;
    }
    QString sql = QString("select * from user_info where name = '%1' and pwd = '%2'").arg(caName).arg(caPwd);
    qDebug() << "查找用户名和密码是否存在 sql" << sql;
    QSqlQuery q;
    if(!q.exec(sql) || !q.next()){
        return false;
    }
    //更新在线状态
    sql = QString("update user_info set online=1 where name='%1'").arg(caName);
    qDebug() << "将用户的online字段置为1 sql" << sql;
    return q.exec(sql);
}

void OperateDB::handleOffline(const char *caName)
{
    if(caName == NULL){
        return;
    }
    QString sql = QString("update user_info set online=0 where name='%1'").arg(caName);
    qDebug() << "将用户的online字段值置为0 sql" << sql;
    QSqlQuery q;
    bool ret = q.exec(sql);
    qDebug() << "handleOffline ret" << ret;
}

int OperateDB::handleFindUser(const char *caName)
{
    if(caName == NULL){
        return -1;
    }
    QString sql = QString("select online from user_info where name='%1' ").arg(caName);
    qDebug() << "查找用户online字段 sql" << sql;
    QSqlQuery q;
    if(!q.exec(sql)){
        //执行失败，返回-1
        return -1;
    }
    if(q.next()){
        //查询到了，返回online字段
        return q.value(0).toInt();
    }
    //没查到用户，返回2
    return 2;
}

OperateDB::~OperateDB()
{
    m_db.close();
}

OperateDB::OperateDB(QObject *parent) : QObject(parent)
{
    m_db = QSqlDatabase::addDatabase("QMYSQL");
}
