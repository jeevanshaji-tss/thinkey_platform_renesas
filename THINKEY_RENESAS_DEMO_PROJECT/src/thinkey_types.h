/*
 * THINKey Digital Key Framework for Vehicle
 * Copyright (C) 2022 ThinkSeed Systems Pvt Ltd.  All Rights Reserved.
 */

/**
 * @file thinkey_types.h
 * @author Sanjeev M
 * @date 25 Sep 2021
 * @brief THINKey type definitions.
 *
 * This Header File defines all type definitions needed for application that
 * uses THINKey Framework. THINKey Framework is an implementation of the 
 * CCC Digital Key Standard, from ThinkSeed Systems
 *
 */

#ifndef THINKEY_TYPES_H
#define THINKEY_TYPES_H

#include "thinkey_platform_types.h"

#define ALLKEYS_KEY_HANDLE 0xFFFFFFFF

/**
 * @brief Enumeration of all the Transport Types supported by
 * the THINKey Framework 
 *
 * Transport denotes the Primary Medium of Communication in the Vehicle 
 * Access System between the Vehicle and the Device
 */
typedef enum
{
  E_TKEY_BLE, /**< BLE connected local to the Vehicle Access Main ECU*/

  E_TKEY_REMOTE_BLE, /**<< BLE is connected remotely to the 
                       Vehicle Access Main MCU usually over CAN*/

  E_TKEY_EXTERIOR_NFC, /**< Exterior NFC (B-pillar or Door or Other) that
                         shall be usually connected remotely to the Vehicle 
                         Access Main MCU usually over CAN */

  E_TKEY_INTERIOR_NFC, /**< Interior NFC that is connected local to the Vehicle
                        Access Main ECU */

  E_TKEY_INTERIOR_REMOTE_NFC, /**< Interior NFC that is connected remotely
                                to the Vehicle Access Main MCU usally over CAN */

  E_TKEY_INVALID_TRANSPORT = 0xFFFFFFFF, /**< ENUM denoting an INVALID
                                          transport */
  E_THINKEY_TRANSPORT_BLE = E_TKEY_BLE,

  E_THINKEY_TRANSPORT_NFC = E_TKEY_INTERIOR_NFC,

  E_THINKEY_TRANSPORT_INVALID = E_TKEY_INVALID_TRANSPORT

}TKey_Transport_t;

/**
 * @brief Enumeration of all the Ranging Methods supported by
 * the THINKey Framework 
 */
typedef enum
{
  E_TKEY_UWB_CCC, /**< Ranging group consists of UWB Anchors and will
                   be used to do Fine Ranging. 
                Vehicle Access Main ECU */
  
  E_TKEY_BLE_RSSI, /**< UWB Anchor for Ranging will be used to
                      do Coarse Ranging */

  E_TKEY_INVALID_RANGING = 0xFFFFFFFF, /**< ENUM denoting an INVALID Ranging 
                                        method */

  E_THINKEY_RANGING_UWB = E_TKEY_UWB_CCC

}TKey_RangingMethod_t;

/**
 * @brief Enum defining what kind of Authentication is done. Also if
 * Authentication was a failure.
 */
typedef enum
{
  TKEY_QUICK_AUTHENTICATION,/**< Quick Authentication done. 
                              (E.g., Fast Transaction in CCC)*/
  TKEY_NORMAL_AUTHENTICATION,/**< Normal Authencitation done.
                               (E.g., Standard Transaction in CCC) */
  TKEY_QUICK_AUTHENTICATION_FAILURE = 0xFFFFFFFE, /**<Quick Authentication 
                                                    Failed */
  TKEY_NORMAL_AUTHENTICATION_FAILURE = 0xFFFFFFFF /**<Normal Authentication 
                                                    Failed */
}TKey_AuthType_t;


/**
 * @brief Enum that defines the rate of polling that needs to be done to find
 * a new Key.
 */
typedef enum
{
  TKEY_POLLING_INTERVAL_FAST, /**< FAST polling interval selection. The absolute
                                value of the polling interval will depend on the
                                transport */
  TKEY_POLLING_INTERVAL_NORMAL, /**< Normal polling interval selection. 
                                  This will be a setting midway from Slow 
                                  to Fast*/
  TKEY_POLLING_INTERVAL_SLOW /** < Slow polling interval selection. This will 
                                   be the least polling interval that could be
                                   selected */
}TKey_PollingRate_t;


/**
 * @brief Enum that defines Remote Keyless Entry Events
 */
typedef enum {
  E_TKEY_RKE_REQUEST_LOCK, /**< RKE lock request */
  E_TKEY_RKE_REQUEST_UNLOCK, /**< RKE unlock request */
  E_TKEY_RKE_REQUEST_TRIGGER_ALARM, /**< RKE trigger alarm request */
  E_TKEY_RKE_REQUEST_MUTE_ALARM /**< RKE mute alarm request */

} TKey_RKEEvent_t;

/**
 * @brief Range Info structure in which the Range details shall be provided to
 * the Application
 *
 */
typedef struct
{
  TKey_UINT32 uiAnchorID;/**< Anchor ID for which the Range is provided */
  TKey_UINT32 uiRange; /**< Measured Range in milli metres */
  TKey_INT32  iAngleOfArrival; /**< Measured Angle of Arrival in degrees */
}TKey_RangeInfo_t;

/**
 * @brief Anchor Ranges structure to consolidate the Range info from multiple Anchors
 *
 */
typedef struct
{
  TKey_UINT32 uiNumAnchors; /**< Number of Anchors for which the Range is being
                              provided by the THINKey Framework using the
                              @c TKey_CB_pfnMeasuredRange Callback */
  TKey_UINT32 uiRangingGroupID; /**< Group ID which was allocated for this 
                                  Ranging group by Application in 
                                  Configuration */
  TKey_RangeInfo_t *psRangeInfo; /**< Range Info for each of the
                                             Anchors */
}TKey_AnchorRanges_t;

/**
 * @brief Enum that defines Device's approach confidence
 */
typedef enum {
  E_TKEY_DEVICE_APPROACH_CONFIDENCE_LOW, /**< Low approch confidence.
                                          App can start ranging, if needed */
  E_TKEY_DEVICE_APPROACH_CONFIDENCE_MEDIUM, /**< Medium approch confidence.
                                            App can start ranging, if needed */
  E_TKEY_DEVICE_APPROACH_CONFIDENCE_HIGH, /**< High approch confidence.
                                          App should start Ranging, it is is already not started */
} TKey_DeviceApproachConfidence_t;

/**
 * @brief Enum that defines Device's Ranging session status
 */
typedef enum {
  E_TKEY_DEVICE_RANGING_SESSION_TERMINATED, /**< Device has stopped ranging session */

} TKey_DeviceRangingSessionStatus_t;

/* TBD: These are to be eventually removed. We have these here to avoid
 * compilation issues. */
typedef TKey_RangingMethod_t THINKey_eRangingMethodType;
typedef TKey_Transport_t THINKey_eTransportType;
typedef TKey_Transport_t TKey_eTransportType;

#endif /* THINKEY_TYPES_H */
