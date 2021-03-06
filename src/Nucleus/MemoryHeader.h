/*
 * Copyright (C) 2007-2010 Daniel Prevost <dprevost@photonsoftware.org>
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

#ifndef PSON_MEMORY_HEADER_H
#define PSON_MEMORY_HEADER_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Nucleus/Engine.h"
#include "Nucleus/TreeNode.h"
#include "Nucleus/HashTx.h"

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#define PSON_MEMORY_VERSION 1

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * The memory header is allocated at the beginning of the memory-mapped 
 * file (or whatever else replaces it). Although this structure is
 * quite small - 1 block will be reserved for it - this will garantee that 
 * we can modify the shared memory on the fly if this structure is changed 
 * in future releases.
 */
struct psonMemoryHeader
{
   char            cookie[4];
   int             version;
   int             running;  /* flag that will indicates system crashes */
   ptrdiff_t       treeMgrOffset;
   ptrdiff_t       processMgrOffset;
   ptrdiff_t       allocatorOffset;
   ptrdiff_t       dataDefMapOffset;
   ptrdiff_t       keyDefMapOffset;
   size_t          totalLength;
   
   size_t   sizeofPtr;
   bool     bigEndian;
   size_t   blockSize;
   size_t   alignmentStruct;
   size_t   alignmentChar;
   size_t   alignmentInt16;
   size_t   alignmentInt32;
   size_t   alignmentInt64;
   size_t   allocationUnit;
   
   char     cpu_type[20];
   char     compiler[20];
   char     compilerVersion[10];
   char     quasarVersion[10];
   char     creationTime[30];
   
   // The txstatus of the top folder is the txstatus of this "pseudo" 
   // txitem object (not used in an hash object since the top is alone)
   psonHashTxItem topHashItem;
   
   psonTreeNode topTreeNode;
};

typedef struct psonMemoryHeader psonMemoryHeader;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#if defined(PSO_USE_TRACE)
void psonMemoryHeaderDump( psonMemoryHeader   * pHeader,
                           int                  indent,
                           psonSessionContext * pContext );
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSON_MEMORY_HEADER_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

