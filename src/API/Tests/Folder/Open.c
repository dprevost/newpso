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
#include "API/Folder.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main( int argc, char * argv[] )
{
   PSO_HANDLE objHandle,  sessionHandle;
   PSO_HANDLE objHandle2, sessionHandle2;
   int errcode;
   char junk[12];
   
   memset( junk, 0, 12 );
   
   if ( argc > 1 ) {
      errcode = psoInit( argv[1], argv[0] );
   }
   else {
      errcode = psoInit( "10701", argv[0] );
   }
   assert_true( errcode == PSO_OK );
   
   errcode = psoInitSession( &sessionHandle );
   assert_true( errcode == PSO_OK );
   errcode = psoInitSession( &sessionHandle2 );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateFolder( sessionHandle,
                              "/api_folder_open",
                              strlen("/api_folder_open") );
   assert_true( errcode == PSO_OK );

   /* Invalid arguments to tested function. */

   errcode = psoFolderOpen( NULL,
                            "/api_folder_open",
                            strlen("/api_folder_open"),
                            &objHandle );
   assert_true( errcode == PSO_NULL_HANDLE );

   objHandle = (PSO_HANDLE) junk;
   errcode = psoFolderOpen( objHandle,
                            "/api_folder_open",
                            strlen("/api_folder_open"),
                            &objHandle );
   assert_true( errcode == PSO_WRONG_TYPE_HANDLE );

   errcode = psoFolderOpen( sessionHandle,
                            NULL,
                            strlen("/api_folder_open"),
                            &objHandle );
   assert_true( errcode == PSO_INVALID_OBJECT_NAME );

   errcode = psoFolderOpen( sessionHandle,
                            "/api_folder_open",
                            0,
                            &objHandle );
   assert_true( errcode == INVALID_LENGTH );

   errcode = psoFolderOpen( sessionHandle,
                            "/api_folder_open",
                            strlen("/api_folder_open"),
                            NULL );
   assert_true( errcode == PSO_NULL_HANDLE );

   /* End of invalid args. This call should succeed. */
   errcode = psoFolderOpen( sessionHandle,
                            "/api_folder_open",
                            strlen("/api_folder_open"),
                            &objHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoFolderOpen( sessionHandle2,
                            "/api_folder_open",
                            strlen("/api_folder_open"),
                            &objHandle2 );
   assert_true( errcode == PSO_OBJECT_IS_IN_USE );
   
   /* Close the session and try to act on the object */

   errcode = psoFolderClose( objHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoExitSession( sessionHandle );
   assert_true( errcode == PSO_OK );

   /*
    * Warning: sessionHandle is a pointer to deallocated memory. It is
    * a bit dangerous to use it.
    */
   errcode = psoFolderOpen( sessionHandle,
                            "/api_folder_open",
                            strlen("/api_folder_open"),
                            &objHandle );
   assert_true( errcode == PSO_WRONG_TYPE_HANDLE );

   psoExit();

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

