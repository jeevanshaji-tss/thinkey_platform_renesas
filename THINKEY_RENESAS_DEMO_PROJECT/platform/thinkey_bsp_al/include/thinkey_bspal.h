/*
 * thinkey_bspal.h
 *
 *  Created on: Sep 27, 2021
 *      Author: sanje
 */

#ifndef THINKEY_BSPAL_H
#define THINKEY_BSPAL_H
#include "thinkey_platform_types.h"

/**
 * \brief   This function initializes platform BSP.
 */
THINKey_eStatusType THINKey_eBSPInit(void);

/**
 * \brief   This function sets ble interrupt.
 */
THINKey_VOID THINKey_BSPAL_eSetBleInterrupt(THINKey_HANDLE hBleIrqConfig);

#define THINKEY_ASSERT(VALUE)


#endif /* THINKEY_BSPAL_H */
