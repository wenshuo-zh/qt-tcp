#ifndef FILE_H
#define FILE_H

#include "protocol.h"

#include <QWidget>

namespace Ui {
class File;
}

class File : public QWidget
{
    Q_OBJECT

public:
    QString m_strUserPath;
    QString m_strCurPath;
    QList<FileInfo*> m_pFileInfoList;
    explicit File(QWidget *parent = nullptr);
    ~File();
    void flushFile();
    void updateFileList(QList<FileInfo*> pFileInfoList);

private slots:
    void on_mkdir_PB_clicked();

    void on_flush_PB_clicked();

private:
    Ui::File *ui;
};

#endif // FILE_H
