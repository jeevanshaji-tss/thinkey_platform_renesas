#include "uci_port.h"
#include "uci_utils.h"
#include "uci_api.h"
#include "tloc_ble.h"
#include "r_sci_uart.h"
#include "thinkey_platform_types.h"
#include "thinkey_osal.h"
#include "thinkey_debug.h"
#include "thinkey_ral.h"
#include "FreeRTOS.h"
#include "hal_data.h"
#include "task.h "
void UwbLoop();

// for project ccc only
typedef struct {
    uint8_t seq_num[4];
    uint8_t session[4];
    uint8_t raw_distance[2];
    int8_t  raw_aoa_azimuth[2];
    uint8_t slot_idx;
    int8_t  rssi;
    int8_t  snr;
} range_data_aux_info_t;

#define CH_TAPS_NUM 30

#define START_RANGE 0x10
#define STOP_RANGE 0x20

typedef struct {
    float re_im[CH_TAPS_NUM][2];
    uint8_t raw_distance[2];
    uint8_t res[2];
} chtaps_data_t;

/****************************************************************************//**
 *
 *                                 APP global variables
 *
 *******************************************************************************/
static bool sched_uh_async = false;
static uint32_t time_to_sched_uh = UINT32_MAX;

uint8_t ccc_ursk[32] = {0xed, 0x07, 0xa8, 0x0d, 0x2b, 0xeb, 0x00, 0xf7, 0x85, 0xaf, 0x26, 0x27, 0xc9, 0x6a, 0xe7, 0xc1,
                        0x18, 0x50, 0x42, 0x43, 0xcb, 0x2c, 0x32, 0x26, 0xb3, 0x67, 0x9d, 0xaa, 0x0f, 0x7e, 0x61, 0x6c};
uci_app_cfg_t uci_app_cfg;
uci_core_cfg_t uci_core_cfg;
uwbs_t *p_uwbs = NULL;

/****************************************************************************//**
 *
 *                                 APP functions
 *
 *******************************************************************************/
int32_t uci_get_uwbs_id()
{
	return (p_uwbs!= NULL)?p_uwbs->id:0;
}

int32_t uci_get_bus_id()
{
	return (p_uwbs!= NULL)?p_uwbs->bus:0;
}

void uci_sched_me(void)
{
    sched_uh_async = true;
}

void uci_msg_callback(uwbs_t *uwbs, uci_cb_evt_t *evt)
{
    uint8_t param[100];
    static uint16_t uwLatestRange;
    static uint32_t uwLatestSeqNum;
    static uint8_t ucNLos;

	switch (evt->type)
    {
    case API_RET_NTF:
        {
            uci_api_ret_t *aret = &evt->pl.aret;
            SEGGER_RTT_printf(0,"%s uwbs:%d api async evt, name enum:%d status:0x%04lX\r\n",
                    __func__, uwbs->id, aret->whoami, aret->status);
            if (aret->status != STATUS_OK) {
                SEGGER_RTT_printf(0,"ERROR: check UWB chip and connection\r\n");
            	break;
            }

            switch(aret->whoami)
            {
                case UCI_DEV_RESET:
                {
                	// send core set config command
                    param[0] = UCI_DEV_CFG_LOW_POWER_MODE;
                    uci_core_set_config(uwbs, &param[0], 1);
                }
                break;

                case UIC_CORE_SET_CONFIG:
                {
                	//send session init command
                    uci_session_init(uwbs, SESSION_ID, RANGING_SESSION);
                }
            	break;

                case UCI_SESSION_INIT:
                {
                    SEGGER_RTT_printf(0,"session init....\r\n");
                	//
                	//send session set app config command
                    uint8_t index = 0;
                    param[index++] = UCI_APP_CFG_MULTI_NODE_MODE;
                    param[index++] = UCI_APP_CFG_CHANNEL_NUMBER;
                    param[index++] = UCI_APP_CFG_PREAMBLE_CODE_INDEX;

                    param[index++] = UCI_APP_CFG_AOA_RESULT_REQ;
                    /* The power is set from the Mk8000 in dec-6 release shouldn't set here */
                    //param[index++] = UCI_APP_CFG_TX_POWER_LEVEL;
                    param[index++] = UCI_APP_CFG_RX_ANT_ID;

                    param[index++] = UCI_APP_CFG_NUMBER_OF_CONTROLEES;
                    param[index++] = UCI_APP_CFG_SLOT_DURATION;
                    param[index++] = UCI_APP_CFG_RANGING_INTERVAL;
                    param[index++] = UCI_APP_CFG_SLOTS_PER_RR;
                    param[index++] = UCI_APP_CFG_RESPONDER_SLOT_INDEX;

                    param[index++] = UCI_APP_CFG_STS_INDEX;
                    param[index++] = UCI_APP_CFG_HOPPING_MODE;
                    param[index++] = UCI_APP_CFG_HOP_MODE_KEY;
                    param[index++] = UCI_APP_CFG_RANGING_PROTOCOL_VER;
                    param[index++] = UCI_APP_CFG_UWB_CONFIG_ID;
                    param[index++] = UCI_APP_CFG_PULSESHAPE_COMBO;

                    param[index++] = UCI_APP_CFG_DEVICE_TYPE;
                    param[index++] = UCI_APP_CFG_DEVICE_ROLE;

                    uci_session_set_app_config(uwbs, SESSION_ID, param, index);
                }
            	break;

                case  UCI_SESSION_DEINIT:
                {
                    SEGGER_RTT_printf(0,"session deinit....\r\n");
                }
                break;

                case UCI_GET_DEV_INFO:
                {
                	uci_get_dev_info_t *dinfo = &aret->param->dev_info;
                	SEGGER_RTT_printf(0,"ger_ver:[%d.%d], mac_ver:[%d.%d], phy_ver:[%d.%d], test_ver:[%d.%d]\r\n",
                            dinfo->major_ger_ver, dinfo->minor_ger_ver,
                            dinfo->major_mac_ver, dinfo->minor_mac_ver,
                            dinfo->major_phy_ver, dinfo->minor_phy_ver,
                            dinfo->major_test_ver, dinfo->minor_test_ver);
                }
                break;

                case UCI_CORE_GET_CONFIG:
                {
                    uci_get_cfg_rsp_t *grsp = &aret->param->get_rsp;
                    for (int32_t i = 0; i < grsp->num; i++)
                    {
                        uci_get_rsp_param_t *param = grsp->params + i;

                        uint32_t value = 0;
                        if (param->length == 1)
                        {
                            value = *param->value;
                        }
                        else if (param->length == 2)
                        {
                            value = uint16_decode(param->value);
                        }
                        else if (param->length == 4)
                        {
                            value = uint32_decode(param->value);
                        }
                        SEGGER_RTT_printf(0,"tag:[0x%02X], len:[%d], val:[%ld]\r\n", param->tag, param->length, value);
                    }

                }
                break;

                case UCI_SESSION_GET_APP_CFG:
                {
                    uci_get_cfg_rsp_t *grsp = &aret->param->get_rsp;
                    for (int32_t i = 0; i < grsp->num; i++)
                    {
                        uci_get_rsp_param_t *param = grsp->params + i;

                        uint32_t value = 0;
                        if (param->length == 1)
                        {
                            value = *param->value;
                        }
                        else if (param->length == 2)
                        {
                            value = uint16_decode(param->value);
                        }
                        else if (param->length == 4)
                        {
                            value = uint32_decode(param->value);
                        }
                        SEGGER_RTT_printf(0,"tag:[0x%02X], len:[%d], val:[%ld]\r\n", param->tag, param->length, value);

                        if(param->tag ==UCI_APP_CFG_DEVICE_MAC_ADDRESS )
                        {
                            SEGGER_RTT_printf(0,"get dev mac addr: %lx\r\n", value);
                    		uci_app_cfg.dev_mac_addr[0] = value&0xff;
                    		uci_app_cfg.dev_mac_addr[1] = (value>>8) & 0xff;
                        }
                    }
                }
                break;

                case UCI_SESSION_SET_APP_CFG:
                {
                    uci_set_cfg_rsp_t *srsp = &aret->param->set_rsp;
                    for (int32_t i = 0; i < srsp->num; i++)
                    {
                        uci_set_rsp_param_t *param = srsp->params + i;
                        SEGGER_RTT_printf(0,"tag:[0x%02X], status:[%d]\r\n", param->tag, param->status);
                    }

                    /* set ursk */
                    uci_vendor_set_ccc_ursk(uwbs, ccc_ursk, sizeof(ccc_ursk));
                }
                break;

                case UCI_VENDOR_SET_CCC_URSK:
                {
                    // send range start command
                    uci_range_start(uwbs, SESSION_ID);
                }
				break;

                case UCI_RANGE_START:
                {
                    SEGGER_RTT_printf(0,"ranging start....\r\n");
                }
            	break;

                case UCI_RANGE_STOP:
                {
                    SEGGER_RTT_printf(0,"ranging stop....\r\n");
                    uci_session_deinit(uwbs, SESSION_ID);
                }
            	break;

                default:
                {
                	break;
                }
            }
        }
    break;

    case RANGE_DATA_NTF:
        {
            uci_range_data_ntf_t *range_data = &evt->pl.range_data;
            SEGGER_RTT_printf(0,"\r\nuwbs:[%u]\r\n\tblock_idx:[%ld], session:[0x%lx]\r\n",
                    uwbs->id,
                    range_data->seq_num,
                    range_data->session);

            uci_meas_t *meas = range_data->meas;
            SEGGER_RTT_printf(0,"\tslot_idx:[%u], dist:%ucm, azimuth:%ddeg, aoa_fom:%u, NLoS:%u\r\n",
                    meas->slot_idx,
                    meas->distance,
                    meas->aoa_azimuth,
                    meas->aoa_azimuth_fom,
                    meas->nLoS);
            uwLatestRange = meas->distance;
            uwLatestSeqNum = range_data->seq_num;
            if (3 == meas->nLoS) {
                ucNLos = 1;
            } else {
                ucNLos = 0;
            }
        }
    break;

        // for project ccc only
    case VENDOR_DATA_NTF:
        {
            uci_vendor_data_ntf_t *vnd_ntf = &evt->pl.vendor_data;
            range_data_aux_info_t *rdai = (range_data_aux_info_t *)vnd_ntf->data;
            chtaps_data_t *ch_d = (chtaps_data_t *) vnd_ntf->data;

            if (vnd_ntf->bytes == sizeof(range_data_aux_info_t))
            {
                SEGGER_RTT_printf(0,"\r\nuwbs:[%u] \r\n\tblock_idx:[%ld], session:[0x%lx]\r\n",
                    uwbs->id,
                    uint32_decode(rdai->seq_num),
                    uint32_decode(rdai->session));

                SEGGER_RTT_printf(0,"\tslot_idx:[%u], raw_distance:%ucm, raw_azimuth:%ddeg, RSSI:%ddBm, SNR:%ddB\r\n",
                    rdai->slot_idx,
                    uint16_decode(rdai->raw_distance),
                    (int16_t)uint16_decode(rdai->raw_aoa_azimuth),
                    rdai->rssi,
                    rdai->snr);
                uint16_t uwRawDistance = uint16_decode(rdai->raw_distance);
               //sendData(uwLatestSeqNum, uwLatestRange, rdai->rssi, ucNLos, uwRawDistance, rdai->snr);
            }
            else if (vnd_ntf->bytes == sizeof(chtaps_data_t))
            {
                SEGGER_RTT_printf(0,"CIB Data:\r\n");
                /* If we print the whole CIR data here, the UWB data stop coming after some time. Maybe becuase we are
                   looping UWB and Btal one by one is adding some delay and some time stamp check maybe getting failed
                   in the uci library. Commenting this printing for now */
#if 0
                for (uint8_t i = 0; i < CH_TAPS_NUM; i++)
                {
                    printf("\t%f:%f\r\n", ch_d->re_im[i][0], ch_d->re_im[i][1]);
                }
#endif
            }
            else
            {
                SEGGER_RTT_printf(0,"\tdata length error, check UCI Host if compatible with UWBS's firmware\r\n");
            }
        }
    break;

    default:
        SEGGER_RTT_printf(0,"uwbs:[%d] evt not handle:[%d]\r\n", uwbs->id, evt->type);
    break;
    }
}

void uwb_ranging_start(void)
{
    // set ranging role based on GPIO setting
    // gpio low:	UWB_ROLE_TAG
	// gpio high: 	UWB_ROLE_ANCHOR
	//uint8_t ranging_role = gpio_read(GPIO_RANGING_ROLE)?ROLE_RESPONDER:ROLE_INITIATOR;
	uint8_t ranging_role =ROLE_RESPONDER;
    // set uwbs id based on GPIO setting
	//uint8_t uwbs_id = gpio_read(GPIO_RESPONER_ID_PIN_0) | (gpio_read(GPIO_RESPONER_ID_PIN_1)<<1);
	uint8_t uwbs_id = 0;
    uci_host_set_timeout(200);
    uci_host_set_flush_switch(true);

	//init core config
    UCI_API_CORE_CFG_INIT(uci_core_cfg);

    //init uci app config
	UCI_API_APP_CFG_INIT(uci_app_cfg);

    uci_app_cfg.multi_node_mode = BROADCAST;
    uci_app_cfg.chl_num = CHL_NUM_9;
    uci_app_cfg.preamble_ci = BPRF_PREAMBLE_CI_9;

    uci_app_cfg.aoa_req = AOA_REQ_ENABLE;
    uci_app_cfg.tx_pwr_level = 15;
    uci_app_cfg.rx_ant_id = 3;

    uci_app_cfg.controlees = MAX_UWBS;
    uci_app_cfg.slot_duration = 3 * 1200;
    uci_app_cfg.ranging_intv = 96;

#if (MAX_UWBS <= 2)
    uci_app_cfg.slots_per_rr = 6;
#elif (MAX_UWBS <= 4)
    uci_app_cfg.slots_per_rr = 8;
#elif (MAX_UWBS <= 8)
    uci_app_cfg.slots_per_rr = 12;
#elif (MAX_UWBS <= 10)
    uci_app_cfg.slots_per_rr = 16;
#else
    #error MAX_UWBS invalid
#endif

#if 1
    uci_app_cfg.hopping_mode = HOPPING_MODE_DISABLE;
#else
    uci_app_cfg.hopping_mode = HOPPING_MODE_CONT_AES_SEQ;
#endif

    uci_app_cfg.sts_idx = 123456789;
    uci_app_cfg.hop_mode_key = 0x87654321;
    uci_app_cfg.ranging_proto_ver = 0x0100;
    uci_app_cfg.uwb_cfg_id = 0x0001;
    uci_app_cfg.pulseshape_combo = 0x00;

	if(ranging_role == ROLE_RESPONDER) 		//anchor
	{
		uci_app_cfg.dev_type = TYPE_CONTROLLEE;
		uci_app_cfg.dev_role = ROLE_RESPONDER;
		uci_app_cfg.rspdr_slot_idx = UCI_UWBS_ID_BASE+uwbs_id;
	}else {									//tag
		uci_app_cfg.dev_type = TYPE_CONTROLLER;
		uci_app_cfg.dev_role = ROLE_INITIATOR;
		uci_app_cfg.rspdr_slot_idx = UCI_UWBS_ID_BASE;
	}

	uci_app_cfg.controlees = MAX_UWBS;

    //create uci uwb instance
	p_uwbs = uci_create_uwbs(1, uci_app_cfg.rspdr_slot_idx, MAX_UCI_RX_BUF_NUM, &uci_core_cfg, &uci_app_cfg, uci_msg_callback);

	SEGGER_RTT_printf(0,"set ranging role to %s\r\n", (ranging_role==ROLE_INITIATOR)?"TAG":"ANCHOR");
    SEGGER_RTT_printf(0,"set uwb id to %ld\r\n", uci_get_uwbs_id());

    // kick off the first uci command
    uci_core_device_reset(p_uwbs, 0);

    while(true){
        UwbLoop();

    }
    return;
}

void UwbLoop() {
#if 0
    if (sched_uh_async || (time_to_sched_uh <= uci_get_tick()))
    {
        sched_uh_async = false;

        uint32_t ttw = uci_host_loop();

        if (ttw != UINT32_MAX)
        {
            time_to_sched_uh = ttw + uci_get_tick();
        }
        else
        {
            time_to_sched_uh = UINT32_MAX;
        }
    }
#else if
    uci_host_loop();
    delay_ms(1);
#endif
}

/*******************************************************************************
* Function Name: main
********************************************************************************
* Summary:
* This is the main function for CM4 CPU.
* Reads one byte from the serial terminal and echoes back the read byte.
*
* Parameters:
*  void
*
* Return:
*  int
*
*******************************************************************************/
void UwbInit(void)
{
    /* Initialize the device and peripherals */
    hal_init();

    SEGGER_RTT_printf(0,"\r\n");
    SEGGER_RTT_printf(0,"**************************\r\n");
    SEGGER_RTT_printf(0,"*      MK UWB CCC Demo   *\r\n");
    SEGGER_RTT_printf(0,"**************************\r\n");

    SEGGER_RTT_printf(0,"uci_host ver:[%s]\r\n", uci_host_version());

    /* reset MK8000 */
    uwb_reset();
}

void UwbStart()
{
    /* init and start uci ranging test*/
    uwb_ranging_start();
}

// TODO: BELOW FUNCTIONS ARE ADDED
THINKey_eStatusType THINKey_eRangingInit(){

    fsp_err_t err = FSP_SUCCESS;
    THINKey_UINT32 uiStatus = E_THINKEY_SUCCESS;
    THINKEY_DEBUG_INFO("Ranging Initialisation started");
    err = R_SCI_UART_Open (&g_uart1_ctrl, &g_uart1_cfg);
    return uiStatus;

}

TKey_StatusType_t THINKey_RAL_setUrsk(TKey_BYTE *psUrsk) {
    SEGGER_RTT_printf(0,"THINKey_setUrsk\r\n");
    return E_TKEY_SUCCESS;
}

THINKey_eStatusType THINKey_eRangingStart(THINKey_HANDLE hKeyHandle, THINKey_pfnRangingCB pfnRangingCB)
{
    THINKey_eStatusType eStatus = E_THINKEY_SUCCESS;
    fsp_err_t err =FSP_SUCCESS;
    THINKEY_DEBUG_INFO("...START RANGING...");
    err= R_SCI_UART_Write(&g_uart1_ctrl, START_RANGE, sizeof(START_RANGE));

    return eStatus;
}

TKey_StatusType_t THINKey_RangingStop() {
    TKey_StatusType_t eStatus = E_THINKEY_SUCCESS;
    THINKEY_DEBUG_INFO("...STOP RANGING...");
    fsp_err_t err =FSP_SUCCESS;
    err= R_SCI_UART_Write(&g_uart1_ctrl, STOP_RANGE, sizeof(STOP_RANGE));

    return eStatus;
}
/* [] END OF FILE */
