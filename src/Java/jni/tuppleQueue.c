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
#include "org_photon_TuppleQueue.h"
#include "API/Queue.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_TuppleQueue
 * Method:    findConstructor
 * Signature: ([Ljava/lang/reflect/Constructor;)Ljava/lang/reflect/Constructor;
 */
JNIEXPORT jobject JNICALL
Java_org_photon_TuppleQueue_findConstructor( JNIEnv     * env,
                                             jobject      jobj,
                                             jobjectArray jctrs )
{
   return NULL;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_TuppleQueue
 * Method:    getFirstTupple
 * Signature: (J)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL
Java_org_photon_TuppleQueue_getFirstTupple( JNIEnv * env,
                                            jobject  jobj,
                                            jlong    jhandle )
{
   return NULL;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_TuppleQueue
 * Method:    getNextTupple
 * Signature: (J)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL
Java_org_photon_TuppleQueue_getNextTupple( JNIEnv * env,
                                           jobject  jobj,
                                           jlong    jhandle )
{
   return NULL;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_TuppleQueue
 * Method:    popTupple
 * Signature: (J)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL
Java_org_photon_TuppleQueue_popTupple( JNIEnv * env,
                                       jobject  jobj,
                                       jlong    jhandle )
{
   return NULL;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_TuppleQueue
 * Method:    pushTupple
 * Signature: (J[Ljava/lang/Object;)I
 */
JNIEXPORT jint JNICALL
Java_org_photon_TuppleQueue_pushTupple( JNIEnv     * env,
                                        jobject      jobj,
                                        jlong        jhandle,
                                        jobjectArray jarray )
{
   return -1;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_TuppleQueue
 * Method:    pushNowTupple
 * Signature: (J[Ljava/lang/Object;)I
 */
JNIEXPORT jint JNICALL
Java_org_photon_TuppleQueue_pushNowTupple( JNIEnv     * env,
                                           jobject      jobj,
                                           jlong        jhandle,
                                           jobjectArray jarray)
{
   return -1;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_TuppleQueue
 * Method:    validateConstructor
 * Signature: (Ljava/lang/reflect/Constructor;)Ljava/lang/reflect/Constructor;
 */
JNIEXPORT jobject JNICALL
Java_org_photon_TuppleQueue_validateConstructor( JNIEnv * env,
                                                 jobject  jobj,
                                                 jobject  jctr )
{
   return NULL;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

