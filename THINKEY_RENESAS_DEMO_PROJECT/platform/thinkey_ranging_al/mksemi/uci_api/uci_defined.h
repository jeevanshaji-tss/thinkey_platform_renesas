
/**
 * @file    uci_defined.h
 * @brief   Error codes, events, parameters, etc. defined by FiRa UCI
 * @author  Mauna Kea Semiconductor
 * @copyright   Copyright (c) 2019-2022 Mauna Kea Semiconductor Holdings.
 *              All rights reserved.
 * @date    2022-7-14
 * @ingroup uci_api
 * @{
 */

#ifndef _UCI_DEFINED_H_
#define _UCI_DEFINED_H_

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "uci_cfg.h"

/**
 * @brief   UCI standard error codes
 */
typedef enum
{
    /* Generic Status Codes */
    STATUS_OK                                 = 0x00,   /*!< Success */
    STATUS_REJECTED                           = 0x01,   /*!< Intended operation is not supported in the current state */
    STATUS_FAILED                             = 0x02,   /*!< Intended operation is failed to complete */
    STATUS_SYNTAX_ERROR                       = 0x03,   /*!< UCI packet structure is not per spec */
    STATUS_INVALID_PARAM                      = 0x04,   /*!< Config ID is not correct, and it is not present in UWBS */
    STATUS_INVALID_RANGE                      = 0x05,   /*!< Config ID is correct, and value is not in proper range */
    STATUS_INVALID_MESSAGE_SIZE               = 0x06,   /*!< UCI packet payload size is not as per spec */
    STATUS_UNKNOWN_GID                        = 0x07,   /*!< UCI Group ID is not per spec */
    STATUS_UNKNOWN_OID                        = 0x08,   /*!< UCI Opcode ID is not per spec */
    STATUS_READ_ONLY                          = 0x09,   /*!< Config ID is read-only */
    STATUS_COMMAND_RETRY                      = 0x0A,   /*!< UWBS request retransmission from Host */
    /* 0x0B - 0x0F For RFU */

    /* UWB Session Specific Status Codes */
    STATUS_ERROR_SESSION_NOT_EXIST            = 0x11,   /*!< Session is not existing or not created */
    STATUS_ERROR_SESSION_DUPLICATE            = 0x12,   /*!< Session is already created/exist */
    STATUS_ERROR_SESSION_ACTIVE               = 0x13,   /*!< Session is active */
    STATUS_ERROR_MAX_SESSIONS_EXCEEDED        = 0x14,   /*!< Max. number of sessions already created */
    STATUS_ERROR_SESSION_NOT_CONFIGURED       = 0x15,   /*!< Session is not configured with required app configurations */
    STATUS_ERROR_ACTIVE_SESSIONS_ONGOING      = 0x16,   /*!< Sessions are actively running in UWBS */
    STATUS_ERROR_MULTICAST_LIST_FULL          = 0x17,   /*!< Indicates when multicast list is full during one to many ranging */
    STATUS_ERROR_ADDRESS_NOT_FOUND            = 0x18,   /*!< Indicates when short address is not available multicast list */
    STATUS_ERROR_ADDRESS_ALREADY_PRESENT      = 0x19,   /*!< Indicates when short address is already present */
    /* 0x20 - 0x1F For RFU */

    /* UWB Ranging Session Specific Status Codes */
    STATUS_RANGING_TX_FAILED                  = 0x20,   /*!< Failed to transmit UWB packet */
    STATUS_RANGING_RX_TIMEOUT                 = 0x21,   /*!< No UWB packet detected by the receiver */
    STATUS_RANGING_RX_PHY_DEC_FAILED          = 0x22,   /*!< UWB packet channel decoding error */
    STATUS_RANGING_RX_PHY_TOA_FAILED          = 0x23,   /*!< Failed to detect time of arrival of the UWB packet from CIR samples */
    STATUS_RANGING_RX_PHY_STS_FAILED          = 0x24,   /*!< UWB packet STS segment mismatch */
    STATUS_RANGING_RX_MAC_DEC_FAILED          = 0x25,   /*!< MAC CRC or syntax error */
    STATUS_RANGING_RX_MAC_IE_DEC_FAILED       = 0x26,   /*!< IE syntax error */
    STATUS_RANGING_RX_MAC_IE_MISSING          = 0x27,   /*!< Expected IE missing in the packet */

    /* Proprietary Status Codes */
    /* 0x50 - 0xFF for Vendor Specific Status Codes */
} uci_status_e;

/**
 * @brief   Multicast update results
 */
typedef enum
{
    MULTICAST_LIST_UPDATE_OK    = 0,      /*!< update completed */
    MULTICAST_LIST_FULL         = 1,      /*!< multicast list is full */
    KEY_FETCH_FAIL              = 2,      /*!< Failed to obtain the key corresponding to sub_session */
    SUB_SESSION_ID_NOT_FOUND    = 3,      /*!< sub_session does not exist */
    /* 0x04 - 0x1F for RFU */
} uci_mc_upd_status_e;

/**
 * @brief   Session state
 */
typedef enum
{
    SESSION_STATE_INIT       = 0,       /*!< Session has been initialized */
    SESSION_STATE_DEINIT     = 1,       /*!< Session has been deinitialized */
    SESSION_STATE_ACTIVE     = 2,       /*!< Session is active */
    SESSION_STATE_IDLE       = 3,       /*!< Session is idle */
    /* 0x04 - 0xFF for RFU */
} uci_sess_state_e;

/**
 * @brief   Reason codes for session state change
 */
typedef enum
{
    SESSION_MANAGEMENT_COMMANDS               = 0,      /*!< Changed by UCI command */
    MAX_RANGING_ROUND_RETRY_COUNT_REACHED     = 1,      /*!< Number of retries to reach the specified value */
    MAX_NUMBER_OF_MEASUREMENTS_REACHED        = 2,      /*!< Number of measurements to reach the specified value */
    /* 0x03 - 0x1F for RFU */
    SLOT_LENGTH_NOT_SUPPORTED                 = 0x20,   /*!< Slot length not supported */
    INSUFFICIENT_SLOTS_PER_RR                 = 0x21,   /*!< Slots per ranging round is not enough to complete ranging */
    MAC_ADDRESS_MODE_NOT_SUPPORTED            = 0x22,   /*!< Address mode not supported */
    INVALID_RANGING_INTERVAL                  = 0x23,   /*!< Ranging interval not supported */
    INVALID_STS_CONFIG                        = 0x24,   /*!< STS is illegal */
    INVALID_RFRAME_CONFIG                     = 0x25,   /*!< RFRAME is illegal */
    /* 0x26 - 0x7F for RFU */
    /* 0x80 - 0xFF for Reserved for Vendor Specific use */
} uci_sess_st_chg_reason_code_e;

/**
 * @brief   UWBS Status
 */
typedef enum
{
    /* 0x00 RFU */
    DEVICE_STATE_READY      = 1,        /*!< Ready */
    DEVICE_STATE_ACTIVE     = 2,        /*!< Active */
    /* 0x03 - 0xFE for RFU */
    DEVICE_STATE_ERROR      = 0xFF,     /*!< Error */
} uci_dev_state_e;

/**
 * @brief   Low power mode, @ref UCI_DEV_CFG_DEVICE_STATE
 */
typedef enum
{
    LP_MODE_DISABLE     = 0,    /*!< disable */
    LP_MODE_ENABLE,             /*!< enable */
} uci_lp_mode_e;

/**
 * @brief   Device type, @ref UCI_APP_CFG_DEVICE_TYPE
 */
typedef enum
{
    TYPE_CONTROLLEE     = 0,    /*!< controlee, receive RCM */
    TYPE_CONTROLLER,            /*!< controler, send RCM */
} uci_dev_type_e;

/**
 * @brief   Ranging mode, @ref UCI_APP_CFG_RANGING_ROUND_USAGE
 */
typedef enum
{
    SS_TWR_DEFER        = 1,    /*!< defered SS_TWR */
    DS_TWR_DEFER,               /*!< defered DS_TWR */
    SS_TWR_NON_DEFER,           /*!< non-defered SS_TWR */
    DS_TWR_NON_DEFER,           /*!< non-defered DS_TWR */
} uci_ranging_mode_e;

/**
 * @brief   STS mode, @ref UCI_APP_CFG_STS_CONFIG
 */
typedef enum
{
    STS_STATIC      = 0,        /*!< Static, incremented for each slot, reset to 0 for each ranging cycle */
    STS_DYNAMIC,                /*!< Dynamic, incrementing for each slot */
    STS_DYNAMIC_RSPNDR_KYE,     /*!< Dynamic, associated with each responder's sub_session Key */
} uci_sts_cfg_e;

/**
 * @brief   Ranging mode, @ref UCI_APP_CFG_MULTI_NODE_MODE
 */
typedef enum
{
    UNICAST         = 0,        /*!< 1 on 1 */
    BROADCAST,                  /*!< 1 to many */
    MULTICAST,                  /*!< many to many */
} uci_multi_node_mode_e;

/**
 * @brief   Channel number, @ref UCI_APP_CFG_CHANNEL_NUMBER
 */
typedef enum
{
    CHL_NUM_5       = 5,
    CHL_NUM_6       = 6,
    CHL_NUM_8       = 8,
    CHL_NUM_9       = 9,
    CHL_NUM_10      = 10,
    CHL_NUM_12      = 12,
    CHL_NUM_13      = 13,
    CHL_NUM_14      = 14,
} uci_chl_num_e;

/**
 * @brief   FCS type, @ref UCI_APP_CFG_MAC_FCS_TYPE
 */
typedef enum
{
    FCS_CRC_16      = 1,
    FCS_CRC_32,
} uci_fcs_type_e;

/**
 * @brief   Ranging Control, @ref UCI_APP_CFG_RANGING_ROUND_CONTROL
 */
typedef enum
{
    SETUP_RESULT_RPT_MSG    = 0x01,     /*!< Ranging Result Report Message (RRRM) */
    SETUP_CTRL_MSG          = 0x02,     /*!< Control Message (CM) */
    SETUP_MEAS_RPT_MSG      = 0x80,     /*!<  Measurement Report Message (MRM) */
} uci_ranging_round_ctrl_e;

/**
 * @brief AOA function control, @ref UCI_APP_CFG_AOA_RESULT_REQ
 */
typedef enum
{
    AOA_REQ_DISABLE = 0,
    AOA_REQ_ENABLE,
} uci_aoa_req_e;

/**
 * @brief   Ranging data notification control, @ref UCI_APP_CFG_RANGE_DATA_NTF_CONFIG
 */
typedef enum
{
    RANGING_DATA_NTF_DISABLE    = 0,    /*!< disable */
    RANGING_DATA_NTF_ENABLE,            /*!< enable */
    RANGING_DATA_NTF_PROX_RANGE,        /*!< Determined by @ref UCI_APP_CFG_RANGE_DATA_NTF_PROXIMITY_NEAR and @ref UCI_APP_CFG_RANGE_DATA_NTF_PROXIMITY_FAR */
} uci_raning_data_ntf_e;

/**
 * @brief   device role, @ref UCI_APP_CFG_DEVICE_ROLE
 */
typedef enum
{
    ROLE_RESPONDER  = 0,
    ROLE_INITIATOR,
} uci_dev_role_e;

/**
 * @brief   RFRAME type, @ref UCI_APP_CFG_RFRAME_CONFIG
 */
typedef enum
{
    RFRAME_SP0      = 0,
    RFRAME_SP1,
    RFRAME_RFU,
    RFRAME_SP3,
} uci_rframe_type_e;

/**
 * @brief   preamble code index, @ref UCI_APP_CFG_PREAMBLE_CODE_INDEX
 */
typedef enum
{
    BPRF_PREAMBLE_CI_9      = 9,
    BPRF_PREAMBLE_CI_10,
    BPRF_PREAMBLE_CI_11,
    BPRF_PREAMBLE_CI_12,
    HPRF_PREAMBLE_CI_25     = 25,
    HPRF_PREAMBLE_CI_26,
    HPRF_PREAMBLE_CI_27,
    HPRF_PREAMBLE_CI_28,
    HPRF_PREAMBLE_CI_29,
    HPRF_PREAMBLE_CI_30,
    HPRF_PREAMBLE_CI_31,
    HPRF_PREAMBLE_CI_32,
} uci_preamble_ci_e;

/**
 * @brief   SFD type, @ref UCI_APP_CFG_SFD_ID
 */
typedef enum
{
    BPRF_SFD_0      = 0,
    BPRF_SFD_2      = 2,
    HPRF_SFD_1      = 1,
    HPRF_SFD_2,
    HPRF_SFD_3,
    HPRF_SFD_4,
} uci_sfd_id_e;

/**
 * @brief   PSDU data rate, @ref UCI_APP_CFG_PSDU_DATA_RATE
 */
typedef enum
{
    PSDU_DATA_RATE_6M8  = 0,
    PSDU_DATA_RATE_7M8,
    PSDU_DATA_RATE_27M2,
    PSDU_DATA_RATE_31M2,
    PSDU_DATA_RATE_850K,
} uci_psdu_data_rate_e;

/**
 * @brief   PSR symbol length, @ref UCI_APP_CFG_PREAMBLE_DURATION
 */
typedef enum
{
    PSR_SYMBOLS_32  = 0,
    PSR_SYMBOLS_64,
} uci_preamble_dura_e;

/**
 * @brief   Ranging timing structure, @ref UCI_APP_CFG_RANGING_TIME_STRUCT
 */
typedef enum
{
    INTERVAL_BASED_SCHE = 0,
    BLOCK_BASED_SCHE,
} uci_ranging_tme_stru_e;

/**
 * @brief   Transmit power adaptation, @ref UCI_APP_CFG_TX_ADAPTIVE_PAYLOAD_POWER
 */
typedef enum
{
    TX_ADAP_PWR_DISABLE = 0,
    TX_ADAP_PWR_ENABLE,
} uci_tx_adap_pwr_e;

/**
 * @brief   PRF mode, @ref UCI_APP_CFG_PRF_MODE
 */
typedef enum
{
    BPRF_PRF_62M4   = 0,
    HPRF_PRF_124M8,
    HPRF_PRF_249M6,
} uci_prf_mode_e;

/**
 * @brief   Ranging scheduling mode, @ref UCI_APP_CFG_SCHEDULED_MODE
 */
typedef enum
{
    TIME_SCHE_RANGING   = 1,
} uci_sche_mode_e;

/**
 * @brief   key rotaion, @ref UCI_APP_CFG_KEY_ROTATION
 */
typedef enum
{
    KEY_ROTATION_DISABLE = 0,
    KEY_ROTATION_ENABLE,
} uci_key_rotation_e;

/**
 * @brief   MAC Addr mode, @ref UCI_APP_CFG_MAC_ADDRESS_MODE
 */
typedef enum
{
    MAC_2BYTES_MHR_2BYTES = 0,      /*!< MAC address is 2 bytes and 2 bytes to be used in MAC header */
    MAC_2BYTES_MHR_8BYTES = 2,      /*!< MAC address is 8 bytes and 8 bytes to be used in MAC header */
} uci_mac_addr_mode_e;

/**
 * @brief STS segments, @ref UCI_APP_CFG_NUMBER_OF_STS_SEGMENTS
 */
typedef enum
{
    STS_SEGMENTS_NO = 0,
    STS_SEGMENTS_1,
    STS_SEGMENTS_2,
    STS_SEGMENTS_3,
    STS_SEGMENTS_4,
} uci_sts_segments_e;

/**
 * @brief   hopping mode, @ref UCI_APP_CFG_HOPPING_MODE
 */
typedef enum
{
    HOPPING_MODE_DISABLE        = 0,    /*!< NO hopping */
    HOPPING_MODE_FIRA,                  /*!< Hopping in the way defined by FiRa*/
    HOPPING_MODE_CONT_DEF_SEQ   = 0xA0, /*!< CCC continuous hopping with default sequence */
    HOPPING_MODE_ADAP_DEF_SEQ,          /*!< CCC adaptive hopping with default sequence */
    HOPPING_MODE_CONT_AES_SEQ,          /*!< CCC continuous hopping with aes sequence */
    HOPPING_MODE_ADAP_AES_SEQ,          /*!< CCC adaptive hopping with aes sequence */
} uci_hopping_mode_e;

/**
 * @brief RRRM report type, @ref UCI_APP_CFG_RESULT_REPORT_CONFIG
 */
typedef enum
{
    TOF_REPORT          = 0x01,
    AOA_AZIMUTH_RPT     = 0x02,
    AOA_ELEVATION_RPT   = 0x04,
    AOA_FOM_RPT         = 0x08,
} uci_rrrm_rpt_cfg_e;

/**
 * @brief   PHR datarate, @ref UCI_APP_CFG_BPRF_PHR_DATA_RATE
 */
typedef enum
{
    BPHR_PHR_DR_850K    = 0,
    BPHR_PHR_DR_6M8,
} uci_bphr_phr_dr_e;

/**
 * @brief   STS symbol length, @ref UCI_APP_CFG_STS_LENGTH
 */
typedef enum
{
    STS_LEN_SYMBOL_32   = 0,
    STS_LEN_SYMBOL_64,
    STS_LEN_SYMBOL_128,
} uci_sts_lenght_e;

/**
 * @brief multicast list update type
 */
typedef enum
{
    ADD_CONTROLLES      = 0,
    DELETE_CONTROLLES,
} uci_upd_mc_act_e;

/**
 * @brief   session type
 */
typedef enum
{
    RANGING_SESSION = 0,
    DEV_TEST_MODE   = 0xD0,
} uci_session_type_e;

/**
 * @brief   Device Configuration Parameters
 */
typedef enum
{
    UCI_DEV_CFG_DEVICE_STATE        = 0x00,   /*!< Device State, read only, @ref uci_dev_state_e */
    UCI_DEV_CFG_LOW_POWER_MODE      = 0x01,   /*!< enable/disable the low power mode, @ref uci_lp_mode_e */
    /* 0x02 0x09F for RFU */
    /* 0xA0 - 0xDF for Reserved for Vendor specific */
    /* 0xE0 - 0xE2 for extension of IDs */
    /* 0xE3 - 0xFF for Vendor Specific use */
} uci_dev_cfg_tag_e;

/**
 * @brief   Application Configuration Parameters
 */
typedef enum
{
    UCI_APP_CFG_DEVICE_TYPE                     = 0x00, /*!< @ref uci_dev_type_e */
    UCI_APP_CFG_RANGING_ROUND_USAGE             = 0x01, /*!< @ref uci_ranging_mode_e */
    UCI_APP_CFG_STS_CONFIG                      = 0x02, /*!< @ref uci_sts_cfg_e */
    UCI_APP_CFG_MULTI_NODE_MODE                 = 0x03, /*!< @ref uci_multi_node_mode_e*/
    UCI_APP_CFG_CHANNEL_NUMBER                  = 0x04, /*!< @ref uci_chl_num_e */
    UCI_APP_CFG_NUMBER_OF_CONTROLEES            = 0x05, /*!< Number of controlees(N) 1<=N<=8 */
    UCI_APP_CFG_DEVICE_MAC_ADDRESS              = 0x06, /*!< Mac address of the UWBS itself participating in UWB session, 2 or 8 bytes */
    UCI_APP_CFG_DST_MAC_ADDRESS                 = 0x07, /*!< MAC Address list(N) for NUMBER_OF_CONTROLEES devices participating in UWB Session */
    UCI_APP_CFG_SLOT_DURATION                   = 0x08, /*!< Unsigned integer that specifies duration of a ranging slot in the unit of RSTU */
    UCI_APP_CFG_RANGING_INTERVAL                = 0x09, /*!< Ranging interval in the unit of 1200 RSTU which is 1 ms between beginning of one ranging round to the beginning of the next */
    UCI_APP_CFG_STS_INDEX                       = 0x0A, /*!< Test for FiRa, sts_index0 for CCC */
    UCI_APP_CFG_MAC_FCS_TYPE                    = 0x0B, /*!< @ref uci_fcs_type_e */
    UCI_APP_CFG_RANGING_ROUND_CONTROL           = 0x0C, /*!< @ref uci_ranging_round_ctrl_e */
    UCI_APP_CFG_AOA_RESULT_REQ                  = 0x0D, /*!< @ref uci_aoa_req_e */
    UCI_APP_CFG_RANGE_DATA_NTF_CONFIG           = 0x0E, /*!< @ref uci_raning_data_ntf_e */
    UCI_APP_CFG_RANGE_DATA_NTF_PROXIMITY_NEAR   = 0x0F, /*!< sets the lower bound in cm above which the ranging notifications should automatically */
    UCI_APP_CFG_RANGE_DATA_NTF_PROXIMITY_FAR    = 0x10, /*!< sets the upper bound in cm above which the ranging notifications should automatically */
    UCI_APP_CFG_DEVICE_ROLE                     = 0x11, /*!< @ref uci_dev_role_e */
    UCI_APP_CFG_RFRAME_CONFIG                   = 0x12, /*!< @ref uci_rframe_type_e */
    UCI_APP_CFG_PREAMBLE_CODE_INDEX             = 0x14, /*!< @ref uci_preamble_ci_e */
    UCI_APP_CFG_SFD_ID                          = 0x15, /*!< @ref uci_sfd_id_e */
    UCI_APP_CFG_PSDU_DATA_RATE                  = 0x16, /*!< @ref uci_psdu_data_rate_e */
    UCI_APP_CFG_PREAMBLE_DURATION               = 0x17, /*!< @ref uci_preamble_dura_e */
    UCI_APP_CFG_RANGING_TIME_STRUCT             = 0x1A, /*!< @ref uci_ranging_tme_stru_e */
    UCI_APP_CFG_SLOTS_PER_RR                    = 0x1B, /*!< Number of slots for per ranging round */
    UCI_APP_CFG_TX_ADAPTIVE_PAYLOAD_POWER       = 0x1C, /*!< @ref uci_tx_adap_pwr_e */
    UCI_APP_CFG_RESPONDER_SLOT_INDEX            = 0x1E, /*!< used to choose Responder index in Two Way Ranging */
    UCI_APP_CFG_PRF_MODE                        = 0x1F, /*!< @ref uci_prf_mode_e */
    UCI_APP_CFG_SCHEDULED_MODE                  = 0x22, /*!< @ref uci_sche_mode_e */
    UCI_APP_CFG_KEY_ROTATION                    = 0x23, /*!< @ref uci_key_rotation_e */
    UCI_APP_CFG_KEY_ROTATION_RATE               = 0x24, /*!< Key rotation rate parameter defines n, with 2^n being the rotation rate of some keys used during Dynamic STS Ranging, where n is in the range of 0<=n<=15 */
    UCI_APP_CFG_SESSION_PRIORITY                = 0x25, /*!< Priority value for a Session */
    UCI_APP_CFG_MAC_ADDRESS_MODE                = 0x26, /*!< @ref uci_mac_addr_mode_e */
    UCI_APP_CFG_VENDOR_ID                       = 0x27, /*!< Unique ID for vendor. Unique ID for vendor. This parameter is used to set vUpper64[15:0] for static STS */
    UCI_APP_CFG_STATIC_STS_IV                   = 0x28, /*!< Arbitrary value for static STS configuration which will be defined by vendor */
    UCI_APP_CFG_NUMBER_OF_STS_SEGMENTS          = 0x29, /*!< @ref uci_sts_segments_e */
    UCI_APP_CFG_MAX_RR_RETRY                    = 0x2A, /*!< Number of failed ranging round (RR) attempts before stopping the session and move the Session State to SESSION_STATE_IDLE */
    UCI_APP_CFG_UWB_INITIATION_TIME             = 0x2B, /*!< Time taken for UWB initiation in unit of 1200 RSTU which is 1ms */
    UCI_APP_CFG_HOPPING_MODE                    = 0x2C, /*!< @ref uci_hopping_mode_e */
    UCI_APP_CFG_BLOCK_STRIDE_LENGTH             = 0x2D, /*!< Block Stride length indicates how many ranging blocks will be skipped after the completion of current ranging block */
    UCI_APP_CFG_RESULT_REPORT_CONFIG            = 0x2E, /*!< Used to enable/disable the result reports to be included in the RRRM */
    UCI_APP_CFG_IN_BAND_TERMINATION_ATTEMPT_COUNT = 0x2F, /*!< used to configure how many times an in-band termination signal shall be sent by a controller/initiator to a controlee */
    UCI_APP_CFG_SUB_SESSION_ID                  = 0x30, /*!< Sub-Session ID for the controlee device. This config is mandatory, and it is applicable if STS_CONFIG is set to 2 for controlee device*/
    UCI_APP_CFG_BPRF_PHR_DATA_RATE              = 0x31, /*!< @ref uci_bphr_phr_dr_e */
    UCI_APP_CFG_MAX_NUMBER_OF_MEASUREMENTS      = 0x32, /*!< set maximum number of ranging measurements to be executed in a session */
    UCI_APP_CFG_STS_LENGTH                      = 0x35, /*!< @ref uci_sts_lenght_e */

    // Additional configuration applicable only for CCC session
    UCI_APP_CFG_HOP_MODE_KEY                    = 0xA0, /*!< Key to generate hopping sequence */
    UCI_APP_CFG_RANGING_PROTOCOL_VER            = 0xA3, /*!< Version of the ranging protocol */
    UCI_APP_CFG_UWB_CONFIG_ID                   = 0xA4, /*!< UWB Configuration ID in the range */
    UCI_APP_CFG_PULSESHAPE_COMBO                = 0xA5,  /*!< Pulse shape combinations */

    // vendor
    UCI_APP_CFG_TX_POWER_LEVEL                  = 0xF2, /*!< Tx power level, not dBm */
    UCI_APP_CFG_RX_ANT_ID                       = 0xF3, /*!< Rx antenna id, 0 ~ 3 */

    /* 0x13,0x18,0x19,0x1D,0x20,0x21,0x36-0x9F for RFU */
    /* 0xA0 - 0xDF for Vendor Specific use */
    /* 0xE0 - 0xE2 for extension IDs */
    /* 0xE3 - 0xFF for Vendor Specific ID space */
} uci_app_cfg_tag_e;

/**
 * @brief   core parameters(device configuration parameters)
 */
typedef struct
{
    uint8_t state;              /*!< @ref uci_dev_state_e */
    uint8_t lp_mode;            /*!< @ref uci_lp_mode_e */
} uci_core_cfg_t;

/**
 * @brief   application configuration parameters
 */
typedef struct
{
    uint8_t dev_type;           /*!< @ref UCI_APP_CFG_DEVICE_TYPE */
    uint8_t ranging_mode;       /*!< @ref UCI_APP_CFG_RANGING_ROUND_USAGE */
    uint8_t sts_cfg;            /*!< @ref UCI_APP_CFG_STS_CONFIG */
    uint8_t multi_node_mode;    /*!< @ref UCI_APP_CFG_MULTI_NODE_MODE */
    uint8_t chl_num;            /*!< @ref UCI_APP_CFG_CHANNEL_NUMBER */
    uint8_t controlees;         /*!< @ref UCI_APP_CFG_NUMBER_OF_CONTROLEES */
    uint8_t dev_mac_addr[8];    /*!< @ref UCI_APP_CFG_DEVICE_MAC_ADDRESS */
    uint8_t dst_mac_addr[UCI_MAX_CONTROLEES][8];
                                /*!< @ref UCI_APP_CFG_DST_MAC_ADDRESS */
    uint16_t slot_duration;     /*!< @ref UCI_APP_CFG_SLOT_DURATION */
    uint32_t ranging_intv;      /*!< @ref UCI_APP_CFG_RANGING_INTERVAL */
    uint32_t sts_idx;           /*!< @ref UCI_APP_CFG_STS_INDEX */
    uint8_t fcs_type;           /*!< @ref UCI_APP_CFG_MAC_FCS_TYPE */
    uint8_t ranging_round_ctrl; /*!< @ref UCI_APP_CFG_RANGING_ROUND_CONTROL */
    uint8_t aoa_req;            /*!< @ref UCI_APP_CFG_AOA_RESULT_REQ */
    uint8_t ranging_data_ntf_cfg;/*!< @ref UCI_APP_CFG_RANGE_DATA_NTF_CONFIG */
    uint16_t ranging_data_ntf_prox_near;
                                /*!< @ref UCI_APP_CFG_RANGE_DATA_NTF_PROXIMITY_NEAR */
    uint16_t ranging_data_ntf_prox_far;
                                /*!< @ref UCI_APP_CFG_RANGE_DATA_NTF_PROXIMITY_FAR */
    uint8_t dev_role;           /*!< @ref UCI_APP_CFG_DEVICE_ROLE */
    uint8_t rframe_type;        /*!< @ref UCI_APP_CFG_RFRAME_CONFIG */
    uint8_t preamble_ci;        /*!< @ref UCI_APP_CFG_PREAMBLE_CODE_INDEX */
    uint8_t sfd_id;             /*!< @ref UCI_APP_CFG_SFD_ID */
    uint8_t psdu_data_rate;     /*!< @ref UCI_APP_CFG_PSDU_DATA_RATE */
    uint8_t preamble_duration;  /*!< @ref UCI_APP_CFG_PREAMBLE_DURATION */
    uint8_t time_struct;        /*!< @ref UCI_APP_CFG_RANGING_TIME_STRUCT */
    uint8_t slots_per_rr;       /*!< @ref UCI_APP_CFG_SLOTS_PER_RR */
    uint8_t adap_pwr;           /*!< @ref UCI_APP_CFG_TX_ADAPTIVE_PAYLOAD_POWER */
    uint8_t rspdr_slot_idx;     /*!< @ref UCI_APP_CFG_RESPONDER_SLOT_INDEX */
    uint8_t prf_mode;           /*!< @ref UCI_APP_CFG_PRF_MODE */
    uint8_t sche_mode;          /*!< @ref UCI_APP_CFG_SCHEDULED_MODE */
    uint8_t key_rotation;       /*!< @ref UCI_APP_CFG_KEY_ROTATION */
    uint8_t key_rotation_rate;  /*!< @ref UCI_APP_CFG_KEY_ROTATION_RATE */
    uint8_t session_priority;   /*!< @ref UCI_APP_CFG_SESSION_PRIORITY */
    uint8_t mac_addr_mode;      /*!< @ref UCI_APP_CFG_MAC_ADDRESS_MODE */
    uint16_t vendor_id;         /*!< @ref UCI_APP_CFG_VENDOR_ID */
    uint8_t static_sts_iv[6];   /*!< @ref UCI_APP_CFG_STATIC_STS_IV */
    uint8_t sts_segmemts;       /*!< @ref UCI_APP_CFG_NUMBER_OF_STS_SEGMENTS */
    uint8_t max_rr_retry;       /*!< @ref UCI_APP_CFG_MAX_RR_RETRY */
    uint32_t uwb_init_time;     /*!< @ref UCI_APP_CFG_UWB_INITIATION_TIME */
    uint8_t hopping_mode;       /*!< @ref UCI_APP_CFG_HOPPING_MODE */
    uint8_t block_stride;       /*!< @ref UCI_APP_CFG_BLOCK_STRIDE_LENGTH */
    uint8_t rrrm_rpt_cfg;       /*!< @ref UCI_APP_CFG_RESULT_REPORT_CONFIG */
    uint8_t inband_term_attem_cnt;
                                /*!< @ref UCI_APP_CFG_IN_BAND_TERMINATION_ATTEMPT_COUNT */
    uint32_t sub_session_id;    /*!< @ref UCI_APP_CFG_SUB_SESSION_ID */
    uint8_t bphr_phr_dr;        /*!< @ref UCI_APP_CFG_BPRF_PHR_DATA_RATE */
    uint16_t max_meas;          /*!< @ref UCI_APP_CFG_MAX_NUMBER_OF_MEASUREMENTS */
    uint8_t sts_lenght;         /*!< @ref UCI_APP_CFG_STS_LENGTH */

    uint32_t hop_mode_key;      /*!< @ref UCI_APP_CFG_HOP_MODE_KEY */
    uint16_t ranging_proto_ver; /*!< @ref UCI_APP_CFG_RANGING_PROTOCOL_VER */
    uint16_t uwb_cfg_id;        /*!< @ref UCI_APP_CFG_UWB_CONFIG_ID */
    uint8_t pulseshape_combo;   /*!< @ref UCI_APP_CFG_PULSESHAPE_COMBO */

    int8_t tx_pwr_level;        /*!< @ref UCI_APP_CFG_TX_POWER_LEVEL */
    uint8_t rx_ant_id;          /*!< @ref UCI_APP_CFG_RX_ANT_ID */
} uci_app_cfg_t;

/**
 * @brief   Multicast list update result
 */
typedef struct
{
    uint8_t     mac[2];         /*!< MAC */
    uint32_t    sub_session;    /*!< sub_session id */
    uint8_t     status;         /*!< @ref uci_mc_upd_status_e */
} uci_mc_status_t;

/**
 * @brief   Multicast list update structure
 */
typedef struct
{
    uint8_t     mac[2];         /*!< MAC */
    uint32_t    sub_session;    /*!< sub_session id */
} uci_mc_update_t;

/**
 * @brief   Device status
 */
typedef struct
{
    uint8_t state;              /*!< @ref uci_dev_state_e */
} uci_dev_status_ntf_t;

/**
 * @brief   General error
 */
typedef struct
{
    uint8_t status;             /*!< @ref uci_status_e */
} uci_ger_err_ntf_t;

/**
 * @brief   Session state change
 */
typedef struct
{
    uint32_t    session;        /*!< session id */
    uint8_t     state;          /*!< @ref uci_sess_state_e */
    uint8_t     code;           /*!< @ref uci_sess_st_chg_reason_code_e */
} uci_sess_state_ntf_t;

/**
 * @brief   Multicast list update result
 */
typedef struct
{
    uint32_t session;           /*!< session id */
    uint8_t remaining_size;     /*!< Remaining multicast list */
    uint8_t num;                /*!< Length of multicast list */
    uci_mc_status_t *controlees;/*!< Status of multicast list */
} uci_mc_lst_ntf_t;

/**
 * @brief   Single UWBS ranging result
 */
typedef struct
{
    uint8_t mac_addr[8];        /*!< MAC */
    uint8_t status;             /*!< @ref uci_status_e */
    uint8_t nLoS;               /*!< true of false */
    int16_t distance;           /*!< CM */
    int16_t aoa_azimuth;        /*!< degree */
    uint8_t aoa_azimuth_fom;    /*!< FoM of azimuth */
    int16_t aoa_elevation;      /*!< degree */
    uint8_t aoa_elevation_fom;  /*!< FoM of elevation */
    int16_t aoa_dst_azimuth;    /*!< degree */
    uint8_t aoa_dst_azimuth_fom;/*!< FoM of destination azimuth */
    int16_t aoa_dst_elevation;  /*!< degree */
    uint8_t aoa_dst_elevation_fom;/*!< FoM of destination elevation */
    uint8_t slot_idx;           /*!< @ref UCI_APP_CFG_RESPONDER_SLOT_INDEX */
} uci_meas_t;

/**
 * @brief   Ranging result notification
 */
typedef struct
{
    uint32_t seq_num;           /*!< Ranging round count */
    uint32_t session;           /*!< session id */
    uint32_t curr_intv;         /*!< Ranging interval, ms */
    uint8_t type;               /*!< Ranging type, always 1 */
    uint8_t addressing_mode;    /*!< MAC addr mode, 0 short address, 1 extended address */
    uint8_t meas_num;           /*!< Number of ranging results */
    uci_meas_t *meas;           /*!< Ranging data */
} uci_range_data_ntf_t;

/**
 * @brief   Data received from peer device
 */
typedef struct {
    uint8_t     *data;          /*!< Data received from peer device */
    uint16_t    bytes;          /*!< Data lenght in bytes */
} uci_vendor_data_ntf_t;

/**
 * @brief   device information
 */
typedef struct
{
    uint8_t major_ger_ver;
    uint8_t minor_ger_ver;
    uint8_t major_mac_ver;
    uint8_t minor_mac_ver;
    uint8_t major_phy_ver;
    uint8_t minor_phy_ver;
    uint8_t major_test_ver;
    uint8_t minor_test_ver;
    uint8_t vnd_info_len;
    uint8_t *vnd_info;
} uci_get_dev_info_t;

/**
 * @brief core/app structure of return of geting parameters
 */
typedef struct
{
    uint8_t     tag;
    uint8_t     length;
    uint8_t const *value;
} uci_get_rsp_param_t;

/**
 * @brief core/app structure of return of seting parameters
 */
typedef struct
{
    uint8_t tag;
    uint8_t status;
} uci_set_rsp_param_t;

/**
 * @brief   device capability
 */
typedef struct
{
    uint8_t num;
    uci_get_rsp_param_t *params;
} uci_get_caps_info_t;

/**
 * @brief core/app structure of return of seting parameters
 */
typedef struct
{
    uint8_t num;
    uci_set_rsp_param_t *params;
} uci_set_cfg_rsp_t;

/**
 * @brief core/app structure of return of geting parameters
 */
typedef struct
{
    uint8_t num;
    uci_get_rsp_param_t *params;
} uci_get_cfg_rsp_t;

/**
 * @brief   Session state
 */
typedef struct
{
    uint8_t state;          /*!< @ref uci_sess_state_e */
} uci_sess_get_state_t;

/**
 * @brief   Number of ranging count of specified session
 */
typedef struct
{
    uint32_t count;
} uci_get_ranging_cnt_t;

/**
 * @brief   Number of sessions
 */
typedef struct
{
    uint8_t count;
} uci_sess_get_cnt_t;

#endif // _UCI_DEFINED_H_

/** @} */
