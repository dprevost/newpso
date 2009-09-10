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

#include "Nucleus/Cursor.h"
#include "Nucleus/LinkedList.h"
#include "Nucleus/Transaction.h"
#include "Nucleus/Folder.h"
#include "Nucleus/HashMap.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#if defined(PSO_USE_TRACE)
void psonCursorDump( psonCursor * pCursor, int indent )
{
   DO_INDENT( indent );
   fprintf(stderr, "psonCursor (%p) offset = "PSO_PTRDIFF_T_FORMAT"\n",
      pCursor, SET_OFFSET(pCursor) );
   if ( pCursor == NULL ) return;

   psonMemObjectDump( &pCursor->memObject, indent + 2 );

   psonLinkedListDump( &pCursor->listOfElements, indent + 2 );

   psonBlockGroupDump( &pCursor->blockGroup, indent + 2 );
}
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psonCursorEmpty( psonCursor         * pCursor,
                      psonSessionContext * pContext )
{
   psonCursorItem* pCursorItem = NULL;
   psonLinkNode * pNode = NULL;

   PSO_PRE_CONDITION( pCursor  != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( pCursor->memObject.objType == PSON_IDENT_CURSOR );
   PSO_TRACE_ENTER( pContext );

   while ( psonLinkedListGetFirst( &pCursor->listOfElements, &pNode, pContext ) ) {
      pCursorItem = (psonCursorItem *)
         ((char*)pNode - offsetof( psonCursorItem, node ));
      // Bug - must reduce the reference count of 
      
      switch( pCursorItem->itemType ) {
         
      case PSON_HASH_ITEM:
         break;
         
      case PSON_HASH_TX_ITEM:
         psonHashMapRelease( (psonHashMap *) pCursorItem->parent,
                             GET_PTR_FAST(pCursorItem->itemOffset, psonHashTxItem),
                             pContext );
         break;
         
      default:
         break;
      }
      
      psonFree( &pCursor->memObject,
                (unsigned char *)pCursorItem,
                sizeof(psonCursorItem),
                pContext );
   }
   PSO_TRACE_EXIT( pContext );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psonCursorFini( psonCursor         * pCursor,
                     psonSessionContext * pContext )
{
   PSO_PRE_CONDITION( pCursor  != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( pCursor->memObject.objType == PSON_IDENT_CURSOR );
   PSO_TRACE_ENTER( pContext );

   psonCursorEmpty( pCursor, pContext );
   
   psonLinkedListFini( &pCursor->listOfElements, pContext );
   psonMemObjectFini(  &pCursor->memObject, PSON_ALLOC_API_OBJ, pContext );

   PSO_TRACE_EXIT( pContext );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonCursorGetFirst( psonCursor         * pCursor,
                         psonCursorItem    ** ppItem,
                         psonSessionContext * pContext )
{
   psonLinkNode * pNode = NULL;
   bool okList;
   
   PSO_PRE_CONDITION( pCursor  != NULL );
   PSO_PRE_CONDITION( ppItem   != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( pCursor->memObject.objType == PSON_IDENT_CURSOR );
   PSO_TRACE_ENTER( pContext );
   
   /* This call can only fail if the queue is empty. */
   okList = psonLinkedListPeakFirst( &pCursor->listOfElements, &pNode, pContext );
   
   if ( okList ) {
      *ppItem = (psonCursorItem*) 
         ((char*)pNode - offsetof( psonCursorItem, node ));

      PSO_TRACE_EXIT( pContext );
      return true;
   }
   
   /* 
    * If we come here, there are no data items to retrieve. As 
    * long as we clearly say that the internal iterator is reset (in case a 
    * "Get Previous" is implemented later), we can just release the iterator
    * at this point.
    */
   *ppItem = NULL;
   
   psocSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_IS_EMPTY );

   PSO_TRACE_EXIT( pContext );
   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonCursorGetLast( psonCursor         * pCursor,
                        psonCursorItem    ** ppItem,
                        psonSessionContext * pContext )
{
   psonLinkNode * pNode = NULL;
   bool okList;
   
   PSO_PRE_CONDITION( pCursor  != NULL );
   PSO_PRE_CONDITION( ppItem   != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( pCursor->memObject.objType == PSON_IDENT_CURSOR );
   PSO_TRACE_ENTER( pContext );
   
   /* This call can only fail if the queue is empty. */
   okList = psonLinkedListPeakLast( &pCursor->listOfElements, &pNode, pContext );
   
   if ( okList ) {
      *ppItem = (psonCursorItem*) 
         ((char*)pNode - offsetof( psonCursorItem, node ));

      PSO_TRACE_EXIT( pContext );
      return true;
   }
   
   /* 
    * If we come here, there are no data items to retrieve. As 
    * long as we clearly say that the internal iterator is reset (in case a 
    * "Get Previous" is implemented later), we can just release the iterator
    * at this point.
    */
   *ppItem = NULL;
   
   psocSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_IS_EMPTY );

   PSO_TRACE_EXIT( pContext );
   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonCursorGetNext( psonCursor         * pCursor,
                        psonCursorItem     * pOldItem,
                        psonCursorItem    ** ppItem,
                        psonSessionContext * pContext )
{
   psonLinkNode * pNode = NULL;
   bool okList;
   
   PSO_PRE_CONDITION( pCursor  != NULL );
   PSO_PRE_CONDITION( pOldItem != NULL );
   PSO_PRE_CONDITION( ppItem   != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( pCursor->memObject.objType == PSON_IDENT_CURSOR );
   PSO_TRACE_ENTER( pContext );
   
   okList =  psonLinkedListPeakNext( &pCursor->listOfElements, 
                                     &pOldItem->node, 
                                     &pNode,
                                     pContext );
   if ( okList ) {
      *ppItem = (psonCursorItem*)
         ((char*)pNode - offsetof( psonCursorItem, node ));
      
      PSO_TRACE_EXIT( pContext );
      return true;
   }
   
   /* 
    * If we come here, there are no additional data items to retrieve. As 
    * long as we clearly say that the internal iterator is reset (in case a 
    * "Get Previous" is implemented later), we can just release the iterator
    * at this point.
    */
   *ppItem = NULL;
   
   psocSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_REACHED_THE_END );

   PSO_TRACE_EXIT( pContext );
   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonCursorGetPrevious( psonCursor         * pCursor,
                            psonCursorItem     * pOldItem,
                            psonCursorItem    ** ppItem,
                            psonSessionContext * pContext )
{
   psonLinkNode * pNode = NULL;
   bool okList;
   
   PSO_PRE_CONDITION( pCursor  != NULL );
   PSO_PRE_CONDITION( pOldItem != NULL );
   PSO_PRE_CONDITION( ppItem   != NULL );
   PSO_PRE_CONDITION( pContext  != NULL );
   PSO_PRE_CONDITION( pCursor->memObject.objType == PSON_IDENT_CURSOR );
   PSO_TRACE_ENTER( pContext );
   
   okList =  psonLinkedListPeakPrevious( &pCursor->listOfElements, 
                                         &pOldItem->node, 
                                         &pNode,
                                         pContext );
   if ( okList ) {
      *ppItem = (psonCursorItem*)
         ((char*)pNode - offsetof( psonCursorItem, node ));
      
      PSO_TRACE_EXIT( pContext );
      return true;
   }
   
   /* 
    * If we come here, there are no additional data items to retrieve. As 
    * long as we clearly say that the internal iterator is reset (in case a 
    * "Get Previous" is implemented later), we can just release the iterator
    * at this point.
    */
   *ppItem = NULL;
   
   psocSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_REACHED_THE_END );

   PSO_TRACE_EXIT( pContext );
   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonCursorInit( psonCursor          * pCursor,
                     ptrdiff_t             parentOffset,
                     size_t                numberOfBlocks,
                     psonSessionContext  * pContext )
{
   psoErrors errcode;
   
   PSO_PRE_CONDITION( pCursor      != NULL );
   PSO_PRE_CONDITION( pContext     != NULL );
   PSO_PRE_CONDITION( parentOffset != PSON_NULL_OFFSET );
   PSO_PRE_CONDITION( numberOfBlocks > 0 );
   PSO_TRACE_ENTER( pContext );
   
   errcode = psonMemObjectInit( &pCursor->memObject, 
                                PSON_IDENT_CURSOR,
                                &pCursor->blockGroup,
                                numberOfBlocks,
                                pContext );
   if ( errcode != PSO_OK ) {
      psocSetError( &pContext->errorHandler,
                    g_psoErrorHandle,
                    errcode );
      PSO_TRACE_EXIT( pContext );
      return false;
   }

   psonLinkedListInit( &pCursor->listOfElements, pContext );

   PSO_TRACE_EXIT( pContext );
   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonCursorInsertFirst( psonCursor         * pCursor,
                            unsigned char      * pItem,
                            psonItemTypeEnum     itemType,
                            psonSessionContext * pContext )
{
   psonCursorItem * pCursorItem;
   
   PSO_PRE_CONDITION( pCursor  != NULL );
   PSO_PRE_CONDITION( pItem    != NULL )
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( pCursor->memObject.objType == PSON_IDENT_CURSOR );
   PSO_PRE_CONDITION( itemType > PSON_FIRST_ITEM && itemType < PSON_LAST_ITEM );
   PSO_TRACE_ENTER( pContext );

   pCursorItem = (psonCursorItem *) psonMalloc( &pCursor->memObject,
                                                sizeof(psonCursorItem),
                                                pContext );
   if ( pCursorItem == NULL ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, 
                    PSO_NOT_ENOUGH_PSO_MEMORY );
      PSO_TRACE_EXIT( pContext );
      return false;
   }
   
   psonLinkNodeInit( &pCursorItem->node, pContext );
      
   pCursorItem->itemOffset = SET_OFFSET(pItem);
   pCursorItem->itemType = itemType;

   psonLinkedListPutFirst( &pCursor->listOfElements,
                           &pCursorItem->node,
                           pContext );
      
   PSO_TRACE_EXIT( pContext );
   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonCursorInsertLast( psonCursor         * pCursor,
                           unsigned char      * pItem,
                           psonItemTypeEnum     itemType,
                           psonSessionContext * pContext )
{
   psonCursorItem * pCursorItem;
   
   PSO_PRE_CONDITION( pCursor  != NULL );
   PSO_PRE_CONDITION( pItem    != NULL )
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( pCursor->memObject.objType == PSON_IDENT_CURSOR );
   PSO_PRE_CONDITION( itemType > PSON_FIRST_ITEM && itemType < PSON_LAST_ITEM );
   PSO_TRACE_ENTER( pContext );

   pCursorItem = (psonCursorItem *) psonMalloc( &pCursor->memObject,
                                                sizeof(psonCursorItem),
                                                pContext );
   if ( pCursorItem == NULL ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, 
                    PSO_NOT_ENOUGH_PSO_MEMORY );
      PSO_TRACE_EXIT( pContext );
      return false;
   }
   
   psonLinkNodeInit( &pCursorItem->node, pContext );
      
   pCursorItem->itemOffset = SET_OFFSET(pItem);
   pCursorItem->itemType = itemType;

   psonLinkedListPutLast( &pCursor->listOfElements,
                          &pCursorItem->node,
                          pContext );
      
   PSO_TRACE_EXIT( pContext );
   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psonCursorSize( psonCursor         * pCursor,
                     size_t             * pNumItems,
                     psonSessionContext * pContext )
{
   PSO_PRE_CONDITION( pCursor   != NULL );
   PSO_PRE_CONDITION( pNumItems != NULL );
   PSO_PRE_CONDITION( pCursor->memObject.objType == PSON_IDENT_CURSOR );
   PSO_TRACE_ENTER( pContext );

   *pNumItems = pCursor->listOfElements.currentSize;

   PSO_TRACE_EXIT( pContext );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

