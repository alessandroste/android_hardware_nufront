/**
 * @addtogroup platform_src_sirf_pal
 * @{
 */

/*
 *                   SiRF Technology, Inc. GPS Software
 *
 *    Copyright (c) 2005-2009 by SiRF Technology, Inc.  All rights reserved.
 *
 *    This Software is protected by United States copyright laws and
 *    international treaties.  You may not reverse engineer, decompile
 *    or disassemble this Software.
 *
 *    WARNING:
 *    This Software contains SiRF Technology Inc.�s confidential and
 *    proprietary information. UNAUTHORIZED COPYING, USE, DISTRIBUTION,
 *    PUBLICATION, TRANSFER, SALE, RENTAL OR DISCLOSURE IS PROHIBITED
 *    AND MAY RESULT IN SERIOUS LEGAL CONSEQUENCES.  Do not copy this
 *    Software without SiRF Technology, Inc.�s  express written
 *    permission.   Use of any portion of the contents of this Software
 *    is subject to and restricted by your signed written agreement with
 *    SiRF Technology, Inc.
 *
 */

/**
 * @file   sirf_pal_hw_reset.h
 *
 * @brief  SiRF PAL hardware reset API.
 */

#ifndef SIRF_PAL_HW_RESET_H_INCLUDED
#define SIRF_PAL_HW_RESET_H_INCLUDED

#include "sirf_types.h"

#define SIRF_PAL_HW_RESET_HIGH 1
#define SIRF_PAL_HW_RESET_LOW  0

/* ----------------------------------------------------------------------------
 *   Function Prototypes
 * ------------------------------------------------------------------------- */

/* Enter C naming convention */
#ifdef __cplusplus
extern "C" {
#endif

tSIRF_RESULT SIRF_PAL_HW_WriteRESET( tSIRF_UINT32 level );
tSIRF_RESULT SIRF_PAL_HW_OpenRESET( tSIRF_UINT32 level );
tSIRF_RESULT SIRF_PAL_HW_CloseRESET( void );

/* Leave C naming convention */
#ifdef __cplusplus
}
#endif


#endif /* !SIRF_PAL_HW_RESET_H_INCLUDED */

/**
 * @}
 * End of file.
 */


