/** \file
   ---------------------------------------------------------------
   Copyright (C) 2021. Panthronics AG - All Rights Reserved.

   This material may not be reproduced, displayed, modified or
   distributed without the express prior written permission of the
   Panthronics AG.

   PLEASE REFER TO OUR DISCLAIMER NOTICE IN THE PROJECT ROOT.
   ---------------------------------------------------------------

    Project     : PTX100R
    Module      : NDEF Defines
    File        : ptxNDEF_T5TOP.h

    Description : General Definition / Types for NDEF-Operations
*/

/**
 * \addtogroup grp_ptx_api_ndef_shared NDEF Defines
 *
 * @{
 */

#ifndef APIS_PTX_T5T_NDEF_DEFINES_H_
#define APIS_PTX_T5T_NDEF_DEFINES_H_

/*
 * ####################################################################################################################
 * INCLUDES
 * ####################################################################################################################
 */

#include <stdint.h>

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
 * \brief NDEF Tag Life-Cycle
 */
typedef enum ptxNDEF_TagLifeCycle
{
    TagLC_NoNDEFTag,
    TagLC_Initialized,
    TagLC_ReadWrite,
    TagLC_ReadOnly,

} ptxNDEF_TagLifeCycle_t;

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* Guard */

