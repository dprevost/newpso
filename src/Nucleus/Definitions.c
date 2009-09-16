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

#include "Nucleus/Definitions.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#if defined(PSO_USE_TRACE)
void psonDataDefinitionDump( psonDataDefinition * pDataDefinition, int indent )
{
   unsigned int i, j;

   DO_INDENT( indent );
   fprintf( stderr, "psonDataDefinition (%p) offset = "PSO_PTRDIFF_T_FORMAT"\n",
      pDataDefinition, SET_OFFSET(pDataDefinition) );
   if ( pDataDefinition == NULL ) return;

   switch( pDataDefinition->type ) {

   case PSO_DEF_FIRST_TYPE:
      DO_INDENT( indent );
      fprintf( stderr, "Type is first - probably an error\n" );
      break;

   case PSO_DEF_NONE:
      DO_INDENT( indent );
      fprintf( stderr, "Type is none - might be an error\n" );
      break;
   
   case PSO_DEF_USER_DEFINED:
      DO_INDENT( indent );
      fprintf( stderr, "Type is user defined\n" );
      break;
   
   case PSO_DEF_PHOTON_ODBC_SIMPLE:
      DO_INDENT( indent );
      fprintf( stderr, "Type is a simplified version of ODBC\n" );
      break;

   case PSO_DEF_PROTO_BUF:
      DO_INDENT( indent );
      fprintf( stderr, "Type is Google protocol buffer\n" );
      break;

   case PSO_DEF_LAST_TYPE:
      DO_INDENT( indent );
      fprintf( stderr, "Type is last - probably an error\n" );
      break;
   }
   
   fprintf( stderr, "Length of definition: %ud\n", 
      pDataDefinition->definitionLength );

   if ( pDataDefinition->definitionLength > 0 ) {
      DO_INDENT( indent );
      fprintf( stderr, "Definition in hex     ASCII\n" );
   }
   for ( i = 0; i < pDataDefinition->definitionLength; ++i ) {
      
      fprintf( stderr, "%X", pDataDefinition->definition[i] );
      if ( ((i+1)%25) == 0 ) {
         fprintf( stderr, " " );
         for ( j = i-24; j < i+1; ++j ) {
            if ( isprint(pDataDefinition->definition[j]) ) {
               fprintf( stderr, "%c", (char)pDataDefinition->definition[j] );
            }
            else {
               fprintf( stderr, " " );
            }
         }
         fprintf( stderr, "\n" );
         if ( i != pDataDefinition->definitionLength-1 ) DO_INDENT( indent );
      }
   }

   DO_INDENT( indent );
   fprintf( stderr, "psonDataDefinition END\n" );
}
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#if defined(PSO_USE_TRACE)
void psonKeyDefinitionDump( psonKeyDefinition * pKeyDefinition, int indent )
{
   DO_INDENT( indent );
   fprintf( stderr, "psonKeyDefinition (%p) offset = "PSO_PTRDIFF_T_FORMAT"\n",
      pKeyDefinition, SET_OFFSET(pKeyDefinition) );
   if ( pKeyDefinition == NULL ) return;

   DO_INDENT( indent );
   fprintf( stderr, "psonKeyDefinition END\n" );
}
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#if defined(PSO_USE_TRACE)
void psonMemObjIdentifierDump( psonMemObjIdent identifier, int indent )
{
   switch( identifier ) {
   case PSON_IDENT_CLEAR:
      DO_INDENT( indent );
      fprintf( stderr, "Object type: \"Clear!!!\" likely an error\n" );
      break;
      
   case PSON_IDENT_LIMBO:
      DO_INDENT( indent );
      fprintf( stderr, "Object type: \"Limbo!!!\" possibly an error\n" );
      break;
      
   case PSON_IDENT_ALLOCATED:
      DO_INDENT( indent );
      fprintf( stderr, "Object type: \"Allocated!!!\" likely an error\n" );
      break;
      
   case PSON_IDENT_FIRST:
      DO_INDENT( indent );
      fprintf( stderr, "Object type: \"First!!!\" likely an error\n" );
      break;
      
   case PSON_IDENT_ALLOCATOR:
      DO_INDENT( indent );
      fprintf( stderr, "Object type: Main memory allocator\n" );
      break;
      
   case PSON_IDENT_FOLDER:
      DO_INDENT( indent );
      fprintf( stderr, "Object type: Folder\n" );
      break;
      
   case PSON_IDENT_HASH_MAP:
      DO_INDENT( indent );
      fprintf( stderr, "Object type: Hash Map\n" );
      break;
      
   case PSON_IDENT_PROCESS:
      DO_INDENT( indent );
      fprintf( stderr, "Object type: Process\n" );
      break;
      
   case PSON_IDENT_PROCESS_MGR:
      DO_INDENT( indent );
      fprintf( stderr, "Object type: Process Manager\n" );
      break;
      
   case PSON_IDENT_QUEUE:
      DO_INDENT( indent );
      fprintf( stderr, "Object type: Queue\n" );
      break;
      
   case PSON_IDENT_SESSION:
      DO_INDENT( indent );
      fprintf( stderr, "Object type: Session\n" );
      break;
      
   case PSON_IDENT_TRANSACTION:
      DO_INDENT( indent );
      fprintf( stderr, "Object type: Transaction\n" );
      break;
      
   case PSON_IDENT_MAP:
      DO_INDENT( indent );
      fprintf( stderr, "Object type: Fast Map\n" );
      break;
      
   case PSON_IDENT_CURSOR:
      DO_INDENT( indent );
      fprintf( stderr, "Object type: Cursor\n" );
      break;
      
   case PSON_IDENT_LAST:
      DO_INDENT( indent );
      fprintf( stderr, "Object type: \"Last!!!\" likely an error\n" );
      break;
      
   default:
      DO_INDENT( indent );
      fprintf( stderr, "Object type: Unknown, likely an error\n" );
      break;
   }
}
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
