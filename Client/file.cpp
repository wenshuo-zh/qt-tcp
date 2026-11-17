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

void File::on_del_PB_clicked()
{
    QListWidgetItem* pItem = ui->listWidget->currentItem();
    if(!pItem){
        return;
    }
    int ret = QMessageBox::question(this, "删除文件", QString("是否确认删除文件 %1? ").arg(pItem->text()));
    if (ret != QMessageBox::Yes) {
        return;
    }
    QString strPath = QString("%1/%2").arg(m_strCurPath).arg(pItem->text());
    PDU* pdu = mkPDU(strPath.toStdString().size() + 1);
    pdu->uiType = ENUM_TYPE_DELETE_FILE_REQUEST;
    memcpy(pdu->caMsg, strPath.toStdString().c_str(),
    strPath.toStdString().size());
    Client::getInstance().sendMsg(pdu);
}

void File::on_rename_PB_clicked()
{
    QListWidgetItem* pItem = ui->listWidget->currentItem();
    if(!pItem){
        return;
    }
    bool ok = false;
    QString strNewName = QInputDialog::getText(this, "重命名文件", "文件名：",
                                               QLineEdit::Normal, QString(), &ok);
    QByteArray oldName = pItem->text().toUtf8();
    QByteArray newName = strNewName.toUtf8();
    if(!ok || newName.isEmpty() || oldName.size() > 32 || newName.size() > 32){
        QMessageBox::critical(this, "重命名失败", "长度非法");
        return;
    }
    PDU* pdu = mkPDU(m_strCurPath.toStdString().size()+1);
    pdu->uiType = ENUM_TYPE_RENAME_FILE_REQUEST;
    memcpy(pdu->caMsg, m_strCurPath.toStdString().c_str(), m_strCurPath.toStdString().size());
    memcpy(pdu->caData, oldName.constData(), oldName.size());
    memcpy(pdu->caData+32, newName.constData(), newName.size());
    Client::getInstance().sendMsg(pdu);
}

void File::on_move_PB_clicked()
{
    QListWidgetItem* pItem = ui->listWidget->currentItem();
    if(!pItem){
        return;
    }
    bool ok = false;
    QString strNewPath = QInputDialog::getText(this, "移动文件", "目标目录路径：",
                                               QLineEdit::Normal, QString(), &ok);
    if(!ok || strNewPath.trimmed().isEmpty()){
        return;
    }

    QString strOldDir = m_strCurPath;
    QString strFileName = pItem->text();
    if(strFileName.isEmpty() || strFileName.toStdString().size() > 32){
        QMessageBox::critical(this, "移动文件失败", "文件名长度非法");
        return;
    }
    uint uiOldPathLen = strOldDir.toStdString().size();
    uint uiNewPathLen = strNewPath.toStdString().size();
    PDU* pdu = mkPDU(uiOldPathLen + uiNewPathLen);
    pdu->uiType = ENUM_TYPE_MOVE_FILE_REQUEST;
    //caMsg:[原目录][新目录]
    memcpy(pdu->caMsg, strOldDir.toStdString().c_str(), uiOldPathLen);
    memcpy(pdu->caMsg+uiOldPathLen, strNewPath.toStdString().c_str(), uiNewPathLen);
    //caData:[原文件名][原目录长度][新目录长度]
    memcpy(pdu->caData, strFileName.toStdString().c_str(), strFileName.toStdString().size());
    memcpy(pdu->caData+32, &uiOldPathLen, sizeof(uint));
    memcpy(pdu->caData+32+sizeof(uint), &uiNewPathLen, sizeof(uint));
    Client::getInstance().sendMsg(pdu);
}

void File::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    foreach(FileInfo* pFileInfo, m_pFileInfoList){
        if(pFileInfo->caName == item->text() && pFileInfo->iFileType != 0){
            return;
        }
    }
    m_strCurPath = QString("%1/%2").arg(m_strCurPath).arg(item->text());
    flushFile();
}

void File::on_return_PB_clicked()
{
    if(m_strCurPath == m_strUserPath){
        return;
    }
    int index = m_strCurPath.lastIndexOf('/');
    m_strCurPath.remove(index, m_strCurPath.size()-index);
    flushFile();
}
