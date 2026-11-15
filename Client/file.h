#ifndef FILE_H
#define FILE_H

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
    explicit File(QWidget *parent = nullptr);
    ~File();

private slots:
    void on_mkdir_PB_clicked();

private:
    Ui::File *ui;
};

#endif // FILE_H
