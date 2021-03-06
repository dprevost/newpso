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

#include <jni.h>
#include <photon/photon.h>

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */


/*
 * Class:     org_photon_psoHashmap
 * Method:    initSession
 * Signature: (Z)J
 */
JNIEXPORT jlong JNICALL Java_org_photon_psoHashmap_initSession (
   JNIEnv * env, 
   jobject  obj )
{
   int errcode;
   jclass exc;
   char msg[100];
   PSO_HANDLE handle;
   PSO_HANDLE shmemHandle;
   
   errcode = psoInitSession( shmemHandle, &handle );

   // Normal return
   if ( errcode == PSO_OK ) return (size_t) handle;
   
   // Throw a java exception

   exc = (*env)->FindClass( env, "org/photon/psoException" );
   if ( exc  != NULL ) {
      sprintf( msg, "photon Error = %d", errcode );
      (*env)->ThrowNew( env, exc, msg );
   }

   return (size_t) NULL; 
}
   
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

