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
#include <string.h>

#include "jni_photon.h"
#include "org_photon_Folder.h"
#include "API/DataDefinition.h"
#include "API/KeyDefinition.h"
#include "API/Folder.h"

jfieldID g_idFolderHandle;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_Folder
 * Method:    initIDs
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_org_photon_Folder_initIDs( JNIEnv * env, jclass folderClass )
{
   g_idFolderHandle = (*env)->GetFieldID( env, folderClass, "handle", "J" );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_Folder
 * Method:    psoClose
 * Signature: (J)V
 */
JNIEXPORT void JNICALL 
Java_org_photon_Folder_psoClose( JNIEnv * env, 
                                 jobject  jobj, 
                                 jlong    jhandle )
{
   int errcode;
   size_t handle = (size_t) jhandle;
   
   errcode = psoFolderClose( (PSO_HANDLE)handle );

   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_Folder
 * Method:    psoCreateFolder
 * Signature: (JLjava/lang/String;)I
 */
JNIEXPORT jint JNICALL
Java_org_photon_Folder_psoCreateFolder( JNIEnv  * env, 
                                        jobject   jobj,
                                        jlong     jhandle,
                                        jstring   jname )
{
   int errcode;

   /* Native variables */
   size_t handle = (size_t) jhandle;
   const char * name;

   name = (*env)->GetStringUTFChars( env, jname, NULL );
   if ( name == NULL ) {
      return PSO_NOT_ENOUGH_HEAP_MEMORY; // out-of-memory exception by the JVM
   }

   errcode = psoFolderCreateFolder( (PSO_HANDLE) handle, 
                                    name,
                                    strlen(name) );

   (*env)->ReleaseStringUTFChars( env, jname, name );

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_Folder
 * Method:    psoCreateKeyedObject
 * Signature: (JLjava/lang/String;Lorg/photon/ObjectDefinition;Lorg/photon/KeyDefinition;Lorg/photon/DataDefinition;)I
 */
JNIEXPORT jint JNICALL 
Java_org_photon_Folder_psoCreateMap( JNIEnv     * env,
                                     jobject      jobj,
                                     jlong        jhandle,
                                     jstring      jname, 
                                     jobject      jdefinition,
                                     jobject      jkeyDef )
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

   errcode = psoFolderCreateMap( (PSO_HANDLE) handle,
                                   name,
                                   strlen(name),
                                   pDefinition,
                                   NULL );

   (*env)->ReleaseStringUTFChars( env, jname, name );
   free( pDefinition );

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_Folder
 * Method:    psoCreateObject
 * Signature: (JLjava/lang/String;Lorg/photon/ObjectDefinition;Lorg/photon/DataDefinition;)I
 */
JNIEXPORT jint JNICALL 
Java_org_photon_Folder_psoCreateQueue( JNIEnv   * env,
                                       jobject    jobj,
                                       jlong      jhandle,
                                       jstring    jname, 
                                       jobject    jdefinition )
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

   errcode = psoFolderCreateQueue( (PSO_HANDLE) handle,
                                   name,
                                   strlen(name),
                                   pDefinition );

   (*env)->ReleaseStringUTFChars( env, jname, name );
   free( pDefinition );
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_Folder
 * Method:    psoDefinition
 * Signature: (JLjava/lang/String;Lorg/photon/ObjectDefinition;)I
 */
JNIEXPORT jint JNICALL
Java_org_photon_Folder_psoDefinition( JNIEnv * env,
                                      jobject  jobj,
                                      jlong    jhandle,
                                      jstring  jname,
                                      jobject  jdefinition )
{
   int errcode;
   size_t handle = (size_t) jhandle;
   const char * objectName;
   psoObjectDefinition * pDefinition;
   
   objectName = (*env)->GetStringUTFChars( env, jname, NULL );
   if ( objectName == NULL ) {
      return PSO_NOT_ENOUGH_HEAP_MEMORY; // out-of-memory exception by the JVM
   }

   errcode = psoaFolderGetDef( (PSO_HANDLE) handle,
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
 * Class:     org_photon_Folder
 * Method:    psoDestroyObject
 * Signature: (JLjava/lang/String;)I
 */
JNIEXPORT jint JNICALL 
Java_org_photon_Folder_psoDestroyObject( JNIEnv * env, 
                                         jobject  jobj, 
                                         jlong    jhandle, 
                                         jstring  jname )
{
   int errcode;
   size_t handle = (size_t) jhandle;
   const char * objectName;
   
   objectName = (*env)->GetStringUTFChars( env, jname, NULL );
   if ( objectName == NULL ) {
      return PSO_NOT_ENOUGH_HEAP_MEMORY; // out-of-memory exception by the JVM
   }
   
   errcode = psoFolderDestroyObject( (PSO_HANDLE) handle,
                                     objectName,
                                     strlen(objectName) );

   (*env)->ReleaseStringUTFChars( env, jname, objectName );

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_Folder
 * Method:    psoGetFirst
 * Signature: (JLorg/photon/FolderEntry;)I
 */
JNIEXPORT jint JNICALL 
Java_org_photon_Folder_psoGetFirst( JNIEnv * env, 
                                    jobject  jobj, 
                                    jlong    jhandle,
                                    jobject  jentry )
{
   int errcode;
   size_t handle = (size_t) jhandle;
   psoFolderEntry cEntry;
   jstring jname;
   
   errcode = psoFolderGetFirst( (PSO_HANDLE)handle, &cEntry );
   if ( errcode == PSO_OK ) {
      jname = getNotNullTerminatedString( env, 
         cEntry.name, PSO_MAX_NAME_LENGTH );
      if ( jname == NULL ) return PSO_NOT_ENOUGH_HEAP_MEMORY;

      (*env)->SetObjectField( env, jentry, g_idEntryName,   jname );
      (*env)->SetIntField( env, jentry, g_idEntryType,   cEntry.type );
      (*env)->SetIntField( env, jentry, g_idEntryStatus, cEntry.status );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_Folder
 * Method:    psoGetNext
 * Signature: (JLorg/photon/FolderEntry;)I
 */
JNIEXPORT jint JNICALL 
Java_org_photon_Folder_psoGetNext( JNIEnv * env, 
                                   jobject  jobj, 
                                   jlong    jhandle,
                                   jobject  jentry )
{
   int errcode;
   size_t handle = (size_t) jhandle;
   psoFolderEntry cEntry;
   jstring jname;
   
   errcode = psoFolderGetNext( (PSO_HANDLE)handle, &cEntry );
   if ( errcode == PSO_OK ) {
      jname = getNotNullTerminatedString( env, 
         cEntry.name, PSO_MAX_NAME_LENGTH );
      if ( jname == NULL ) return PSO_NOT_ENOUGH_HEAP_MEMORY;
      
      (*env)->SetObjectField( env, jentry, g_idEntryName,   jname );
      (*env)->SetIntField( env, jentry, g_idEntryType,   cEntry.type );
      (*env)->SetIntField( env, jentry, g_idEntryStatus, cEntry.status );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_Folder
 * Method:    psoKeyDefinition
 * Signature: (JLjava/lang/String;Lorg/photon/KeyDefinition;)I
 */
JNIEXPORT jint JNICALL
Java_org_photon_Folder_psoKeyDefinition( JNIEnv * env,
                                         jobject  jobj,
                                         jlong    jhandle,
                                         jstring  jname,
                                         jobject  jdefinition )
{
   int errcode;
   size_t handle = (size_t) jhandle;
   const char * objectName;
   psoKeyDefinition * pDefinition;
   
   objectName = (*env)->GetStringUTFChars( env, jname, NULL );
   if ( objectName == NULL ) {
      return PSO_NOT_ENOUGH_HEAP_MEMORY; // out-of-memory exception by the JVM
   }

   errcode = psoaFolderGetKeyDef( (PSO_HANDLE) handle,
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
 * Class:     org_photon_Folder
 * Method:    psoOpen
 * Signature: (Lorg/photon/Session;Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL 
Java_org_photon_Folder_psoOpen( JNIEnv * env, 
                                jobject  jobj, 
                                jobject  jsession, 
                                jstring  jstr )
{
   int errcode;
   PSO_HANDLE handle;
   size_t sessionHandle;
   const char *folderName;

   folderName = (*env)->GetStringUTFChars( env, jstr, NULL );
   if ( folderName == NULL ) {
      return PSO_NOT_ENOUGH_HEAP_MEMORY; // out-of-memory exception by the JVM
   }

   sessionHandle = (*env)->GetLongField( env, jsession, g_idSessionHandle );
   
   errcode = psoFolderOpen( (PSO_HANDLE) sessionHandle,
                            folderName,
                            strlen(folderName),
                            &handle );
   (*env)->ReleaseStringUTFChars( env, jstr, folderName );

   if ( errcode == PSO_OK ) {
      (*env)->SetLongField( env, jobj, g_idFolderHandle, (size_t) handle );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_Folder
 * Method:    psoStatus
 * Signature: (JLorg/photon/ObjectStatus;)I
 */
JNIEXPORT jint JNICALL 
Java_org_photon_Folder_psoStatus( JNIEnv * env, 
                                  jobject  jobj, 
                                  jlong    jhandle, 
                                  jobject  jstatus )
{
   int errcode;
   size_t handle = (size_t)jhandle;
   psoObjStatus status;

   errcode = psoFolderStatus( (PSO_HANDLE) handle, &status );

   if ( errcode == 0 ) {
      C2Java_ObjStatus( env, jstatus, &status );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

