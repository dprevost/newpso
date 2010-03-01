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

#include "Nucleus/BlockGroup.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#if defined(PSO_USE_TRACE)
void psonBlockGroupDump( psonBlockGroup     * pGroup,
                         int                  indent,
                         psonSessionContext * pContext )
{
   DO_INDENT( pContext, indent );
   fprintf( pContext->tracefp, 
      "psonBlockGroup (%p) offset = "PSO_PTRDIFF_T_FORMAT"\n",
      pGroup, SET_OFFSET(pContext->pBaseAddress, pGroup) );
   if ( pGroup == NULL ) return;

   psonMemObjIdentifierDump( pGroup->objType, indent + 2, pContext );

   psonLinkNodeDump( &pGroup->node, indent + 2, pContext );

   DO_INDENT( pContext, indent + 2 );
   fprintf( pContext->tracefp, "Number of blocks: "PSO_SIZE_T_FORMAT"\n",
      pGroup->numBlocks );

   DO_INDENT( pContext, indent + 2 );
   fprintf( pContext->tracefp, "Maximum available memory: "PSO_SIZE_T_FORMAT"\n",
      pGroup->maxFreeBytes );
   
   DO_INDENT( pContext, indent + 2 );
   fprintf( pContext->tracefp, "Current amount of free memory: "PSO_SIZE_T_FORMAT"\n",
      pGroup->freeBytes );
   
   psonLinkedListDump( &pGroup->freeList, indent + 2, pContext );
   
   if ( pGroup->isDeletable ) {
      DO_INDENT( pContext, indent + 2 );
      fprintf( pContext->tracefp, "This group of blocks can be deleted\n" );
   }
   else {
      DO_INDENT( pContext, indent + 2 );
      fprintf( pContext->tracefp, "This group of blocks cannot be deleted (first group)\n" );
   }

   psonMemBitmapDump( &pGroup->bitmap, indent + 2, pContext );

   DO_INDENT( pContext, indent );
   fprintf( pContext->tracefp, "psonBlockGroup END\n" );
}
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psonBlockGroupFini( psonBlockGroup     * pGroup,
                         psonSessionContext * pContext )
{
   PSO_PRE_CONDITION( pGroup != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_TRACE_ENTER_NUCLEUS( pContext );

   psonMemBitmapFini(  &pGroup->bitmap, pContext );
   psonLinkedListFini( &pGroup->freeList, pContext );
   psonLinkNodeFini(   &pGroup->node, pContext );

   pGroup->numBlocks = 0;
   pGroup->maxFreeBytes = 0;
   pGroup->freeBytes = 0;
   pGroup->objType = PSON_IDENT_CLEAR;

   PSO_TRACE_EXIT_NUCLEUS( pContext, true );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Note: no need to initialize the endBlock struct. It is set by the 
 * memory allocator when calling psonMallocBlocks().
 */
void psonBlockGroupInit( psonBlockGroup     * pGroup,
                         ptrdiff_t            firstBlockOffset,
                         size_t               numBlocks,
                         psonMemObjIdent      objType,
                         psonSessionContext * pContext )
{
   ptrdiff_t groupOffset;
   size_t currentLength;
   psonFreeBufferNode * firstNode;
   
   PSO_PRE_CONDITION( pGroup != NULL );
   PSO_PRE_CONDITION( firstBlockOffset != PSON_NULL_OFFSET );
   PSO_PRE_CONDITION( numBlocks > 0 );
   PSO_PRE_CONDITION( objType > PSON_IDENT_FIRST && objType < PSON_IDENT_LAST );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_TRACE_ENTER_NUCLEUS( pContext );

   pGroup->numBlocks = numBlocks;
   pGroup->objType = PSON_IDENT_PAGE_GROUP & objType;
   
   psonLinkNodeInit( &pGroup->node, pContext );
   psonLinkedListInit( &pGroup->freeList, pContext );
   
   psonMemBitmapInit( &pGroup->bitmap,
                      firstBlockOffset,
                      numBlocks << PSON_BLOCK_SHIFT, 
                      PSON_ALLOCATION_UNIT,
                      pContext );
   
   /* Is the blockGroup struct at the beginning of the group ? */
   groupOffset = SET_OFFSET(pContext->pBaseAddress, pGroup);
   if ( SET_OFFSET(pContext->pBaseAddress, pGroup) == firstBlockOffset ) {
      /* 
       * Yes, we are. This blockGroup is therefore NOT the blockGroup 
       * initially allocated when an object is first allocated.
       */
      pGroup->isDeletable = true;
      currentLength = 0;
   }
   else {
      /*
       * No, we're not. Can't deallocate this groupBlock when empty!
       * The offset of "this" (pGroup) tells us how much memory was
       * allocated for the header of the object.
       */
      pGroup->isDeletable = false;
      currentLength = groupOffset-firstBlockOffset;
   }

   currentLength += offsetof(psonBlockGroup,bitmap) +
                    offsetof(psonMemBitmap,bitmap) +
                    psonGetBitmapLengthBytes( numBlocks << PSON_BLOCK_SHIFT, 
                                              PSON_ALLOCATION_UNIT,
                                              pContext );
   currentLength = ((currentLength-1)/PSON_ALLOCATION_UNIT+1)*PSON_ALLOCATION_UNIT;
   
   pGroup->maxFreeBytes = (numBlocks << PSON_BLOCK_SHIFT) - currentLength;
   
   /* remove the space required by the psonEndBlockGroup struct */
   pGroup->maxFreeBytes -= PSON_ALLOCATION_UNIT;
   pGroup->freeBytes = pGroup->maxFreeBytes;
   
   /* 
    * So we have one free buffer, starting at offset "currentLength"
    * + firstBlockOffset with length "maxFreeBytes". Insert it in our freeList.
    */
   GET_PTR(pContext->pBaseAddress, firstNode, firstBlockOffset+currentLength, psonFreeBufferNode );
   psonLinkNodeInit( &firstNode->node, pContext );
   firstNode->numBuffers = pGroup->maxFreeBytes/PSON_ALLOCATION_UNIT;

   psonLinkedListPutFirst( &pGroup->freeList, &firstNode->node, pContext );

   /* And set the bitmap */
   psonSetBufferAllocated( &pGroup->bitmap,
                           firstBlockOffset,
                           currentLength,
                           pContext );
   psonSetBufferAllocated( &pGroup->bitmap, 
                           psonEndBlockOffset(firstBlockOffset, numBlocks), 
                           PSON_ALLOCATION_UNIT,
                           pContext );
   PSO_TRACE_EXIT_NUCLEUS( pContext, true );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

