/* -*- c -*- */
/* :mode=c:  - For jedit, previous line for emacs */
/*
 * Copyright (C) 2006-2010 Daniel Prevost <dprevost@photonsoftware.org>
 *
 * This file is part of Photon (photonsoftware.org).
 *
 * This file may be distributed and/or modified under the terms of the
 * GNU General Public License version 2 or version 3 as published by the 
 * Free Software Foundation and appearing in the file COPYING.GPL2 and 
 * COPYING.GPL3 included in the packaging of this software.
 *
 * Licensees holding a valid Photon Commercial license can use this file 
 * in accordance with the terms of their license.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "MemBitmap.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

inline
bool psonIsBufferFree( psonMemBitmap      * pBitmap,
                       ptrdiff_t            offset,
                       psonSessionContext * pContext )
{
   size_t byte, bit;
   size_t inUnitsOfAllocation;
   ptrdiff_t localOffset;
   bool rc;

   PSO_PRE_CONDITION( pBitmap != NULL );
   PSO_PRE_CONDITION( offset  != PSON_NULL_OFFSET );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_TRACE_ENTER_NUCLEUS( pContext );

   localOffset = offset - pBitmap->baseAddressOffset;
   if ( localOffset < 0 ) {
      PSO_TRACE_EXIT_NUCLEUS( pContext, false );
      return false;
   }
   
   inUnitsOfAllocation = localOffset / pBitmap->allocGranularity;
   if ( inUnitsOfAllocation >= pBitmap->lengthInBits ) {
      PSO_TRACE_EXIT_NUCLEUS( pContext, false );
      return false;
   }
   byte = inUnitsOfAllocation >> 3; /* Equivalent to divide by  8  */

   /*
    * We use the highest bit for the lower page so that the bitmap
    * is "ordered".
    */
   bit = 7 - (inUnitsOfAllocation & 7);

   rc = ( (pBitmap->bitmap[byte] & (unsigned char)(1 << bit)) == 0 );

   PSO_TRACE_EXIT_NUCLEUS( pContext, rc );
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

inline
void psonSetBufferAllocated( psonMemBitmap      * pBitmap,
                             ptrdiff_t            offset,
                             size_t               length,
                             psonSessionContext * pContext )
{
   size_t byte, bit, i;
   size_t inUnitsOfAllocation;
   ptrdiff_t localOffset;
   
   PSO_PRE_CONDITION( pBitmap  != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( offset  != PSON_NULL_OFFSET );
   PSO_PRE_CONDITION( length > 0 );
   PSO_TRACE_ENTER_NUCLEUS( pContext );
   
   localOffset = offset - pBitmap->baseAddressOffset;

   inUnitsOfAllocation = localOffset / pBitmap->allocGranularity;
   length = length / pBitmap->allocGranularity;
   
   byte = inUnitsOfAllocation >> 3; /* Equivalent to divide by  8  */
   /*
    * We use the highest bit for the lower page so that the bitmap
    * is "ordered".
    */
   bit = 7 - (inUnitsOfAllocation & 7);

   for ( i = 0; i < length; ++i ) {
      /* Setting the bit to one */
      pBitmap->bitmap[byte] |= (unsigned char)(1 << bit);
      if ( bit == 0 ) {
         bit = 8;
         byte++;
      }
      bit--;
   }
   PSO_TRACE_EXIT_NUCLEUS( pContext, true );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

inline
void psonSetBufferFree( psonMemBitmap      * pBitmap,
                        ptrdiff_t            offset,
                        size_t               length,
                        psonSessionContext * pContext )
{
   size_t byte, bit, i;
   size_t inUnitsOfAllocation;
   ptrdiff_t localOffset;

   PSO_PRE_CONDITION( pBitmap  != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( offset  != PSON_NULL_OFFSET );
   PSO_PRE_CONDITION( length > 0 );
   PSO_TRACE_ENTER_NUCLEUS( pContext );

   localOffset = offset - pBitmap->baseAddressOffset;
   
   inUnitsOfAllocation = localOffset / pBitmap->allocGranularity;
   length = length / pBitmap->allocGranularity;
   
   byte = inUnitsOfAllocation >> 3; /* Equivalent to divide by  8  */
   /*
    * We use the highest bit for the lower page so that the bitmap
    * is "ordered".
    */
   bit = 7 - (inUnitsOfAllocation & 7);
   
   for ( i = 0; i < length; ++i ) {
      /* Setting the bit to zero */
      pBitmap->bitmap[byte] &= (unsigned char)(~(1 << bit));
      if ( bit == 0 ) {
         bit = 8;
         byte++;
      }
      bit--;
   }
   PSO_TRACE_EXIT_NUCLEUS( pContext, true );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

inline
size_t psonGetBitmapLengthBytes( size_t               length,
                                 size_t               allocationUnit,
                                 psonSessionContext * pContext )
{
   /* Testing that it is non-zero and a power of two */
   PSO_PRE_CONDITION( allocationUnit > 0  && 
                      ! (allocationUnit & (allocationUnit-1)) );
   PSO_PRE_CONDITION( length > 0 );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_TRACE_ENTER_NUCLEUS( pContext );
   
   /* We "align" it to a multiple of allocationUnit */
   length = ((length - 1) / allocationUnit + 1 ) * allocationUnit;

   PSO_TRACE_EXIT_NUCLEUS( pContext, true );
   return ( (length/allocationUnit - 1) >> 3 ) + 1;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

