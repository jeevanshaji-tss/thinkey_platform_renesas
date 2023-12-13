/** \file
   ---------------------------------------------------------------
   Copyright (C) 2021. Panthronics AG - All Rights Reserved.

   This material may not be reproduced, displayed, modified or
   distributed without the express prior written permission of the
   Panthronics AG.

   PLEASE REFER TO OUR DISCLAIMER NOTICE IN THE PROJECT ROOT.
   ---------------------------------------------------------------

    Project     : PTX100x
    Module      : PLAT
    File        : ptxPLAT_EXT.h

    Description : Common include for External #includes / 3rd Party Code
*/

#ifndef COMPS_PLAT_PTXPLATEXT_H_
#define COMPS_PLAT_PTXPLATEXT_H_


/*
 * ####################################################################################################################
 * INCLUDES
 * ####################################################################################################################
 */
#if 0
#if defined(__GNUC__) || defined(__GNUG__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wmissing-prototypes"
    #pragma GCC diagnostic ignored "-Wstrict-prototypes"
        #include "hal_data.h"
        #include "r_external_irq_api.h"
        #include "cmsis_gcc.h"
    #pragma GCC system_header
    #pragma GCC diagnostic pop
#else
    #include "hal_data.h"
    #include "r_external_irq_api.h"
    #include "cmsis_gcc.h"
#endif
#endif

#endif /* Guard */

