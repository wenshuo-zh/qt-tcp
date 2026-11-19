#include "client.h"
#include "index.h"
#include "reshandler.h"
#include "string"
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <qfiledialog.h>
#include <qmessagebox.h>
ResHandler::ResHandler()
{

}

void ResHandler::regist(PDU *pdu)
{
    bool ret;
    memcpy(&ret, pdu->caData, sizeof (bool));
    qDebug() << "regist ret" << ret;
    if(ret){
        QMessageBox::information(&Client::getInstance(), "提示", "注册成功");
    }
    else{
        QMessageBox::information(&Client::getInstance(), "提示", "注册失败");
    }
}

void ResHandler::login(PDU *pdu)
{
    bool ret;
    memcpy(&ret, pdu->caData, sizeof (bool));
    qDebug() << "login ret" << ret;
    if(ret){
        Index::getInstance().show();
        Client::getInstance().hide();
    }
    else{
        QMessageBox::information(&Client::getInstance(), "提示", "登录失败");
    }
}

void ResHandler::findUser(PDU *pdu)
{
    int ret;
    memcpy(&ret, pdu->caData, sizeof (int));
    qDebug() << "findUser ret" << ret;
    if(ret == 0){
        QMessageBox::information(&Index::getInstance(), "查找在线用户", "用户不在线");
    }else if(ret == 1){
        QMessageBox::information(&Index::getInstance(), "查找在线用户", "用户在线");
    }else if(ret == 2){
        QMessageBox::information(&Index::getInstance(), "查找在线用户", "用户不存在");
    }else if(ret == -1){
        QMessageBox::information(&Index::getInstance(), "查找在线用户", "查找失败");
    }
}

void ResHandler::onlineUser(PDU *pdu)
{
    //用户名数量
    uint uiSize = pdu->uiMsgLen / 32;
    char caTmp[32] = {'\0'};
    QStringList userList;
    for(uint i = 0; i < uiSize; i++){
        memcpy(caTmp, pdu->caMsg + i*32, 32);
        userList.append(caTmp);
    }
    Index::getInstance().getFriend()->m_pOnlineUser->updateLW(userList);
}

void ResHandler::addFriend(PDU* pdu)
{
    int ret;
    memcpy(&ret, pdu->caData, sizeof (int));
    qDebug() << "addFriend ret" << ret;
    if(ret == 0){
        QMessageBox::information(&Index::getInstance(), "添加好友", "用户不在线");
    }else if(ret == -2){
        QMessageBox::information(&Index::getInstance(), "添加好友", "该用户已经是好友");
    }else if(ret == -3){
        QMessageBox::warning(&Index::getInstance(), "添加好友", "不能添加自己为好友！");
    }else if(ret == -1){
        QMessageBox::information(&Index::getInstance(), "添加好友", "服务器错误：请联系开发人员");
    }
}

void ResHandler::addFriendResend(PDU *pdu)
{
    char caName[32] = {'\0'};
    memcpy(caName, pdu->caData, 32);
    int ret = QMessageBox::question(&Index::getInstance(), "添加好友", QString("是否同意'%1'添加好友的请求？").arg(caName));
    PDU* respdu = mkPDU();
    memcpy(respdu->caData, pdu->caData, 64);
    if(ret != QMessageBox::Yes){
        respdu->uiType = ENUM_TYPE_ADD_FRIEND_REFUSE_REQUEST;
    }else {
        respdu->uiType = ENUM_TYPE_ADD_FRIEND_AGREE_REQUEST;
    }
    Client::getInstance().sendMsg(respdu);
}



void ResHandler::addFriendAgree(PDU* pdu)
{
    bool ret;
    memcpy(&ret, pdu->caData, sizeof (bool));
    if(ret){
        QMessageBox::information(&Index::getInstance(), "添加好友", "添加好友成功");
        Index::getInstance().getFriend()->flushFriend();
    }else{
        QMessageBox::information(&Index::getInstance(), "添加好友", "添加好友失败");
    }
}

void ResHandler::addFriendRefuse()
{
    QMessageBox::information(&Index::getInstance(), "添加好友失败", "对方拒绝你的好友申请");
}

void ResHandler::flushFriend(PDU* pdu)
{
    QStringList friendList;
    uint iSize = pdu->uiMsgLen / 32;
    char caTmp[32] = {'\0'};
    for(uint i = 0; i < iSize; i++){
        memcpy(caTmp, pdu->caMsg + i*32, 32);
        friendList.append(caTmp);
    }
    Index::getInstance().getFriend()->update_LW(friendList);
}

void ResHandler::deleteFriend(PDU *pdu)
{
    bool ret;
    memcpy(&ret, pdu->caData, sizeof (bool));
    if(ret){
        QMessageBox::information(&Index::getInstance(), "删除好友", "删除好友成功");
        Index::getInstance().getFriend()->flushFriend();
    }else{
        QMessageBox::information(&Index::getInstance(), "删除好友", "删除好友失败");
    }
}

void ResHandler::chat(PDU *pdu)
{
    Chat* c = Index::getInstance().getFriend()->m_pChat;
    char caCurName[33] = {'\0'};
    memcpy(caCurName, pdu->caData, 32);
    char caTarName[33] = {'\0'};
    memcpy(caTarName, pdu->caData+32, 32);
    //发送者用户名
    QString strFromName = QString::fromUtf8(caCurName);
    //接收者用户名
    QString strTarName = QString::fromUtf8(caTarName);
    //当前登录用户
    QString strLoginName = Client::getInstance().m_strLoginName;
    if(strTarName != strLoginName){
        return;
    }

    //构造msg结构体
    ChatMessage msg;
    msg.sender = strFromName;
    msg.content = QString::fromUtf8(pdu->caMsg);
    msg.isMine = false;
    c->addChatMessage(strFromName, msg);

    if(c->m_strChatName == strFromName && !c->isHidden()){
        c->appendMessage(msg);
    }else if(c->isHidden() || c->m_strChatName.isEmpty()){
        //窗口隐藏或尚无当前会话时，切换到发送者并显示历史消息
        c->switchChat(strFromName);
    }
    if(c->isHidden()){
        c->show();
    }
}

void ResHandler::mkdir(PDU *pdu)
{
    bool ret;
    memcpy(&ret, pdu->caData, sizeof(bool));
    if(ret){
        Index::getInstance().getFile()->flushFile();
    }else{
        QMessageBox::information(&Index::getInstance(), "新建文件夹", "新建文件夹失败");
    }
}

void ResHandler::flushFile(PDU *pdu)
{
    int iCount = pdu->uiMsgLen / sizeof (FileInfo);
    QList<FileInfo*> pFileInfoLsit;
    for(int i = 0; i < iCount; i++){
        FileInfo* pFileInfo = new FileInfo;
        memcpy(pFileInfo->caName, pdu->caMsg+i*sizeof(FileInfo), sizeof(FileInfo));
        pFileInfoLsit.append(pFileInfo);
    }
    Index::getInstance().getFile()->updateFileList(pFileInfoLsit);
}

void ResHandler::deleteFile(PDU *pdu)
{
    bool ret;
    memcpy(&ret, pdu->caData, sizeof(bool));
    if(ret){
        Index::getInstance().getFile()->flushFile();
    }else{
        QMessageBox::information(&Index::getInstance(), "删除文件夹", "删除文件夹失败");
    }
}

void ResHandler::renameFile(PDU *pdu)
{
    bool ret;
    memcpy(&ret, pdu->caData, sizeof(bool));
    if(ret){
        Index::getInstance().getFile()->flushFile();
    }else{
        QMessageBox::information(&Index::getInstance(), "重命名文件", "重命名文件失败");
    }
}

void ResHandler::moveFile(PDU *pdu)
{
    bool ret;
    memcpy(&ret, pdu->caData, sizeof(bool));
    if(ret){
        Index::getInstance().getFile()->flushFile();
    }else{
        QMessageBox::information(&Index::getInstance(), "移动文件", "移动文件失败");
    }
}

void ResHandler::uploadFileInit(PDU *pdu)
{
    bool ret;
    bool isFinished;
    memcpy(&ret, pdu->caData, sizeof(bool));
    memcpy(&isFinished, pdu->caData + sizeof(bool),sizeof(bool));
    if(!ret){
        QMessageBox::information(&Index::getInstance(), "上传文件", "上传文件失败");
    }else if(isFinished){
        QMessageBox::information(&Index::getInstance(), "上传文件", "上传文件完成");
        Index::getInstance().getFile()->flushFile();
    }else{
        Index::getInstance().getFile()->uploadFile();
    }
}

void ResHandler::uploadFileData(PDU *pdu)
{
    bool ret;
    memcpy(&ret, pdu->caData, sizeof(bool));
    if(ret){
        QMessageBox::information(&Index::getInstance(), "上传文件", "上传文件完成");
        Index::getInstance().getFile()->flushFile();
    }else{
        QMessageBox::information(&Index::getInstance(), "上传文件","上传文件失败");
    }
}

void ResHandler::cancelDownloadFile()
{
    PDU* pdu = mkPDU();
    pdu->uiType = ENUM_TYPE_DOWNLOAD_FILE_DATA_REQUEST;
    pdu->caData[0] = 1; // 0 请求下一块，1 取消下载
    Client::getInstance().sendMsg(pdu);
}

void ResHandler::downloadFileInit(PDU *pdu)
{
    int ret;
    bool isFinished;
    memcpy(&ret, pdu->caData, sizeof(int));
    memcpy(&isFinished, pdu->caData + sizeof(int), sizeof(bool));
    memcpy(&m_iDownloadFileSize, pdu->caData+sizeof (int)+sizeof (bool), sizeof (qint64));
    const uint uiFileNameOffset = sizeof(int) + sizeof(bool) + sizeof(qint64);
    QString strFileName = QString::fromUtf8(pdu->caData + uiFileNameOffset);
    if(ret == -1){
        QMessageBox::information(&Index::getInstance(), "下载文件失败", "初始化失败或文件不存在");
    }else if(ret == 0){
        QMessageBox::information(&Index::getInstance(), "下载文件失败", "打开文件失败");
    }else{
        QString strInitPath = QDir::home().filePath(strFileName);
        QString strSavePath = QFileDialog::getSaveFileName(
            &Index::getInstance(), "选择下载位置", strInitPath, "所有文件 (*)");
        if(strSavePath.isEmpty()){
            cancelDownloadFile();
            return;
        }

        QString strSourceSuffix = QFileInfo(strFileName).completeSuffix();
        QString strSaveSuffix = QFileInfo(strSavePath).completeSuffix();
        if(strSourceSuffix.compare(strSaveSuffix, Qt::CaseInsensitive) != 0){
            QMessageBox::information(&Index::getInstance(), "下载文件失败",
                                     "保存文件的扩展名需要与源文件一致");
            cancelDownloadFile();
            return;
        }

        m_fDownloadFile.setFileName(strSavePath);
        bool res = m_fDownloadFile.open(QIODevice::WriteOnly);
        if(!res){
            QMessageBox::information(&Index::getInstance(), "下载文件失败", "本地文件创建失败");
            cancelDownloadFile();
            return;
        }
        m_iDownloadFileReceivedSize = 0;
        if(isFinished){
            m_fDownloadFile.close();
            QMessageBox::information(&Index::getInstance(), "下载文件", "下载文件完成");
            return;
        }
        PDU* requestPdu = mkPDU();
        requestPdu->uiType = ENUM_TYPE_DOWNLOAD_FILE_DATA_REQUEST;
        Client::getInstance().sendMsg(requestPdu);
    }
}

void ResHandler::downloadFileData(PDU *pdu)
{
    if(m_iDownloadFileReceivedSize+pdu->uiMsgLen > m_iDownloadFileSize){//超量判断
        m_fDownloadFile.close();
        m_fDownloadFile.remove();
        QMessageBox::information(&Index::getInstance(), "下载文件失败", "文件读取错误");
        return;
    }
    qint64 ret = m_fDownloadFile.write(pdu->caMsg, pdu->uiMsgLen);//获取实际写入的字节数
    if(ret != pdu->uiMsgLen){
        m_fDownloadFile.close();
        m_fDownloadFile.remove();
        QMessageBox::information(&Index::getInstance(), "下载文件失败", "文件读取不完整");
        return;
    }

    m_iDownloadFileReceivedSize += ret;
    if(m_iDownloadFileReceivedSize < m_iDownloadFileSize){
        PDU* requestPdu = mkPDU();
        requestPdu->uiType = ENUM_TYPE_DOWNLOAD_FILE_DATA_REQUEST;
        Client::getInstance().sendMsg(requestPdu);
        return;
    }
    m_fDownloadFile.close();
    QMessageBox::information(&Index::getInstance(), "下载文件", "下载文件成功");
}
