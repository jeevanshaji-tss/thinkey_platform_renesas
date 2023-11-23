/** \file
   ---------------------------------------------------------------
   Copyright (C) 2021. Panthronics AG - All Rights Reserved.

   This material may not be reproduced, displayed, modified or
   distributed without the express prior written permission of the
   Panthronics AG.

   PLEASE REFER TO OUR DISCLAIMER NOTICE IN THE PROJECT ROOT.
   ---------------------------------------------------------------

    Project     : PTX100x
    Module      : PTX100x Felica-DTE
    File        : ptxFeliCa_DTE.h

    Description : Collection of functions for FeliCa compliance Tests.
*/

/**
 * \addtogroup grp_ptx_api_felica_dte FeliCa DTE API
 *
 * @{
 */

#ifndef APIS_PTX_FELICA_DTE_H_
#define APIS_PTX_FELICA_DTE_H_

/*
 * ####################################################################################################################
 * INCLUDES
 * ####################################################################################################################
 */

#include <stdint.h>
#include "ptxStatus.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * ####################################################################################################################
 * DEFINES / TYPES
 * ####################################################################################################################
 */

/*
 * ####################################################################################################################
 * TYPES
 * ####################################################################################################################
 */

/**
 * \brief FeliCa-DTE Test Type
 */
typedef enum ptxFeliCa_DTE_BitRate
{
    FELICA_DTE_BITRATE_212 = 1,                             /**< RF Bitrate 212 kBit/s */
    FELICA_DTE_BITRATE_424 = 2,                             /**< RF Bitrate 424 kBit/s */

} ptxFeliCa_DTE_BitRate_t;

/**
 * \brief FeliCa-DTE Test Type
 */
typedef enum ptxFeliCa_DTE_TestID
{
    FELICA_DTE_TestID_PerformanceTest,                      /**< RF-Performance Tests */

} ptxFeliCa_DTE_TestID_t;

/**
 * \brief RF-Test Test Parameters for PRBS-Test
 */
typedef struct ptxFeliCa_DTE_PerformanceTest
{
    uint8_t                             NrTests;            /**< Number of Tests/Repetitions */
    uint8_t                             *ResultBuffer;      /**< Pointer to Result-Buffer */
    uint8_t                             ResultBufferSize;   /**< Size of Result-Buffer */
    ptxFeliCa_DTE_BitRate_t             Bitrate;            /**< Desired Bitrate */
    uint32_t                            TimeOutMS;          /**< Timeout value in [ms] for a single test-run */

} ptxFeliCa_DTE_PerformanceTest_t;

/**
 * \brief RF-Test Test Parameters for PRBS-Test
 */
typedef union ptxFeliCa_DTE_Params
{
    ptxFeliCa_DTE_PerformanceTest_t     PerformanceTest;    /**< RFU */

} ptxFeliCa_DTE_Params_t;

/**
 * \brief FeliCa-DTE Test Progress Parameters
 */
typedef struct ptxFeliCa_DTE_TestProgressParams
{
    uint8_t                             ExitProcessing;     /**< Flag to abort an ongoing Test-execution (if applicable) */
    uint8_t                             NrTestsProcessed;   /**< Info how many tests were executed so far (if applicable) */

} ptxFeliCa_DTE_TestProgressParams_t;

/**
 * \brief Progress Callback Function
 */
typedef void (*ptxFeliCa_DTE_ProgressFn_t)(ptxFeliCa_DTE_TestProgressParams_t *progressParams);

/**
 * \brief FeliCa-DTE Test Parameters
 */
typedef struct ptxFeliCa_DTE_TestParams
{
    ptxFeliCa_DTE_TestID_t              ID;                 /**< Compliance Test-Identifier */
    ptxFeliCa_DTE_Params_t              Params;             /**< Test Parameter */
    ptxFeliCa_DTE_ProgressFn_t          ProgressCB;         /**< Optional Callback-Function to get notified about the current progress */

} ptxFeliCa_DTE_TestParams_t;

/**
 * \brief FeliCa-DTE Initialization Parameters
 */
typedef struct ptxFeliCa_DTE_InitParams
{
#ifdef PTX_PRODUCT_TYPE_IOT_READER
    struct ptxIoTRd         *IoTRdComp;         /**< Main Stack component */
#else
    struct ptxPOS           *POSComp;           /**< Main Stack component */
#endif
} ptxFeliCa_DTE_InitParams_t;

/**
 * \brief FeliCa-DTE Component
 */
typedef struct ptxFeliCa_DTE
{
    /* Components */
    ptxStatus_Comps_t       CompId;             /**< Component Id */

#ifdef PTX_PRODUCT_TYPE_IOT_READER
    struct ptxIoTRd         *IoTRdComp;         /**< Main Stack component */
#else
    struct ptxPOS           *POSComp;           /**< Main Stack component */
#endif
} ptxFeliCa_DTE_t;

/*
 * ####################################################################################################################
 * API FUNCTIONS
 * ####################################################################################################################
 */

/**
 * \brief Initializes the FeliCa-DTE Component.
 *
 * \param[in]   feliCaDTEComp           Pointer to an existing instance of the FeliCa-DTE component.
 * \param[in]   initParams              Pointer to initialization parameters.
 *
 * \return Status, indicating whether the operation was successful.
 */
ptxStatus_t ptxFeliCa_DTE_Init (ptxFeliCa_DTE_t *feliCaDTEComp, ptxFeliCa_DTE_InitParams_t *initParams);

/**
 * \brief Deinitializes the FeliCa-DTE Component.
 *
 * \param[in]   feliCaDTEComp           Pointer to an existing instance of the FeliCa-DTE component.
 *
 * \return Status, indicating whether the operation was successful. See \ref ptxStatus_t.
 */
ptxStatus_t ptxFeliCa_DTE_Deinit (ptxFeliCa_DTE_t *feliCaDTEComp);

/**
 * \brief Enables/Disables the FeliCa-DTE mode in the system and configures the test parameters.
 *
 * \param[in]   feliCaDTEComp           Pointer to an existing instance of the FeliCa-DTE component.
 * \param[in]   enableMode              Enables (!= 0) or Disables (0) the FeliCa-DTE mode.
 * \param[in]   testParams              Various test parameters for FeliCa compliance tests.
 *
 * \return Status, indicating whether the operation was successful.
 */
ptxStatus_t ptxFeliCa_DTE_EnableMode (ptxFeliCa_DTE_t *feliCaDTEComp, uint8_t enableMode, ptxFeliCa_DTE_TestParams_t *testParams);

/**
 * \brief Performs a FeliCa compliance test with given test parameters.
 *
 * \param[in]   feliCaDTEComp           Pointer to an existing instance of the FeliCa-DTE component.
 * \param[in]   testParams              Various test parameters for FeliCa compliance tests.
 *
 * \return Status, indicating whether the operation was successful. See ptxStatus_t.
 *
 */
ptxStatus_t ptxFeliCa_DTE_RunTest(ptxFeliCa_DTE_t *feliCaDTEComp, ptxFeliCa_DTE_TestParams_t *testParams);

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* Guard */

