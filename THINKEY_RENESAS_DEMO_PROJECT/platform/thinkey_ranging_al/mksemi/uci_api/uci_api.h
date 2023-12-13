
/**
 * @defgroup uci_host UCI Host
 * @brief   The code running on the FiRa UCI Host side and provides interfaces for controlling and receiving data from UWBS
 */

/**
 * @defgroup uci_api API
 * @brief   Provide FiRa UCI related operation interfaces and data structures
 * @{
 * @ingroup uci_host
 * @{
 * @file    uci_api.h
 * @brief   Provide UCI Host related operation interfaces and data structures
 * @details  This UCI Host library has the following features
 *          - Works asynchronously, caches relevant commands after APIs get called, and then send UCI Message Frame in @ref uci_host_loop
 *          - When received CMD Response or Notification, user needs to call @ref uci_process_recv_pkt and @ref uci_host_loop \n
 *              If parsing is successful, user will be notified through callback @ref uci_cb_t
 *          - User needs to ensure that after calling @ref uci_process_recv_pkt, the provided buffer must remain unchanged  \n
 *              otherwise the UCI Host will consider the data error and discard it
 *          - Has the ability to process multiple consecutively arriving data. The specific number of caches is specified when creating UWBS, refer to @ref uci_create_uwbs
 *          - Support creating multiple buses, each bus allows to mount multiple UWBS, the number depends on the memory provided by the platform
 *          - The stack consumed inside the API is not less than @ref UCI_MAX_STACK_BUF
 *          - UWBS UCI messages belonging to different buses can be processed in parallel
 *          - UWBS UCI packets on the same bus are processed in the order in which they were submitted  \n
 *              and subsequent packets can only be processed after the current packet is processed (response or timeout).
 * @note    Read the relevant chapters of UWB COMMAND INTERFACE SPECIFICATION to help understand the meaning of the api provided by UCI Host and its parameters  \n
 *          Before using, you need to provide platform-related functions, take a look at uci_port.h
 * @note    The typical steps to start ranging
 *          - @ref uci_create_uwbs create as many UWBS as you need
 *          - @ref uci_core_device_reset reset UWBS if needed
 *          - @ref uci_session_init create the same session for all UWBS just created
 *          - @ref uci_session_set_app_config configure ranging parameters for this session
 *          - @ref uci_range_start start ranging
 *          - @ref uci_host_loop kick UCI Host when times up or @ref uci_sched_me tells you
 *          - @ref uci_destroy_uwbs destroy specific UWBS instance if no longer needed
 * @author  Mauna Kea Semiconductor
 * @copyright   Copyright (c) 2019-2022 Mauna Kea Semiconductor Holdings.
 *              All rights reserved.
 * @date    2022-7-14
 */

#ifndef _UCI_API_H_
#define _UCI_API_H_

#include "uci_common.h"

/**
 * @brief   Set each member of @ref uci_core_cfg_t as the default value of FiRa
 * @param[in]   core variable to be initialized
 */
#define UCI_API_CORE_CFG_INIT(core) do {            \
        core.state = (uint8_t)DEVICE_STATE_READY;   \
        core.lp_mode = (uint8_t)LP_MODE_ENABLE;     \
    } while (0)

/**
 * @brief   Assign each member of @ref uci_app_cfg_t to the default value of FiRa
 * @param[in]   app variable to be initialized
 * @note    Only initialize members with default values, others are not assigned, pay attention before use
 */
#define UCI_API_APP_CFG_INIT(app)   do {                                        \
        memset(&app, 0x00, sizeof(app));                                        \
        app.ranging_mode = (uint8_t)DS_TWR_DEFER;                               \
        app.sts_cfg = (uint8_t)STS_STATIC;                                      \
        app.chl_num = (uint8_t)CHL_NUM_9;                                       \
        app.slot_duration = 2400;                                               \
        app.ranging_intv = 50;                                                  \
        app.fcs_type = FCS_CRC_16;                                              \
        app.ranging_round_ctrl = (uint8_t)SETUP_RESULT_RPT_MSG | SETUP_CTRL_MSG;\
        app.aoa_req = (uint8_t)AOA_REQ_ENABLE;                                  \
        app.ranging_data_ntf_cfg = (uint8_t)RANGING_DATA_NTF_ENABLE;            \
        app.ranging_data_ntf_prox_near = 0;                                     \
        app.ranging_data_ntf_prox_far = 20000;                                  \
        app.rframe_type = (uint8_t)RFRAME_SP1;                                  \
        app.preamble_ci = (uint8_t)BPRF_PREAMBLE_CI_10;                         \
        app.sfd_id = (uint8_t)HPRF_SFD_1;                                       \
        app.psdu_data_rate = (uint8_t)PSDU_DATA_RATE_31M2;                      \
        app.preamble_duration = (uint8_t)PSR_SYMBOLS_64;                        \
        app.time_struct = (uint8_t)BLOCK_BASED_SCHE;                            \
        app.slots_per_rr = 25;                                                  \
        app.adap_pwr = (uint8_t)TX_ADAP_PWR_DISABLE;                            \
        app.prf_mode = 2;                                                       \
        app.sche_mode = (uint8_t)TIME_SCHE_RANGING;                             \
        app.key_rotation =(uint8_t) KEY_ROTATION_DISABLE;                       \
        app.key_rotation_rate = 0;                                              \
        app.session_priority = 50;                                              \
        app.mac_addr_mode = (uint8_t)MAC_2BYTES_MHR_2BYTES;                     \
        app.sts_segmemts = 0;                                                   \
        app.max_rr_retry = 0;                                                   \
        app.hopping_mode = (uint8_t)HOPPING_MODE_DISABLE;                       \
        app.block_stride = 0;                                                   \
        app.rrrm_rpt_cfg = (uint8_t)TOF_REPORT;                                 \
        app.inband_term_attem_cnt = 1;                                          \
        app.bphr_phr_dr = (uint8_t)BPHR_PHR_DR_850K;                            \
        app.max_meas = 0;                                                       \
        app.sts_lenght = (uint8_t)STS_LEN_SYMBOL_64;                            \
        app.tx_pwr_level = 15;                                                  \
    } while (0)
    
/**
 * @brief   Set time for waiting response from UWBS
 * @param[in]   ms waiting interval, in ms. Defaults to @ref UCI_RESP_TIMEOUT_MS
 * @note    The time start counting when @ref uci_send is called
 * @return  void
 */
void uci_host_set_timeout(uint32_t ms);

/**
 * @brief   Set the way to deal with command caches whenever timeout occured
 * @param[in]   flush if true drop all command caches whenever timeout occured. Defaults to @ref UCI_FLUSH_IF_TIMEOUT
 * @note    It only flush command caches for those UWBS who not respond in time 
 * @return  void
 */
void uci_host_set_flush_switch(bool flush);

/**
 * @brief   Get version of this UCI Host Library
 * @return  version string
 */
const char *uci_host_version(void);

/**
 * @brief   Create UWBS instance
 * @param[in]   bus_id bus id, maintained by user. Specify that which physical bus this UWBS instance connected to
 * @param[in]   uwbs_id uwbs id, maintained by user. Specify the identity for this UWBS instance    \n
 *              please note that this very identify must different from the others that connected to the same bus
 * @param[in]   buf_recs number of slots that used to record the pending packets submitted by @ref uci_process_recv_pkt
 * @param[in]   core Provide initial value for created @ref uwbs_t::core
 * @param[in]   app Provide initial value for created @ref uwbs_t::app
 * @param[in]   cb The callback when parsing the pending packet is successful
 * @note    If @ref uci_process_recv_pkt returns @ref UEC_BUF_RECS_OVERFLOW, use larger @p buf_recs \n
 *          Please note that it is no meaning if @p buf_recs bigger than the number of buffers which used to received UCI message
 * @return  create result
 *  @retval NULL failed to create, check    \n
 *              - whether the parameters are legal
 *              - whether the dynamic memory enough
 *  @retval !NULL created successfully 
 */
uwbs_t *uci_create_uwbs(uint8_t bus_id, uint8_t uwbs_id, uint8_t buf_recs, uci_core_cfg_t *core, uci_app_cfg_t *app, uci_cb_t cb);

/**
 * @brief   Destroy UWBS instance
 * @param[in]   uwbs UWBS instance
 * @param[in]   force delete the instance no matter if there are cached commands to be send if true
 * @note    Please note that this @p uwbs will no be destroyed immediately but defer to next time the @ref uci_host_loop gets called   \n
 *          UCI Host will notice you asynchronously while the @p uwbs gets destroyed, refer to @ref UCI_UWBS_DESTROYED
 * @return  Return the request result, refer to @ref uci_err_codes_e
 */
int32_t uci_destroy_uwbs(uwbs_t *uwbs, bool force);

/**
 * @brief   Reset UWBS
 * @param[in]   uwbs UWBS instance
 * @param[in]   code reset type, only 0
 * @return  Return the request result, refer to @ref uci_err_codes_e
 */
int32_t uci_core_device_reset(uwbs_t *uwbs, uint8_t code);

/**
 * @brief   Get device infomation of UWBS
 * @param[in]   uwbs UWBS instance
 * @note    Device information is described by @ref uci_get_dev_info_t
 * @return  Return the request result, refer to @ref uci_err_codes_e
 */
int32_t uci_core_get_device_info(uwbs_t *uwbs);

/**
 * @brief   Get capabilities of UWBS
 * @param[in]   uwbs UWBS instance
 * @note    Device capabilities are described by @ref uci_get_caps_info_t
 * @return  Return the request result, refer to @ref uci_err_codes_e
 */
int32_t uci_core_get_caps_info(uwbs_t *uwbs);

/**
 * @brief   Get @ref uwbs_t::core of UWBS
 * @param[in]   uwbs UWBS instance
 * @param[in]   list parameter list, refer to @ref uci_dev_cfg_tag_e
 * @param[in]   num number of parameters
 * @note    Get the real configuration of UWBS via UCI Message, then writeback to @ref uwbs_t::core \n
 *          The result described by @ref uci_get_cfg_rsp_t
 * @return  Return the request result, refer to @ref uci_err_codes_e
 */
int32_t uci_core_get_config(uwbs_t *uwbs, uint8_t *list, uint8_t num);

/**
 * @brief   Set @ref uwbs_t::core of UWBS
 * @param[in]   uwbs UWBS instance
 * @param[in]   list parameter list, refer to @ref uci_dev_cfg_tag_e
 * @param[in]   num number of parameters
 * @note    Write @ref uwbs_t::core first, then set UWBS via UCI Message   \n
 *          The result is described by @ref uci_get_cfg_rsp_t
 * @return  Return the request result, refer to @ref uci_err_codes_e
 */
int32_t uci_core_set_config(uwbs_t *uwbs, uint8_t *list, uint8_t num);

/**
 * @brief   Create ranging session
 * @param[in]   uwbs UWBS instance
 * @param[in]   session session id
 * @param[in]   type session type, @ref RANGING_SESSION is the only valid type
 * @note    Before calling all session or ranging related APIs, you should create a session first
 * @return  Return the request result, refer to @ref uci_err_codes_e
 */
int32_t uci_session_init(uwbs_t *uwbs, uint32_t session, uci_session_type_e type);

/**
 * @brief   Get the number of sessions created for this UWBS
 * @param[in]   uwbs UWBS instance
 * @note    Query results are described by @ref uci_sess_get_cnt_t
 * @return  Return the request result, refer to @ref uci_err_codes_e
 */
int32_t uci_session_get_count(uwbs_t *uwbs);

/**
 * @brief   Get the status of the specified session
 * @param[in]   uwbs UWBS instance
 * @param[in]   session the session id to be queried
 * @note    The obtained result is described by @ref uci_sess_get_state_t, and the session state is described by @ref uci_sess_state_e
 * @return  Return the request result, refer to @ref uci_err_codes_e
 */
int32_t uci_session_get_state(uwbs_t *uwbs, uint32_t session);

/**
 * @brief   Get @ref uwbs_t::app of UWBS
 * @param[in]   uwbs UWBS instance
 * @param[in]   session the session id to be queried
 * @param[in]   list parameter list, reference @ref uci_app_cfg_tag_e
 * @param[in]   num number of parameters
 * @note    Get the real configuration of UWBS via UCI CMD, then writeback to @ref uwbs_t::app  \n
 *          The result described by @ref uci_get_cfg_rsp_t
 * @return  Return the request result, refer to @ref uci_err_codes_e
 */
int32_t uci_session_get_app_config(uwbs_t *uwbs, uint32_t session, uint8_t *list, uint8_t num);

/**
 * @brief   Set @ref uwbs_t::app of UWBS
 * @param[in]   uwbs UWBS instance
 * @param[in]   session the session id to be set
 * @param[in]   list parameter list, reference @ref uci_app_cfg_tag_e
 * @param[in]   num number of parameters
 * @note    Write @ref uwbs_t::app first, then set UWBS via UCI CMD    \n
 *          The result is described by @ref uci_get_cfg_rsp_t
 * @return  Return the request result, refer to @ref uci_err_codes_e
 */
int32_t uci_session_set_app_config(uwbs_t *uwbs, uint32_t session, uint8_t *list, uint8_t num);

/**
 * @brief   Update the multicast address list of the specified session
 * @param[in]   uwbs UWBS instance
 * @param[in]   session the session id to be updated
 * @param[in]   action update type, add or delete
 * @param[in]   update the multicast object to be updated
 * @param[in]   num number of mulcicats to be update
 * @note    UWBS first responds to the update request result, and then informs the update result through @ref uci_mc_lst_ntf_t
 * @return  Return the request result, refer to @ref uci_err_codes_e
 */
int32_t uci_session_update_controller_multicast_list(uwbs_t *uwbs, uint32_t session, uci_upd_mc_act_e action, uci_mc_update_t *update, uint8_t num);

/**
 * @brief   Destroy the specified session
 * @param[in]   uwbs UWBS instance
 * @param[in]   session session id to be destroyed
 * @return  Return the request result, refer to @ref uci_err_codes_e
 */
int32_t uci_session_deinit(uwbs_t *uwbs, uint32_t session);

/**
 * @brief   Specify the session to start ranging
 * @param[in]   uwbs UWBS instance
 * @param[in]   session session id to be started
 * @note    UWBS first responds to the update request result, and then informs the ranging result through @ref uci_range_data_ntf_t
 * @return  Return the request result, refer to @ref uci_err_codes_e
 */
int32_t uci_range_start(uwbs_t *uwbs, uint32_t session);

/**
 * @brief   Specify the session to stop ranging
 * @param[in]   uwbs UWBS instance
 * @param[in]   session session id to be stopped
 * @return  Return the request result, refer to @ref uci_err_codes_e
 */
int32_t uci_range_stop(uwbs_t *uwbs, uint32_t session);

/**
 * @brief   Query the number of ranging count for a specified session
 * @param[in]   uwbs UWBS instance
 * @param[in]   session the session id to be queried
 * @note    The query result is described by @ref uci_get_ranging_cnt_t
 * @return  Return the request result, refer to @ref uci_err_codes_e
 */
int32_t uci_range_get_ranging_count(uwbs_t *uwbs, uint32_t session);

/**
 * @brief   Set message for custom ranging which also named RCM or Pre-Poll
 * @param[in]   uwbs UWBS instance
 * @param[in]   data message payload
 * @param[in]   bytes message length in bytes
 * @return  Return the request result, refer to @ref uci_err_codes_e
 */
int32_t uci_vendor_set_tx_data(uwbs_t *uwbs, uint8_t *data, uint8_t bytes);

/**
 * @brief   Set CCC URSK
 * @param[in]   uwbs UWBS instance
 * @param[in]   ursk new URSK
 * @param[in]   bytes bytes of USRSK, has to be 32
 * @todo    URSK may need to be configured for a specific session
 * @return  Return the request result, refer to @ref uci_err_codes_e
 */
int32_t uci_vendor_set_ccc_ursk(uwbs_t *uwbs, uint8_t *ursk, uint8_t bytes);

/**
 * @brief   Run UCI Host
 * @return  Returns the next time to wake up
 *  @retval UINT32_MAX All things processed, no need to wake up
 *  @retval !UINT32_MAX There are still business to be processed, run this interface after the specified time
 */
uint32_t uci_host_loop(void);

#endif // _UCI_API_H_

/** @} */
/** @} */
