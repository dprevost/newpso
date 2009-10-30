/*
 * Copyright (C) 2007-2009 Daniel Prevost <dprevost@photonsoftware.org>
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

#ifndef PSON_TREE_NODE_H
#define PSON_TREE_NODE_H

#include "Nucleus/Engine.h"
#include "Nucleus/SessionContext.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
	
/**
 * This structure contains the information needed for all the leaves and
 * the branches of the tree of objects/containers visible from the API.
 */
struct psonTreeNode
{
   /** Offset of the object */
   ptrdiff_t offset;
   
   /** The object type as seen from the API. */
   enum psoObjectType apiType;
   
   /** Count the number of uncommitted/unrollbacked transaction ops are
    * still to be processed on this object (or more exactly on its data).
    * This is used to make sure that an object is not destroyed while
    * transactions (on its data) are not processed yet.
    */
   size_t txCounter;
  
   /** Offset to the transaction info (psonTxStatus). */
   ptrdiff_t txStatusOffset;

   /** Offset to the parent of this object. */
   /* PSON_NULL_OFFSET for top folder ("/") */
   ptrdiff_t myParentOffset;
   
};

typedef struct psonTreeNode psonTreeNode;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline 
void psonTreeNodeInit( psonTreeNode       * pNode,
                       ptrdiff_t            myOffset,
                       psoObjectType        apiType,
                       ptrdiff_t            txStatusOffset,
                       ptrdiff_t            parentOffset,
                       psonSessionContext * pContext )
{
   PSO_PRE_CONDITION( pNode != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_TRACE_ENTER_NUCLEUS( pContext );
   
   pNode->offset         = myOffset;
   pNode->apiType        = apiType;
   pNode->txCounter      = 0;
   pNode->txStatusOffset = txStatusOffset;
   pNode->myParentOffset = parentOffset;

   PSO_TRACE_EXIT_NUCLEUS( pContext, true );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline 
void psonTreeNodeFini( psonTreeNode       * pNode,
                       psonSessionContext * pContext )
{
   PSO_PRE_CONDITION( pNode != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_TRACE_ENTER_NUCLEUS( pContext );
   
   pNode->offset         = PSON_NULL_OFFSET;
   pNode->apiType        = (psoObjectType)0;
   pNode->txCounter      = 0;
   pNode->txStatusOffset = PSON_NULL_OFFSET;
   pNode->myParentOffset = PSON_NULL_OFFSET;

   PSO_TRACE_EXIT_NUCLEUS( pContext, true );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#if defined(PSO_USE_TRACE)
void psonTreeNodeDump( psonTreeNode       * pNode,
                       int                  indent,
                       psonSessionContext * pContext );
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSON_TREE_NODE_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

