/** \file
   ---------------------------------------------------------------
   Copyright (C) 2021. Panthronics AG - All Rights Reserved.

   This material may not be reproduced, displayed, modified or
   distributed without the express prior written permission of the
   Panthronics AG.

   PLEASE REFER TO OUR DISCLAIMER NOTICE IN THE PROJECT ROOT.
   ---------------------------------------------------------------

    Project     : PTX100x
    Module      : NSC
    File        : ptxNSC_CE.h

    Description :
*/

#ifndef COMPS_NSC_NSC_CE_PTXNSC_CE_H_
#define COMPS_NSC_NSC_CE_PTXNSC_CE_H_

/*
 * ####################################################################################################################
 * INCLUDES
 * ####################################################################################################################
 */
#include "ptxStatus.h"
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * ####################################################################################################################
 * DEFINES / TYPES
 * ####################################################################################################################
 */

/*
 * Forward declaration
 */
struct ptxNSC;

/*
 * ####################################################################################################################
 * API FUNCTIONS
 * ####################################################################################################################
 */

/**
 * \brief Function used to set up Routing Table for HCE application. All protocols and technologies go to Host.
 *
 * \param[in]   nscCtx              Pointer to an initialized instance of the NSC.
 *
 * \return Status, indicating whether the operation was successful. See ptxStatus_t.
 *
 */
ptxStatus_t ptxNSC_Set_Listen_RoutingTable_HCE ( struct ptxNSC *nscCtx );

#ifdef __cplusplus
}
#endif

#endif /* Guard */

