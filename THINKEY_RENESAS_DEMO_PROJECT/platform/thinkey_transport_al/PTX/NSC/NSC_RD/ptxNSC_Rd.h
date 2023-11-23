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
    File        : ptxNSC_Rd.h

    Description :
*/

#ifndef COMPS_NSC_NSC_RD_PTXNSC_RD_H_
#define COMPS_NSC_NSC_RD_PTXNSC_RD_H_

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
 * \brief Function used to check the presence check on ISO-DEP protocol. Mechanism NACK.
 *
 * \param[in]   nscCtx              Pointer to an initialized instance of the NSC.
 *
 * \return Status, indicating whether the operation was successful. See ptxStatus_t.
 *
 */
ptxStatus_t ptxNSC_Rd_RfPressCheck_Nack (struct ptxNSC *nscCtx);


/**
 * \brief Function used to check the presence check on ISO-DEP protocol. Mechanism Empty Frame.
 *
 * \param[in]   nscCtx              Pointer to an initialized instance of the NSC.
 *
 * \return Status, indicating whether the operation was successful. See ptxStatus_t.
 *
 */
ptxStatus_t ptxNSC_Rd_RfPressCheck_EmptyFrame (struct ptxNSC *nscCtx);


/**
 * \brief Function used to check the presence check on NFC-DEP protocol. Mechanism Empty Frame.
 *
 * \param[in]   nscCtx              Pointer to an initialized instance of the NSC.
 *
 * \return Status, indicating whether the operation was successful. See ptxStatus_t.
 *
 */
ptxStatus_t ptxNSC_Rd_RfPressCheck_AttentionCmd (struct ptxNSC *nscCtx);


/**
 * \brief T5T Isolated EoF.
 *
 * \param[in]       nscCtx              Pointer to the component structure.
 *
 * \return Status, indicating whether the operation was successful. See ptxStatus_t.
 *
 */
ptxStatus_t ptxNSC_T5T_IsolatedEoF(struct ptxNSC *nscCtx);

/**
 * \brief Function used to send NSC Rf Clt Msg to the chip.
 *
 * \param[in]   nscCtx              Pointer to an initialized instance of the NSC.
 * \param[in]   pldBytes            Pointer to the payload bytes.
 * \param[in]   pldParityBits       Pointer to the parity bits (as bytes) of the payload bytes.
 * \param[in]   length              Length of /ref pldBytes and /ref pldParityBits.
 *
 * \return Status, indicating whether the operation was successful. See ptxStatus_t.
 *
 */
ptxStatus_t ptxNSC_Rd_RfCltMsg (struct ptxNSC *nscCtx, uint8_t *pldBytes, uint8_t *pldParityBits, size_t length);

#ifdef __cplusplus
}
#endif

#endif /* Guard */

