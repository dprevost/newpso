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

#include "Nucleus/Engine.h"
#include "Nucleus/SessionContext.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#if defined(PSO_USE_TRACE)
void psonMemObjIdentifierDump( psonMemObjIdent      identifier,
                               int                  indent,
                               psonSessionContext * pContext )
{
   switch( identifier ) {
   case PSON_IDENT_CLEAR:
      DO_INDENT( pContext, indent );
      fprintf( pContext->tracefp, "Object type: \"Clear!!!\" likely an error\n" );
      break;
      
   case PSON_IDENT_LIMBO:
      DO_INDENT( pContext, indent );
      fprintf( pContext->tracefp, "Object type: \"Limbo!!!\" possibly an error\n" );
      break;
      
   case PSON_IDENT_ALLOCATED:
      DO_INDENT( pContext, indent );
      fprintf( pContext->tracefp, "Object type: \"Allocated!!!\" likely an error\n" );
      break;
      
   case PSON_IDENT_FIRST:
      DO_INDENT( pContext, indent );
      fprintf( pContext->tracefp, "Object type: \"First!!!\" likely an error\n" );
      break;
      
   case PSON_IDENT_ALLOCATOR:
      DO_INDENT( pContext, indent );
      fprintf( pContext->tracefp, "Object type: Main memory allocator\n" );
      break;
      
   case PSON_IDENT_FOLDER:
      DO_INDENT( pContext, indent );
      fprintf( pContext->tracefp, "Object type: Folder\n" );
      break;
      
   case PSON_IDENT_HASH_MAP:
      DO_INDENT( pContext, indent );
      fprintf( pContext->tracefp, "Object type: Hash Map\n" );
      break;
      
   case PSON_IDENT_PROCESS:
      DO_INDENT( pContext, indent );
      fprintf( pContext->tracefp, "Object type: Process\n" );
      break;
      
   case PSON_IDENT_PROCESS_MGR:
      DO_INDENT( pContext, indent );
      fprintf( pContext->tracefp, "Object type: Process Manager\n" );
      break;
      
   case PSON_IDENT_QUEUE:
      DO_INDENT( pContext, indent );
      fprintf( pContext->tracefp, "Object type: Queue\n" );
      break;
      
   case PSON_IDENT_SESSION:
      DO_INDENT( pContext, indent );
      fprintf( pContext->tracefp, "Object type: Session\n" );
      break;
      
   case PSON_IDENT_TRANSACTION:
      DO_INDENT( pContext, indent );
      fprintf( pContext->tracefp, "Object type: Transaction\n" );
      break;
      
   case PSON_IDENT_MAP:
      DO_INDENT( pContext, indent );
      fprintf( pContext->tracefp, "Object type: Fast Map\n" );
      break;
      
   case PSON_IDENT_CURSOR:
      DO_INDENT( pContext, indent );
      fprintf( pContext->tracefp, "Object type: Cursor\n" );
      break;
      
   case PSON_IDENT_LAST:
      DO_INDENT( pContext, indent );
      fprintf( pContext->tracefp, "Object type: \"Last!!!\" likely an error\n" );
      break;
      
   default:
      DO_INDENT( pContext, indent );
      fprintf( pContext->tracefp, "Object type: Unknown, likely an error\n" );
      break;
   }
}
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
