/*
 * Copyright (C) 2009 Daniel Prevost <dprevost@photonsoftware.org>
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

#ifndef PSON_CURSOR_H
#define PSON_CURSOR_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Nucleus/Engine.h"      
#include "Nucleus/MemoryObject.h"
#include "Nucleus/TreeNode.h"
#include "Nucleus/BlockGroup.h"
#include "Nucleus/LinkedList.h"
#include "Nucleus/TxStatus.h"
#include "Nucleus/Definitions.h"
#include "Nucleus/HashTx.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

struct psonCursorItem
{
   psonLinkNode node;

   /** Offset to the specific item */
   ptrdiff_t  itemOffset;

   /** Needed to decrement the reference count */
   void * parent;
   
   enum psonItemTypeEnum itemType;
};

typedef struct psonCursorItem psonCursorItem;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

struct psonCursor
{
   /** Always first */
   struct psonMemObject memObject;

   /** Our own doubly-linked list, to hold the data. */
   psonLinkedList listOfElements;

   /** Variable size struct - always put at the end */
   struct psonBlockGroup blockGroup;

};

typedef struct psonCursor psonCursor;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#if defined(PSO_USE_TRACE)
void psonCursorDump( psonCursor * pCursor, int indent );
#endif

void psonCursorEmpty( psonCursor         * pCursor,
                      psonSessionContext * pContext );

void psonCursorFini( psonCursor         * pCursor,
                     psonSessionContext * pContext );

bool psonCursorGetFirst( psonCursor         * pCursor,
                         psonCursorItem    ** ppItem,
                         psonSessionContext * pContext );

bool psonCursorGetLast( psonCursor         * pCursor,
                        psonCursorItem    ** ppItem,
                        psonSessionContext * pContext );

bool psonCursorGetNext( psonCursor         * pCursor,
                        psonCursorItem     * pOldItem,
                        psonCursorItem    ** ppItem,
                        psonSessionContext * pContext );

bool psonCursorGetPrevious( psonCursor         * pCursor,
                            psonCursorItem     * pOldItem,
                            psonCursorItem    ** ppItem,
                            psonSessionContext * pContext );

bool psonCursorInit( psonCursor         * pCursor,
                     ptrdiff_t            parentOffset,
                     size_t               numberOfBlocks,
                     psonSessionContext * pContext );

bool psonCursorInsertFirst( psonCursor         * pCursor,
                            unsigned char      * pItem,
                            psonItemTypeEnum     itemType,
                            psonSessionContext * pContext );

bool psonCursorInsertLast( psonCursor         * pCursor,
                           unsigned char      * pItem,
                           psonItemTypeEnum     itemType,
                           psonSessionContext * pContext );

void psonCursorSize( psonCursor * pCursor,
                     size_t     * pNumItems );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSON_CURSOR_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

