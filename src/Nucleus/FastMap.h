/*
 * Copyright (C) 2008-2010 Daniel Prevost <dprevost@photonsoftware.org>
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

#ifndef PSON_FASTMAP_H
#define PSON_FASTMAP_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Nucleus/Engine.h"
#include "Nucleus/MemoryObject.h"
#include "Nucleus/TreeNode.h"
#include "Nucleus/BlockGroup.h"
#include "Nucleus/Hash.h"
#include "Nucleus/HashMap.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

struct psonFastMapItem
{
   psonHashItem * pHashItem;
};

typedef struct psonFastMapItem psonFastMapItem;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

struct psonFastMap
{
   /** Always first */
   struct psonMemObject memObject;

   /** Basic info for all leaves and branches of our tree. */
   ptrdiff_t nodeOffset;
   
   struct psonHash hashObj;

   /** Offset to the data definition */
   ptrdiff_t dataDefOffset;

   /** Offset to the key definition */
   ptrdiff_t keyDefOffset;

   ptrdiff_t latestVersion;

   ptrdiff_t editVersion;
   
   /** Variable size struct - always put at the end */
   struct psonBlockGroup blockGroup;

};

typedef struct psonFastMap psonFastMap;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonFastMapCopy( psonFastMap        * pHashMap, 
                      psonFastMap        * pNewHashMap,
                      psonHashTxItem     * pHashItem,
                      psonSessionContext * pContext );

bool psonFastMapDelete( psonFastMap        * pHashMap,
                        const void         * key,
                        uint32_t             keyLength, 
                        psonSessionContext * pContext );

#if defined(PSO_USE_TRACE)
void psonFastMapDump( psonFastMap        * pMap,
                      int                  indent, 
                      psonSessionContext * pContext );
#endif

void psonFastMapEmpty( psonFastMap        * pHashMap,
                       psonSessionContext * pContext );

void psonFastMapFini( psonFastMap        * pHashMap,
                      psonSessionContext * pContext );

bool psonFastMapGet( psonFastMap        * pHashMap,
                     const void         * pKey,
                     uint32_t             keyLength, 
                     psonHashItem      ** ppItem,
                     uint32_t             bufferLength,
                     psonSessionContext * pContext );

bool psonFastMapGetFirst( psonFastMap        * pHashMap,
                          psonFastMapItem    * pItem,
                          uint32_t             keyLength,
                          uint32_t             bufferLength,
                          psonSessionContext * pContext );

bool psonFastMapGetNext( psonFastMap        * pHashMap,
                         psonFastMapItem    * pItem,
                         uint32_t             keyLength,
                         uint32_t             bufferLength,
                         psonSessionContext * pContext );

bool psonFastMapInit( psonFastMap         * pHashMap,
                      ptrdiff_t             parentOffset,
                      size_t                numberOfBlocks,
                      size_t                expectedNumOfChilds,
                      psonTreeNode        * pNode,
                      ptrdiff_t             hashItemOffset,
                      psoObjectDefinition * pDefinition,
                      psoKeyDefinition    * pKeyDefinition,
                      psonSessionContext  * pContext );

bool psonFastMapInsert( psonFastMap        * pHashMap,
                        const void         * pKey,
                        uint32_t             keyLength,
                        const void         * pItem,
                        uint32_t             itemLength,
                        psonSessionContext * pContext );

void psonFastMapRelease( psonFastMap        * pHashMap,
                         psonHashItem       * pHashItem,
                         psonSessionContext * pContext );

bool psonFastMapReplace( psonFastMap        * pHashMap,
                         const void         * pKey,
                         uint32_t             keyLength,
                         const void         * pItem,
                         uint32_t             itemLength,
                         psonSessionContext * pContext );

void psonFastMapStatus( psonFastMap        * pHashMap,
                        psoObjStatus       * pStatus,
                        psonSessionContext * pContext );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSON_FASTMAP_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

