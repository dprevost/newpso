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

#include <jni.h>
#include <photon/photon.h>
#include <string.h>

#include "jni_photon.h"
#include "org_photon_Session.h"
#include "API/DataDefinition.h"
#include "API/KeyDefinition.h"

jfieldID g_idSessionHandle;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_Session
 * Method:    initIDs
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_org_photon_Session_initIDs( JNIEnv * env, jclass sessionClass )
{
   g_idSessionHandle = (*env)->GetFieldID( env, sessionClass, "handle", "J" );
   if ( g_idSessionHandle == NULL ) return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_Session
 * Method:    psoCommit
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL 
Java_org_photon_Session_psoCommit( JNIEnv  * env, 
                                   jobject   jobj, 
                                   jlong     jhandle )
{
   int errcode;
   size_t handle = (size_t) jhandle;
   
   errcode = psoCommit( (PSO_HANDLE)handle );
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_Session
 * Method:    psoCreateFolder
 * Signature: (JLjava/lang/String;)I
 */
JNIEXPORT jint JNICALL
Java_org_photon_Session_psoCreateFolder( JNIEnv * env,
                                         jobject  jobj,
                                         jlong    jhandle,
                                         jstring  jname )
{
   int errcode;

   /* Native variables */
   size_t handle = (size_t) jhandle;
   const char * name;

   name = (*env)->GetStringUTFChars( env, jname, NULL );
   if ( name == NULL ) {
      return PSO_NOT_ENOUGH_HEAP_MEMORY; // out-of-memory exception by the JVM
   }

   errcode = psoCreateFolder( (PSO_HANDLE) handle,
                              name,
                              strlen(name) );

   (*env)->ReleaseStringUTFChars( env, jname, name );

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_Session
 * Method:    psoCreateObject
 * Signature: (JLjava/lang/String;Lorg/photon/ObjectDefinition;J)I
 */
JNIEXPORT jint JNICALL
Java_org_photon_Session_psoCreateQueue( JNIEnv * env,
                                        jobject  jobj,
                                        jlong    jhandle,
                                        jstring  jname,
                                        jobject  jdefinition )
{
   int errcode;

   /* Native variables */
   size_t handle = (size_t) jhandle;
   const char * name;
   psoObjectDefinition * pDefinition;
  
   pDefinition = Java2C_ObjectDefinition( env, jdefinition );
   if ( pDefinition == NULL ) return PSO_NOT_ENOUGH_HEAP_MEMORY;

   name = (*env)->GetStringUTFChars( env, jname, NULL );
   if ( name == NULL ) {
      free( pDefinition );
      return PSO_NOT_ENOUGH_HEAP_MEMORY; // out-of-memory exception by the JVM
   }

   errcode = psoCreateQueue( (PSO_HANDLE) handle,
                             name,
                             strlen(name),
                             pDefinition );
   
   (*env)->ReleaseStringUTFChars( env, jname, name );
   free( pDefinition );

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
/*
 * Class:     org_photon_Session
 * Method:    psoCreateKeyedObject
 * Signature: (JLjava/lang/String;Lorg/photon/ObjectDefinition;JJ)I
 */
JNIEXPORT jint JNICALL
Java_org_photon_Session_psoCreateMap( JNIEnv * env,
                                      jobject  jobj,
                                      jlong    jhandle,
                                      jstring  jname,
                                      jobject  jdefinition,
                                      jobject  jkeyDefinition )
{
   int errcode;

   /* Native variables */
   size_t handle = (size_t) jhandle;
   const char * name;
   psoObjectDefinition * pDefinition;
  
   pDefinition = Java2C_ObjectDefinition( env, jdefinition );
   if ( pDefinition == NULL ) return PSO_NOT_ENOUGH_HEAP_MEMORY;
   
   name = (*env)->GetStringUTFChars( env, jname, NULL );
   if ( name == NULL ) {
      free( pDefinition );
      return PSO_NOT_ENOUGH_HEAP_MEMORY; // out-of-memory exception by the JVM
   }

#if 0
   errcode = psoCreateMap( (PSO_HANDLE) handle,
                           name,
                           strlen(name),
                           pDefinition,
                           pKeyDefinition );
#endif
   (*env)->ReleaseStringUTFChars( env, jname, name );
   free( pDefinition );
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_Session
 * Method:    psoDestroyObject
 * Signature: (JLjava/lang/String;)I
 */
JNIEXPORT jint JNICALL 
Java_org_photon_Session_psoDestroyObject( JNIEnv  * env,
                                          jobject   jobj,
                                          jlong     jhandle,
                                          jstring   jname )
{
   int errcode;
   size_t handle = (size_t) jhandle;
   const char * objectName;
   
   objectName = (*env)->GetStringUTFChars( env, jname, NULL );
   if ( objectName == NULL ) {
      return PSO_NOT_ENOUGH_HEAP_MEMORY; // out-of-memory exception by the JVM
   }
   
   errcode = psoDestroyObject( (PSO_HANDLE) handle,
                               objectName,
                               strlen(objectName) );

   (*env)->ReleaseStringUTFChars( env, jname, objectName );

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_Session
 * Method:    psoFini
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL 
Java_org_photon_Session_psoFini( JNIEnv  * env,
                                 jobject   jobj,
                                 jlong     jhandle )
{
   int errcode;
   size_t handle = (size_t)jhandle;
   
   errcode = psoExitSession( (PSO_HANDLE) handle );
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_Session
 * Method:    psoGetDefinition
 * Signature: (JLjava/lang/String;Lorg/photon/ObjectDefinition;)I
 */
JNIEXPORT jint JNICALL
Java_org_photon_Session_psoGetDefinition( JNIEnv * env,
                                          jobject  jobj,
                                          jlong    jhandle,
                                          jstring  jname,
                                          jobject  jdefinition )
{
   int errcode;
   size_t handle = (size_t) jhandle;
   const char * objectName;
   psoObjectDefinition * pDefinition = NULL;
   
   objectName = (*env)->GetStringUTFChars( env, jname, NULL );
   if ( objectName == NULL ) {
      return PSO_NOT_ENOUGH_HEAP_MEMORY; // out-of-memory exception by the JVM
   }

   errcode = psoaSessionGetDef( (PSO_HANDLE) handle,
                                objectName,
                                strlen(objectName),
                                &pDefinition );
   (*env)->ReleaseStringUTFChars( env, jname, objectName );

   if ( errcode == 0 ) {
      errcode = C2Java_ObjectDefinition( env,
                                         jdefinition,
                                         pDefinition );     
   }

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_Session
 * Method:    psoGetKeyDefinition
 * Signature: (JLjava/lang/String;Lorg/photon/KeyDefinition;)I
 */
JNIEXPORT jint JNICALL
Java_org_photon_Session_psoGetKeyDefinition( JNIEnv * env,
                                             jobject  jobj,
                                             jlong    jhandle,
                                             jstring  jname,
                                             jobject  jdefinition )
{
   int errcode;
   size_t handle = (size_t) jhandle;
   const char * objectName;
   psoKeyDefinition * pDefinition = NULL;
   
   objectName = (*env)->GetStringUTFChars( env, jname, NULL );
   if ( objectName == NULL ) {
      return PSO_NOT_ENOUGH_HEAP_MEMORY; // out-of-memory exception by the JVM
   }

   errcode = psoaSessionGetKeyDef( (PSO_HANDLE) handle,
                                   objectName,
                                   strlen(objectName),
                                   &pDefinition );
   (*env)->ReleaseStringUTFChars( env, jname, objectName );

   if ( errcode == 0 ) {
      errcode = C2Java_KeyDefinition( env,
                                      jdefinition,
                                      pDefinition );     
   }

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_Session
 * Method:    psoGetInfo
 * Signature: (JLorg/photon/Info;)I
 */
JNIEXPORT jint JNICALL 
Java_org_photon_Session_psoGetInfo( JNIEnv  * env,
                                    jobject   jobj,
                                    jlong     jhandle,
                                    jobject   jinfo )
{
   int errcode;
   size_t handle = (size_t) jhandle;
   psoInfo info;
   jstring jstr;
   
   errcode = psoGetInfo( (PSO_HANDLE) handle, &info );
   if ( errcode == 0 ) {

      (*env)->SetLongField( env, jinfo, g_idInfoTotalSizeInBytes, info.totalSizeInBytes );
      (*env)->SetLongField( env, jinfo, g_idInfoAllocatedSizeInBytes, info.allocatedSizeInBytes );
      (*env)->SetLongField( env, jinfo, g_idInfoNumObjects, info.numObjects );
      (*env)->SetLongField( env, jinfo, g_idInfoNumGroups, info.numGroups );
      (*env)->SetLongField( env, jinfo, g_idInfoNumMallocs, info.numMallocs );
      (*env)->SetLongField( env, jinfo, g_idInfoNumFrees, info.numFrees );
      (*env)->SetLongField( env, jinfo, g_idInfoLargestFreeInBytes, info.largestFreeInBytes );
      (*env)->SetIntField ( env, jinfo, g_idInfoMemoryVersion, info.memoryVersion );
      (*env)->SetIntField ( env, jinfo, g_idInfoBigEndian, info.bigEndian );

      jstr = getNotNullTerminatedString( env, info.compiler, 20 );
      if ( jstr == NULL ) return PSO_NOT_ENOUGH_HEAP_MEMORY;
      (*env)->SetObjectField( env, jinfo, g_idInfoCompiler, jstr );
      (*env)->DeleteLocalRef( env, jstr );

      jstr = getNotNullTerminatedString( env, info.compilerVersion, 10 );
      if ( jstr == NULL ) return PSO_NOT_ENOUGH_HEAP_MEMORY;
      (*env)->SetObjectField( env, jinfo, g_idInfoCompilerVersion, jstr );
      (*env)->DeleteLocalRef( env, jstr );

      jstr = getNotNullTerminatedString( env, info.platform, 20 );
      if ( jstr == NULL ) return PSO_NOT_ENOUGH_HEAP_MEMORY;
      (*env)->SetObjectField( env, jinfo, g_idInfoPlatform, jstr );
      (*env)->DeleteLocalRef( env, jstr );

      jstr = getNotNullTerminatedString( env, info.dllVersion, 10 );
      if ( jstr == NULL ) return PSO_NOT_ENOUGH_HEAP_MEMORY;
      (*env)->SetObjectField( env, jinfo, g_idInfoDllVersion, jstr );
      (*env)->DeleteLocalRef( env, jstr );

      jstr = getNotNullTerminatedString( env, info.quasarVersion, 10 );
      if ( jstr == NULL ) return PSO_NOT_ENOUGH_HEAP_MEMORY;
      (*env)->SetObjectField( env, jinfo, g_idInfoQuasarVersion, jstr );
      (*env)->DeleteLocalRef( env, jstr );

      jstr = getNotNullTerminatedString( env, info.creationTime, 30 );
      if ( jstr == NULL ) return PSO_NOT_ENOUGH_HEAP_MEMORY;
      (*env)->SetObjectField( env, jinfo, g_idInfoCreationTime, jstr );
      (*env)->DeleteLocalRef( env, jstr );
      
   }
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_Session
 * Method:    psoGetStatus
 * Signature: (JLjava/lang/String;Lorg/photon/ObjectStatus;)I
 */
JNIEXPORT jint JNICALL 
Java_org_photon_Session_psoGetStatus( JNIEnv  * env,
                                      jobject   jobj,
                                      jlong     jhandle,
                                      jstring   jname,
                                      jobject   jstatus )
{
   int errcode;
   size_t handle = (size_t) jhandle;
   const char * objectName;
   psoObjStatus status;
   
   objectName = (*env)->GetStringUTFChars( env, jname, NULL );
   if ( objectName == NULL ) {
      return PSO_NOT_ENOUGH_HEAP_MEMORY; // out-of-memory exception by the JVM
   }

   errcode = psoGetStatus( (PSO_HANDLE) handle,
                           objectName,
                           strlen(objectName),
                           &status );
   (*env)->ReleaseStringUTFChars( env, jname, objectName );

   if ( errcode == 0 ) {
      C2Java_ObjStatus( env, jstatus, &status );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_Session
 * Method:    psoInit
 * Signature: ()I
 */
JNIEXPORT jint JNICALL 
Java_org_photon_Session_psoInit( JNIEnv * env, jobject jobj )
{
   int errcode;
   PSO_HANDLE handle;
   
   errcode = psoInitSession( &handle );

   // Normal return
   if ( errcode == PSO_OK ) {
      (*env)->SetLongField( env, jobj, g_idSessionHandle, (size_t) handle );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_Session
 * Method:    psoRollback
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL
Java_org_photon_Session_psoRollback( JNIEnv  * env, 
                                     jobject   jobj, 
                                     jlong     jhandle )
{
   int errcode;
   size_t handle = (size_t) jhandle;
   
   errcode = psoRollback( (PSO_HANDLE)handle );
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

