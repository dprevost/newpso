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

#include "LinkedList.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#if defined(PSO_USE_TRACE)
void psonLinkedListDump( psonLinkedList * pList, int indent )
{
   DO_INDENT( indent );
   fprintf( stderr, "psonLinkedList (%p) offset = "PSO_PTRDIFF_T_FORMAT"\n",
      pList, SET_OFFSET(pList) );
   if ( pList == NULL ) return;
   
   DO_INDENT( indent + 2 );
   fprintf( stderr, "Current size of the list: "PSO_SIZE_T_FORMAT"\n",
      pList->currentSize );

   if ( pList->initialized == PSON_LIST_SIGNATURE ) {
      DO_INDENT( indent + 2 );
      fprintf( stderr, "Signature is ok.\n" );
   }
   else {
      DO_INDENT( indent + 2 );
      fprintf( stderr, "Signature is wrong - value is %d, exepted: %d\n",
         pList->initialized, PSON_LIST_SIGNATURE );
   }

   DO_INDENT( indent );
   fprintf( stderr, "psonLinkedList END\n" );
}
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* Cleanup the list once we're done with it. */
void psonLinkedListFini( psonLinkedList     * pList,
                         psonSessionContext * pContext )
{   
   PSO_PRE_CONDITION( pList != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   /* Test to see if the list is initialized */
   PSO_INV_CONDITION( pList->initialized == PSON_LIST_SIGNATURE );
   PSO_TRACE_ENTER( pContext );
   
   /* We reset the node element to PSON_NULL_OFFSET. */
   psonLinkNodeInit( &pList->head, pContext );

   pList->currentSize    = 0;
   pList->initialized    = 0;

   PSO_TRACE_EXIT( pContext, true );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psonLinkedListInit( psonLinkedList     * pList,
                         psonSessionContext * pContext )
{
   PSO_PRE_CONDITION( pList != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_TRACE_ENTER( pContext );
   
   psonLinkNodeInit( &pList->head, pContext );
   pList->currentSize = 0;

   /* Make the list circular by pointing it back to itself. */
   pList->head.previousOffset = pList->head.nextOffset = 
      SET_OFFSET( &pList->head );

   pList->initialized = PSON_LIST_SIGNATURE;

   PSO_TRACE_EXIT( pContext, true );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psonLinkedListReset( psonLinkedList     * pList,
                          psonSessionContext * pContext )
{
   PSO_PRE_CONDITION( pList != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   /* Test to see if the list is initialized */
   PSO_INV_CONDITION( pList->initialized == PSON_LIST_SIGNATURE );
   PSO_TRACE_ENTER( pContext );

   pList->currentSize = 0;

   /* Make the list circular by pointing it back to itself. */
   pList->head.previousOffset = pList->head.nextOffset = 
      SET_OFFSET( &pList->head );

   PSO_TRACE_EXIT( pContext, true );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonLinkedListIsValid( psonLinkedList     * pList,
                            psonLinkNode       * pUnknown,
                            psonSessionContext * pContext )
{
   bool valid = false;
   
   psonLinkNode* pItem;

   PSO_PRE_CONDITION( pList != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   /* Test to see if the list is initialized */
   PSO_INV_CONDITION( pList->initialized == PSON_LIST_SIGNATURE );
   PSO_PRE_CONDITION( pUnknown   != NULL );
   PSO_TRACE_ENTER( pContext );

   pItem = &pList->head;
   
   GET_PTR( pItem, pItem->nextOffset, psonLinkNode );
   while ( pItem != &pList->head ) {
      if ( pItem == pUnknown ) {
         valid = true;
         break;
      }
      
      GET_PTR( pItem, pItem->nextOffset, psonLinkNode );
   }

   PSO_TRACE_EXIT( pContext, valid );
   return valid;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

