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

#ifndef PSON_QUEUE_H
#define PSON_QUEUE_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Nucleus/Engine.h"      
#include "Nucleus/MemoryObject.h"
#include "Nucleus/TreeNode.h"
#include "Nucleus/BlockGroup.h"
#include "Nucleus/LinkedList.h"
#include "Nucleus/TxStatus.h"
#include "Nucleus/Definitions.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum psonQueueEnum
{
   PSON_QUEUE_FIRST = 101,
   PSON_QUEUE_LAST  = 202
   
};

typedef enum psonQueueEnum psonQueueEnum;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

struct psonQueueItem
{
   psonTxStatus  txStatus;

   psonLinkNode node;

   size_t dataLength;
   
   unsigned char data[1];

};

typedef struct psonQueueItem psonQueueItem;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

struct psonQueue
{
   /** Always first */
   struct psonMemObject memObject;

   /** Basic info for all leaves and branches of our tree. */
   ptrdiff_t nodeOffset;

   /** The type of queue (as decided when psoCreateQueue() was called). */
   enum psoObjectType queueType;

   /** Our own doubly-linked list, to hold the data. */
   psonLinkedList listOfElements;

   /** Offset to the data definition */
   ptrdiff_t  dataDefOffset;

   /**
    * Number of valid items. Valid items are the number of items NOT counting
    * items that might be added (but not committed) - also, items which are
    * removed but not committed are counted as valid).
    */
   size_t numValidItems;

   /** Variable size struct - always put at the end */
   struct psonBlockGroup blockGroup;

};

typedef struct psonQueue psonQueue;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psonQueueCommitAdd( psonQueue          * pQueue, 
                         ptrdiff_t            itemOffset,
                         psonSessionContext * pContext );

void psonQueueCommitRemove( psonQueue          * pQueue, 
                            ptrdiff_t            itemOffset,
                            psonSessionContext * pContext );

#if defined(PSO_USE_TRACE)
void psonQueueDump( psonQueue          * pQueue,
                    int                  indent,
                    psonSessionContext * pContext );
#endif

void psonQueueFini( psonQueue          * pQueue,
                    psonSessionContext * pContext );

bool psonQueueGetFirst( psonQueue          * pQueue,
                        psonQueueItem     ** ppIterator,
                        uint32_t             bufferLength,
                        psonSessionContext * pContext );

bool psonQueueGetNext( psonQueue          * pQueue,
                       psonQueueItem     ** ppIterator,
                       uint32_t             bufferLength,
                       psonSessionContext * pContext );

bool psonQueueInit( psonQueue           * pQueue,
                    ptrdiff_t             parentOffset,
                    size_t                numberOfBlocks,
                    psonTreeNode        * pNode,
                    psoObjectDefinition * pDefinition,
                    psonSessionContext  * pContext );

bool psonQueueInsert( psonQueue          * pQueue,
                      const void         * pItem, 
                      uint32_t             length,
                      enum psonQueueEnum   firstOrLast,
                      psonSessionContext * pContext );

bool psonQueueInsertNow( psonQueue          * pQueue,
                         const void         * pItem, 
                         uint32_t             length,
                         enum psonQueueEnum   firstOrLast,
                         psonSessionContext * pContext );

bool psonQueueRelease( psonQueue          * pQueue,
                       psonQueueItem      * pQueueItem,
                       psonSessionContext * pContext );

bool psonQueueRemove( psonQueue          * pQueue,
                      psonQueueItem     ** ppQueueItem,
                      enum psonQueueEnum   firstOrLast,
                      uint32_t             bufferLength,
                      psonSessionContext * pContext );

void psonQueueRollbackAdd( psonQueue          * pQueue, 
                           ptrdiff_t            itemOffset,
                           psonSessionContext * pContext  );

void psonQueueRollbackRemove( psonQueue          * pQueue, 
                              ptrdiff_t            itemOffset,
                              psonSessionContext * pContext );

void psonQueueStatus( psonQueue          * pQueue,
                      psoObjStatus       * pStatus,
                      psonSessionContext * pContext );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSON_QUEUE_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

