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
#include "org_photon_ObjectStatus.h"

jfieldID g_id_Type;
jfieldID g_id_Status;
jfieldID g_id_NumBlocks;
jfieldID g_id_NumBlockGroup;
jfieldID g_id_NumDataItem;
jfieldID g_id_FreeBytes;
jfieldID g_id_MaxDataLength;
jfieldID g_id_MaxKeyLength;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_ObjectStatus
 * Method:    initIDs
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_org_photon_ObjectStatus_initIDs( JNIEnv * env, jclass statusClass )
{
   g_id_Type = (*env)->GetFieldID( env, statusClass, "type", "I" );
   if ( g_id_Type == NULL ) return;
   g_id_Status = (*env)->GetFieldID( env, statusClass, "status", "I" );
   if ( g_id_Status == NULL ) return;
   g_id_NumBlocks = (*env)->GetFieldID( env, statusClass, "numBlocks", "J" );
   if ( g_id_NumBlocks == NULL ) return;
   g_id_NumBlockGroup = (*env)->GetFieldID( env, statusClass, "numBlockGroup", "J" );
   if ( g_id_NumBlockGroup == NULL ) return;
   g_id_NumDataItem = (*env)->GetFieldID( env, statusClass, "numDataItem", "J" );
   if ( g_id_NumDataItem == NULL ) return;
   g_id_FreeBytes = (*env)->GetFieldID( env, statusClass, "freeBytes", "J" );
   if ( g_id_FreeBytes == NULL ) return;
   g_id_MaxDataLength = (*env)->GetFieldID( env, statusClass, "maxDataLength", "I" );
   if ( g_id_MaxDataLength == NULL ) return;
   g_id_MaxKeyLength = (*env)->GetFieldID( env, statusClass, "maxKeyLength", "I" );
   if ( g_id_MaxKeyLength == NULL ) return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
/*
 * Internal to the jni library.
 */
void C2Java_ObjStatus( JNIEnv       * env,
                       jobject        jstatus,
                       psoObjStatus * pStatus )
{
   (*env)->SetIntField(  env, jstatus, g_id_Type,          pStatus->type );
   (*env)->SetIntField(  env, jstatus, g_id_Status,        pStatus->status );
   (*env)->SetLongField( env, jstatus, g_id_NumBlocks,     pStatus->numBlocks );
   (*env)->SetLongField( env, jstatus, g_id_NumBlockGroup, pStatus->numBlockGroup );
   (*env)->SetLongField( env, jstatus, g_id_NumDataItem,   pStatus->numDataItem );
   (*env)->SetLongField( env, jstatus, g_id_FreeBytes,     pStatus->freeBytes );
   (*env)->SetIntField(  env, jstatus, g_id_MaxDataLength, pStatus->maxDataLength );
   (*env)->SetIntField(  env, jstatus, g_id_MaxKeyLength,  pStatus->maxKeyLength );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

