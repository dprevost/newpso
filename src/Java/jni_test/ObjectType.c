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

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_ObjectTypeTest
 * Method:    getFolder
 * Signature: ()I
 */
JNIEXPORT int JNICALL 
Java_org_photon_ObjectTypeTest_getFolder( JNIEnv * env, jobject jobj )
{
   return PSO_FOLDER;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_ObjectTypeTest
 * Method:    getHashMap
 * Signature: ()I
 */
JNIEXPORT int JNICALL 
Java_org_photon_ObjectTypeTest_getHashMap( JNIEnv * env, jobject jobj )
{
   return PSO_HASH_MAP;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_ObjectTypeTest
 * Method:    getFastMap
 * Signature: ()I
 */
JNIEXPORT int JNICALL 
Java_org_photon_ObjectTypeTest_getFastMap( JNIEnv * env, jobject jobj )
{
   return PSO_FAST_MAP;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_ObjectTypeTest
 * Method:    getQueue
 * Signature: ()I
 */
JNIEXPORT int JNICALL 
Java_org_photon_ObjectTypeTest_getQueue( JNIEnv * env, jobject jobj )
{
   return PSO_QUEUE;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_ObjectTypeTest
 * Method:    getLifo
 * Signature: ()I
 */
JNIEXPORT int JNICALL 
Java_org_photon_ObjectTypeTest_getLifo( JNIEnv * env, jobject jobj )
{
   return PSO_LIFO;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
