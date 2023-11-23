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
    File        : ptxNSC_Notifications.h

    Description :
*/

#ifndef COMPS_NSC_PTXNSC_NOTIFICATIONS_H_
#define COMPS_NSC_PTXNSC_NOTIFICATIONS_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Callback function for Waiting For Rx Events (asynchronously).
 *
 */
struct ptxNSC;

/**
 * \note Internal function used as interface for the different variants for the different diversities on the handling of Notifications or Transparent messages
 */
void ptxNSC_ProcessNtf ( struct ptxNSC *nscCtx, uint8_t *ntfBuff, size_t ntfBuffLen);

#ifdef __cplusplus
}
#endif

#endif /* Guard */

