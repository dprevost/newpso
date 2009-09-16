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

#include "Nucleus/LinkNode.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#if defined(PSO_USE_TRACE)
void psonFreeBufferNodeDump( psonFreeBufferNode * pBuffer, int indent )
{
   DO_INDENT( indent );
   fprintf( stderr, "psonFreeBufferNode (%p) offset = "PSO_PTRDIFF_T_FORMAT"\n",
      pBuffer, SET_OFFSET(pBuffer) );
   if ( pBuffer == NULL ) return;

   psonLinkNodeDump( &pBuffer->node, indent + 2 );

   DO_INDENT( indent + 2 );
   fprintf( stderr, "Number of buffers: "PSO_SIZE_T_FORMAT"\n",
      pBuffer->numBuffers );

   DO_INDENT( indent );
   fprintf( stderr, "psonFreeBufferNode END\n" );
}
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#if defined(PSO_USE_TRACE)
void psonLinkNodeDump( psonLinkNode * pNode, int indent )
{
   DO_INDENT( indent );
   fprintf( stderr, "psonLinkNode (%p) offset = "PSO_PTRDIFF_T_FORMAT"\n",
      pNode, SET_OFFSET(pNode) );
   if ( pNode == NULL ) return;

   DO_INDENT( indent + 2 );
   fprintf( stderr, "Next offset: "PSO_PTRDIFF_T_FORMAT"\n",
      pNode->nextOffset );

   DO_INDENT( indent + 2 );
   fprintf( stderr, "Previous offset: "PSO_PTRDIFF_T_FORMAT"\n",
      pNode->previousOffset );

   DO_INDENT( indent );
   fprintf( stderr, "psonLinkNode END\n" );
}
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

