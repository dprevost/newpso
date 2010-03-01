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

#include "Nucleus/Definitions.h"
#include "Nucleus/SessionContext.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#if defined(PSO_USE_TRACE)
void psonDataDefinitionDump( psonDataDefinition * pDataDefinition,
                             int                  indent,
                             psonSessionContext * pContext )
{
   unsigned int i, j;

   DO_INDENT( pContext, indent );
   fprintf( pContext->tracefp, "psonDataDefinition (%p) offset = "PSO_PTRDIFF_T_FORMAT"\n",
      pDataDefinition, SET_OFFSET(pContext->pBaseAddress, pDataDefinition) );
   if ( pDataDefinition == NULL ) return;

   switch( pDataDefinition->type ) {

   case PSO_DEF_FIRST_TYPE:
      DO_INDENT( pContext, indent );
      fprintf( pContext->tracefp, "Type is first - probably an error\n" );
      break;

   case PSO_DEF_USER_DEFINED:
      DO_INDENT( pContext, indent );
      fprintf( pContext->tracefp, "Type is user defined\n" );
      break;
   
   case PSO_DEF_PHOTON_ODBC_SIMPLE:
      DO_INDENT( pContext, indent );
      fprintf( pContext->tracefp, "Type is a simplified version of ODBC\n" );
      break;

   case PSO_DEF_PROTO_BUF:
      DO_INDENT( pContext, indent );
      fprintf( pContext->tracefp, "Type is Google protocol buffer\n" );
      break;

   case PSO_DEF_LAST_TYPE:
      DO_INDENT( pContext, indent );
      fprintf( pContext->tracefp, "Type is last - probably an error\n" );
      break;
   }
   
   fprintf( pContext->tracefp, "Length of definition: %ud\n", 
      pDataDefinition->definitionLength );

   if ( pDataDefinition->definitionLength > 0 ) {
      DO_INDENT( pContext, indent );
      fprintf( pContext->tracefp, "Definition in hex     ASCII\n" );
   }
   for ( i = 0; i < pDataDefinition->definitionLength; ++i ) {
      
      fprintf( pContext->tracefp, "%X", pDataDefinition->definition[i] );
      if ( ((i+1)%25) == 0 ) {
         fprintf( pContext->tracefp, " " );
         for ( j = i-24; j < i+1; ++j ) {
            if ( isprint(pDataDefinition->definition[j]) ) {
               fprintf( pContext->tracefp, "%c", (char)pDataDefinition->definition[j] );
            }
            else {
               fprintf( pContext->tracefp, " " );
            }
         }
         fprintf( pContext->tracefp, "\n" );
         if ( i != pDataDefinition->definitionLength-1 ) DO_INDENT( pContext, indent );
      }
   }

   DO_INDENT( pContext, indent );
   fprintf( pContext->tracefp, "psonDataDefinition END\n" );
}
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#if defined(PSO_USE_TRACE)
void psonKeyDefinitionDump( psonKeyDefinition  * pKeyDefinition,
                            int                  indent,
                            psonSessionContext * pContext )
{
   DO_INDENT( pContext, indent );
   fprintf( pContext->tracefp, "psonKeyDefinition (%p) offset = "PSO_PTRDIFF_T_FORMAT"\n",
      pKeyDefinition, SET_OFFSET(pContext->pBaseAddress, pKeyDefinition) );
   if ( pKeyDefinition == NULL ) return;

   DO_INDENT( pContext, indent );
   fprintf( pContext->tracefp, "psonKeyDefinition END\n" );
}
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#if defined(PSO_USE_TRACE2)
void psonMemObjIdentifierDump( psonMemObjIdent identifier, int indent )
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
