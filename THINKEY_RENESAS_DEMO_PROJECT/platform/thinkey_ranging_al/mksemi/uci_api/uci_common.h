
/**
 * @file    uci_common.h
 * @brief   Defines the data structure exposed by UCI Host to upper-level users
 * @author  Mauna Kea Semiconductor
 * @copyright   Copyright (c) 2019-2022 Mauna Kea Semiconductor Holdings.
 *              All rights reserved.
 * @date    2022-7-14
 * @ingroup uci_api
 * @{
 */

#ifndef _UCI_COMMON_H_
#define _UCI_COMMON_H_

#include "uci_defined.h"

#include "uci_port.h"
#include "uci_debug.h"
#include "uci_utils.h"

/**
 * @brief error codes for UCI Host
 */
typedef enum {
    UEC_SUCCESS         = 0,        /*!< Function successfully executed */
    UEC_OUT_OF_MEMORY   = 0xFF00,   /*!< @ref uci_malloc return NULL, insufficient memory */
    UEC_PARAM_INVALID,              /*!< Invalid Parameter */
    UEC_ERR_INTERNAL,               /*!< Internal error, variables that managed by UCI Host may be corrupted */
    UEC_UWBS_NOT_FOUND,             /*!< UWBS instance does not exist */
    UEC_BUF_RECS_OVERFLOW,          /*!< The UWBS instance can not process frames any more, increase @p buf_recs when create this instance */
    UEC_COMM_TIMEOUT,               /*!< Can't not receive expected response from UWBS in time */
    UEC_ADDR_NON_ALIGNED,           /*!< The provided address was not 4-bytes aligned */
    UEC_RESP_NOT_EXPECTED,          /*!< Received unexpected response from UWBS. This is may be caused by one of the followings  \n
                                        - data committed to @ref uci_process_recv_pkt was not a complete UCI Message frame. It means that don't call @ref uci_process_recv_pkt until you really get a complete frame
                                        - data committed to @ref uci_process_recv_pkt had been changed before parsed(we defer data parsing to @ref uci_host_loop). Don’t reuse the buffer until we tell you via @ref uci_buf_release
                                        - UWBS just can't deal with the request that we just issued, this is may be a compatible problem
                                    */
    UEC_UWBS_IS_BUSY,               /*!< The UWBS instance has any cached command(s) to be send, can not be delete now */
    UEC_UWBS_IS_DESTROYING,         /*!< The UWBS is pending to be destroyed, can not accept any request any longer */
} uci_err_codes_e;

/**
 * @brief   Event types of callback @ref uci_cb_t
 */
typedef enum {
    DEV_STATUS_UPD      = 0,    /*!< UWBS Status Update, @ref uci_dev_status_ntf_t */
    GER_ERR_NTF,                /*!< Generic Error Notification, @ref uci_ger_err_ntf_t */
    SESSION_STATUS_UPD,         /*!< Session status update, @ref uci_sess_state_ntf_t */
    UPD_MULTICAST_NTF,          /*!< Multicast update result notification, @ref uci_mc_lst_ntf_t */
    RANGE_DATA_NTF,             /*!< Ranging data update, @ref uci_range_data_ntf_t */
    VENDOR_DATA_NTF,            /*!< Data received from peer device */
    API_RET_NTF,                /*!< UCI API asynchronous callback, @ref uci_api_ret_t */
} uci_cb_evt_type_e;

/**
 * @brief   UCI API name enumeration, @ref uci_api_ret_t::whoami identifies specific API events when @ref API_RET_NTF
 */
typedef enum {
    UCI_DEV_RESET       = 0,    /*!< @ref uci_core_device_reset */
    UCI_GET_DEV_INFO,           /*!< @ref uci_core_get_device_info */
    UCI_GET_CAPS_INFO,          /*!< @ref uci_core_get_caps_info */
    UCI_CORE_GET_CONFIG,        /*!< @ref uci_core_get_config */
    UIC_CORE_SET_CONFIG,        /*!< @ref uci_core_set_config */
    UCI_SESSION_INIT,           /*!< @ref uci_session_init */
    UCI_SESSION_GET_CNT,        /*!< @ref uci_session_get_count */
    UCI_SESSION_GET_STATE,      /*!< @ref uci_session_get_state */
    UCI_SESSION_GET_APP_CFG,    /*!< @ref uci_session_get_app_config */
    UCI_SESSION_SET_APP_CFG,    /*!< @ref uci_session_set_app_config */
    UCI_SESSION_UPD_MC_LIST,    /*!< @ref uci_session_update_controller_multicast_list */
    UCI_SESSION_DEINIT,         /*!< @ref uci_session_deinit */
    UCI_RANGE_START,            /*!< @ref uci_range_start */
    UCI_RANGE_STOP,             /*!< @ref uci_range_stop */
    UCI_RANGE_GET_CNT,          /*!< @ref uci_range_get_ranging_count */
    UCI_VENDOR_SET_TX_DATA,     /*!< @ref uci_vendor_set_tx_data */
    UCI_VENDOR_SET_CCC_URSK,    /*!< @ref uci_vendor_set_ccc_ursk */
    UCI_UWBS_DESTROYED,         /*!< The UWBS instance has been destroyed */
    UCI_API_NAME_MAX,
} uci_api_name_e;

/**
 * @brief   The payload of the UCI API asynchronous callback
 */
typedef union {
    uci_get_dev_info_t      dev_info;   /*!< @ref uci_core_get_device_info */
    uci_get_caps_info_t     caps_info;  /*!< @ref uci_core_get_caps_info */
    uci_set_cfg_rsp_t       set_rsp;    /*!< @ref uci_core_set_config or @ref uci_session_set_app_config */
    uci_get_cfg_rsp_t       get_rsp;    /*!< @ref uci_core_get_config or @ref uci_session_get_app_config */
    uci_sess_get_state_t    sess_state; /*!< @ref uci_session_get_state */
    uci_get_ranging_cnt_t   ranging_cnt;/*!< @ref uci_range_get_ranging_count */
    uci_sess_get_cnt_t      sess_cnt;   /*!< @ref uci_session_get_count */
} uci_api_pl_t;

/**
 * @brief   UCI API execution result asynchronous callback structure
 */
typedef struct {
    uint8_t         whoami;     /*!< Identifies which api triggers the callback, @ref uci_api_name_e */
    int32_t         status;     /*!< The api request result answered by UWBS, @ref uci_status_e @ref uci_err_codes_e */
    uci_api_pl_t    *param;     /*!< Some APIs have additional return values */
} uci_api_ret_t;

/**
 * @brief   UCI Host callback structure
 */
typedef struct {
    uint8_t         type;       /*!< Event type, @ref uci_cb_evt_type_e */

    union {
        uci_dev_status_ntf_t    dev_status;     /*!< UWBS Status Change Notification */
        uci_ger_err_ntf_t       ger_err;        /*!< Generic error with UWBS */
        uci_sess_state_ntf_t    sess_state;     /*!< Session state change notification */
        uci_mc_lst_ntf_t        mc_lst;         /*!< Multicast update result notification */
        uci_range_data_ntf_t    range_data;     /*!< Ranging result notification */
        uci_vendor_data_ntf_t   vendor_data;    /*!< Data received from peer device */
        uci_api_ret_t           aret;           /*!< UCI API execution result callback */
    } pl;
} uci_cb_evt_t;

/**
 * @brief   @@ref uwbs_t forward declaration
 */
typedef struct _uwbs uwbs_t;

/**
 * @brief   UWBS event callback
 * @param[in]   uwbs UWBS instance
 * @param[in]   evt event
 */
typedef void (*uci_cb_t)(uwbs_t *uwbs, uci_cb_evt_t *evt);

/**
 * @brief   UWBS instance structure
 * @todo    Whether core and app consider using pointers, which are passed in from outside, can reduce memory consumption when creating multiple instances
 */
struct _uwbs {
    uci_cb_t        cb;     /*!< Event callback */
    uint8_t         bus;    /*!< bus_id passed in when creating */
    uint8_t         id;     /*!< uwbs_id passed in when creating */
    uci_core_cfg_t  core;   /*!< core parameters, parameters type refer to @ref uci_dev_cfg_tag_e */
    uci_app_cfg_t   app;    /*!< app parameters, parameters type refer to @ref uci_app_cfg_tag_e */
};

#endif // _UCI_COMMON_H_

/** @} */
