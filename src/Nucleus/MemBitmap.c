/*
 * Copyright (C) 2006-2009 Daniel Prevost <dprevost@photonsoftware.org>
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

#if defined(PSO_USE_TRACE)
void psonMemBitmapDump( psonMemBitmap * pBitmap, int indent )
{
   DO_INDENT( indent );
   fprintf(stderr, "psonMemBitmap (%p) offset = "PSO_PTRDIFF_T_FORMAT"\n",
      pBitmap, SET_OFFSET(pBitmap) );
   if ( pBitmap == NULL ) return;

   DO_INDENT( indent + 2 );
   fprintf( stderr, "Length in bits: "PSO_SIZE_T_FORMAT"\n",
      pBitmap->lengthInBits );
   
   DO_INDENT( indent + 2 );
   fprintf( stderr, "Allocation granularity: "PSO_SIZE_T_FORMAT"\n",
      pBitmap->allocGranularity );
   
   DO_INDENT( indent + 2 );
   fprintf( stderr, "Base address offset: "PSO_PTRDIFF_T_FORMAT"\n",
      pBitmap->baseAddressOffset );
   
#if 0
   unsigned char bitmap[1];
#endif
}
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psonMemBitmapFini( psonMemBitmap      * pBitmap,
                        psonSessionContext * pContext )
{
   size_t len, i;
   PSO_PRE_CONDITION( pBitmap != NULL );
   PSO_TRACE_ENTER( pContext );
   
   len = ( (pBitmap->lengthInBits - 1 ) >> 3 ) + 1;
   for ( i = 0; i < len; ++i ) {
      pBitmap->bitmap[i] = 0;
   }
   
   pBitmap->lengthInBits = 0;
   pBitmap->allocGranularity = 0;
   pBitmap->baseAddressOffset = PSON_NULL_OFFSET;
   
   PSO_TRACE_EXIT( pContext );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psonMemBitmapInit( psonMemBitmap      * pBitmap,
                        ptrdiff_t            baseAddressOffset,
                        size_t               totalLength,
                        size_t               allocGranularity,
                        psonSessionContext * pContext )
{
   size_t len, i;
   
   PSO_PRE_CONDITION( pBitmap     != NULL );
   PSO_PRE_CONDITION( baseAddressOffset != PSON_NULL_OFFSET );
   PSO_PRE_CONDITION( totalLength      > 0 );
   /* Testing that it is non-zero and a power of two */
   PSO_PRE_CONDITION( allocGranularity > 0  && 
                      ! (allocGranularity & (allocGranularity-1)) );
   PSO_TRACE_ENTER( pContext );

   pBitmap->lengthInBits = totalLength/allocGranularity;
   pBitmap->allocGranularity = allocGranularity;
   pBitmap->baseAddressOffset = baseAddressOffset;
   
   len = ( (pBitmap->lengthInBits - 1 ) >> 3 ) + 1;
   for ( i = 0; i < len; ++i ) {
      pBitmap->bitmap[i] = 0;
   }
   PSO_TRACE_EXIT( pContext );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

