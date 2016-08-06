/**
 * @addtogroup platform_src_sirf_util_codec
 * @ingroup host
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
 * FILENAME:  sirf_codec.h
 *
 * DESCRIPTION: Routine prototypes and symbol definitions
 *
 ***************************************************************************/

#ifndef __SIRF_CODEC_H__
#define __SIRF_CODEC_H__

#include "sirf_errors.h"
#include "sirf_types.h"


/* The codec options:
 * On input the lower 8 bits is used to determine which message to encode or
 * decode from the input on a one to many codec.  The upper bits are reserved
 * for additional options.  Each codec will define the message ID values if
 * they deviate from the default of a 1 to 1 encode and decode defined here
 */
#define SIRF_CODEC_OPTIONS_GET_FIRST_MSG (0x00000001)
#define SIRF_CODEC_OPTIONS_ENCODE_ASCII  (0x80000000)
#define SIRF_CODEC_OPTIONS_NO_WRAPPER    (0x40000000)

/* On output the Options value holds the total number of messages that can
 * be produced from the input message and which message number this is */
#define SIRF_CODEC_OPTIONS_GET_MSG_NUMBER( n )       (((n)>>8) & 0x000000FF)
#define SIRF_CODEC_OPTIONS_GET_MAX_MSG_NUMBER( n )   ( (n)     & 0x000000FF)

#define SIRF_CODEC_OPTIONS_MAKE_MSG_NUMBER( num, max ) \
       ((num << 8) | max)


typedef tSIRF_RESULT (*tSIRF_CODEC_Encode)( tSIRF_UINT32  message_id, 
                                            tSIRF_VOID   *message_structure, 
                                            tSIRF_UINT32  message_length,
                                            tSIRF_UINT8  *packet, 
                                            tSIRF_UINT32 *packet_length,
                                            tSIRF_UINT32 *options );

typedef tSIRF_RESULT (*tSIRF_CODEC_Decode)( tSIRF_UINT8  *payload, 
                                            tSIRF_UINT32  payload_length,
                                            tSIRF_UINT32 *message_id, 
                                            tSIRF_VOID   *message_structure, 
                                            tSIRF_UINT32 *message_length,
                                            tSIRF_UINT32 *options );




/* Stream converters ------------------------------------------------------- */

#define SIRF_SWAPIN64(bytestream)\
   ( ((tSIRF_UINT64)*((bytestream)+0) << 56)\
   | ((tSIRF_UINT64)*((bytestream)+1) << 48)\
   | ((tSIRF_UINT64)*((bytestream)+2) << 40)\
   | ((tSIRF_UINT64)*((bytestream)+3) << 32)\
   | ((tSIRF_UINT64)*((bytestream)+4) << 24)\
   | ((tSIRF_UINT64)*((bytestream)+5) << 16)\
   | ((tSIRF_UINT64)*((bytestream)+6) <<  8)\
   | ((tSIRF_UINT64)*((bytestream)+7)      ))

#define SIRF_SWAPIN32(bytestream)\
   ( ((tSIRF_UINT32)*((bytestream)+0) << 24)\
   | ((tSIRF_UINT32)*((bytestream)+1) << 16)\
   | ((tSIRF_UINT32)*((bytestream)+2) <<  8)\
   | ((tSIRF_UINT32)*((bytestream)+3)      ))

#define SIRF_SWAPIN16(bytestream)\
   (((tSIRF_UINT16)*((bytestream)+0) << 8)\
   | ((tSIRF_UINT16)*((bytestream)+1)    ))

/* This does sign extension */
#define SIRF_SWAPIN_S24(bytestream)\
     ((tSIRF_INT32)(   ((tSIRF_UINT32)*((bytestream)+0) << 24)\
                    | ((tSIRF_UINT32)*((bytestream)+1) << 16)\
                    | ((tSIRF_UINT32)*((bytestream)+2) <<  8)) / 0x100)

#define SIRF_SWAPIN_U24(bytestream)\
     (((tSIRF_UINT32)*((bytestream)+0) << 16)\
     | ((tSIRF_UINT32)*((bytestream)+1) << 8)\
     | ((tSIRF_UINT32)*((bytestream)+2)   ))

/* Double format out is bits 31..0 then 63..32 per GSW3.x spec */
#define SIRF_SWAPOUT_DOUBLE(hostint64, bytestream) {\
   const tSIRF_UINT8 *hostbyte = (const tSIRF_UINT8*)&(hostint64) + 7;\
     (*((bytestream)++)) = *(hostbyte-4);\
     (*((bytestream)++)) = *(hostbyte-5);\
     (*((bytestream)++)) = *(hostbyte-6);\
     (*((bytestream)++)) = *(hostbyte-7);\
     (*((bytestream)++)) = *(hostbyte);  \
     (*((bytestream)++)) = *(hostbyte-1);\
     (*((bytestream)++)) = *(hostbyte-2);\
     (*((bytestream)++)) = *(hostbyte-3); }

/* Currently not used but availabe for integer 64 big endian export */
#define SIRF_SWAPOUT64(hostint64, bytestream) {\
   const tSIRF_UINT8 *hostbyte = (const tSIRF_UINT8*)&(hostint64) + 7;\
   (*((bytestream)++)) = *(hostbyte--);\
   (*((bytestream)++)) = *(hostbyte--);\
   (*((bytestream)++)) = *(hostbyte--);\
   (*((bytestream)++)) = *(hostbyte--);\
   (*((bytestream)++)) = *(hostbyte--);\
   (*((bytestream)++)) = *(hostbyte--);\
   (*((bytestream)++)) = *(hostbyte--);\
   (*((bytestream)++)) = *(hostbyte); }

#define SIRF_SWAPOUT24(hostlong, bytestream) {\
   const tSIRF_UINT8 *hostbyte = (const tSIRF_UINT8*)&(hostlong) + 2;\
   (*((bytestream)++)) = *(hostbyte--);\
   (*((bytestream)++)) = *(hostbyte--);\
   (*((bytestream)++)) = *(hostbyte); }

#define SIRF_SWAPOUT32(hostlong, bytestream) {\
   const tSIRF_UINT8 *hostbyte = (const tSIRF_UINT8*)&(hostlong) + 3;\
   (*((bytestream)++)) = *(hostbyte--);\
   (*((bytestream)++)) = *(hostbyte--);\
   (*((bytestream)++)) = *(hostbyte--);\
   (*((bytestream)++)) = *(hostbyte); }

#define SIRF_SWAPOUT16(hostshort, bytestream) {\
   (*((bytestream)++)) = *((const tSIRF_UINT8*)&(hostshort)+1);\
   (*((bytestream)++)) = *((const tSIRF_UINT8*)&(hostshort)); }

#define SIRF_COPYIN64(bytestream)\
   ( ((tSIRF_UINT64)*((bytestream)+0) << 56)\
   | ((tSIRF_UINT64)*((bytestream)+1) << 48)\
   | ((tSIRF_UINT64)*((bytestream)+2) << 40)\
   | ((tSIRF_UINT64)*((bytestream)+3) << 32)\
   | ((tSIRF_UINT64)*((bytestream)+4) << 24)\
   | ((tSIRF_UINT64)*((bytestream)+5) << 16)\
   | ((tSIRF_UINT64)*((bytestream)+6) <<  8)\
   | ((tSIRF_UINT64)*((bytestream)+7)      ))

#define SIRF_COPYIN32(bytestream)\
   ( ((tSIRF_UINT32)*((bytestream)+0) << 24)\
   | ((tSIRF_UINT32)*((bytestream)+1) << 16)\
   | ((tSIRF_UINT32)*((bytestream)+2) <<  8)\
   | ((tSIRF_UINT32)*((bytestream)+3)      ))

#define SIRF_COPYIN16(bytestream)\
   (((tSIRF_UINT16)*((bytestream)+0) <<  8 )\
   | ((tSIRF_UINT16)*((bytestream)+1)))

/* This does sign extension */
#define SIRF_COPYIN_S24(bytestream)\
     ((tSIRF_INT32)(  ((tSIRF_UINT32)*((bytestream)+0) << 24)\
                    | ((tSIRF_UINT32)*((bytestream)+1) << 16)\
                    | ((tSIRF_UINT32)*((bytestream)+2) <<8   )) / 0x100)

#define SIRF_COPYIN_U24(bytestream)\
     (((tSIRF_UINT32)*((bytestream)+0) << 16)\
     | ((tSIRF_UINT32)*((bytestream)+1) << 8)\
     | ((tSIRF_UINT32)*((bytestream)+2)   ))

/* Double format out is bits 31..0 then 63..32 per GSW3.x spec */
#define SIRF_COPYOUT_DOUBLE(hostint64, bytestream) {\
   const tSIRF_UINT8 *hostbyte = (const tSIRF_UINT8*)&(hostint64);\
   (*((bytestream)++)) = (hostbyte[4]);\
   (*((bytestream)++)) = (hostbyte[5]);\
   (*((bytestream)++)) = (hostbyte[6]);\
   (*((bytestream)++)) = (hostbyte[7]);\
   (*((bytestream)++)) = (hostbyte[0]);\
   (*((bytestream)++)) = (hostbyte[1]);\
   (*((bytestream)++)) = (hostbyte[2]);\
   (*((bytestream)++)) = (hostbyte[3]); }

#define SIRF_COPYOUT64(hostint64, bytestream) {\
   const tSIRF_UINT8 *hostbyte = (const tSIRF_UINT8*)&(hostint64);\
   (*((bytestream)++)) = *(hostbyte++);\
   (*((bytestream)++)) = *(hostbyte++);\
   (*((bytestream)++)) = *(hostbyte++);\
   (*((bytestream)++)) = *(hostbyte++);\
   (*((bytestream)++)) = *(hostbyte++);\
   (*((bytestream)++)) = *(hostbyte++);\
   (*((bytestream)++)) = *(hostbyte++);\
   (*((bytestream)++)) = *(hostbyte); }

#define SIRF_COPYOUT32(hostlong, bytestream) {\
   const tSIRF_UINT8 *hostbyte = (const tSIRF_UINT8*)&(hostlong);\
   (*((bytestream)++)) = *(hostbyte++);\
   (*((bytestream)++)) = *(hostbyte++);\
   (*((bytestream)++)) = *(hostbyte++);\
   (*((bytestream)++)) = *(hostbyte); }

#define SIRF_COPYOUT24(hostlong, bytestream) {\
   const tSIRF_UINT8 *hostbyte = (const tSIRF_UINT8*)&(hostlong);\
   (*((bytestream)++)) = *(hostbyte++);\
   (*((bytestream)++)) = *(hostbyte++);\
   (*((bytestream)++)) = *(hostbyte); }

#define SIRF_COPYOUT16(hostshort, bytestream) {\
   (*((bytestream)++)) = *((const tSIRF_UINT8*)&(hostshort));\
   (*((bytestream)++)) = *((const tSIRF_UINT8*)&(hostshort)+1); }

#if defined (_ENDIAN_LITTLE)
   /* import macros for little endian: */
   /* NOTE: must use {} around these macros when calling in a loop */
#   define SIRFBINARY_IMPORT_UINT8(bytestream) (                        *((bytestream)++))
#   define SIRFBINARY_IMPORT_UINT16(bytestream) ((tSIRF_UINT16) SIRF_SWAPIN16((bytestream))); bytestream+=2
#   define SIRFBINARY_IMPORT_UINT24(bytestream) ((tSIRF_UINT32) SIRF_SWAPIN_U24((bytestream))); bytestream+=3

#   define SIRFBINARY_IMPORT_UINT32(bytestream) ((tSIRF_UINT32) SIRF_SWAPIN32((bytestream))); bytestream+=4
#   define SIRFBINARY_IMPORT_UINT64(bytestream) ((tSIRF_UINT64) SIRF_SWAPIN64((bytestream))); bytestream+=8
#   define SIRFBINARY_IMPORT_SINT8(bytestream) ((tSIRF_INT8)            (*((bytestream)++)))
#   define SIRFBINARY_IMPORT_SINT16(bytestream) ((tSIRF_INT16) SIRF_SWAPIN16((bytestream))); bytestream+=2
#   define SIRFBINARY_IMPORT_SINT24(bytestream) ((tSIRF_INT32) SIRF_SWAPIN_S24((bytestream))); bytestream+=3
#   define SIRFBINARY_IMPORT_SINT32(bytestream) ((tSIRF_INT32) SIRF_SWAPIN32((bytestream))); bytestream+=4
#   define SIRFBINARY_IMPORT_SINT64(bytestream) ((tSIRF_INT32) SIRF_SWAPIN64((bytestream))); bytestream+=8

#   define SIRFBINARY_IMPORT_FLOAT(hostfloat_i, bytestream_i) {\
      tSIRF_UINT8 *hostbyte_i = (tSIRF_UINT8*)&(hostfloat_i);\
      SIRF_SWAPOUT32(*(bytestream_i), hostbyte_i);\
      (bytestream_i)+=4; }

#   define SIRFBINARY_IMPORT_DOUBLE(hostdouble_i, bytestream_i) {\
      tSIRF_UINT8 *hostbyte_i = (tSIRF_UINT8*)&(hostdouble_i);\
      SIRF_SWAPOUT_DOUBLE(*(bytestream_i), hostbyte_i);\
      (bytestream_i)+=8; }

#   define SIRFBINARY_EXPORT8(src, dst) (*((dst)++) = (tSIRF_UINT8)(src))
#   define SIRFBINARY_EXPORT16(src, dst) SIRF_SWAPOUT16((src),(dst))
#   define SIRFBINARY_EXPORT24(src, dst) SIRF_SWAPOUT24((src),(dst))
#   define SIRFBINARY_EXPORT32(src, dst) SIRF_SWAPOUT32((src),(dst))
#   define SIRFBINARY_EXPORT64(src, dst) SIRF_SWAPOUT64((src),(dst))
#   define SIRFBINARY_EXPORT_DOUBLE(src, dst) SIRF_SWAPOUT_DOUBLE((src),(dst))

#elif defined (_ENDIAN_BIG)
   /* import macros for big endian: */
   /* NOTE: must use {} around these macros when calling in a loop */
#   define SIRFBINARY_IMPORT_UINT8( bytestream) (                        *((bytestream)++))
#   define SIRFBINARY_IMPORT_UINT16(bytestream) ((tSIRF_UINT16) SIRF_COPYIN16((bytestream))); bytestream+=2
#   define SIRFBINARY_IMPORT_UINT24(bytestream) ((tSIRF_UINT16) SIRF_COPYIN_U24((bytestream))); bytestream+=2
#   define SIRFBINARY_IMPORT_UINT32(bytestream) ((tSIRF_UINT32) SIRF_COPYIN32((bytestream))); bytestream+=4
#   define SIRFBINARY_IMPORT_UINT64(bytestream) ((tSIRF_UINT64) SIRF_COPYIN64((bytestream))); bytestream+=8
#   define SIRFBINARY_IMPORT_SINT8( bytestream) ((tSIRF_INT8)            (*((bytestream)++)))
#   define SIRFBINARY_IMPORT_SINT16(bytestream) ((tSIRF_INT16) SIRF_COPYIN16((bytestream))); bytestream+=2
#   define SIRFBINARY_IMPORT_SINT24(bytestream) ((tSIRF_INT16) SIRF_COPYIN_S24((bytestream))); bytestream+=2
#   define SIRFBINARY_IMPORT_SINT32(bytestream) ((tSIRF_INT32) SIRF_COPYIN32((bytestream))); bytestream+=4
#   define SIRFBINARY_IMPORT_SINT64(bytestream) ((tSIRF_INT64) SIRF_COPYIN64((bytestream))); bytestream+=8

#   define SIRFBINARY_IMPORT_FLOAT(hostfloat_i, bytestream_i) {\
      tSIRF_UINT8 *hostbyte_i = (tSIRF_UINT8*)&(hostfloat_i);\
      SIRF_COPYOUT32(*(bytestream_i), hostbyte_i);\
      (bytestream_i)+=4; }

#   define SIRFBINARY_IMPORT_DOUBLE(hostdouble_i, bytestream_i) {\
      tSIRF_UINT8 *hostbyte_i = (tSIRF_UINT8*)&(hostdouble_i);\
      SIRF_COPYOUT_DOUBLE(*(bytestream_i), hostbyte_i);\
      (bytestream_i)+=8; }

#   define SIRFBINARY_EXPORT8( src, dst) (*((dst)++) = (tSIRF_UINT8)(src))
#   define SIRFBINARY_EXPORT16(src, dst) SIRF_COPYOUT16((src),(dst))
#   define SIRFBINARY_EXPORT24(src, dst) SIRF_COPYOUT24((src),(dst))
#   define SIRFBINARY_EXPORT32(src, dst) SIRF_COPYOUT32((src),(dst))
#   define SIRFBINARY_EXPORT64(src, dst) SIRF_COPYOUT64((src),(dst))
#   define SIRFBINARY_EXPORT_DOUBLE(src, dst) SIRF_COPYOUT_DOUBLE((src),(dst))

#else
   #error Endian not selected
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------------
 * Import functions
 -----------------------------------------------------------------------------*/
tSIRF_UINT32 ImportUINT8( tSIRF_UINT8 *Dst, 
                          tSIRF_UINT8 **pSrc, 
                          tSIRF_BOOL ascii_packet );
tSIRF_UINT32 ImportUINT16( tSIRF_UINT16 *Dst, 
                           tSIRF_UINT8 **pSrc, 
                           tSIRF_BOOL ascii_packet );
tSIRF_UINT32 ImportINT16( tSIRF_INT16 *Dst,
                          tSIRF_UINT8 **pSrc,
                          tSIRF_BOOL ascii_packet );
tSIRF_UINT32 ImportUINT24( tSIRF_UINT32 *Dst,
                           tSIRF_UINT8 **pSrc,
                           tSIRF_BOOL ascii_packet );
tSIRF_UINT32 ImportINT24( tSIRF_INT32 *Dst, 
                          tSIRF_UINT8 **pSrc, 
                          tSIRF_BOOL ascii_packet );
tSIRF_UINT32 ImportUINT32( tSIRF_UINT32 *Dst,
                           tSIRF_UINT8 **pSrc,
                           tSIRF_BOOL ascii_packet );
tSIRF_UINT32 ImportINT32( tSIRF_INT32 *Dst,
                          tSIRF_UINT8 **pSrc,
                          tSIRF_BOOL ascii_packet );
tSIRF_UINT32 ImportINT64( tSIRF_INT64 *Dst,
                          tSIRF_UINT8 **pSrc,
                          tSIRF_BOOL ascii_packet );
tSIRF_UINT32 ImportUINT64(tSIRF_UINT64 *Dst,
                          tSIRF_UINT8 **pSrc,
                          tSIRF_BOOL ascii_packet );
tSIRF_UINT32 ImportFLOAT( tSIRF_FLOAT *Dst,
                           tSIRF_UINT8 **pSrc,
                           tSIRF_BOOL ascii_packet );
tSIRF_UINT32 ImportDOUBLE( tSIRF_DOUBLE *Dst,
                           tSIRF_UINT8 **pSrc,
                           tSIRF_BOOL ascii_packet );
tSIRF_UINT32 ImportSTRING( tSIRF_UINT8 *Dst, 
                           tSIRF_UINT16 *dst_size,
                           tSIRF_UINT8 **pSrc, 
                           tSIRF_UINT32 src_size,
                           tSIRF_BOOL ascii_packet );

/*------------------------------------------------------------------------------
 * Export functions
 -----------------------------------------------------------------------------*/
tSIRF_UINT32 ExportUINT8( tSIRF_UINT8 *Src,
                          tSIRF_UINT8 **pDst,
                          tSIRF_UINT32 size,
                          tSIRF_BOOL ascii_packet );
tSIRF_UINT32 ExportINT16( tSIRF_INT16 *Src,
                          tSIRF_UINT8 **pDst,
                          tSIRF_UINT32 size,
                          tSIRF_BOOL ascii_packet );
tSIRF_UINT32 ExportUINT16( tSIRF_UINT16 *Src,
                           tSIRF_UINT8 **pDst,
                           tSIRF_UINT32 size,
                           tSIRF_BOOL ascii_packet );
tSIRF_UINT32 ExportINT24( tSIRF_INT32 *Src,
                          tSIRF_UINT8 **pDst,
                          tSIRF_UINT32 size,
                          tSIRF_BOOL ascii_packet );
tSIRF_UINT32 ExportUINT24( tSIRF_UINT32 *Src,
                           tSIRF_UINT8 **pDst,
                           tSIRF_UINT32 size,
                           tSIRF_BOOL ascii_packet );
tSIRF_UINT32 ExportINT32( tSIRF_INT32 *Src,
                          tSIRF_UINT8 **pDst,
                          tSIRF_UINT32 size,
                          tSIRF_BOOL ascii_packet );
tSIRF_UINT32 ExportUINT32( tSIRF_UINT32 *Src,
                           tSIRF_UINT8 **pDst,
                           tSIRF_UINT32 size,
                           tSIRF_BOOL ascii_packet );
tSIRF_UINT32 ExportINT64( tSIRF_INT64 *Src,
                          tSIRF_UINT8 **pDst,
                          tSIRF_UINT32 size,
                          tSIRF_BOOL ascii_packet );
tSIRF_UINT32 ExportUINT64( tSIRF_UINT64 *Src,
                           tSIRF_UINT8 **pDst,
                           tSIRF_UINT32 size,
                           tSIRF_BOOL ascii_packet );
tSIRF_UINT32 ExportFLOAT( tSIRF_FLOAT *Src,
                          tSIRF_UINT8 **pDst,
                           tSIRF_UINT32 size,
                          tSIRF_BOOL ascii_packet );
tSIRF_UINT32 ExportDOUBLE( tSIRF_DOUBLE *Src,
                           tSIRF_UINT8 **pDst,
                           tSIRF_UINT32 size,
                           tSIRF_BOOL ascii_packet );
tSIRF_UINT32 ExportSTRING( tSIRF_UINT8 *Src, 
                           tSIRF_UINT16 src_size,
                           tSIRF_UINT8 **pDst, 
                           tSIRF_UINT32 dst_size,
                           tSIRF_BOOL ascii_packet );

tSIRF_RESULT SIRF_CODEC_Encode( tSIRF_UINT32 message_id, 
                                tSIRF_VOID *message_structure, 
                                tSIRF_UINT32 message_length,
                                tSIRF_UINT8* packet,             /* Returned */
                                tSIRF_UINT32 *packet_length );   /* Returned */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __SIRF_CODEC_H__ */

/**
 * @}
 * End of file.
 */

