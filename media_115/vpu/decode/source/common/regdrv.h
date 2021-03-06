/*------------------------------------------------------------------------------
--                                                                            --
--       This software is confidential and proprietary and may be used        --
--        only as expressly authorized by a licensing agreement from          --
--                                                                            --
--                            Hantro Products Oy.                             --
--                                                                            --
--                   (C) COPYRIGHT 2006 HANTRO PRODUCTS OY                    --
--                            ALL RIGHTS RESERVED                             --
--                                                                            --
--                 The entire notice above must be reproduced                 --
--                  on all copies and should not be removed.                  --
--                                                                            --
--------------------------------------------------------------------------------
--
--  Description :
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: regdrv.h,v $
--  $Revision: 1.20 $
--  $Date: 2010/09/07 06:47:34 $
--
------------------------------------------------------------------------------*/

#ifndef REGDRV_H
#define REGDRV_H

/*------------------------------------------------------------------------------
    Include headers
------------------------------------------------------------------------------*/

#include "basetype.h"

/*------------------------------------------------------------------------------
    Module defines
------------------------------------------------------------------------------*/

#define DEC_8170_IRQ_RDY            0x01
#define DEC_8170_IRQ_BUS            0x02
#define DEC_8170_IRQ_BUFFER         0x04
#define DEC_8170_IRQ_ASO            0x08
#define DEC_8170_IRQ_ERROR          0x10
#define DEC_8170_IRQ_SLICE          0x20
#define DEC_8170_IRQ_TIMEOUT        0x40

#define DEC_8190_IRQ_RDY            DEC_8170_IRQ_RDY
#define DEC_8190_IRQ_BUS            DEC_8170_IRQ_BUS
#define DEC_8190_IRQ_BUFFER         DEC_8170_IRQ_BUFFER
#define DEC_8190_IRQ_ASO            DEC_8170_IRQ_ASO
#define DEC_8190_IRQ_ERROR          DEC_8170_IRQ_ERROR
#define DEC_8190_IRQ_SLICE          DEC_8170_IRQ_SLICE
#define DEC_8190_IRQ_TIMEOUT        DEC_8170_IRQ_TIMEOUT

typedef enum
{
/* include script-generated part */
#include "8170enum.h"
    HWIF_DEC_IRQ_STAT,
    HWIF_PP_IRQ_STAT,
    HWIF_LAST_REG,

    /* aliases */
    HWIF_MPEG4_DC_BASE = HWIF_I4X4_OR_DC_BASE,
    HWIF_INTRA_4X4_BASE = HWIF_I4X4_OR_DC_BASE,
    /* VP6 */
    HWIF_VP6HWGOLDEN_BASE = HWIF_REFER4_BASE,
    HWIF_VP6HWPART1_BASE = HWIF_REFER13_BASE,
    HWIF_VP6HWPART2_BASE = HWIF_RLC_VLC_BASE,
    HWIF_VP6HWPROBTBL_BASE = HWIF_QTABLE_BASE,
    /* progressive JPEG */
    HWIF_PJPEG_COEFF_BUF = HWIF_DIR_MV_BASE,

    /* MVC */
    HWIF_INTER_VIEW_BASE = HWIF_REFER15_BASE

} hwIfName_e;

/*------------------------------------------------------------------------------
    Data types
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    Function prototypes
------------------------------------------------------------------------------*/

void SetDecRegister(u32 * regBase, u32 id, u32 value);
u32 GetDecRegister(const u32 * regBase, u32 id);

#endif /* #ifndef REGDRV_H */
