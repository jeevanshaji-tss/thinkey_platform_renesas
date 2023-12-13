#ifndef _TLOC_BLE_H_
#define _TLOC_BLE_H_

void BleInit();
void BleLoop();
int sendData(uint32_t uiSeqNum, uint16_t uwDistance, uint8_t ucConfidence, uint8_t ucNLos, uint16_t uwRawDistance, int8_t cSnr);
#endif /* _TLOC_BLE_H_ */
