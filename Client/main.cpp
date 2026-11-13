#include "client.h"
#include "index.h"

#include <QApplication>
#include <QFile>
#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QFile styleFile(":/client.qss");
    if(styleFile.open(QIODevice::ReadOnly)){
        a.setStyleSheet(QString::fromUtf8(styleFile.readAll()));
    }
    a.setWindowIcon(QIcon(":/Assets/chat-icon.svg"));
    Client::getInstance().show();
    return a.exec();
}
