/**
 * @file    common_n.h
 *
 * @brief   common TWR defines, types and fn()
 *
 * @attention
 *
 * Copyright 2016-2020 (c) Decawave Ltd, Dublin, Ireland.
 *
 * All rights reserved.
 *
 * @author
 */

#ifndef CORE_SRC_SRV_COMMON_N_H_
#define CORE_SRC_SRV_COMMON_N_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "deca_device_api.h"
#include "deca_interface.h"
#include "uwb_frames.h"

#define BPRF_SET_1 (1)  //SP0 IEEE SFD
#define BPRF_SET_2 (2)  //SP0 4z SFD
#define BPRF_SET_3 (3)  //SP1 4z SFD
#define BPRF_SET_4 (4)  //SP3 4z SFD

//Comment below to see the debug information in a real-time, diag_printf is a buffered I/O
#define DEBUG_SP3_MSG(...)

#ifndef DEBUG_SP3_MSG
#include "deca_dbg.h"
#define DEBUG_SP3_MSG   diag_printf
#endif


#ifndef TWR_ENTER_CRITICAL
#define TWR_ENTER_CRITICAL    taskENTER_CRITICAL
#endif

#ifndef TWR_EXIT_CRITICAL
#define TWR_EXIT_CRITICAL     taskEXIT_CRITICAL
#endif

#ifndef M_PI
#define M_PI    (3.141592654f)
#endif

#ifndef M_PI_2
#define M_PI_2  (1.570796327f)
#endif

#ifndef TWO_PI
#define TWO_PI  (2*M_PI)
#endif


// ----------------------------------------------------------------------------

struct mini_diag_s
{//temporary diagnostics for PDoA debugging
    uint8_t     DTUNE5;
    uint32_t    CIA_TDOA_0;
    uint32_t    CIA_TDOA_1_PDOA;
    uint16_t    IP_DIAG_10;
    uint16_t    CY0_DIAG_10;
    uint16_t    CY0_TOA_HI;
    uint16_t    CY1_TOA_HI;
};

typedef struct mini_diag_s mini_diag_t;

struct pdoa_s{
    int16_t     pdoa;    /* DW3000 PDOA */
    mini_diag_t mDiag;
};

typedef struct pdoa_s pdoa_t;


struct result_s
{
    uint16_t    addr16;
    uint16_t    rangeNum;       //number from Tag Poll and Final messages, which indicates the current range number
    uint32_t    resTime_us;     //reception time of the end of the Final from the Tag wrt node's SuperFrame start, microseconds
    float       pdoa_raw_deg;   //pdoa_raw: phase differences in degrees without any correction [-180 .. 180]
    float       pdoa_raw_degP;  //pdoa_raw: phase differences in degrees from Poll message
    float       dist_cm;        //distance to the tag in cm, corrected to a rngOffset_mm
    float       x_cm;           //X of the tag wrt to the node, cm
    float       y_cm;           //Y of the tag wrt to the node, cm
    float       clockOffset_pphm;//clock offset in hundredths of ppm (i.e. 1ppm is 100)
    uint16_t    flag;           //service message data from the tag (low byte) and node (high byte), bitmask (defined as "RES_FLAG_")
    int16_t     acc_x;          //Normalized accel data X from the Tag, mg: acc_x
    int16_t     acc_y;          //Normalized accel data Y from the Tag, mg: acc_y
    int16_t     acc_z;          //Normalized accel data Z from the Tag, mg: acc_z

    pdoa_t      pollPDOA;
    pdoa_t      finalPDOA;

    int8_t      tMaster_C;      //temperature of Master in degree centigrade

    float path_diff;
};

typedef struct result_s result_t;

/* TxPckt */
struct tx_pckt_s
{
    int16_t        psduLen;

    union {
        std_msg_t           stdMsg;
        twr_msg_t           twrMsg;

        blink_msg_t         blinkMsg;
        poll_msg_t          pollMsg;
        final_msg_accel_t   finalMsg;

        rng_cfg_msg_t       rngCfgMsg;
        rng_cfg_upd_msg_t   rngCfgUpdMsg;
        resp_pdoa_msg_t     respMsg;
    } msg;

    uint8_t        txFlag;          // Holds Tx sending parameters: extended set for DW3000

    uint32_t    delayedTxTimeH_sy;  // Delayed transmit time (in 4ns)
    uint32_t    delayedRxTime_sy;   // Delay after Tx when to switch on receiver (in SY=1.0256us)
    uint16_t    delayedRxTimeout_sy;// How long the receiver will be switched on after Tx (in SY=1.0256us)

};

typedef struct tx_pckt_s tx_pckt_t;

// ----------------------------------------------------------------------------
//
void tn_app_config(rxtx_configure_t *p);
void rxtx_configure(rxtx_configure_t *p);
error_e tx_start(tx_pckt_t * pTxPckt);

#if (defined(CUSTOM_BOARD_INC) && (CUSTOM_BOARD_INC == type2ab_evb)) /*added on Sept 23,2021 by guanshan.miao@murata.com*/
/*OTP Memory Addresses definition*/
#define DW3000_OPT_EUI_ADDRESS1     (0x00)
#define DW3000_OPT_EUI_ADDRESS2     (0x01)
#define TX_CH5_ADDRESS          (0x11)
#define TX_CH9_ADDRESS          (0x13)
#define PG_CNT_ADDRESS          (0x1B)  // Channel 5 -> Bytes [3:2] , Channel 9 -> Bytes [1:0]
#define PG_DLY_TRIM_ADDRESS     (0x1E)  //Trim -> Byte [0], PGDLY Channel 5 -> Byte [3] , Channel 9 -> Byte [2]
#define ANT_DLY_ADDRESS         (0x1C)  // Channel 5 -> Bytes [3:2] , Channel 9 -> Bytes [1:0]
#define REF_ADDRESS             (0x19)  // Temp -> Byte [1] , VBAT -> Byte [0]
#define PDOA_CODE_PRF64         (0x1A)  // Channel 5 -> Bytes [3:2] , Channel 9 -> Bytes [1:0]
#define OTP_REVISION                (0x1F) // otp resvision ->Byte[0]
#define S2U_DW3000_OTP_MAP_MAX      (0x7F)  //0x7F

/* Murata Protducts Calibrated*/
typedef struct dwt_calibrate_s
{
    dwt_txconfig_t dwt_txconfig;
    uint16_t    internal_ant_delay;    /* antenna delay(internal,such PCB,)*/
    uint16_t    pdoacode; /*calibrated by litepoint IQFact Plus*/
}dwt_calibrate_t;
uint8_t chan_to_index(uint8_t i);
void dw3xxx_calibration_parameters_update(dwt_calibrate_t * dwt_calibrate);
#endif /* end of (CUSTOM_BOARD_INC && CUSTOM_BOARD_INC == type2ab_evb) */


#ifdef __cplusplus
}
#endif

#endif /* CORE_SRC_SRV_COMMON_N_H_ */
