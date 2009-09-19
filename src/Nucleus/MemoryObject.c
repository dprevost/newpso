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

#include "MemoryObject.h"
#include "BlockGroup.h"
#include "MemoryAllocator.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#if defined(PSO_USE_TRACE)
void psonMemObjectDump( psonMemObject * pMemObj, int indent )
{
   DO_INDENT( indent );
   fprintf( stderr, "psonMemObject (%p) offset = "PSO_PTRDIFF_T_FORMAT"\n",
      pMemObj, SET_OFFSET(pMemObj) );
   if ( pMemObj == NULL ) return;
   
   psonMemObjIdentifierDump( pMemObj->objType, indent + 2 );
   
#if 0   
   /** The lock... obviously */
   psocProcessLock lock;
#endif

   /** Total number of blocks for the current object */
   DO_INDENT( indent + 2 );
   fprintf( stderr, "Total number of blocks: "PSO_SIZE_T_FORMAT"\n", 
      pMemObj->totalBlocks );
   
   psonLinkedListDump( &pMemObj->listBlockGroup, indent + 2 );

   DO_INDENT( indent );
   fprintf( stderr, "psonMemObject END\n" );
}
#endif

/** 
 * Initialize a psonMemObject struct. This will also initialize the lock
 * itself (each basic memory object has a lock), the initial group of
 * blocks struct representing the memory allocated for the object and 
 * the linked list of all such groups of blocks that might be allocated 
 * in the future, as needed.
 *
 * \param[in] pMemObj A pointer to the data struct we are initializing.
 * \param[in] objType The buffer used to store the message.
 * \param[in] pGroup  A pointer to the psonBlockGroup struct.
 * \param[in] numBlocks The initial number of Blocks allocated to this object.
 *
 * \retval PSO_OK  No error found
 * \retval PSO_NOT_ENOUGH_RESOURCES Something went wrong in allocating 
 *                                  resources for the Process Lock.
 *
 * \pre \em pMemObj cannot be NULL.
 * \pre \em objType must be valid (greater than PSON_IDENT_FIRST and less than 
 *          PSON_IDENT_LAST).
 * \pre \em pGroup cannot be NULL.
 * \pre \em numBlocks must be greater than zero.
 */

enum psoErrors 
psonMemObjectInit( psonMemObject      * pMemObj,
                   psonMemObjIdent      objType,
                   psonBlockGroup     * pGroup,
                   size_t               numBlocks,
                   psonSessionContext * pContext )
{
   bool ok;
   
   PSO_PRE_CONDITION( pMemObj != NULL );
   PSO_PRE_CONDITION( pGroup  != NULL );
   PSO_PRE_CONDITION( numBlocks > 0 );
   PSO_PRE_CONDITION( objType > PSON_IDENT_FIRST && 
                      objType < PSON_IDENT_LAST );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_TRACE_ENTER( pContext );

   /* In case InitProcessLock fails */
   pMemObj->objType = PSON_IDENT_CLEAR;
   
   ok =  psocInitProcessLock( &pMemObj->lock );
   PSO_POST_CONDITION( ok == true || ok == false );
   /*
    * The only possible error is a lack of resources when using semaphores, 
    * i.e. when the number of semaphores is greater than SEM_VALUE_MAX.
    */
   if ( ! ok ) {
      PSO_TRACE_EXIT( pContext, false );
      return PSO_NOT_ENOUGH_RESOURCES;
   }
    
   pMemObj->objType = objType;
   psonLinkedListInit( &pMemObj->listBlockGroup, pContext );
   
   psonBlockGroupInit( pGroup,
                       SET_OFFSET(pMemObj),
                       numBlocks,
                       objType,
                       pContext );
   
   /* Add the blockGroup to the list of groups of the memObject */
   psonLinkNodeInit( &pGroup->node, pContext );
   psonLinkedListPutFirst( &pMemObj->listBlockGroup, 
                           &pGroup->node,
                           pContext );
                           
   pMemObj->totalBlocks = numBlocks;
   
   PSO_TRACE_EXIT( pContext, true );
   return PSO_OK;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 * Terminate access to (destroy) a psonMemObject struct. It will release
 * groups of blocks of memory associated with this object.
 *
 *
 * \param[in] pMemObj A pointer to the data struct we are initializing.
 * \param[in] pContext A pointer to our session information
 *
 * \pre \em pMemObj cannot be NULL.
 * \pre \em pContext cannot be NULL.
 * \pre \em objType must be valid (greater than PSON_IDENT_FIRST and less than 
 *          PSON_IDENT_LAST).
 */

enum psoErrors 
psonMemObjectFini( psonMemObject      * pMemObj,
                   psonAllocTypeEnum    allocType,
                   psonSessionContext * pContext )
{
   psonLinkNode* firstNode = NULL, *dummy;
   bool ok;
   psonBlockGroup* pGroup;

   PSO_PRE_CONDITION( pMemObj  != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( pMemObj->objType > PSON_IDENT_FIRST && 
                      pMemObj->objType < PSON_IDENT_LAST );
   PSO_TRACE_ENTER( pContext );

   /*
    * We retrieve the first node - and leave it alone.
    */
   ok = psonLinkedListGetFirst( &pMemObj->listBlockGroup, &firstNode, pContext );
   PSO_PRE_CONDITION( ok );

   while ( psonLinkedListGetFirst(&pMemObj->listBlockGroup, &dummy, pContext) ) {
      pGroup = (psonBlockGroup*)( 
         (unsigned char*)dummy - offsetof(psonBlockGroup,node));

      psonFreeBlocks( pContext->pAllocator, 
                      PSON_ALLOC_ANY,
                      (unsigned char*)pGroup, 
                      pGroup->numBlocks,
                      pContext );
   }
                      
   pMemObj->objType = PSON_IDENT_CLEAR;

   pMemObj->totalBlocks = 0;

   psonLinkedListFini( &pMemObj->listBlockGroup, pContext );

   if ( ! psocFiniProcessLock( &pMemObj->lock ) ) {
      PSO_TRACE_EXIT( pContext, false );
      return PSO_SEM_DESTROY_ERROR;
   }
   
   pGroup = (psonBlockGroup*)(
      (unsigned char*)firstNode - offsetof(psonBlockGroup,node));

   /*
    * This must be the last access to the memory object.
    */
   psonFreeBlocks( pContext->pAllocator, 
                   allocType,
                   (unsigned char*)pMemObj, 
                   pGroup->numBlocks,
                   pContext );
   
   PSO_TRACE_EXIT( pContext, true );
   return PSO_OK;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

unsigned char* psonMalloc( psonMemObject*      pMemObj,
                           size_t              numBytes,
                           psonSessionContext* pContext )
{
   size_t numChunks, requestedChunks, remainingChunks;
   psonFreeBufferNode *oldNode, *currentNode, *newNode;
   psonBlockGroup* oldGroup, *currentGroup;
   psonLinkNode* dummy;
   unsigned char* ptr;
   size_t requestedBlocks, i;
   bool okGroup, okChunk;
   
   PSO_PRE_CONDITION( pMemObj  != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( numBytes > 0 );
   PSO_TRACE_ENTER( pContext );
   
   requestedChunks = (numBytes-1)/PSON_ALLOCATION_UNIT + 1;
   
   /*
    * The first loop is done on the list of block groups of the current 
    * memory object.
    */
   okGroup = psonLinkedListPeakFirst( &pMemObj->listBlockGroup,
                                      &dummy,
                                      pContext );
   while ( okGroup ) {
      currentGroup = (psonBlockGroup*)( 
         (unsigned char*)dummy - offsetof(psonBlockGroup,node));

      /*
       * No point in trying if the amount of free space is less than
       * the requested size for that group.
       */
      if ( currentGroup->freeBytes > requestedChunks *PSON_ALLOCATION_UNIT ) {
         /*
          * This second loop is done on the list of free chunks in the 
          * current block group.
          */
         okChunk = psonLinkedListPeakFirst( &currentGroup->freeList,
                                            &dummy,
                                            pContext );
         while ( okChunk ) {
            currentNode = (psonFreeBufferNode*)( 
               (unsigned char*)dummy - offsetof(psonFreeBufferNode,node));
            numChunks = ((psonFreeBufferNode*)currentNode)->numBuffers;
            if ( numChunks >= requestedChunks ) {
               /* We got it */
               remainingChunks = numChunks - requestedChunks;
               if ( remainingChunks == 0 ) {
                  /* Remove the chunk from the list */
                  psonLinkedListRemoveItem( &currentGroup->freeList, 
                                            &currentNode->node,
                                            pContext );
               }
               else {
                  newNode = (psonFreeBufferNode*)
                            ((unsigned char*) currentNode + 
                            (requestedChunks*PSON_ALLOCATION_UNIT));
                  newNode->numBuffers = remainingChunks;
                  psonLinkNodeInit( &newNode->node, pContext );
                  psonLinkedListReplaceItem( &currentGroup->freeList, 
                                             &currentNode->node, 
                                             &newNode->node,
                                             pContext );
                  /*
                   * Put the offset of the first free chunk on the last free 
                   * chunk. This makes it simpler/faster to rejoin groups 
                   * of free chunks. But only if there is more than one free
                   * chunk.
                   */
                  if ( remainingChunks > 1 ) {
                     ptr = (unsigned char*) newNode + 
                        (remainingChunks-1) * PSON_ALLOCATION_UNIT; 
                     *((ptrdiff_t *)ptr) = SET_OFFSET(newNode);
                  }
               }

               currentGroup->freeBytes -= requestedChunks*PSON_ALLOCATION_UNIT;
               /* Set the bitmap of chunks of the current block group. */
               psonSetBufferAllocated( &currentGroup->bitmap, 
                                       SET_OFFSET(currentNode), 
                                       requestedChunks*PSON_ALLOCATION_UNIT,
                                       pContext );
                              
               PSO_TRACE_EXIT( pContext, true );
               return (unsigned char*) currentNode;
            } /* end if of numChunks >= requestedChunks */
   
            oldNode = currentNode;
            okChunk = psonLinkedListPeakNext( &currentGroup->freeList,
                                              &oldNode->node,
                                              &dummy,
                                              pContext );
         } /* end of second loop on chunks */
      } /* end of test on freeBytes */
      
      oldGroup = currentGroup;
      okGroup = psonLinkedListPeakNext( &pMemObj->listBlockGroup,
                                        &oldGroup->node,
                                        &dummy,
                                        pContext );
   } /* end of first loop on block groups */

   /*
    * If we come here, it means that we did not find a free buffer of
    * the proper size. So we need to add blocks to the object.
    *  - find how many blocks to get
    *  - get them
    *  - initialize them
    *  - alloc the buffer the caller requested from that new chunk of memory.
    */
    
   i = ( sizeof(psonBlockGroup) - 1 ) / PSON_ALLOCATION_UNIT + 1;
   requestedBlocks = (((requestedChunks+i)*PSON_ALLOCATION_UNIT - 1) >> PSON_BLOCK_SHIFT) + 1;
   /* We increment the size by 3%, if 3% is bigger than the request */
   i = 3 * pMemObj->totalBlocks / 100;
   if ( i < requestedBlocks ) i = requestedBlocks;
   
   currentGroup = (psonBlockGroup*) psonMallocBlocks( pContext->pAllocator,
                                                      PSON_ALLOC_ANY,
                                                      i,
                                                      pContext );
   if ( currentGroup == NULL && i > requestedBlocks ) {
      /* 
       * We retry again with a smaller number, the minimal increase needed
       * to satisfy the original request.
       */
      if ( psocGetLastError( &pContext->errorHandler ) == 
         PSO_NOT_ENOUGH_PSO_MEMORY ) {

         i = requestedBlocks;
         currentGroup = (psonBlockGroup*) psonMallocBlocks( pContext->pAllocator,
                                                            PSON_ALLOC_ANY,
                                                            i,
                                                            pContext );
      }
   }
   if ( currentGroup != NULL ) {
      psonBlockGroupInit( currentGroup, 
                          SET_OFFSET( currentGroup ), 
                          i, 
                          pMemObj->objType,
                          pContext );
      /* Add the blockGroup to the list of groups of the memObject */
      psonLinkNodeInit( &currentGroup->node, pContext );
      psonLinkedListPutLast( &pMemObj->listBlockGroup,
                             &currentGroup->node,
                             pContext );
      pMemObj->totalBlocks += i;
      
      /* Allocate the memory (the chunks) needed to satisfy the request. */
      okChunk = psonLinkedListPeakFirst( &currentGroup->freeList,
                                         &dummy,
                                         pContext );
      if ( okChunk ) {
         currentNode = (psonFreeBufferNode*)( 
            (unsigned char*)dummy + offsetof(psonFreeBufferNode,node));
         numChunks = ((psonFreeBufferNode*)currentNode)->numBuffers;
         remainingChunks = numChunks - requestedChunks;
         
         if ( remainingChunks == 0 ) {
            /* Remove the node from the list */
            psonLinkedListRemoveItem( &currentGroup->freeList, 
                                      &currentNode->node,
                                      pContext );
         }
         else {
            newNode = (psonFreeBufferNode*)
                      ((unsigned char*) currentNode + 
                      (requestedChunks*PSON_ALLOCATION_UNIT));
            newNode->numBuffers = remainingChunks;
            psonLinkNodeInit( &newNode->node, pContext );
            psonLinkedListReplaceItem( &currentGroup->freeList, 
                                       &currentNode->node, 
                                       &newNode->node,
                                       pContext );
            /*
             * Put the offset of the first free chunk on the last free 
             * chunk. This makes it simpler/faster to rejoin groups 
             * of free chunks. But only if there is more than one free
             * chunk.
             */
            if ( remainingChunks > 1 ) {
               ptr = (unsigned char*) newNode + 
                  (remainingChunks-1) * PSON_ALLOCATION_UNIT; 
               *((ptrdiff_t *)ptr) = SET_OFFSET(newNode);
            }
         }

         currentGroup->freeBytes -= requestedChunks*PSON_ALLOCATION_UNIT;
         /* Set the bitmap */
         psonSetBufferAllocated( &currentGroup->bitmap, 
                                 SET_OFFSET(currentNode), 
                                 requestedChunks*PSON_ALLOCATION_UNIT,
                                 pContext );
                              
         PSO_TRACE_EXIT( pContext, true );
         return (unsigned char*) currentNode;
      }
   }
   
   /** 
    * \todo Eventually, it might be a good idea to separate "no memory"
    * versus a lack of a chunk big enough to accomodate the # of requested
    * blocks.
    */
   psocSetError( &pContext->errorHandler, 
                 g_psoErrorHandle, 
                 PSO_NOT_ENOUGH_PSO_MEMORY );

   PSO_TRACE_EXIT( pContext, false );
   return NULL;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psonFree( psonMemObject*      pMemObj,
               unsigned char*      ptr, 
               size_t              numBytes,
               psonSessionContext* pContext )
{
   psonBlockGroup* oldGroup, *currentGroup, *goodGroup = NULL;
   psonLinkNode* dummy;
   bool otherBufferisFree = false;
   unsigned char* p;
   psonFreeBufferNode* otherNode;
   ptrdiff_t offset;
   size_t numBlocks;
   bool ok;
   
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( pMemObj  != NULL );
   PSO_PRE_CONDITION( ptr      != NULL );
   PSO_PRE_CONDITION( numBytes > 0 );
   PSO_TRACE_ENTER( pContext );

   /*
    * Ajust numBytes so that it is a multiple of PSON_ALLOCATION_UNIT,
    * without truncating it, of course...
    */
   numBytes = ((numBytes-1)/PSON_ALLOCATION_UNIT+1)*PSON_ALLOCATION_UNIT;
   
   /* Find to which blockgroup ptr belongs to */
   ok = psonLinkedListPeakFirst( &pMemObj->listBlockGroup,
                                 &dummy,
                                 pContext );
   while ( ok ) {
      currentGroup = (psonBlockGroup*)( 
         (unsigned char*)dummy - offsetof(psonBlockGroup,node));

      offset = currentGroup->bitmap.baseAddressOffset;
      if ( ptr >= GET_PTR_FAST( offset, unsigned char) && 
           ptr < GET_PTR_FAST( offset+ (currentGroup->numBlocks << PSON_BLOCK_SHIFT), 
                          unsigned char ) ) {
         goodGroup = currentGroup;
         break;
      }
      oldGroup = currentGroup;
      ok = psonLinkedListPeakNext( &pMemObj->listBlockGroup,
                                   &oldGroup->node,
                                   &dummy,
                                   pContext );
   }
   PSO_PRE_CONDITION( goodGroup != NULL );

   goodGroup->freeBytes += numBytes;

   /* 
    * Check if the chunk before the current chunk (ptr)
    * is in the freeList or not.
    */
   p = ptr - PSON_ALLOCATION_UNIT;
   otherBufferisFree = psonIsBufferFree( &goodGroup->bitmap,
                                         SET_OFFSET(p),
                                         pContext );
   if ( otherBufferisFree ) {
      /* Find the start of that free group of chunks */
      if ( psonIsBufferFree( &goodGroup->bitmap, 
                             SET_OFFSET( ptr - 2*PSON_ALLOCATION_UNIT ),
                             pContext ) ) {
         /* The free group has more than one chunk */
         offset = *((ptrdiff_t*)p);
         GET_PTR( p, offset, unsigned char);
      }
      
      /*
       * The node is already in the linked list! All we need to do is
       * to modify the number of chunks that this node has.
       */
      ((psonFreeBufferNode*)p)->numBuffers += (numBytes/PSON_ALLOCATION_UNIT);
   }
   else {
      /*
       * The previous buffer is not free, so we need to insert our released
       * buffer as a new node in the list.
       */
      p = ptr; /* This is needed further down */
      ((psonFreeBufferNode*)p)->numBuffers = numBytes/PSON_ALLOCATION_UNIT;
      psonLinkNodeInit( &((psonFreeBufferNode*)p)->node, pContext );
      psonLinkedListPutLast( &goodGroup->freeList, 
                             &((psonFreeBufferNode*)p)->node,
                             pContext );
   }

   /* 
    * Check if the chunk after the current group-of-chunks-to-be-released
    * is in the freeList or not.
    *
    * Note: "p" at this point represents the node which contain our buffer
    *       and, possibly, the previous buffer if it was free.
    */
   otherNode = (psonFreeBufferNode*)( ptr + numBytes);
   otherBufferisFree = psonIsBufferFree( &goodGroup->bitmap, 
                                         SET_OFFSET(otherNode),
                                         pContext );
   if ( otherBufferisFree ) {
      ((psonFreeBufferNode*)p)->numBuffers += otherNode->numBuffers;
      psonLinkedListRemoveItem( &goodGroup->freeList, 
                                &otherNode->node,
                                pContext );
      memset( otherNode, 0, sizeof(psonFreeBufferNode) );
   }

   if ( ((psonFreeBufferNode*)p)->numBuffers*PSON_ALLOCATION_UNIT == 
      goodGroup->maxFreeBytes && goodGroup->isDeletable ) {

      numBlocks = goodGroup->numBlocks;
      pMemObj->totalBlocks -= numBlocks;
      /* Remove the blockGroup to the list of groups of the memObject */
      psonLinkedListRemoveItem( &pMemObj->listBlockGroup,
                                &goodGroup->node,
                                pContext );

      psonBlockGroupFini( goodGroup, pContext );
      
      psonFreeBlocks( pContext->pAllocator,
                      PSON_ALLOC_ANY,
                      (unsigned char*)goodGroup,
                      numBlocks,
                      pContext );
   }
   else {
      /* Set the bitmap */
      psonSetBufferFree( &goodGroup->bitmap, SET_OFFSET(ptr),
                         numBytes,
                         pContext );
      /*
       * Put the offset of the first free block on the last free block.
       * This makes it simpler/faster to rejoin groups of free blocks. But 
       * only if the number of blocks in the group > 1.
       */
      if ( ((psonFreeBufferNode*)p)->numBuffers > 1 ) {
         /* Warning - we reuse ptr here */
          ptr = p + 
             (((psonFreeBufferNode*)p)->numBuffers-1) * PSON_ALLOCATION_UNIT; 
          *((ptrdiff_t *)ptr) = SET_OFFSET(p);
      }
   }
   PSO_TRACE_EXIT( pContext, true );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psonMemObjectStatus( psonMemObject      * pMemObj, 
                          psoObjStatus       * pStatus,
                          psonSessionContext * pContext )
{
   psonLinkNode * dummy;
   psonBlockGroup * pGroup;
   bool ok;
   
   PSO_PRE_CONDITION( pMemObj != NULL );
   PSO_PRE_CONDITION( pStatus != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_TRACE_ENTER( pContext );

   pStatus->numBlocks = pMemObj->totalBlocks;
   pStatus->numBlockGroup = pMemObj->listBlockGroup.currentSize;
   pStatus->freeBytes = 0;
   
   /*
    * We retrieve the first node
    */
   ok = psonLinkedListPeakFirst( &pMemObj->listBlockGroup,
                                 &dummy,
                                 pContext );
   PSO_PRE_CONDITION( ok );

   while ( ok ) {
      pGroup = (psonBlockGroup*)( 
         (unsigned char*)dummy - offsetof(psonBlockGroup,node));
      pStatus->freeBytes += pGroup->freeBytes;

      ok = psonLinkedListPeakNext( &pMemObj->listBlockGroup,
                                   dummy,
                                   &dummy,
                                   pContext );
   }
   PSO_TRACE_EXIT( pContext, true );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

