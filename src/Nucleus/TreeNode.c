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

#include "Nucleus/TreeNode.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#if defined(PSO_TRACE)

char * apiTypes[] = {
   "Folder",
   "LIFO queue",
   "FIFO queue",
   "Hash Map",
   "Fast Map"
};

void psonTreeNodeDump( psonTreeNode * pNode )
{
   fprintf(stderr, "psonTreeNode (%p) offset = "PSO_PTRDIFF_T_FORMAT"\n",
      pNode, SET_OFFSET(pNode) );
   if ( pNode == NULL ) return;
   fprintf( stderr, "  offset of the object: "PSO_PTRDIFF_T_FORMAT"\n",
      pNode->offset );
   if ( pNode->apiType < PSO_FOLDER || pNode->apiType >= PSO_LAST_OBJECT_TYPE ) {
      fprintf( stderr, "  object api type: unknown (value: %d)\n", pNode->apiType );
   }
   else {
      fprintf( stderr, "  object api type: %s\n", apiTypes[pNode->apiType-1] );
   }
   fprintf( stderr, "  tx counter: %d\n", pNode->txCounter );

   fprintf( stderr, "  offset of the tx status object: "PSO_PTRDIFF_T_FORMAT"\n",
      pNode->txStatusOffset );
   fprintf( stderr, "  offset of the parent object: "PSO_PTRDIFF_T_FORMAT"\n",
      pNode->myParentOffset );
}

#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
