/**
 * @addtogroup platform_src_sirf_util_demo
 * @{
 */

 /**************************************************************************
 *                                                                         *
 *                   SiRF Technology, Inc. GPS Software                    *
 *                                                                         *
 *    Copyright (c) 2005-2009 by SiRF Technology, Inc. All rights reserved.*
 *                                                                         *
 *    This Software is protected by United States copyright laws and       *
 *    international treaties.  You may not reverse engineer, decompile     *
 *    or disassemble this Software.                                        *
 *                                                                         *
 *    WARNING:                                                             *
 *    This Software contains SiRF Technology Inc.�s confidential and       *
 *    proprietary information. UNAUTHORIZED COPYING, USE, DISTRIBUTION,    *
 *    PUBLICATION, TRANSFER, SALE, RENTAL OR DISCLOSURE IS PROHIBITED      *
 *    AND MAY RESULT IN SERIOUS LEGAL CONSEQUENCES.  Do not copy this      *
 *    Software without SiRF Technology, Inc.�s  express written            *
 *    permission.   Use of any portion of the contents of this Software    *
 *    is subject to and restricted by your signed written agreement with   *
 *    SiRF Technology, Inc.                                                *
 *                                                                         *
 ***************************************************************************
 *
 * MODULE:  HOST
 *
 * FILENAME:  sirf_demo.h
 *
 * DESCRIPTION: Routine prototypes and symbol definitions
 *
 *
 *
 *  Keywords for Perforce.  Do not modify.
 *
 *  $File: //customs/customer/Marvell-U1/sirf/Software/sirf/util/demo/sirf_demo.h $
 *
 *  $DateTime: 2011/07/29 13:26:23 $
 *
 *  $Revision: #1 $
 *
 ***************************************************************************/

#ifndef __SIRF_DEMO_H__
#define __SIRF_DEMO_H__

#include "sirf_types.h"
#include "sirf_pal.h"

/***************************************************************************
   Global Symbols
***************************************************************************/

#define SIRF_DEMO_VERSION   "4.0Eng"

/* Input flags for SIRFNAV_DEMO_Create */
#define SIRF_AUX_ENABLED    0x01
#define SIRF_LOG_ENABLED    0x02
#define SIRF_TCPIP_ENABLED  0x04
#define SIRF_AUX_OPEN_FAIL  0x08

typedef enum {
   G_MESSAGE_MODE_UNDEFINED = 0x00, 
   SIRFNAV_DEMO_PROTOCOL_SIRFLOC, 
   SIRFNAV_DEMO_PROTOCOL_SIRF, 
   SIRFNAV_DEMO_PROTOCOL_NMEA
} tMessage_Mode;

/* AUX Ports */
#if (SIRF_EXT_AUX >= 1) && (SIRF_EXT_AUX <= 3)
   #define MAX_AUX_PORTS      SIRF_EXT_AUX  /* maximum is 3 */
#else
   #define MAX_AUX_PORTS      1
#endif

/***************************************************************************
   Function Prototypes
***************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

tSIRF_UINT32 SIRFNAV_DEMO_Create( tSIRF_UINT8* port_str,tSIRF_UINT32 baud, 
                                  tSIRF_UINT16 tcpip_port,
                                  char *logname, tSIRF_UINT8 input_flags );
tSIRF_RESULT SIRFNAV_DEMO_Delete( tSIRF_VOID );
tSIRF_RESULT SIRFNAV_DEMO_Receive( tSIRF_UINT32 message_id, 
                                   tSIRF_VOID *message_structure, 
                                   tSIRF_UINT32 message_length );
tSIRF_BOOL SIRFNAV_DEMO_Running( tSIRF_VOID );

tSIRF_RESULT SIRFNAV_DEMO_Send_Passthrough( tSIRF_VOID *buffer, tSIRF_UINT32 PktLen );
tSIRF_VOID SIRFNAV_DEMO_Send( tSIRF_UINT32 message_id, 
                              tSIRF_VOID *message_structure, 
                              tSIRF_UINT32 message_length );

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __SIRF_DEMO_H__ */

/**
 * @}
 */
