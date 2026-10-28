#include "protocol.h"
#include "stdlib.h"
#include "string.h"

PDU *mkPDU(uint uiMsLen)
{
    uint uiTotalLen = uiMsLen + sizeof(PDU);
    PDU* pdu = (PDU*)malloc(uiTotalLen);
    if(pdu == NULL){
        exit(1);
    }
    //起始地址, 要填入的字节值, 填充的字节数
    memset(pdu, 0, uiTotalLen); //全部填入0
    pdu->uiTotalLen = uiTotalLen;
    pdu->uiMsgLen = uiMsLen;
    return pdu;
}
