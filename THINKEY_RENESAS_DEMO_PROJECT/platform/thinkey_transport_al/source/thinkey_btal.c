/*
 * thinkey_btal.c
 *
 *  Created on: 13-Oct-2021
 *      Author: adarshhegde
 *//*
 * thinkey_btal.c
 *
 *  Created on: 13-Oct-2021
 *      Author: adarshhegde
 */
#include "FreeRTOS.h"
#include "task.h"

#include "thinkey_btal.h"
#include "thinkey_debug.h"
#include "thinkey_transport.h"
#include "thinkey_transport_event_handler.h"
#include "thinkey_tab_app.h"
#include "thinkey_osal.h"


//#include "nrf_sdm.h"
//#include "nrf_sdh.h"
//#include "nrf_sdh_ble.h"
//#include "nrf_ble_gatt.h"

//#include "ble.h"
//#include "ble_advertising.h"
//#include "ble_conn_params.h"
//#include "sdk_errors.h"
//#include "app_error.h"
//#include "app_timer.h"
//#include "ble_l2cap.h"


#define PASSIVE_ENTRY_ADV_NAME              "DK_PE"                            /**< Name of device. Will be included in the advertising data. */
#define TAB_ADVERTISEMENT_NAME              "DK_TAB"                            /**< Name of device. Will be included in the advertising data. */
#define OWNER_PAIRING_ADV_NAME              "DigitalKey_OP"                            /**< Name of device. Will be included in the advertising data. */

#define MIN_CONN_INTERVAL                   MSEC_TO_UNITS(10, UNIT_1_25_MS)        /**< Minimum acceptable connection interval (0.4 seconds). */
#define MAX_CONN_INTERVAL                   MSEC_TO_UNITS(50, UNIT_1_25_MS)        /**< Maximum acceptable connection interval (0.65 second). */
#define SLAVE_LATENCY                       0                                       /**< Slave latency. */
#define CONN_SUP_TIMEOUT                    MSEC_TO_UNITS(4000, UNIT_10_MS)         /**< Connection supervisory timeout (4 seconds). */


#define APP_ADV_INTERVAL                    300                                     /**< The advertising interval (in units of 0.625 ms. This value corresponds to 187.5 ms). */
#define APP_BLE_CONN_CFG_TAG                1                                       /**< A tag identifying the SoftDevice BLE configuration. */
#define TAB_APP_CONN_CFG_TAG                2
#define APP_BLE_OBSERVER_PRIO               3                                       /**< Application's BLE observer priority. You shouldn't need to modify this value. */

#define FIRST_CONN_PARAMS_UPDATE_DELAY      APP_TIMER_TICKS(100)                   /**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (5 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY       APP_TIMER_TICKS(30000)                  /**< Time between each call to sd_ble_gap_conn_param_update after the first call (30 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT        10000                                       /**< Number of attempts before giving up the connection parameter negotiation. */


#define MAX_BLE_CONN_HANDLES 2
#define BLE_CONN_TAB_HANDLE 0
#define BLE_CONN_PHONE_HANDLE 1

#define L2CAP_RX_MPS                         60                                /**< Size of L2CAP Rx MPS (must be at least BLE_L2CAP_MPS_MIN).*/
#define L2CAP_TX_MPS                         512                                /**< Size of L2CAP Tx MPS (must be at least BLE_L2CAP_MPS_MIN).*/
#define L2CAP_RX_MTU                         512                                /**< Rx L2CAP MTU size (must be at least BLE_L2CAP_MTU_MIN).*/

#define L2CAP_SDU_MAX_BUFFER_SIZE 260

#define RANGING_DATA_MAX_LENGTH 2 * 4 /* Max Size of ranging data send to tab app in bytes  */
#define STATUS_MESSAGE_MAX_LENGTH 64 /* Max Size of message status messsage printed on Tab in bytes*/

//static uint16_t m_conn_handle[MAX_BLE_CONN_HANDLES] = {BLE_CONN_HANDLE_INVALID, BLE_CONN_HANDLE_INVALID};    /**< Handle of the current connection. */
static uint16_t m_l2cap_cid;
//static ble_uuid_t mDigitalKeyOPUUID =
//{
//    .uuid = 0xFFF5 ,
//    .type = BLE_UUID_TYPE_BLE
//};
//static ble_uuid_t mDigitalKeyPEUUID =
//{
//    .uuid = 0xFFF6 ,
//    .type = BLE_UUID_TYPE_BLE
//};
//static ble_uuid_t mTabAppUUID;

//static ble_gap_sec_params_t msSecParams;                                     /**< Security requirements for this application. */
//static ble_gap_sec_keyset_t msSecKeySet;
//static ble_gap_enc_key_t msLocalEncKey;
//static ble_gap_id_key_t msLocalIdKey;
//static ble_gap_sign_info_t msLocalSignKey;
//static ble_gap_lesc_p256_pk_t msLocalPublicKey;
//static ble_gap_id_key_t msPeerIdKey;
//static ble_gap_sign_info_t msPeerSignKey;
//static ble_gap_lesc_p256_pk_t msPeerPublicKey;


static uint16_t muDigitalkeyServiceHandle;
static uint16_t muTabServiceHandle;
//static ble_gatts_char_handles_t msPcmCharHandle;
//static ble_gatts_char_handles_t msRangingDataCharHandle;
//static ble_gatts_char_handles_t msStatusMessageCharHandle;
//static ble_gatts_char_handles_t msOPStatusCharHandle;
//static ble_gatts_char_handles_t msDoorOpenThresholdValueCharHandle;
//static ble_gatts_char_handles_t msStartEngineThresholdValueCharHandle;
static THINKey_UINT16 usPcmInitialValue = 129;
static THINKey_UINT32 uiRangingDataInitialValue = 100;
static uint16_t usL2capChannelId = 0;
static THINKey_BYTE acL2capSDUData[L2CAP_SDU_MAX_BUFFER_SIZE];
static TKey_BOOL bOPStatus = TKey_FALSE;

NRF_BLE_GATT_DEF(m_gatt);                                           /**< GATT module instance. */
BLE_ADVERTISING_DEF(mDigitalKeyAdv);                                 /**< Advertising module instance. */
BLE_ADVERTISING_DEF(mTabAdv);

#define BLE_QUEUE_LENGTH 10
#define BTAL_EVENT_TASK_NAME "BTAL Task"
#define THIKEY_BTAL_EVENT_PRIORITY 5
#define THINKEY_BTAL_STACK_SIZE 2048
#define PRINT_DATA 0

THINKey_VOID task_process_events(THINKey_VOID* param);
THINKey_VOID* vProcessQueue;//TODO: for experimentation

THINKey_BOOL bBtInitDone = THINKey_FALSE;
THINKey_BOOL bTabConnected = THINKey_FALSE;

static void l2cap_params_init(void);

/**@brief Function for handling BLE events.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 * @param[in]   p_context   Unused.
 */
static void ble_evt_handler(/*ble_evt_t const * p_ble_evt, void * p_context*/)
{
//    ret_code_t eNrfErrorCode;
//    THINKey_eStatusType eTkeyResult;
//    sTabAppMessageType sTabAppMessage;
//    THINKey_sEventQueueMsgType sMessage;
//    THINKey_sTransportSSHandleType *psTransportSSHandle;
//	THINKEY_DEBUG_INFO("ble_evt_handler: 0x%x",p_ble_evt->header.evt_id);
//    switch (p_ble_evt->header.evt_id)
//    {
//        case BLE_GAP_EVT_CONNECTED:
//            if (!bTabConnected)
//            {
//                sTabAppMessage.eEvent = E_TAB_DEVICE_CONNECTED;
//
//                m_conn_handle[BLE_CONN_TAB_HANDLE] = p_ble_evt->evt.gap_evt.conn_handle;
//                THINKEY_DEBUG_INFO("Tab connected:%d", m_conn_handle[BLE_CONN_TAB_HANDLE]);
//                THINKey_sTabAppParamType *psTabTaskParams = hGetTabAppHandle();
//                bTabConnected = THINKey_TRUE;
//                eTkeyResult = THINKey_OSAL_eQueueSend
//                        (psTabTaskParams->vTabAppQueue, &sTabAppMessage);
//                if(E_THINKEY_SUCCESS == eTkeyResult)
//                {
//                    THINKEY_DEBUG_INFO ("Send to tab app success\r\n");
//                }
//
//            }
//            break;
//
//        case BLE_GAP_EVT_DISCONNECTED:
//            THINKEY_DEBUG_INFO("Disconnected, reason %d.",
//                          p_ble_evt->evt.gap_evt.params.disconnected.reason);
//            if (p_ble_evt->evt.gap_evt.conn_handle == m_conn_handle[BLE_CONN_TAB_HANDLE])
//            {
//                m_conn_handle[BLE_CONN_TAB_HANDLE] = BLE_CONN_HANDLE_INVALID;
//                THINKEY_DEBUG_ERROR("Tab bt disconnected! :%d", p_ble_evt->evt.gap_evt.conn_handle);
//            }
//            break;
//
//        case BLE_GAP_EVT_PHY_UPDATE_REQUEST:
//        {
//            THINKEY_DEBUG_INFO("PHY update request.");
//            ble_gap_phys_t const phys =
//            {
//                .rx_phys = BLE_GAP_PHY_AUTO,
//                .tx_phys = BLE_GAP_PHY_AUTO,
//            };
//            eNrfErrorCode = sd_ble_gap_phy_update(p_ble_evt->evt.gap_evt.conn_handle, &phys);
//            APP_ERROR_CHECK(eNrfErrorCode);
//        } break;
//
//        case BLE_GATTC_EVT_TIMEOUT:
//            // Disconnect on GATT Client timeout event.
//            THINKEY_DEBUG_INFO("GATT Client Timeout.");
//            eNrfErrorCode = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle,
//                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
//            APP_ERROR_CHECK(eNrfErrorCode);
//            break;
//
//        case BLE_GATTS_EVT_TIMEOUT:
//            // Disconnect on GATT Server timeout event.
//            THINKEY_DEBUG_INFO("GATT Server Timeout.");
//            eNrfErrorCode = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle,
//                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
//            APP_ERROR_CHECK(eNrfErrorCode);
//            break;
//        case BLE_GATTS_EVT_WRITE:
//            THINKEY_DEBUG_INFO("BLE_GATTS_EVT_WRITE");
//            ble_gatts_evt_write_t *writeParam = &p_ble_evt->evt.gatts_evt.params.write;
//            THINKEY_DEBUG_INFO("hadle:%d uuid:%x type:%d operation:%d len:%d data[0]:%x auth:%d",
//                    writeParam->handle,
//                    writeParam->uuid.uuid,
//                    writeParam->uuid.type,
//                    writeParam->op,
//                    writeParam->len,
//                    writeParam->data[0],
//                    writeParam->auth_required);
//            THINKEY_DEBUG_INFO("msOPStatusCharHandle: %d", msOPStatusCharHandle.value_handle);
//            if (writeParam->handle == msOPStatusCharHandle.value_handle) {
//                memset(&sTabAppMessage,0x00, sizeof(sTabAppMessage));
//                THINKEY_DEBUG_INFO("writeParam->data[0]: %d", writeParam->data[0]);
//                if (writeParam->data[0] == 1) {
//                    sTabAppMessage.eEvent = E_TAB_APP_ENABLE_OP;
//                    bOPStatus = TKey_TRUE;
//                } else {
//                    sTabAppMessage.eEvent = E_TAB_APP_DISABLE_OP;
//                    bOPStatus = TKey_FALSE;
//                }
//                THINKey_sTabAppParamType *psTabTaskParams = hGetTabAppHandle();
//                eTkeyResult = THINKey_OSAL_eQueueSend
//                        (psTabTaskParams->vTabAppQueue, &sTabAppMessage);
//                if(E_THINKEY_SUCCESS == eTkeyResult)
//                {
//                    THINKEY_DEBUG_INFO ("Send to tab app success\r\n");
//                }
//            }
//            break;
//        case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
//            THINKEY_DEBUG_INFO("BLE_GAP_EVT_SEC_PARAMS_REQUEST");
//            ble_gap_sec_params_t *psPeerSecParam = &(p_ble_evt->evt.gap_evt.params.sec_params_request.peer_params);
//            THINKEY_DEBUG_INFO("bond: %d mitm:%d lesc:%d keypress:%d io_caps:%d oob:%d min_key_size:%d max_key_size:%d",psPeerSecParam->bond,
//                    psPeerSecParam->mitm, psPeerSecParam->lesc, psPeerSecParam->keypress, psPeerSecParam->io_caps, psPeerSecParam->oob,
//                    psPeerSecParam->min_key_size, psPeerSecParam->max_key_size);
//            THINKEY_DEBUG_INFO("kdist_own: enc:%d id:%d sign:%d link:%d", psPeerSecParam->kdist_own.enc, psPeerSecParam->kdist_own.id,
//                     psPeerSecParam->kdist_own.sign,  psPeerSecParam->kdist_own.link);
//            THINKEY_DEBUG_INFO("kdist_peer: enc:%d id:%d sign:%d link:%d", psPeerSecParam->kdist_peer.enc, psPeerSecParam->kdist_peer.id,
//                     psPeerSecParam->kdist_peer.sign,  psPeerSecParam->kdist_peer.link);
//            psTransportSSHandle = hGetTransportSSHandle();
//            sMessage.eCommand = E_THINKEY_START_PAIRING;
//            sMessage.uiParam1 = p_ble_evt->evt.gap_evt.conn_handle;
//            eTkeyResult = THINKey_OSAL_eQueueSend (psTransportSSHandle->hTPEventQueue, &sMessage);
//            break;
//
//        case BLE_GAP_EVT_AUTH_KEY_REQUEST:
//            THINKEY_DEBUG_INFO("BLE_GAP_EVT_AUTH_KEY_REQUEST");
//            break;
//
//        case BLE_GAP_EVT_LESC_DHKEY_REQUEST:
//            THINKEY_DEBUG_INFO("BLE_GAP_EVT_LESC_DHKEY_REQUEST");
//            break;
//
//         case BLE_GAP_EVT_AUTH_STATUS:
//            THINKEY_DEBUG_INFO("BLE_GAP_EVT_AUTH_STATUS: status=0x%x bond=0x%x lv4: %d kdist_own:0x%x kdist_peer:0x%x",
//                          p_ble_evt->evt.gap_evt.params.auth_status.auth_status,
//                          p_ble_evt->evt.gap_evt.params.auth_status.bonded,
//                          p_ble_evt->evt.gap_evt.params.auth_status.sm1_levels.lv4,
//                          *((uint8_t *)&p_ble_evt->evt.gap_evt.params.auth_status.kdist_own),
//                          *((uint8_t *)&p_ble_evt->evt.gap_evt.params.auth_status.kdist_peer));
//            if (p_ble_evt->evt.gap_evt.params.auth_status.bonded) {
//                psTransportSSHandle = hGetTransportSSHandle();
//                sMessage.eCommand = E_THINKEY_START_BONDING;
//                eTkeyResult = THINKey_OSAL_eQueueSend(
//                    psTransportSSHandle->hTPEventQueue, &sMessage);
//            } else {
//                THINKEY_DEBUG_ERROR("Bonding Failed from peer!!");
//            }
//            break;
//
//		case BLE_L2CAP_EVT_CH_SETUP_REFUSED:
//			THINKEY_DEBUG_INFO("L2cap refused: source:%d status:%d"
//				,p_ble_evt->evt.l2cap_evt.params.ch_setup_refused.source,
//			p_ble_evt->evt.l2cap_evt.params.ch_setup_refused.status);
//
//			break;
//        case BLE_L2CAP_EVT_CH_SETUP_REQUEST:
//            THINKEY_DEBUG_INFO("l2cap channel setup request for psm: %d connectionHandle:%d",
//                p_ble_evt->evt.l2cap_evt.params.ch_setup_request.le_psm, p_ble_evt->evt.l2cap_evt.conn_handle);
//            m_conn_handle[BLE_CONN_PHONE_HANDLE] = p_ble_evt->evt.l2cap_evt.conn_handle;
//            ble_l2cap_ch_setup_params_t ch_setup_params = {{0}};
//            ch_setup_params.status = BLE_L2CAP_CH_STATUS_CODE_SUCCESS;
//            ch_setup_params.rx_params.rx_mps = L2CAP_RX_MPS;
//            if (ch_setup_params.rx_params.rx_mps < BLE_L2CAP_MPS_MIN)
//            {
//                ch_setup_params.rx_params.rx_mps = BLE_L2CAP_MPS_MIN;
//            }
//            ch_setup_params.rx_params.rx_mtu = L2CAP_RX_MTU;
//            if (ch_setup_params.rx_params.rx_mtu < BLE_L2CAP_MTU_MIN)
//            {
//                ch_setup_params.rx_params.rx_mtu = BLE_L2CAP_MTU_MIN;
//            }
//            ch_setup_params.rx_params.sdu_buf.p_data = NULL;
//            ch_setup_params.rx_params.sdu_buf.len = 0;
//            usL2capChannelId = p_ble_evt->evt.l2cap_evt.local_cid;
//            vTaskDelay(50/portTICK_PERIOD_MS);
//            THINKEY_DEBUG_INFO("ConnectionHandle: %d channelId:%d ",m_conn_handle[BLE_CONN_PHONE_HANDLE], usL2capChannelId );
//            eNrfErrorCode = sd_ble_l2cap_ch_setup(m_conn_handle[BLE_CONN_PHONE_HANDLE],
//                                     &usL2capChannelId,
//                                     &ch_setup_params);
//            if (eNrfErrorCode != NRF_SUCCESS)
//            {
//                THINKEY_DEBUG_ERROR("sd_ble_l2cap_ch_setup retruned:%d", eNrfErrorCode);
//            }
//            break;
//        case BLE_L2CAP_EVT_CH_SETUP:
//            THINKEY_DEBUG_INFO("l2cap channel setup done.conn Handle: %d Cid: %d, credits: %d, tx_mps:%d, tc_mtu:%d",
//            p_ble_evt->evt.l2cap_evt.conn_handle,
//            p_ble_evt->evt.l2cap_evt.local_cid,
//            p_ble_evt->evt.l2cap_evt.params.ch_setup.tx_params.credits,
//            p_ble_evt->evt.l2cap_evt.params.ch_setup.tx_params.tx_mps,
//            p_ble_evt->evt.l2cap_evt.params.ch_setup.tx_params.tx_mtu);
//            /* TODO: Handle credits */
//            if (m_conn_handle[BLE_CONN_PHONE_HANDLE] == p_ble_evt->evt.l2cap_evt.conn_handle)
//            {
//                m_l2cap_cid = p_ble_evt->evt.l2cap_evt.local_cid;
//                ble_data_t sL2capSDUBuffer = {
//                    .p_data = acL2capSDUData,
//                    .len = L2CAP_SDU_MAX_BUFFER_SIZE
//                };
//                vTaskDelay(50/portTICK_PERIOD_MS);
//                eNrfErrorCode = sd_ble_l2cap_ch_rx(m_conn_handle[BLE_CONN_PHONE_HANDLE], m_l2cap_cid,&sL2capSDUBuffer);
//                THINKEY_DEBUG_INFO("sd_ble_l2cap_ch_rx retruned :%d", eNrfErrorCode);
//                 /*Send to event queue*/
//                psTransportSSHandle = hGetTransportSSHandle();
//                if (bOPStatus) {
//                    sMessage.eCommand = E_THINKEY_NEW_DEVICE_CONNECTED;
//                } else {
//                    sMessage.eCommand = E_THINKEY_DEVICE_CONNECTED;
//                }
//                sMessage.uiParam1 = p_ble_evt->evt.l2cap_evt.local_cid;
//                eTkeyResult = THINKey_OSAL_eQueueSend
//                            (psTransportSSHandle->hTPEventQueue, &sMessage);
//                if (E_THINKEY_SUCCESS == eTkeyResult)
//                {
//                    THINKEY_DEBUG_INFO ("New Device connected with Handle %x",
//                                p_ble_evt->evt.l2cap_evt.local_cid);
//                }
//
//                /*TODO: This is for UI on Tab. put this in macro? */
//                sTabAppMessage.eEvent = E_TAB_BT_KEY_DETECTED;
//                sTabAppMessage.aiData[0] = E_TAB_BT_KEY_DETECTED;
//                THINKey_sTabAppParamType *psTabTaskParams = hGetTabAppHandle();
//                eTkeyResult = THINKey_OSAL_eQueueSend
//                        (psTabTaskParams->vTabAppQueue, &sTabAppMessage);
//                if(E_THINKEY_SUCCESS == eTkeyResult)
//                {
//                    THINKEY_DEBUG_INFO ("Send to tab app success\r\n");
//                }
//            }
//            break;
//        case BLE_L2CAP_EVT_CH_RX:
//            THINKEY_DEBUG_INFO("l2cap data received: cid:%d dataLen:%d", p_ble_evt->evt.l2cap_evt.local_cid,
//                p_ble_evt->evt.l2cap_evt.params.rx.sdu_len);
//            THINKEY_DEBUG_INFO("l2cap data :%d , len:%d", p_ble_evt->evt.l2cap_evt.params.rx.sdu_buf.p_data,
//            p_ble_evt->evt.l2cap_evt.params.rx.sdu_buf.len);
//#if PRINT_DATA
//            for (int i=0; i <p_ble_evt->evt.l2cap_evt.params.rx.sdu_buf.len ; i++)
//            {
//                THINKEY_DEBUG_INFO("%BLEData[%d]:%x",i, *(p_ble_evt->evt.l2cap_evt.params.rx.sdu_buf.p_data+i));
//            }
//#endif
//			psTransportSSHandle = hGetTransportSSHandle();
//			sMessage.eCommand = E_THINKEY_DATA_RECEIVED;
//			sMessage.uiParam1 = p_ble_evt->evt.l2cap_evt.local_cid;
//			sMessage.uiParam2 = p_ble_evt->evt.l2cap_evt.params.rx.sdu_len;
//
//			//TODO: This results in double copy. Can we optimize?
//			memcpy(sMessage.abData, p_ble_evt->evt.l2cap_evt.params.rx.sdu_buf.p_data, p_ble_evt->evt.l2cap_evt.params.rx.sdu_len);
//			eTkeyResult = THINKey_OSAL_eQueueSend
//					(psTransportSSHandle->hTPEventQueue, &sMessage);
//			if(E_THINKEY_SUCCESS != eTkeyResult)
//			{
//				THINKEY_DEBUG_ERROR("Sending data to queue failed %d",
//						eTkeyResult);
//			}
//            ble_data_t sL2capSDUBuffer = {
//                .p_data = acL2capSDUData,
//                .len = L2CAP_SDU_MAX_BUFFER_SIZE
//            };
//            //vTaskDelay(500/portTICK_PERIOD_MS);
//            eNrfErrorCode = sd_ble_l2cap_ch_rx(m_conn_handle[BLE_CONN_PHONE_HANDLE], m_l2cap_cid,&sL2capSDUBuffer);
//            THINKEY_DEBUG_INFO("sd_ble_l2cap_ch_rx retruned :%d", eNrfErrorCode);
//            break;
//        case BLE_L2CAP_EVT_CH_TX:
//            THINKEY_DEBUG_INFO("lcap tx done. cid:%d dataLen:%d", p_ble_evt->evt.l2cap_evt.local_cid,
//                p_ble_evt->evt.l2cap_evt.params.tx.sdu_buf.len);
//            break;
//            case BLE_L2CAP_EVT_CH_RELEASED:
//            THINKEY_DEBUG_INFO("L2cap disconnected ConnHandel:%d cid:%d", p_ble_evt->evt.l2cap_evt.conn_handle,
//                                p_ble_evt->evt.l2cap_evt.local_cid);
//            if (m_conn_handle[BLE_CONN_PHONE_HANDLE] == p_ble_evt->evt.l2cap_evt.conn_handle &&
//                usL2capChannelId == p_ble_evt->evt.l2cap_evt.local_cid)
//            {
//                m_conn_handle[BLE_CONN_PHONE_HANDLE] = BLE_CONN_HANDLE_INVALID;
//                psTransportSSHandle = hGetTransportSSHandle();
//                sMessage.eCommand = E_THINKEY_DEVICE_DISCONNECTED;
//                sMessage.uiParam1 = m_l2cap_cid;
//                eTkeyResult = THINKey_OSAL_eQueueSend
//                        (psTransportSSHandle->hTPEventQueue, &sMessage);
//                if(E_THINKEY_SUCCESS == eTkeyResult)
//                {
//                    THINKEY_DEBUG_INFO ("Device disconnected with Handle %x",
//                            m_l2cap_cid);
//                }
//            }
//            break;
//        case BLE_GATTC_EVT_HVX:
//            THINKEY_DEBUG_INFO("BLE_GATTC_EVT_HVX. connHandle:%d handle:%d type:%d len", p_ble_evt->evt.gattc_evt.conn_handle,
//                    p_ble_evt->evt.gattc_evt.params.hvx.handle, p_ble_evt->evt.gattc_evt.params.hvx.type,
//                    p_ble_evt->evt.gattc_evt.params.hvx.len);
//            break;
//        default:
//            // No implementation needed.
//            break;
//    }
}

static void l2cap_params_init(void)
{
//	ret_code_t              eNrfErrorCode;
//	ble_cfg_t ble_cfg;
//    /* TODO: These all are dummy values. THe APi is having some issue while decoding at the
//    connectivity chip, so the configuration is hardcoded in the connectivity chip */
//	ble_cfg.conn_cfg.conn_cfg_tag = APP_BLE_CONN_CFG_TAG;
//    THINKEY_DEBUG_ERROR("config tag: %d",ble_cfg.conn_cfg.conn_cfg_tag);
//    ble_cfg.conn_cfg.params.l2cap_conn_cfg.rx_mps        = L2CAP_RX_MPS;
//    ble_cfg.conn_cfg.params.l2cap_conn_cfg.rx_queue_size = 1;
//    ble_cfg.conn_cfg.params.l2cap_conn_cfg.tx_mps        = L2CAP_TX_MPS;
//    ble_cfg.conn_cfg.params.l2cap_conn_cfg.tx_queue_size = 1;
//    ble_cfg.conn_cfg.params.l2cap_conn_cfg.ch_count      = 1;
//	eNrfErrorCode = sd_ble_cfg_set(BLE_CONN_CFG_L2CAP, &ble_cfg, 0);
//	if (eNrfErrorCode != NRF_SUCCESS)
//	{
//		THINKEY_DEBUG_ERROR("sd_ble_cfg_set retruned error:%d",eNrfErrorCode);
//	}
}

/**@brief Function for initializing the BLE stack.
 *
 * @details Initializes the SoftDevice and the BLE event interrupt.
 */
static void ble_stack_init(void)
{
//    ret_code_t eNrfErrorCode;
//
//    eNrfErrorCode = nrf_sdh_enable_request();
//    APP_ERROR_CHECK(eNrfErrorCode);
//
//    // Configure the BLE stack using the default settings.
//    // Fetch the start address of the application RAM.
//    uint32_t ram_start = 0;
//    eNrfErrorCode = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start);
//    APP_ERROR_CHECK(eNrfErrorCode);
//	l2cap_params_init();
//	THINKEY_DEBUG_ERROR("l2cap prama init done");
//    // Enable BLE stack.
//    eNrfErrorCode = nrf_sdh_ble_enable(&ram_start);
//    APP_ERROR_CHECK(eNrfErrorCode);
//
//    // Register a handler for BLE events.
//    NRF_SDH_BLE_OBSERVER(m_ble_observer, APP_BLE_OBSERVER_PRIO, ble_evt_handler, NULL);
}

/**@brief Function for the GAP initialization.
 *
 * @details This function sets up all the necessary GAP (Generic Access Profile) parameters of the
 *          device including the device name, appearance, and the preferred connection parameters.
 */
static void gap_params_init(void)
{
//    ret_code_t              eNrfErrorCode;
//    ble_gap_conn_params_t   gap_conn_params;
//    ble_gap_conn_sec_mode_t sec_mode;
//
//    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);
//
//    eNrfErrorCode = sd_ble_gap_device_name_set(&sec_mode,
//                                          (const uint8_t *)TAB_ADVERTISEMENT_NAME,
//                                          strlen(TAB_ADVERTISEMENT_NAME));
//    APP_ERROR_CHECK(eNrfErrorCode);
//
//    /* TODO:do we really neeed this?*/
//    memset(&gap_conn_params, 0, sizeof(gap_conn_params));
//
//    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
//    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
//    gap_conn_params.slave_latency     = SLAVE_LATENCY;
//    gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;
//
//    eNrfErrorCode = sd_ble_gap_ppcp_set(&gap_conn_params);
//    THINKEY_DEBUG_ERROR("sd_ble_gap_ppcp_set retruned: %d", eNrfErrorCode);
//    APP_ERROR_CHECK(eNrfErrorCode);
}


/**@brief GATT module event handler.
 */
//static void gatt_evt_handler(nrf_ble_gatt_t * p_gatt, nrf_ble_gatt_evt_t const * p_evt)
//{
//    THINKEY_DEBUG_INFO("gatt_evt_handler :%d", p_evt->evt_id);
//    if (p_evt->evt_id == NRF_BLE_GATT_EVT_ATT_MTU_UPDATED)
//    {
//        THINKEY_DEBUG_INFO("GATT ATT MTU on connection 0x%x changed to %d.",
//                     p_evt->conn_handle,
//                     p_evt->params.att_mtu_effective);
//    }

//}

/**@brief Function for initializing the GATT module.
 */
static void gatt_init(void)
{
//    ret_code_t eNrfErrorCode = nrf_ble_gatt_init(&m_gatt, gatt_evt_handler);
//    APP_ERROR_CHECK(eNrfErrorCode);
}


/**@brief Function for putting the chip into sleep mode.
 *
 * @note This function will not return.
 */
static void sleep_mode_enter(void)
{
//    ret_code_t eNrfErrorCode;
//
//    // Go to system-off mode (this function will not return; wakeup will cause a reset).
//    eNrfErrorCode = sd_power_system_off();
//    APP_ERROR_CHECK(eNrfErrorCode);
}

/**@brief Function for handling advertising events.
 *
 * @details This function will be called for advertising events which are passed to the application.
 *
 * @param[in] ble_adv_evt  Advertising event.
 */
//static void on_adv_evt(ble_adv_evt_t ble_adv_evt)
//{
//    THINKEY_DEBUG_INFO("on_adv_evt:%d", ble_adv_evt);
//    switch (ble_adv_evt)
//    {
//        case BLE_ADV_EVT_FAST:
//            THINKEY_DEBUG_INFO("Fast advertising.");
//            break;
//
//        case BLE_ADV_EVT_IDLE:
//            THINKEY_DEBUG_INFO("Advertisment Stopped");
//            sleep_mode_enter();
//            THINKEY_DEBUG_INFO("Sleep mode returned");
//            break;
//
//        default:
//            break;
//    }
//}

/**@brief Function for handling the Connection Parameters Module.
 *
 * @details This function will be called for all events in the Connection Parameters Module which
 *          are passed to the application.
 *          @note All this function does is to disconnect. This could have been done by simply
 *                setting the disconnect_on_fail config parameter, but instead we use the event
 *                handler mechanism to demonstrate its use.
 *
 * @param[in] p_evt  Event received from the Connection Parameters Module.
 */
static void on_conn_params_evt(/*ble_conn_params_evt_t * p_evt*/)
{
//    ret_code_t eNrfErrorCode;
//    THINKEY_DEBUG_INFO("on_conn_params_evt event:%d", p_evt->evt_type);
//    if (p_evt->evt_type == BLE_CONN_PARAMS_EVT_FAILED)
//    {
//        eNrfErrorCode = sd_ble_gap_disconnect(p_evt->conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
//        APP_ERROR_CHECK(eNrfErrorCode);
//    }
}

/**@brief Function for handling a Connection Parameters error.
 *
 * @param[in] nrf_error  Error code containing information about what went wrong.
 */
static void conn_params_error_handler(uint32_t nrf_error)
{
//    THINKEY_DEBUG_INFO("conn_params_error_handler nrf_error:%d", nrf_error);
//    APP_ERROR_HANDLER(nrf_error);
}

/**@brief Function for initializing the Connection Parameters module.
 */
static void conn_params_init(void)
{
//    ret_code_t             eNrfErrorCode;
//    ble_conn_params_init_t cp_init;
//
//    memset(&cp_init, 0, sizeof(cp_init));
//
//    cp_init.p_conn_params                  = NULL;
//    cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
//    cp_init.next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY;
//    cp_init.max_conn_params_update_count   = MAX_CONN_PARAMS_UPDATE_COUNT;
//    /* TODO: what is this cccd handle ? */
//    cp_init.start_on_notify_cccd_handle    = 0;
//    cp_init.disconnect_on_fail             = false;
//    cp_init.evt_handler                    = on_conn_params_evt;
//    cp_init.error_handler                  = conn_params_error_handler;
//
//    eNrfErrorCode = ble_conn_params_init(&cp_init);
//    APP_ERROR_CHECK(eNrfErrorCode);
}

void services_init()
{
//    uint32_t              eNrfErrorCode;
//    ble_add_char_params_t add_char_params;
//    THINKey_BYTE ucUUIDType;
//
//    eNrfErrorCode = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,
//                                        &mDigitalKeyOPUUID,
//                                        &muDigitalkeyServiceHandle);
//    if (NRF_SUCCESS != eNrfErrorCode)
//    {
//        THINKEY_DEBUG_ERROR("Could not add Digital Key ble service: %x", mDigitalKeyOPUUID.uuid);
//    }
//    /* Add psm characteristics to the service */
//    /* First add the Vendor uuid to the table, and store the type value retunrned by the function
//       This value will be used along with 12th and 13th bytes of 16 bytes uuid for all later api calls */
//    /* if more UUID has to be added NRF_SDH_BLE_VS_UUID_COUNT in sdk_config.h has to be increased. Otherwise api
//       will return memory error */
//    ble_uuid128_t sPsmCharUUID =
//    {
//        .uuid128 = {0xA3, 0x80, 0xF9, 0xE5, 0x1E, 0x6B, 0xE4, 0x8B, 0x3A, 0x4B, 0x23, 0x9E, 0x30, 0xA1, 0xB5, 0xD3}
//    };
//    eNrfErrorCode = sd_ble_uuid_vs_add(&sPsmCharUUID, &ucUUIDType);
//    if (eNrfErrorCode != NRF_SUCCESS)
//    {
//        THINKEY_DEBUG_ERROR("could not add sPsmCharUUID. API retruned: %x, type:%d ", eNrfErrorCode, ucUUIDType);
//    }
//    memset(&add_char_params, 0, sizeof(add_char_params));
//
//    /* Only 12 the and 13th byte of 16 byte uuid, in reverse order */
//    add_char_params.uuid              = 0xA130;
//    add_char_params.uuid_type         = ucUUIDType;
//    add_char_params.max_len           = sizeof(THINKey_INT16);
//    add_char_params.init_len          = sizeof(THINKey_INT16);
//    add_char_params.p_init_value      = (uint8_t *)&usPcmInitialValue;
//    add_char_params.char_props.read = 1;
//    add_char_params.read_access     = SEC_OPEN;
//
//    eNrfErrorCode = characteristic_add(muDigitalkeyServiceHandle, &add_char_params, &msPcmCharHandle);
//    if (eNrfErrorCode != NRF_SUCCESS)
//    {
//        THINKEY_DEBUG_ERROR("Could not add pcm charecterictic");
//    }
//
//    /* Adding Tab ble service.======================================
//    TODO: This is only for demo */
//    /* First add the Vendor uuid to the table, and store the type value retunrned by the function
//       This value will be used along with 12th and 13th bytes of 16 bytes uuid for all later api calls */
//    ble_uuid128_t sTabBleUUID =
//    {
//        .uuid128 = {0xDB, 0x01, 0x86, 0x27, 0x99, 0x79, 0xBF, 0xBC, 0x06, 0x47, 0x5F, 0x33, 0xCD, 0xE1, 0x2E, 0x16}
//    };
//    eNrfErrorCode = sd_ble_uuid_vs_add(&sTabBleUUID, &ucUUIDType);
//    if (eNrfErrorCode != NRF_SUCCESS)
//    {
//        THINKEY_DEBUG_ERROR("could not add sTabBleUUID. API retruned: %x, type:%d ", eNrfErrorCode, ucUUIDType);
//    }
//    /* 12th and 13th bytes in reverse order */
//    mTabAppUUID.uuid = 0xE1CD;
//    mTabAppUUID.type = ucUUIDType;
//    eNrfErrorCode = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,
//                                        &mTabAppUUID,
//                                        &muTabServiceHandle);
//    if (NRF_SUCCESS != eNrfErrorCode)
//    {
//        THINKEY_DEBUG_ERROR("Could not add tab service: %x", mTabAppUUID.uuid);
//    }
//
//    /* Add Ranging Data characteristics-------------------------------*/
//    /* First add the Vendor uuid to the table, and store the type value retunrned by the function
//    This value will be used along with 12th and 13th bytes of 16 bytes uuid for all later api calls */
//    ble_uuid128_t sRangingDataUUID =
//    {
//        .uuid128 = {0x08,0x37,  0xDB, 0xA5, 0x87, 0x50, 0xE5, 0x9E, 0x42, 0x4F, 0xB6, 0xFC, 0x53, 0xD6, 0x36, 0xEA}
//    };
//    eNrfErrorCode = sd_ble_uuid_vs_add(&sRangingDataUUID, &ucUUIDType);
//    if (eNrfErrorCode != NRF_SUCCESS)
//    {
//        THINKEY_DEBUG_ERROR("could not add sRangingDataUUID. API retruned: %x, type:%d ", eNrfErrorCode, ucUUIDType);
//    }
//    memset(&add_char_params, 0, sizeof(add_char_params));
//     /* Only 12 the and 13th byte of 16 byte uuid */
//    add_char_params.uuid              = 0xD653;
//    add_char_params.uuid_type         = ucUUIDType;
//    add_char_params.max_len           = RANGING_DATA_MAX_LENGTH;
//    add_char_params.init_len          = sizeof(THINKey_INT32);
//    add_char_params.p_init_value      = (uint8_t *)&uiRangingDataInitialValue;
//    add_char_params.char_props.read = 1;
//    add_char_params.char_props.notify = 1;
//    add_char_params.read_access     = SEC_OPEN;
//    add_char_params.cccd_write_access = SEC_OPEN;
//    eNrfErrorCode = characteristic_add(muTabServiceHandle, &add_char_params, &msRangingDataCharHandle);
//    if (eNrfErrorCode != NRF_SUCCESS)
//    {
//        THINKEY_DEBUG_ERROR("Could not add Ranging data charecterictic");
//    }
//
//    /* Add status message characteristics-----------------------------*/
//    /* First add the Vendor uuid to the table, and store the type value retunrned by the function
//    This value will be used along with 12th and 13th bytes of 16 bytes uuid for all later api calls */
//    ble_uuid128_t sSatusMessageUUID =
//    {
//        .uuid128 = {0x6B, 0x70, 0x30, 0x22, 0xA7, 0x08, 0x2B, 0xBC, 0x30, 0x4F, 0xCB, 0x6C, 0x7F, 0x21, 0xE7, 0xAF}
//    };
//    eNrfErrorCode = sd_ble_uuid_vs_add(&sSatusMessageUUID, &ucUUIDType);
//    if (eNrfErrorCode != NRF_SUCCESS)
//    {
//        THINKEY_DEBUG_ERROR("could not add sSatusMessageUUID. API retruned: %x, type:%d ", eNrfErrorCode, ucUUIDType);
//    }
//    memset(&add_char_params, 0, sizeof(add_char_params));
//     /* Only 12 the and 13th byte of 16 byte uuid */
//    add_char_params.uuid              = 0x217F;
//    add_char_params.uuid_type         = ucUUIDType;
//    add_char_params.max_len           = STATUS_MESSAGE_MAX_LENGTH;
//    add_char_params.init_len          = 0;
//    add_char_params.p_init_value      = NULL;
//    add_char_params.char_props.read = 1;
//    add_char_params.char_props.notify = 1;
//    add_char_params.read_access     = SEC_OPEN;
//    add_char_params.cccd_write_access = SEC_OPEN;
//    eNrfErrorCode = characteristic_add(muTabServiceHandle, &add_char_params, &msStatusMessageCharHandle);
//    if (eNrfErrorCode != NRF_SUCCESS)
//    {
//        THINKEY_DEBUG_ERROR("Could not add Status message charecterictic");
//    }
//
//    /* Add OP status characteristics--------------------------------*/
//    /* First add the Vendor uuid to the table, and store the type value retunrned by the function
//    This value will be used along with 12th and 13th bytes of 16 bytes uuid for all later api calls */
//    ble_uuid128_t sOPStatusUUID =
//    {
//        .uuid128 = {0x35, 0x8C, 0x28, 0xB2, 0x14, 0x45, 0x24, 0x83, 0xE4, 0x40, 0x29, 0xA5, 0x7E, 0xBE, 0x6C, 0x79}
//    };
//    eNrfErrorCode = sd_ble_uuid_vs_add(&sOPStatusUUID, &ucUUIDType);
//    if (eNrfErrorCode != NRF_SUCCESS)
//    {
//        THINKEY_DEBUG_ERROR("could not add sOPStatusUUID. API retruned: %x, type:%d ", eNrfErrorCode, ucUUIDType);
//    }
//    memset(&add_char_params, 0, sizeof(add_char_params));
//     /* Only 12 the and 13th byte of 16 byte uuid */
//    add_char_params.uuid              = 0xBE7E;
//    add_char_params.uuid_type         = ucUUIDType;
//    add_char_params.max_len           = sizeof(TKey_BYTE);
//    add_char_params.init_len          = 0;
//    add_char_params.p_init_value      = NULL;
//    add_char_params.char_props.read = 1;
//    add_char_params.char_props.write= 1;
//    add_char_params.char_props.write_wo_resp = 1;
//    add_char_params.read_access     = SEC_OPEN;
//    add_char_params.cccd_write_access = SEC_OPEN;
//    add_char_params.write_access = SEC_OPEN;
//    eNrfErrorCode = characteristic_add(muTabServiceHandle, &add_char_params, &msOPStatusCharHandle);
//    if (eNrfErrorCode != NRF_SUCCESS)
//    {
//        THINKEY_DEBUG_ERROR("Could not add OP status charecterictic");
//    }
//    /* Add Door Open Threshold Value characteristics---------------------------*/
//    /* First add the Vendor uuid to the table, and store the type value retunrned by the function
//    This value will be used along with 12th and 13th bytes of 16 bytes uuid for all later api calls */
//    ble_uuid128_t sDoorOpenThresholdValueUUID =
//    {
//        .uuid128 = {0x03, 0x1B, 0x18, 0x9E, 0x66, 0x15, 0xFF, 0x96, 0x21, 0x44, 0x48, 0x65, 0x8B, 0xE6, 0xA1, 0x9E}
//    };
//    eNrfErrorCode = sd_ble_uuid_vs_add(&sDoorOpenThresholdValueUUID, &ucUUIDType);
//    if (eNrfErrorCode != NRF_SUCCESS)
//    {
//        THINKEY_DEBUG_ERROR("could not add sDoorOpenThresholdValueUUID. API retruned: %x, type:%d ", eNrfErrorCode, ucUUIDType);
//    }
//    memset(&add_char_params, 0, sizeof(add_char_params));
//     /* Only 12 the and 13th byte of 16 byte uuid */
//    add_char_params.uuid              = 0xE68B;
//    add_char_params.uuid_type         = ucUUIDType;
//    add_char_params.max_len           = sizeof(TKey_UINT32);
//    add_char_params.init_len          = 0;
//    add_char_params.p_init_value      = NULL;
//    add_char_params.char_props.read = 1;
//    add_char_params.char_props.write= 1;
//    add_char_params.char_props.write_wo_resp = 1;
//    add_char_params.read_access     = SEC_OPEN;
//    add_char_params.cccd_write_access = SEC_OPEN;
//    add_char_params.write_access = SEC_OPEN;
//    eNrfErrorCode = characteristic_add(muTabServiceHandle, &add_char_params, &msDoorOpenThresholdValueCharHandle);
//    if (eNrfErrorCode != NRF_SUCCESS)
//    {
//        THINKEY_DEBUG_ERROR("Could not add Door Open Threshold Value charecterictic");
//    }
//
//    /* Add Start Engine Threshold Value characteristics------------------*/
//    /* First add the Vendor uuid to the table, and store the type value retunrned by the function
//    This value will be used along with 12th and 13th bytes of 16 bytes uuid for all later api calls */
//    ble_uuid128_t sStartEngineThresholdValueUUID =
//    {
//        .uuid128 = {0xA3, 0xE4, 0x8C, 0xB4, 0xC8, 0x10, 0x55, 0x9C, 0x8D, 0x4B, 0x75, 0xBF, 0x1F, 0x3E, 0x5B, 0xC8}
//    };
//    eNrfErrorCode = sd_ble_uuid_vs_add(&sStartEngineThresholdValueUUID, &ucUUIDType);
//    if (eNrfErrorCode != NRF_SUCCESS)
//    {
//        THINKEY_DEBUG_ERROR("could not add sStartEngineThresholdValueUUID. API retruned: %x, type:%d ", eNrfErrorCode, ucUUIDType);
//    }
//    memset(&add_char_params, 0, sizeof(add_char_params));
//     /* Only 12 the and 13th byte of 16 byte uuid */
//    add_char_params.uuid              = 0x3E1F;
//    add_char_params.uuid_type         = ucUUIDType;
//    add_char_params.max_len           = sizeof(TKey_UINT16);
//    add_char_params.init_len          = 0;
//    add_char_params.p_init_value      = NULL;
//    add_char_params.char_props.read = 1;
//    add_char_params.char_props.write= 1;
//    add_char_params.char_props.write_wo_resp = 1;
//    add_char_params.read_access     = SEC_OPEN;
//    add_char_params.cccd_write_access = SEC_OPEN;
//    add_char_params.write_access = SEC_OPEN;
//    eNrfErrorCode = characteristic_add(muTabServiceHandle, &add_char_params, &msStartEngineThresholdValueCharHandle);
//    if (eNrfErrorCode != NRF_SUCCESS)
//    {
//        THINKEY_DEBUG_ERROR("Could not add Door Open Threshold Value charecterictic");
//    }
}

static void advertising_init(void)
{
//    ret_code_t             eNrfErrorCode;
//    ble_advertising_init_t init;
//
//    memset(&init, 0, sizeof(init));
//    init.advdata.name_type               = BLE_ADVDATA_FULL_NAME;
//    init.advdata.include_appearance      = false;
//    init.advdata.flags                   = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
//    init.advdata.uuids_complete.uuid_cnt = 1;
//    /* Setting the UUID initially as for tab app service. Later once we start OP/PE advertisement
//       this adv data will be updated */
//    init.advdata.uuids_complete.p_uuids  = &mTabAppUUID;
//
//    init.config.ble_adv_fast_enabled  = true;
//    init.config.ble_adv_fast_interval = APP_ADV_INTERVAL;
//
//    init.evt_handler = on_adv_evt;
//
//    eNrfErrorCode = ble_advertising_init(&mDigitalKeyAdv, &init);
//    APP_ERROR_CHECK(eNrfErrorCode);
//    ble_advertising_conn_cfg_tag_set(&mDigitalKeyAdv, APP_BLE_CONN_CFG_TAG);
//    THINKEY_DEBUG_INFO("Advertisment config set with config tag:%d", APP_BLE_CONN_CFG_TAG);
}
/*
 *  PSOC BT stack init
 *  */
THINKey_eStatusType THINKey_BTAL_eBleStackInit()
{
    THINKey_DEBUG_TAG TAG ="BTAL";
    THINKey_eStatusType eRetStatus = E_THINKEY_SUCCESS;
//    THINKey_UINT32 uiTaskID;
//    do
//    {
//        if(THINKey_TRUE == bBtInitDone)
//        {
//            THINKEY_DEBUG_INFO("Stack Init Done. Just return success\r\n");
//            eRetStatus = E_THINKEY_SUCCESS;
//            break;
//        }
//        ble_stack_init();
//        THINKEY_DEBUG_INFO("ble_stack_init done");
//        gap_params_init();
//        THINKEY_DEBUG_INFO("gap_params_init done");
//        gatt_init();
//        THINKEY_DEBUG_INFO("gatt init done");
//        services_init();
//        THINKEY_DEBUG_INFO("services init done");
//        advertising_init();
//        THINKEY_DEBUG_INFO("advertising_init done");
//        conn_params_init();
//        THINKEY_DEBUG_INFO("connection param init done");
//
//
        vProcessQueue = THINKey_OSAL_hCreateQueue(BLE_QUEUE_LENGTH,
                                              sizeof(THINKey_sBleProcessEvents));
        THINKEY_DEBUG_INFO("THINKey_OSAL_hCreateQueue retruned");
//
//        eRetStatus = THINKey_OSAL_eCreateTask(BTAL_EVENT_TASK_NAME,
//                task_process_events,
//                THINKey_NULL,
//                THIKEY_BTAL_EVENT_PRIORITY,
//                THINKEY_BTAL_STACK_SIZE,
//                &uiTaskID);
//        THINKEY_DEBUG_INFO("BTAL THINKey_OSAL_eCreateTask retrined:%d", eRetStatus);
//        if (E_THINKEY_SUCCESS == eRetStatus)
//        {
//            THINKEY_DEBUG_INFO("%s:BTAL Task created\n", TAG);
//        }//TODO: check for failure
//
//        bBtInitDone = THINKey_TRUE;
//        eRetStatus = E_THINKEY_SUCCESS;
//    } while (THINKey_EXIT);
    return eRetStatus;
}

/*
 * Abstract function to register PSM value
 * */
THINKey_eStatusType THINKey_BTAL_eBleRegisterL2capPSM(THINKey_UINT32 uiL2capPSM)
{
    /* Note: Not used in Indie semi project */
    THINKey_eStatusType eRetStatus = E_THINKEY_SUCCESS;
    return eRetStatus;
}

/*
 * Abstract function to register start owner pairing advertisement
 * */
THINKey_eStatusType THINKey_BTAL_eBleStartAdvert(THINKey_ePairingType ePairingType)
{
    THINKey_eStatusType eRetStatus = E_THINKEY_FAILURE;
//    THINKEY_DEBUG_INFO("Enterting THINKey_BTAL_eBleStartAdvert:%d", ePairingType);
//    ret_code_t             eNrfErrorCode;
//    ble_gap_conn_sec_mode_t sec_mode;
//    do {
//        if (E_THINKEY_OWNER_PAIRING == ePairingType)
//        {
//            THINKEY_DEBUG_INFO("Starting Owner pairing advertisment");
//
//            BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);
//            eNrfErrorCode = sd_ble_gap_device_name_set(&sec_mode,
//                                          (const uint8_t *)OWNER_PAIRING_ADV_NAME,
//                                          strlen(OWNER_PAIRING_ADV_NAME));
//            APP_ERROR_CHECK(eNrfErrorCode);
//            ble_advdata_t advdata;
//            memset(&advdata, 0, sizeof(advdata));
//            uint32_t uiLen = BLE_GAP_ADV_SET_DATA_SIZE_MAX;
//            advdata.name_type               = BLE_ADVDATA_FULL_NAME;
//            advdata.include_appearance      = false;
//            advdata.flags                   = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
//            advdata.uuids_complete.uuid_cnt = 1;
//            advdata.uuids_complete.p_uuids  = &mDigitalKeyOPUUID;
//
//            eNrfErrorCode = ble_advertising_start(&mDigitalKeyAdv, BLE_ADV_MODE_FAST);
//            THINKEY_DEBUG_INFO("ble_advertising_start retruened: %d", eNrfErrorCode);
//            APP_ERROR_CHECK(eNrfErrorCode);
//            eNrfErrorCode = ble_advertising_advdata_update(&mDigitalKeyAdv, &advdata,NULL);
//            THINKEY_DEBUG_INFO("ble_advertising_advdata_update returned:%d", eNrfErrorCode);
//            APP_ERROR_CHECK(eNrfErrorCode);
//            eRetStatus = E_THINKEY_SUCCESS;
//        }
//        else if (E_THINKEY_PASSIVE_ENTRY == ePairingType)
//        {
//            THINKEY_DEBUG_INFO("Starting Passive entry advertisment");
//
//            BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);
//            eNrfErrorCode = sd_ble_gap_device_name_set(&sec_mode,
//                                          (const uint8_t *)PASSIVE_ENTRY_ADV_NAME,
//                                          strlen(PASSIVE_ENTRY_ADV_NAME));
//            APP_ERROR_CHECK(eNrfErrorCode);
//            ble_advdata_t advdata;
//            memset(&advdata, 0, sizeof(advdata));
//            uint32_t uiLen = BLE_GAP_ADV_SET_DATA_SIZE_MAX;
//            advdata.name_type               = BLE_ADVDATA_FULL_NAME;
//            advdata.include_appearance      = false;
//            advdata.flags                   = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
//            advdata.uuids_complete.uuid_cnt = 1;
//            advdata.uuids_complete.p_uuids  = &mDigitalKeyPEUUID;
//
//            eNrfErrorCode = ble_advertising_start(&mDigitalKeyAdv, BLE_ADV_MODE_FAST);
//            THINKEY_DEBUG_INFO("ble_advertising_start retruened: %d", eNrfErrorCode);
//            APP_ERROR_CHECK(eNrfErrorCode);
//            eNrfErrorCode = ble_advertising_advdata_update(&mDigitalKeyAdv, &advdata,NULL);
//            THINKEY_DEBUG_INFO("ble_advertising_advdata_update returned:%d", eNrfErrorCode);
//            APP_ERROR_CHECK(eNrfErrorCode);
//            eRetStatus = E_THINKEY_SUCCESS;
//        }
//        else if (E_THINKEY_TAB_APP_ADVERTISEMENT == ePairingType)
//        {
//            THINKEY_DEBUG_INFO("Starting Tab app advertisment");
//
//            eNrfErrorCode = ble_advertising_start(&mDigitalKeyAdv, BLE_ADV_MODE_FAST);
//            THINKEY_DEBUG_INFO("ble_advertising_start retruened: %d", eNrfErrorCode);
//            APP_ERROR_CHECK(eNrfErrorCode);
//            eRetStatus = E_THINKEY_SUCCESS;
//        }
//    } while(THINKey_EXIT);
    //edited :
    eRetStatus = E_THINKEY_SUCCESS;
    return eRetStatus;
}

THINKey_eStatusType THINKey_BTAL_SendData(THINKey_HANDLE hKeyTransportHandle,
        THINKey_BYTE* bDataBuffer, THINKey_UINT32 uiLength)
{
    THINKey_eStatusType eRetStatus = E_THINKEY_SUCCESS;
//    uint32_t              eNrfErrorCode;
//    ble_data_t sL2capData;
//    sL2capData.p_data = bDataBuffer;
//    sL2capData.len = uiLength;
//    THINKEY_DEBUG_INFO("Send data of length %d to transport %x",
//            uiLength, (THINKey_UINT32)hKeyTransportHandle);
//    eNrfErrorCode = sd_ble_l2cap_ch_tx(m_conn_handle[BLE_CONN_PHONE_HANDLE], m_l2cap_cid, &sL2capData);
//    if (NRF_SUCCESS != eNrfErrorCode)
//    {
//        eRetStatus = E_THINKEY_FAILURE;
//        THINKEY_DEBUG_ERROR("sd_ble_l2cap_ch_tx retruned: %d", eNrfErrorCode);
//    }
    return eRetStatus;
}

void SD_EVT_IRQHandler(void)
{
    BaseType_t yield_req = pdFALSE;
    THINKey_sBleProcessEvents sBleEvent;
    sBleEvent.eCommand = E_THINKEY_BLE_PROCESS_EVENTS;

    THINKey_OSAL_eQueueSendToFromISR(vProcessQueue, &sBleEvent,
                &yield_req);
    THINKEY_DEBUG_INFO("task reshedule required : %d",yield_req);
    /* Switch the task if required. */
    portYIELD_FROM_ISR(yield_req);
}

THINKey_VOID task_process_events (THINKey_VOID* param)
{
//    THINKey_eStatusType eRetStatus = E_THINKEY_SUCCESS;
//    ret_code_t eNrfErrorCode;
//    THINKey_sBleProcessEvents sBleEvent;
//    THINKEY_DEBUG_INFO("Polling for ealier sd events");
//    nrf_sdh_evts_poll(); /* let the handlers run first, incase the EVENT occured before creating this task */
//    for(;;)
//    {
//        /* Block until a BLE command has been received over bleQueueHandle */
//        eRetStatus = THINKey_OSAL_eQueueReceive(vProcessQueue, &sBleEvent);
//        if(E_THINKEY_SUCCESS == eRetStatus)
//        {
//            switch(sBleEvent.eCommand)
//            {
//                case E_THINKEY_BLE_PROCESS_EVENTS:
//                {
//                    THINKEY_DEBUG_INFO("Polling for sd events");
//                    nrf_sdh_evts_poll();
//                    break;
//                }
//                case E_THINKEY_SEND_NOTIFICATION:
//                {
//                    TKey_UINT16 uiDataLen = RANGING_DATA_MAX_LENGTH;
//                    TKey_BYTE * const ucData = (TKey_BYTE *)(sBleEvent.auiData);
//                    ble_gatts_hvx_params_t gattNotficationData =
//                    {
//                        .handle = msRangingDataCharHandle.value_handle,
//                        .type = BLE_GATT_HVX_NOTIFICATION,
//                        .offset = 0,
//                        .p_len = &uiDataLen,
//                        .p_data = ucData
//                    };
//                    THINKEY_DEBUG_ERROR("Calling sd_ble_gatts_hvx %d", gattNotficationData.handle);
//                    eNrfErrorCode = sd_ble_gatts_hvx(m_conn_handle[BLE_CONN_TAB_HANDLE], &gattNotficationData);
//                    THINKEY_DEBUG_ERROR("sd_ble_gatts_hvx returned:%d len:%d", eNrfErrorCode, *(gattNotficationData.p_len));
//                    break;
//                }
//                case E_THINKEY_TAB_SEND_STATUS_MESSAGE:
//                {
//                    THINKEY_DEBUG_INFO("E_THINKEY_TAB_SEND_STATUS_MESSAGE");
//                    TKey_UINT16 uiDataLen = STATUS_MESSAGE_MAX_LENGTH;
//                    TKey_BYTE * const ucData = (TKey_BYTE *)sBleEvent.auiData[0];
//                    ble_gatts_hvx_params_t gattNotficationData =
//                    {
//                        .handle = msStatusMessageCharHandle.value_handle,
//                        .type = BLE_GATT_HVX_NOTIFICATION,
//                        .offset = 0,
//                        .p_len = &uiDataLen,
//                        .p_data = ucData
//                    };
//                    THINKEY_DEBUG_ERROR("Calling sd_ble_gatts_hvx %d", gattNotficationData.handle);
//                    eNrfErrorCode = sd_ble_gatts_hvx(m_conn_handle[BLE_CONN_TAB_HANDLE], &gattNotficationData);
//                    THINKEY_DEBUG_ERROR("sd_ble_gatts_hvx returned:%d len:%d", eNrfErrorCode, *(gattNotficationData.p_len));
//                    break;
//                }
//            }
//        }
//    }
    return;
}

THINKey_eStatusType THINKey_BTAL_eBleStopAdvert() {

//    THINKey_eStatusType eRetStatus = E_THINKEY_SUCCESS;
//
//    sd_ble_gap_adv_stop(mDigitalKeyAdv.adv_handle);
//
//    return eRetStatus;
}

TKey_Status_t tkey_BTAL_BondDevice() {

    THINKey_eStatusType eRetStatus = E_THINKEY_SUCCESS;
	THINKey_sEventQueueMsgType sMessage;
	THINKey_sTransportSSHandleType *psTransportSSHandle = hGetTransportSSHandle();
    sMessage.eCommand = E_THINKEY_PAIR_AND_BOND_DONE;
	eRetStatus = THINKey_OSAL_eQueueSend(
					psTransportSSHandle->hTPEventQueue, &sMessage);
    return eRetStatus;
}

TKey_Status_t tkey_BTAL_PairDevice(TKey_BYTE ucBdHandle) {

    THINKey_eStatusType eRetStatus = E_THINKEY_SUCCESS;
//    ret_code_t eNrfErrorCode;
//        /* Set secuirity params */
//    msSecParams.bond = 1;
//    msSecParams.mitm = 0;
//    msSecParams.lesc = 0;
//    msSecParams.io_caps = BLE_GAP_IO_CAPS_NONE;
//    msSecParams.oob = 0;
//    msSecParams.min_key_size = 16;
//    msSecParams.max_key_size = 16;
//    msSecParams.keypress = 0;
//    /* Set secuirity key set */
//    msSecKeySet.keys_peer.p_enc_key = NULL;
//    msSecKeySet.keys_peer.p_id_key = &msPeerIdKey;
//    msSecKeySet.keys_peer.p_sign_key = &msPeerSignKey;
//    msSecKeySet.keys_peer.p_pk = &msPeerPublicKey;
//    msSecKeySet.keys_own.p_enc_key = &msLocalEncKey;
//    msSecKeySet.keys_own.p_id_key = &msLocalIdKey;
//    msSecKeySet.keys_own.p_sign_key = &msLocalSignKey;
//    msSecKeySet.keys_own.p_pk = &msLocalPublicKey;
//    eNrfErrorCode = sd_ble_gap_sec_params_reply(ucBdHandle, BLE_GAP_SEC_STATUS_SUCCESS,
//            &msSecParams, &msSecKeySet);
//    THINKEY_DEBUG_INFO("sd_ble_gap_sec_params_reply returned:%d", eNrfErrorCode);
//    if (NRF_SUCCESS == eNrfErrorCode) {
//        eRetStatus = E_THINKEY_FAILURE;
//    }
    return eRetStatus;
}
