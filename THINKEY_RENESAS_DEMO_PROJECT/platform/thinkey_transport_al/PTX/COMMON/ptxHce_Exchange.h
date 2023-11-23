/** \file
   ---------------------------------------------------------------
   Copyright (C) 2021. Panthronics AG - All Rights Reserved.

   This material may not be reproduced, displayed, modified or
   distributed without the express prior written permission of the
   Panthronics AG.

   PLEASE REFER TO OUR DISCLAIMER NOTICE IN THE PROJECT ROOT.
   ---------------------------------------------------------------

    Project     : PTX100R
    Module      : HCE API
    File        : ptxHce_Exchange.h

    Description : Data Processing Example Function for HCE.
*/

/**
 *
 * \addtogroup grp_ptx_api_hce_example PTX HCE EXAMPLE API
 * @{
 */
#ifndef COMPS_HCE_PTX_HCEEXCHANGE_H_
#define COMPS_HCE_PTX_HCEEXCHANGE_H_


/*
 * ####################################################################################################################
 * INCLUDES
 * ####################################################################################################################
 */

#include "ptxStatus.h"
#include "ptxHce.h"
#include "ptxT4T.h"

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
 * API FUNCTIONS
 * ####################################################################################################################
 */
/**
 * \brief HCE data processing function
 *
 * @param hce            Pointer to an allocated instance of HCE
 * @param tag            Pointer to an allocated instance of T4T.
 * @param exit_loop      Pointer to a variable to end this function
 * @return Status word, containing a success or error code.
 */
ptxStatus_t ptxHce_EmulateT4T (ptxHce_t *hce, ptxT4T_t *tag, uint8_t *exit_loop);

#ifdef __cplusplus
}
#endif

#endif /* Guard */

/** @} */

