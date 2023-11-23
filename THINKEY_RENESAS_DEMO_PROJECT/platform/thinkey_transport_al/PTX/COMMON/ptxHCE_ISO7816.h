/** \file
   ---------------------------------------------------------------
   Copyright (C) 2021. Panthronics AG - All Rights Reserved.

   This material may not be reproduced, displayed, modified or
   distributed without the express prior written permission of the
   Panthronics AG.

   PLEASE REFER TO OUR DISCLAIMER NOTICE IN THE PROJECT ROOT.
   ---------------------------------------------------------------

    Project     : PTX100x
    Module      : ISO-7816 Support-Header
    File        : ptxHCE_ISO7816.h

    Description :
*/

#ifndef PTX_HCE_ISO7816_H
#define PTX_HCE_ISO7816_H

/*
 * ####################################################################################################################
 * INCLUDE
 * ####################################################################################################################
 */
#include <stdint.h>

/**
 * INTERFACE
 */

/*
 * ####################################################################################################################
 * DEFINITIONS
 * ####################################################################################################################
 */

/**
 * \brief ISO-7816 INS Standard Definitions.
 */
#define PTX_HCE_ISO7816_INS_SELECT                         (uint8_t)0xA4
#define PTX_HCE_ISO7816_INS_READ_BINARY                                (uint8_t)0xB0
#define PTX_HCE_ISO7816_INS_UPDATE_BINARY                              (uint8_t)0xD6
#define PTX_HCE_ISO7816_SELECT_OPTION_P1_BY_NAME                       (uint8_t)0x04
#define PTX_HCE_ISO7816_SELECT_OPTION_P2_NAME_FIRST_ONLY_OCC           (uint8_t)0x00
#define PTX_HCE_ISO7816_SELECT_OPTION_P1_BY_FILEID                     (uint8_t)0x00
#define PTX_HCE_ISO7816_SELECT_OPTION_P2_FILEID_FIRST_ONLY_OCC         (uint8_t)0x0C

/**
 * \brief ISO-7816 CLA Definitions.
 */
#define PTX_HCE_ISO7816_CLA_ISO                            (uint8_t)0x00

/**
 * \brief ISO-7816 Status Word Definitions.
 */
#define PTX_HCE_ISO7816_SW_LEN                             (uint8_t)0x02
#define PTX_HCE_ISO7816_SW_SUCCESS                         (uint16_t)0x9000
#define PTX_HCE_ISO7816_SW_BYTES_REMAINING_00              (uint16_t)0x6100
#define PTX_HCE_ISO7816_SW_WRONG_LENGTH                    (uint16_t)0x6700
#define PTX_HCE_ISO7816_SW_SECURITY_STATUS_NOT_SATISFIED   (uint16_t)0x6982
#define PTX_HCE_ISO7816_SW_FILE_INVALID                    (uint16_t)0x6983
#define PTX_HCE_ISO7816_SW_DATA_INVALID                    (uint16_t)0x6984
#define PTX_HCE_ISO7816_SW_CONDITIONS_NOT_SATISFIED        (uint16_t)0x6985
#define PTX_HCE_ISO7816_SW_COMMAND_NOT_ALLOWED             (uint16_t)0x6986
#define PTX_HCE_ISO7816_SW_APPLET_SELECT_FAILED            (uint16_t)0x6999
#define PTX_HCE_ISO7816_SW_FUNC_NOT_SUPPORTED              (uint16_t)0x6A81
#define PTX_HCE_ISO7816_SW_FILE_NOT_FOUND                  (uint16_t)0x6A82
#define PTX_HCE_ISO7816_SW_RECORD_NOT_FOUND                (uint16_t)0x6A83
#define PTX_HCE_ISO7816_SW_INCORRECT_P1P2                  (uint16_t)0x6A86
#define PTX_HCE_ISO7816_SW_WRONG_P1P2                      (uint16_t)0x6B00
#define PTX_HCE_ISO7816_SW_CORRECT_LENGTH_00               (uint16_t)0x6C00
#define PTX_HCE_ISO7816_SW_INS_NOT_SUPPORTED               (uint16_t)0x6D00
#define PTX_HCE_ISO7816_SW_CLA_NOT_SUPPORTED               (uint16_t)0x6E00
#define PTX_HCE_ISO7816_SW_UNKNOWN                         (uint16_t)0x6F00
#define PTX_HCE_ISO7816_SW_FILE_FULL                       (uint16_t)0x6A84
#define PTX_HCE_ISO7816_SW_LOGICAL_CHANNEL_NOT_SUPPORTED   (uint16_t)0x6881
#define PTX_HCE_ISO7816_SW_SECURE_MESSAGING_NOT_SUPPORTED  (uint16_t)0x6882
#define PTX_HCE_ISO7816_SW_WARNING_STATE_UNCHANGED         (uint16_t)0x6200
#define PTX_HCE_ISO7816_SW_SELECTED_FILE_DEACTIVATED       (uint16_t)0x6283
#define PTX_HCE_ISO7816_SW_NO_INPUT_DATA_AVAILABLE         (uint16_t)0x6286

/**
 * \brief ISO-7816 Offsets for Processing.
 */
#define PTX_HCE_ISO7816_HEADER_LEN                         (uint8_t)0x04
#define PTX_HCE_ISO7816_OFFSET_CLA                         (uint8_t)0x00
#define PTX_HCE_ISO7816_OFFSET_INS                         (uint8_t)0x01
#define PTX_HCE_ISO7816_OFFSET_P1                          (uint8_t)0x02
#define PTX_HCE_ISO7816_OFFSET_P2                          (uint8_t)0x03
#define PTX_HCE_ISO7816_OFFSET_LC                          (uint8_t)0x04
#define PTX_HCE_ISO7816_OFFSET_CDATA                       (uint8_t)0x05
#define PTX_HCE_ISO7816_OFFSET_EXT_CDATA                   (uint8_t)0x07
#define PTX_HCE_ISO7816_OFFSET_LE                          (uint8_t)0x04   /* only valid for C-APDUs without data (!) */

#endif /* Guard */

