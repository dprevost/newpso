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

#include "Common/Common.h"
#include <photon/photon.h>
#include "Tests/PrintError.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main( int argc, char * argv[] )
{
   PSO_HANDLE sessionHandle, folderHandle;
   int errcode;
   psoObjectDefinition def = { PSO_QUEUE, 0, 0, 0 };
   psoFieldDefinition fields[1] = {
      { "Field_1", PSO_VARCHAR, {10} }
   };
   PSO_HANDLE dataDefHandle;
   
   if ( argc > 1 ) {
      errcode = psoInit( argv[1], argv[0] );
   }
   else {
      errcode = psoInit( "10701", argv[0] );
   }
   assert_true( errcode == PSO_OK );
   
   errcode = psoInitSession( &sessionHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateFolder( sessionHandle,
                              "/api_folder_create_object",
                              strlen("/api_folder_create_object") );
   assert_true( errcode == PSO_OK );

   errcode = psoFolderOpen( sessionHandle,
                            "/api_folder_create_object",
                            strlen("/api_folder_create_object"),
                            &folderHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoDataDefCreate( sessionHandle,
                               "Definition",
                               strlen("Definition"),
                               PSO_DEF_PHOTON_ODBC_SIMPLE,
                               (unsigned char *)fields,
                               sizeof(psoFieldDefinition),
                               &dataDefHandle );
   assert_true( errcode == PSO_OK );

   /* Invalid arguments to tested function. */

   errcode = psoFolderCreateQueue( NULL,
                                   "api_folder_create",
                                   strlen("api_folder_create"),
                                   &def,
                                   dataDefHandle );
   assert_true( errcode == PSO_NULL_HANDLE );

   errcode = psoFolderCreateQueue( sessionHandle,
                                   "api_folder_create",
                                   strlen("api_folder_create"),
                                   &def,
                                   dataDefHandle );
   assert_true( errcode == PSO_WRONG_TYPE_HANDLE );

   errcode = psoFolderCreateQueue( folderHandle,
                                   NULL,
                                   strlen("api_folder_create"),
                                   &def,
                                   dataDefHandle );
   assert_true( errcode == PSO_INVALID_OBJECT_NAME );

   errcode = psoFolderCreateQueue( folderHandle,
                                   "api_folder_create",
                                   0,
                                   &def,
                                   dataDefHandle );
   assert_true( errcode == INVALID_LENGTH );

   errcode = psoFolderCreateQueue( folderHandle,
                                   "api_folder_create",
                                   strlen("api_folder_create"),
                                   NULL,
                                   dataDefHandle );
   assert_true( errcode == PSO_NULL_POINTER );

   def.type = PSO_FOLDER;
   errcode = psoFolderCreateQueue( folderHandle,
                                   "api_folder_create",
                                   strlen("api_folder_create"),
                                   &def,
                                   dataDefHandle );
   if ( errcode != PSO_WRONG_OBJECT_TYPE ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   def.type = PSO_QUEUE;

   errcode = psoFolderCreateQueue( folderHandle,
                                   "api_folder_create",
                                   strlen("api_folder_create"),
                                   &def,
                                   NULL );
   assert_true( errcode == PSO_NULL_HANDLE );

   /* End of invalid args. This call should succeed. */
   errcode = psoFolderCreateQueue( folderHandle,
                                   "api_folder_create",
                                   strlen("api_folder_create"),
                                   &def,
                                   dataDefHandle );
   assert_true( errcode == PSO_OK );

   /* Close the folder and try to act on it */
   
   errcode = psoFolderClose( folderHandle );
   assert_true( errcode == PSO_OK );
   errcode = psoFolderCreateQueue( folderHandle,
                                   "api_folder_create2",
                                   strlen("api_folder_create2"),
                                   &def,
                                   dataDefHandle );
   assert_true( errcode == PSO_WRONG_TYPE_HANDLE );

   /* Reopen the folder, close the process and try to act on the session */

   errcode = psoFolderOpen( sessionHandle,
                            "/api_folder_create_object",
                            strlen("/api_folder_create_object"),
                            &folderHandle );
   assert_true( errcode == PSO_OK );
   psoExit();
   
   errcode = psoFolderCreateQueue( folderHandle,
                                   "api_folder_create3",
                                   strlen("api_folder_create3"),
                                   &def,
                                   dataDefHandle );
   assert_true( errcode == PSO_SESSION_IS_TERMINATED );

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

