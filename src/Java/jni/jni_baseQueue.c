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
#include "org_photon_BaseQueue.h"
#include "API/Queue.h"

static jfieldID g_id_handle;
static jfieldID g_id_errcode;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_BaseQueue
 * Method:    initIDs
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_org_photon_BaseQueue_initIDs( JNIEnv * env, jclass queueClass )
{
   g_id_handle = (*env)->GetFieldID( env, queueClass, "handle", "J" );
   if ( g_id_handle == NULL ) return;
   g_id_errcode = (*env)->GetFieldID( env, queueClass, "myerrcode", "I" );
   if ( g_id_errcode == NULL ) return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_BaseQueue
 * Method:    psoClose
 * Signature: (J)V
 */
JNIEXPORT void JNICALL 
Java_org_photon_BaseQueue_psoClose( JNIEnv  * env,
                                    jobject   jobj,
                                    jlong     jhandle )
{
   int errcode;
   size_t handle = (size_t) jhandle;
   
   errcode = psoQueueClose( (PSO_HANDLE)handle );

   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
/*
 * Class:     org_photon_BaseQueue
 * Method:    psoDefinition
 * Signature: (JLorg/photon/ObjectDefinition;)I
 */
JNIEXPORT jint JNICALL
Java_org_photon_BaseQueue_psoDefinition( JNIEnv * env,
                                         jobject  jobj,
                                         jlong    jhandle,
                                         jobject  jdefinition )
{
   int errcode;
   size_t handle = (size_t) jhandle;
   psoObjectDefinition * pDefinition = NULL;

   errcode = psoaQueueGetDef( (PSO_HANDLE) handle, &pDefinition );
                            
   if ( errcode == 0 ) {
      errcode = C2Java_ObjectDefinition( env,
                                         jdefinition,
                                         pDefinition );     
   }

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_BaseQueue
 * Method:    psoGetFirst
 * Signature: (J[B)I
 */
JNIEXPORT jint JNICALL 
Java_org_photon_BaseQueue_psoGetFirst( JNIEnv   * env,
                                       jobject    jobj,
                                       jlong      jhandle,
                                       jbyteArray jbuffer )
{
   int errcode;
   size_t handle = (size_t) jhandle;
   unsigned char * data;
   unsigned int length;
   
   errcode = psoaQueueFirst( (psoaQueue *) handle, &data, &length );
   if ( errcode == 0 ) {
      jbuffer = (*env)->NewByteArray( env, length );
      (*env)->SetByteArrayRegion( env, jbuffer, 0, length, (jbyte*)data );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_BaseQueue
 * Method:    psoGetNext
 * Signature: (J[B)I
 */
JNIEXPORT jint JNICALL
Java_org_photon_BaseQueue_psoGetNext( JNIEnv   * env,
                                      jobject    jobj,
                                      jlong      jhandle,
                                      jbyteArray jbuffer )
{
   int errcode;
   size_t handle = (size_t) jhandle;
   unsigned char * data;
   unsigned int length;
   
   errcode = psoaQueueNext( (psoaQueue *) handle, &data, &length );
   if ( errcode == 0 ) {
      jbuffer = (*env)->NewByteArray( env, length );
      (*env)->SetByteArrayRegion( env, jbuffer, 0, length, (jbyte*)data );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_BaseQueue
 * Method:    psoGetStatus
 * Signature: (JLorg/photon/ObjectStatus;)I
 */
JNIEXPORT jint JNICALL 
Java_org_photon_BaseQueue_psoGetStatus( JNIEnv  * env,
                                        jobject   jobj,
                                        jlong     jhandle,
                                        jobject   jstatus )
{
   int errcode;
   size_t handle = (size_t)jhandle;
   psoObjStatus status;

   errcode = psoQueueStatus( (PSO_HANDLE) handle, &status );

   if ( errcode == 0 ) {
      C2Java_ObjStatus( env, jstatus, &status );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_BaseQueue
 * Method:    psoOpen
 * Signature: (Lorg/photon/Session;Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL
Java_org_photon_BaseQueue_psoOpen( JNIEnv  * env,
                                   jobject   jobj,
                                   jobject   jsession,
                                   jstring   jname )
{
   int errcode;
   PSO_HANDLE handle;
   size_t sessionHandle;
   const char * queueName;

   queueName = (*env)->GetStringUTFChars( env, jname, NULL );
   if ( queueName == NULL ) {
      return PSO_NOT_ENOUGH_HEAP_MEMORY; // out-of-memory exception by the JVM
   }
   sessionHandle = (*env)->GetLongField( env, jsession, g_idSessionHandle );

   errcode = psoQueueOpen( (PSO_HANDLE) sessionHandle,
                           queueName,
                           strlen(queueName),
                           &handle );
   (*env)->ReleaseStringUTFChars( env, jname, queueName );

   if ( errcode == PSO_OK ) {
      (*env)->SetLongField( env, jobj, g_id_handle, (size_t) handle );
   }

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_BaseQueue
 * Method:    psoPop
 * Signature: (J[B)I
 */
JNIEXPORT jbyteArray JNICALL
Java_org_photon_BaseQueue_psoPop( JNIEnv   * env,
                                  jobject    jobj,
                                  jlong      jhandle )
{
   int errcode;
   size_t handle = (size_t) jhandle;
   unsigned char * data;
   unsigned int length;
   jbyteArray jbuffer = NULL;
   
   errcode = psoaQueueRemove( (psoaQueue *) handle, &data, &length );
   if ( errcode == 0 ) {
      jbuffer = (*env)->NewByteArray( env, length );
      (*env)->SetByteArrayRegion( env, jbuffer, 0, length, (jbyte*)data );
   }
   else {
      (*env)->SetIntField( env, jobj, g_id_errcode, errcode );
   }
   
   return jbuffer;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_BaseQueue
 * Method:    psoPush
 * Signature: (J[B)I
 */
JNIEXPORT jint JNICALL
Java_org_photon_BaseQueue_psoPush( JNIEnv   * env,
                                   jobject    jobj,
                                   jlong      jhandle,
                                   jbyteArray jbuffer )
{
   int errcode;
   size_t handle = (size_t) jhandle;
   void * buffer;
   jsize length;
   
   length = (*env)->GetArrayLength( env, jbuffer );
   buffer = (void *)(*env)->GetByteArrayElements( env, jbuffer, NULL );
   if ( buffer == NULL ) {
      return PSO_NOT_ENOUGH_HEAP_MEMORY; // out-of-memory exception by the JVM
   }

   errcode = psoQueuePush( (PSO_HANDLE)handle,
                           buffer,
                           length );
   (*env)->ReleaseByteArrayElements( env, jbuffer, buffer, JNI_ABORT );

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_BaseQueue
 * Method:    psoPushNow
 * Signature: (J[B)I
 */
JNIEXPORT jint JNICALL
va_org_photon_BaseQueue_psoPushNow( JNIEnv * env,
                                    jobject  jobj,
                                    jlong    jhandle,
                                    jbyteArray jbuffer )
{
   int errcode;
   size_t handle = (size_t) jhandle;
   void * buffer;
   jsize length;
   
   length = (*env)->GetArrayLength( env, jbuffer );
   buffer = (void *)(*env)->GetByteArrayElements( env, jbuffer, NULL );
   if ( buffer == NULL ) {
      return PSO_NOT_ENOUGH_HEAP_MEMORY; // out-of-memory exception by the JVM
   }

   errcode = psoQueuePushNow( (PSO_HANDLE)handle,
                              buffer,
                              length );
   (*env)->ReleaseByteArrayElements( env, jbuffer, buffer, JNI_ABORT );

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

