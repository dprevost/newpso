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

#include "Nucleus/LinkNode.h"
#include "Nucleus/SessionContext.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#if defined(PSO_USE_TRACE)
void psonFreeBufferNodeDump( psonFreeBufferNode * pBuffer,
                             int                  indent,
                             psonSessionContext * pContext )
{
   DO_INDENT( pContext, indent );
   fprintf( pContext->tracefp, "psonFreeBufferNode (%p) offset = "PSO_PTRDIFF_T_FORMAT"\n",
      pBuffer, SET_OFFSET(pContext->pBaseAddress, pBuffer) );
   if ( pBuffer == NULL ) return;

   psonLinkNodeDump( &pBuffer->node, indent + 2, pContext );

   DO_INDENT( pContext, indent + 2 );
   fprintf( pContext->tracefp, "Number of buffers: "PSO_SIZE_T_FORMAT"\n",
      pBuffer->numBuffers );

   DO_INDENT( pContext, indent );
   fprintf( pContext->tracefp, "psonFreeBufferNode END\n" );
}
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#if defined(PSO_USE_TRACE)
void psonLinkNodeDump( psonLinkNode       * pNode,
                       int                  indent,
                       psonSessionContext * pContext )
{
   DO_INDENT( pContext, indent );
   fprintf( pContext->tracefp, "psonLinkNode (%p) offset = "PSO_PTRDIFF_T_FORMAT"\n",
      pNode, SET_OFFSET(pContext->pBaseAddress, pNode) );
   if ( pNode == NULL ) return;

   DO_INDENT( pContext, indent + 2 );
   fprintf( pContext->tracefp, "Next offset: "PSO_PTRDIFF_T_FORMAT"\n",
      pNode->nextOffset );

   DO_INDENT( pContext, indent + 2 );
   fprintf( pContext->tracefp, "Previous offset: "PSO_PTRDIFF_T_FORMAT"\n",
      pNode->previousOffset );

   DO_INDENT( pContext, indent );
   fprintf( pContext->tracefp, "psonLinkNode END\n" );
}
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

