/*
 * Copyright (C) 2009-2010 Daniel Prevost <dprevost@photonsoftware.org>
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
#include "Nucleus/SessionContext.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#if defined(PSO_USE_TRACE)

char * apiTypes[] = {
   "Folder",
   "LIFO queue",
   "FIFO queue",
   "Hash Map",
   "Fast Map"
};

void psonTreeNodeDump( psonTreeNode       * pNode,
                       int                  indent,
                       psonSessionContext * pContext )
{
   DO_INDENT( pContext, indent );
   fprintf( pContext->tracefp, "psonTreeNode (%p) offset = "PSO_PTRDIFF_T_FORMAT"\n",
      pNode, SET_OFFSET(g_pBaseAddr, pNode) );
   if ( pNode == NULL ) return;

   DO_INDENT( pContext, indent + 2 );
   fprintf( pContext->tracefp, "offset of the object: "PSO_PTRDIFF_T_FORMAT"\n",
      pNode->offset );
   if ( pNode->apiType < PSO_FOLDER || pNode->apiType >= PSO_LAST_OBJECT_TYPE ) {
      DO_INDENT( pContext, indent + 2 );
      fprintf( pContext->tracefp, "object api type: unknown (value: %d)\n", pNode->apiType );
   }
   else {
      DO_INDENT( pContext, indent + 2 );
      fprintf( pContext->tracefp, "object api type: %s\n", apiTypes[pNode->apiType-1] );
   }
   DO_INDENT( pContext, indent + 2 );
   fprintf( pContext->tracefp, "tx counter: %d\n", pNode->txCounter );

   DO_INDENT( pContext, indent + 2 );
   fprintf( pContext->tracefp, "offset of the tx status object: "PSO_PTRDIFF_T_FORMAT"\n",
      pNode->txStatusOffset );
   DO_INDENT( pContext, indent + 2 );
   fprintf( pContext->tracefp, "offset of the parent object: "PSO_PTRDIFF_T_FORMAT"\n",
      pNode->myParentOffset );

   DO_INDENT( pContext, indent );
   fprintf( pContext->tracefp, "psonTreeNode END\n" );
}

#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

