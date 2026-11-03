#include "operatedb.h"
#include <QSqlDatabase>
#include <QDebug>
#include <QSqlError>
#include <QCoreApplication>
#include <QFile>
#include <QSettings>

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

OperateDB::~OperateDB()
{
    m_db.close();
}

OperateDB::OperateDB(QObject *parent) : QObject(parent)
{
    m_db = QSqlDatabase::addDatabase("QMYSQL");
}
