#ifndef CLIENTTASK_H
#define CLIENTTASK_H

#include <QObject>

class ClientTask : public QObject
{
    Q_OBJECT
public:
    explicit ClientTask(QObject *parent = nullptr);

signals:

};

#endif // CLIENTTASK_H
