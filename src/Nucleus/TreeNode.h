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

#include "Engine.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
	
/**
 * This structure contains the information needed for all the leaves and
 * the branches of the tree of objects/containers visible from the API.
 */
struct pson2TreeNode2
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
  
   /** Offset to the string used for the key. */
//   ptrdiff_t myHashItem;
   
   /** Offset to the transaction info (psonTxStatus). */
   ptrdiff_t txStatusOffset;

   /** Offset to the parent of this object. */
   /* PSON_NULL_OFFSET for top folder ("/") */
   ptrdiff_t myParentOffset;
   
};

/**
 * This structure contains the information needed for all the leaves and
 * the branches of the tree of objects/containers visible from the API.
 */
struct psonTreeNode
{
   /** Count the number of uncommitted/unrollbacked transaction ops are
    * still to be processed on this object (or more exactly on its data).
    * This is used to make sure that an object is not destroyed while
    * transactions (on its data) are not processed yet.
    */
   size_t txCounter;
  
   /** Offset to the string used for the key. */
   ptrdiff_t myHashItem;
   
   /** Offset to the transaction info (psonTxStatus). */
   ptrdiff_t txStatusOffset;

   /** Offset to the parent of this object. */
   /* PSON_NULL_OFFSET for top folder ("/") */
   ptrdiff_t myParentOffset;
   
};

typedef struct psonTreeNode psonTreeNode;
typedef struct pson2TreeNode2 pson2TreeNode2;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline 
void pson2TreeNode2Init( pson2TreeNode2 * pNode,
                       ptrdiff_t      myOffset,
                       psoObjectType  apiType,
                       ptrdiff_t      txStatusOffset,
                       ptrdiff_t      parentOffset )
{
   PSO_PRE_CONDITION( pNode != NULL );
   
   pNode->offset         = myOffset;
   pNode->apiType        = apiType;
   pNode->txCounter      = 0;
   pNode->txStatusOffset = txStatusOffset;
   pNode->myParentOffset = parentOffset;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline 
void pson2TreeNode2Fini( pson2TreeNode2 * pNode )
{
   PSO_PRE_CONDITION( pNode != NULL );
   
   pNode->offset         = PSON_NULL_OFFSET;
   pNode->apiType        = 0;
   pNode->txCounter      = 0;
   pNode->txStatusOffset = PSON_NULL_OFFSET;
   pNode->myParentOffset = PSON_NULL_OFFSET;
//   pNode->myHashItem     = PSON_NULL_OFFSET;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline 
void psonTreeNodeInit( psonTreeNode * pNode,
                       ptrdiff_t      txStatusOffset,
                       ptrdiff_t      parentOffset,
                       ptrdiff_t      hashItemOffset )
{
   PSO_PRE_CONDITION( pNode != NULL );
   
   pNode->txCounter      = 0;
   pNode->txStatusOffset = txStatusOffset;
   pNode->myParentOffset = parentOffset;
   pNode->myHashItem     = hashItemOffset;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline 
void psonTreeNodeFini( psonTreeNode * pNode )
{
   PSO_PRE_CONDITION( pNode != NULL );
   
   pNode->txCounter      = 0;
   pNode->txStatusOffset = PSON_NULL_OFFSET;
   pNode->myParentOffset = PSON_NULL_OFFSET;
   pNode->myHashItem     = PSON_NULL_OFFSET;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSON_TREE_NODE_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

