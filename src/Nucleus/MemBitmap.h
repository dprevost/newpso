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

#ifndef PSON_MEM_BITMAP_H
#define PSON_MEM_BITMAP_H

#include "Nucleus/Engine.h"
#include "Nucleus/SessionContext.h"       

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

struct psonMemBitmap
{
   size_t lengthInBits;
   
   /* */
   size_t allocGranularity;

   ptrdiff_t baseAddressOffset;
   
   unsigned char bitmap[1];
   
};

typedef struct psonMemBitmap psonMemBitmap;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#if defined(PSO_USE_TRACE)
void psonMemBitmapDump( psonMemBitmap      * pBitmap,
                        int                  indent,
                        psonSessionContext * pContext );
#endif

/** 
 * Initialize the psonMemBitmap struct. 
 */
void psonMemBitmapInit( psonMemBitmap      * pBitmap,
                        ptrdiff_t            offset,
                        size_t               totalLength,
                        size_t               allocationUnit,
                        psonSessionContext * pContext );

void psonMemBitmapFini( psonMemBitmap      * pBitmap,
                        psonSessionContext * pContext );

static inline
bool psonIsBufferFree( psonMemBitmap      * pBitmap,
                       ptrdiff_t            offset,
                       psonSessionContext * pContext );

static inline
void psonSetBufferAllocated( psonMemBitmap      * pBitmap,
                             ptrdiff_t            offset,
                             size_t               length,
                             psonSessionContext * pContext );

static inline
void psonSetBufferFree( psonMemBitmap      * pBitmap,
                        ptrdiff_t            offset,
                        size_t               length,
                        psonSessionContext * pContext );

static inline
size_t psonGetBitmapLengthBytes( size_t               length,
                                 size_t               allocationUnit,
                                 psonSessionContext * pContext );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "MemBitmap.inl"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSON_MEM_BITMAP_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

