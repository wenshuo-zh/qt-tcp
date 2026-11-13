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
    QSqlQuery q;
    q.prepare("update user_info set online = 1 "
              "where name = :name and pwd = :pwd and online = 0");
    q.bindValue(":name", caName);
    q.bindValue(":pwd", caPwd);
    if(!q.exec()){
        qDebug() << "登录更新失败" << q.lastError().text();
        return false;
    }
    bool ret = q.numRowsAffected() == 1;
    qDebug() << "handleLogin ret" << ret << "affected rows" << q.numRowsAffected();
    return ret;
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

QStringList OperateDB::handleOnlineUser()
{
    QString sql = QString("select name from user_info where online = 1");
    QSqlQuery q;
    q.exec(sql);
    QStringList res;
    while(q.next()){
        res.append(q.value(0).toString());
    }
    return res;
}

int OperateDB::handleAddFriend(const char *caCurName, const char *caTarName)
{
    //判空返回-1
    if(caCurName == NULL || caTarName == NULL){
        return -1;
    }
    if(QString(caCurName) == QString(caTarName)){
        return -3;
    }
    QString sql = QString(R"(
                          select * from friend
                          where (friend_id = (select id from user_info where name = '%1')
                          and user_id = (select id from user_info where name = '%2'))
                          or (friend_id = (select id from user_info where name = '%2')
                          and user_id = (select id from user_info where name = '%1'));
                          )").arg(caCurName).arg(caTarName);
    qDebug() << "添加好友前判断是否为好友 sql" << sql;
    QSqlQuery q;
    if(!q.exec(sql)){//sql执行失败返回-1
        qDebug() << q.lastError();
        return -1;
    }
    if(q.next()){//已是好友，返回-2
        return -2;
    }
    sql = QString("select online from user_info where name = '%1'").arg(caTarName);
    qDebug() << "添加好友前判断好友在线 sql" << sql;
    if(!q.exec(sql)){//sql执行失败返回-1
        qDebug() << q.lastError();
        return -1;
    }
    if(q.next()){//返回在线情况，0/1
        return q.value(0).toInt();
    }
    return -1;
}

bool OperateDB::handleAddFriendAgree(const char *caCurName, const char *caTarName)
{
    if(caCurName == NULL || caTarName == nullptr){
        return false;
    }
    QString sql = QString(R"(
                          insert into friend(friend_id, user_id)
                          select u1.id, u2.id
                          from user_info u1, user_info u2
                          where u1.name = '%1' and u2.name = '%2'
                          )").arg(caCurName).arg(caTarName);
    qDebug() << "添加好友 sql" << sql;
    QSqlQuery q;
    return q.exec(sql);
}

QStringList OperateDB::handleFlushFriend(const char *caCurName)
{
    QStringList res;
    if(caCurName == NULL){
        return res;
    }
    QString sql = QString(R"(
                          select name from user_info
                          where id in (
                            select friend_id from friend where user_id = (select id from user_info where name = '%1')
                            union
                            select user_id from friend where friend_id = (select id from user_info where name = '%1')
                          ) and online = 1
                          )").arg(caCurName);
    qDebug() << "刷新在线好友 sql" << sql;
    QSqlQuery q;
    q.exec(sql);
    while(q.next()){
        res.append(q.value(0).toString());
    }
    return res;
}

bool OperateDB::handleDeleteFriend(const char *caCurName, const char *caTarName)
{
    if(caCurName == NULL || caTarName == nullptr){
        return false;
    }
    QString sql = QString(R"(
        delete from friend where
        (
            user_id=(select id from user_info where name='%1')
            and
            friend_id=(select id from user_info where name='%2')
        )
        or
        (
            friend_id=(select id from user_info where name='%1')
            and
            user_id=(select id from user_info where name='%2')
        );
    )").arg(caCurName).arg(caTarName);
    qDebug() << "删除好友 sql" << sql;
    QSqlQuery q;
    return q.exec(sql);
}



OperateDB::~OperateDB()
{
    m_db.close();
}

OperateDB::OperateDB(QObject *parent) : QObject(parent)
{
    m_db = QSqlDatabase::addDatabase("QMYSQL");
}
