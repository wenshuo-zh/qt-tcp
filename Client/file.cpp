#include "client.h"
#include "file.h"
#include "ui_file.h"

#include <qinputdialog.h>
#include <qmessagebox.h>

File::File(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::File)
{
    ui->setupUi(this);
    m_strUserPath = QString("%1/%2").arg(Client::getInstance().m_strRootPath).arg(Client::getInstance().m_strLoginName);
    m_strCurPath = m_strUserPath;
}

File::~File()
{
    delete ui;
}

void File::flushFile()
{
    PDU* pdu = mkPDU(m_strCurPath.toStdString().size()+1);
    memcpy(pdu->caMsg, m_strCurPath.toStdString().c_str(), m_strCurPath.toStdString().size());
    pdu->uiType = ENUM_TYPE_FLUSH_FILE_REQUEST;
    Client::getInstance().sendMsg(pdu);
}

void File::updateFileList(QList<FileInfo *> pFileInfoList)
{
    foreach(FileInfo* pFileInfo, m_pFileInfoList){
        delete pFileInfo;
    }
    m_pFileInfoList.clear();
    m_pFileInfoList = pFileInfoList;
    ui->listWidget->clear();
    foreach(FileInfo* pFileInfo, pFileInfoList){
        QListWidgetItem* pItem = new QListWidgetItem;
        if(pFileInfo->iFileType == 0){
            pItem->setIcon(QIcon(QPixmap(":/dir.png")));
        }else{
             pItem->setIcon(QIcon(QPixmap(":/file.png")));
        }
        pItem->setText(pFileInfo->caName);
        ui->listWidget->addItem(pItem);
    }
}

void File::on_mkdir_PB_clicked()
{
    QString strDirname = QInputDialog::getText(this, "新建文件夹", "新建文件夹名: ");
    if(strDirname.isEmpty() || strDirname.toStdString().size() > 32){
        QMessageBox::information(this, "新建文件夹失败", "文件夹名长度非法");
        return;
    }
    PDU* pdu = mkPDU(m_strCurPath.toStdString().size()+1);
    memcpy(pdu->caMsg, m_strCurPath.toStdString().c_str(), m_strCurPath.toStdString().size());
    memcpy(pdu->caData, strDirname.toStdString().c_str(), 32);
    pdu->uiType = ENUM_TYPE_MKDIR_REQUEST;
    Client::getInstance().sendMsg(pdu);
}

void File::on_flush_PB_clicked()
{
    flushFile();
}
