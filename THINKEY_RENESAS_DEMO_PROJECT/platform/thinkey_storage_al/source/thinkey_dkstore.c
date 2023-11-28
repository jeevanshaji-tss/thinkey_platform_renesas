
#include "thinkey_dkstore.h"
#include "thinkey_platform_types.h"
#include "thinkey_debug.h"
//#include "nrf_fstorage.h"
//#include "nrf_fstorage_sd.h"
#include "FreeRTOS.h"
#include "task.h"

bool isWrite = 0;
/* Start and end of section defined in linker script for dkstore */
extern void* __dkstore_start__;
extern void* __dkstore_end__;
#define TKEY_STORAGE_START_ADDR &__dkstore_start__
#define TKEY_STORAGE_END_ADDR &__dkstore_end__

#define TKEY_PUB_KEY_STORAGE_SIZE 68

#define TKEY_STORAGE_PUB_KEY_INIT_BYTE_ADDR TKEY_STORAGE_START_ADDR
#define TKEY_STORAGE_PRIV_KEY_START_ADDR (TKEY_STORAGE_START_ADDR + TKEY_PUB_KEY_STORAGE_SIZE)

#define PRINT_FALSH_DATA 0

TKey_BYTE aucData[TKEY_PUB_KEY_STORAGE_SIZE] = {0};

volatile bool bWriteComplete = false;

/* This is to make the call synchronous */
void checkWriteComplete() {
    while(bWriteComplete != true) {
        nrf_sdh_evts_poll();
        vTaskDelay(100/portTICK_PERIOD_MS);
    }
    bWriteComplete = false;
}

/* Just to switch context and wait for the event*/
static void power_manage(void)
{
    (void) sd_app_evt_wait();
}

void wait_for_flash_ready(/*nrf_fstorage_t const * p_fstorage*/) {
    /* While fstorage is busy, sleep and wait for an event. */
//    while (nrf_fstorage_is_busy(p_fstorage))
//    {
//        power_manage();
//    }
}

static void vStorageStatusCbcallback(/*nrf_fstorage_evt_t * p_evt*/);

/*Define the storage Instance */
//NRF_FSTORAGE_DEF(/*nrf_fstorage_t sKeyStoreInstance*/) =
//{
// .evt_handler = vStorageStatusCbcallback,
// .start_addr = TKEY_STORAGE_START_ADDR,
// .end_addr = TKEY_STORAGE_END_ADDR,
//};

#define NULL_CHECK(a) if(NULL==a){break;}

TKey_StatusType TKey_DkStore_Init() {
    TKey_StatusType eStatus = E_TKEY_FAILURE;
//    ret_code_t retStatus = NRF_ERROR_INVALID_STATE;
//
//    do {
//        retStatus = nrf_fstorage_init(&sKeyStoreInstance, &nrf_fstorage_sd, NULL);
//        if(retStatus != NRF_SUCCESS) {
//            THINKEY_DEBUG_ERROR("Storage Init Failed! %d", retStatus);
//            break;
//        }
//        THINKEY_DEBUG_INFO("Storage INIT SUCCESS!");
//        eStatus = E_TKEY_SUCCESS;
//    } while(TKey_EXIT);
//
//    return eStatus;
}

/* Write data from pucData buffer to flash 
   *size can be :
   65 bytes for publicKey
   32 bytes for privateKey
 */
TKey_StatusType TKey_DkStore_Write(TKey_DKObject_t eObjType, TKey_BYTE*
        pucData, TKey_UINT32 size) {
    TKey_StatusType eStatus = E_TKEY_FAILURE;
//    ret_code_t retStatus = NRF_ERROR_INVALID_STATE;
//
//    /* Bits in flash can only be changed from 1's to 0's. So erase first and then write */
//    TKey_DkStore_Erase();
//
    switch(eObjType)
    {
    case E_TKEY_STORE_PUBLIC_KEY:
        {

            isWrite = 1;
            memcpy(aucData, pucData, size);
#if PRINT_FALSH_DATA
            THINKEY_DEBUG_INFO("Waiting for flash ready");
            for(int i=0; i < size - 4; i++) {
                THINKEY_DEBUG_INFO("%.2x", aucData[i]);
            }
#endif
//            wait_for_flash_ready(&sKeyStoreInstance);
            /* The flash is program unit sensitive.
            * We can store only in the multiple of 4. As Pub key is 65 bytes,
            we whould write the next 4 byte allingned size */
//            retStatus = nrf_fstorage_write(&sKeyStoreInstance,
//                    TKEY_STORAGE_PUB_KEY_INIT_BYTE_ADDR,
//                    aucData, TKEY_PUB_KEY_STORAGE_SIZE, NULL);
//            if(retStatus != NRF_SUCCESS) {
//                THINKEY_DEBUG_ERROR("Storage Write Failed! %d", retStatus);
//                break;
//            }
            THINKEY_DEBUG_INFO("Waiting for flash write done");
            /* Currently we have configured polling method to get events.
             * The BLE task does polling but the callback of write complete
             * was not getting called. Hence doing manual polling here */
//            checkWriteComplete();
            THINKEY_DEBUG_INFO("Flash write done");
            eStatus = E_TKEY_SUCCESS;
        }
        break;
    case E_TKEY_STORE_PRIVATE_KEY:
        {

        }
        break;
    default:
        break;
    }
    return eStatus;
}

/* Read data from flash to pucData buffer
   *size can be :  
   65 bytes for publicKey
   32 bytes for privateKey
 */
TKey_StatusType TKey_DkStore_Read(TKey_DKObject_t eObjType, TKey_BYTE*
        pucData, TKey_UINT32 size) {
    TKey_StatusType eStatus = E_TKEY_FAILURE;
//    ret_code_t retStatus = NRF_ERROR_INVALID_STATE;
//
    switch (eObjType)
    {
    case E_TKEY_STORE_PUBLIC_KEY:

        {
            if(isWrite == 1){
                pucData[0]=0x04;
            }
            /* The flash is program byte alignment sensitive.
            * We can store only in the multiple of 4. As Pub key is 65 bytes,
            * ignoring the first byte which is 04 and storing the x and y
            * component */
            THINKEY_DEBUG_INFO("Waiting for flash ready");
//            wait_for_flash_ready(&sKeyStoreInstance);
//            retStatus = nrf_fstorage_read(&sKeyStoreInstance,
//                    TKEY_STORAGE_PUB_KEY_INIT_BYTE_ADDR, aucData,
//                    TKEY_PUB_KEY_STORAGE_SIZE);
//            if(retStatus != NRF_SUCCESS) {
//                THINKEY_DEBUG_ERROR("Storage Read Failed! %d", retStatus);
//                break;
 //           }
//            memcpy(pucData, aucData, size);
            THINKEY_DEBUG_INFO("Storage Read SUCCESS!");
            eStatus = E_TKEY_SUCCESS;
        }
        break;
//    case E_TKEY_STORE_PRIVATE_KEY:
//        {
//
//        }
//        break;
    default:
        break;
    }
    return eStatus;
} 

TKey_StatusType TKey_DkStore_Erase(){
    TKey_StatusType eStatus = E_TKEY_FAILURE;
//    ret_code_t retStatus = NRF_ERROR_INVALID_STATE;
//
//    do {
//        retStatus = nrf_fstorage_erase(&sKeyStoreInstance,
//                TKEY_STORAGE_START_ADDR, 1, NULL);
//        if(retStatus != NRF_SUCCESS) {
//            THINKEY_DEBUG_ERROR("Storage Erase Failed! %d ", retStatus);
//            break;
//        }
//        checkWriteComplete();
//        THINKEY_DEBUG_INFO("Storage Erase SUCCESS!");
//        eStatus = E_TKEY_SUCCESS;
//    } while(TKey_EXIT);
//    eStatus = E_TKEY_SUCCESS;
    return eStatus;
}


static void vStorageStatusCbcallback(/*nrf_fstorage_evt_t * p_evt*/) {
//    THINKEY_DEBUG_INFO("##########Event Received! ID: %d, Result %d", p_evt->id, p_evt->result);
//    bWriteComplete = true;
}








































