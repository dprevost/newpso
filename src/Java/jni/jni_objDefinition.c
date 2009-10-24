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
#include <stddef.h>
#include <ctype.h> 
#include <wchar.h>

#include "jni_photon.h"
#include "org_photon_ObjectDefinition.h"

jfieldID g_id_type;
jfieldID g_id_minNumOfDataRecords;
jfieldID g_id_minNumBlocks;
jfieldID g_id_dataDefType;
jfieldID g_id_dataDef;
jfieldID g_id_dataDefStr;
jfieldID g_id_dataDefLength;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_ObjectDefinition
 * Method:    initIDs
 * Signature: ()V
 */
JNIEXPORT void JNICALL 
Java_org_photon_ObjectDefinition_initIDs( JNIEnv * env, jclass defClass )
{
   g_id_type = (*env)->GetFieldID( 
      env, defClass, "type", "I" );
   if ( g_id_type == NULL ) return;
   g_id_minNumOfDataRecords = (*env)->GetFieldID(
      env, defClass, "minNumOfDataRecords", "J" );
   if ( g_id_minNumOfDataRecords == NULL ) return;
   g_id_minNumBlocks = (*env)->GetFieldID( 
      env, defClass, "minNumBlocks", "J" );
   if ( g_id_minNumBlocks == NULL ) return;

   g_id_dataDefType = (*env)->GetFieldID(
      env, defClass, "dataDefType", "I" );
   if ( g_id_dataDefType == NULL ) return;

   g_id_dataDef = (*env)->GetFieldID(
      env, defClass, "dataDef", "[B" );
   if ( g_id_dataDef == NULL ) return;
   g_id_dataDefStr = (*env)->GetFieldID(
      env, defClass, "dataDefStr", "Ljava/lang/String;" );
   if ( g_id_dataDefStr == NULL ) return;
   g_id_dataDefLength = (*env)->GetFieldID(
      env, defClass, "dataDefLength", "I" );
   if ( g_id_dataDefLength == NULL ) return;

fprintf( stderr, "id ok %p \n", g_id_dataDefLength );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Internal to the jni library.
 */
psoObjectDefinition * Java2C_ObjectDefinition( JNIEnv  * env,
                                               jobject   jdefinition )
{
   int length;
   psoObjectDefinition * pDefinition;
   const char * str;
   jstring jstr;
   jbyteArray jdefArray;
   jbyte * jdef;
fprintf( stderr, "ok 1\n" );
   /*
    * A bit of a hack. 
    *
    * dataDef can be either an array of bytes or a string. Or nothing.
    *
    * If zero, no dataDef. If < 0, a string. A byte[] otherwise.
    *
    */
   length = (*env)->GetIntField( env, jdefinition, g_id_dataDefLength );
fprintf( stderr, "ok 2 %d %p\n", length, g_id_dataDefLength );
   if ( length == 0 ) {
      pDefinition = (psoObjectDefinition *)
         malloc( sizeof(psoObjectDefinition) );
   }
   else if ( length > 0 ) {
      pDefinition = (psoObjectDefinition *)
         malloc( offsetof(struct psoObjectDefinition, dataDef) + length );
   }
   else {
      pDefinition = (psoObjectDefinition *)
         malloc( offsetof(psoObjectDefinition, dataDef) - length );
   }
fprintf( stderr, "ok 3\n" );
   if ( pDefinition == NULL ) return NULL;
fprintf( stderr, "ok 4\n" );
   
   pDefinition->type  = (*env)->GetIntField( env, jdefinition, g_id_type );
fprintf( stderr, "ok 5\n" );
   pDefinition->minNumOfDataRecords = (size_t) (*env)->GetLongField( env,
      jdefinition, g_id_minNumOfDataRecords );
fprintf( stderr, "ok 6\n" );
   pDefinition->minNumBlocks = (size_t) (*env)->GetLongField( env,
      jdefinition, g_id_minNumBlocks );
fprintf( stderr, "ok 7\n" );
   pDefinition->dataDefType = (*env)->GetIntField( env,
      jdefinition, g_id_dataDefType );
fprintf( stderr, "ok 8\n" );

   pDefinition->dataDefLength = 0;
fprintf( stderr, "ok 8\n" );
   if ( length  > 0 ) {
fprintf( stderr, "ok 10\n" );
      pDefinition->dataDefLength = length;
      jdefArray = (*env)->GetObjectField( env, jdefinition, g_id_dataDef );
      if ( jdefArray == NULL ) return NULL;
      jdef = (*env)->GetByteArrayElements( env, jdefArray, NULL );
      if ( jdef == NULL ) return NULL;

      memcpy( pDefinition->dataDef, jdef, pDefinition->dataDefLength );
      (*env)->ReleaseByteArrayElements( env, jdefArray, jdef, JNI_ABORT );
   }
   else if ( length < 0 ) {
fprintf( stderr, "ok 11\n" );
      pDefinition->dataDefLength = -length;
      jstr = (*env)->GetObjectField( env, jdefinition, g_id_dataDefStr );
      if ( jstr == NULL ) return NULL;
      str = (*env)->GetStringUTFChars( env, jstr, NULL );
      if ( str == NULL ) return NULL;
      
      memcpy( pDefinition->dataDef, str, pDefinition->dataDefLength );
      (*env)->ReleaseStringUTFChars( env, jstr, str );
   }
fprintf( stderr, "ok 12\n" );

   return pDefinition;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Internal to the jni library.
 */
int C2Java_ObjectDefinition( JNIEnv              * env,
                             jobject               jdefinition,
                             psoObjectDefinition * pDefinition )
{
   int i;
   mbstate_t state;
   int num, is_string = 0;
   jbyteArray jba;
   jstring jstr;
   char * str;

   (*env)->SetIntField( env,
      jdefinition, g_id_type, pDefinition->type );
   (*env)->SetLongField( env,
      jdefinition, g_id_minNumOfDataRecords, pDefinition->minNumOfDataRecords );
   (*env)->SetLongField( env,
      jdefinition, g_id_minNumBlocks, pDefinition->minNumBlocks );
   (*env)->SetIntField( env,
      jdefinition, g_id_dataDefType, pDefinition->dataDefType );

   if ( pDefinition->dataDefLength == 0 ) {
       (*env)->SetIntField( env, jdefinition, g_id_dataDefLength, 0 );
   }
   else {
      memset( &state, 0, sizeof(state) );
      num = mbrtowc( (wchar_t *)NULL,
                       (const char *)pDefinition->dataDef,
                       pDefinition->dataDefLength,
                       &state );
      if ( num >= 0 ) {
         // Probably a utf-8 string - no conversion error.
         (*env)->SetIntField( env,
            jdefinition, g_id_dataDefLength, -pDefinition->dataDefLength );

         str = malloc( pDefinition->dataDefLength + 1 );
         if ( str == NULL ) return PSO_NOT_ENOUGH_HEAP_MEMORY;
      
         memcpy( str, pDefinition->dataDef, pDefinition->dataDefLength );
         str[pDefinition->dataDefLength] = 0;
         jstr = (*env)->NewStringUTF( env, str );
         free( str );
         if ( jstr == NULL ) return PSO_NOT_ENOUGH_HEAP_MEMORY;

         (*env)->SetObjectField( env, jdefinition, g_id_dataDefStr, jstr );
      }
      else {
         (*env)->SetIntField( env,
            jdefinition, g_id_dataDefLength, pDefinition->dataDefLength );

         jba = (*env)->NewByteArray( env, pDefinition->dataDefLength );
         if ( jba == NULL ) {
            return PSO_NOT_ENOUGH_HEAP_MEMORY;
         }
         (*env)->SetByteArrayRegion( env, jba, 0, pDefinition->dataDefLength,
            (jbyte *)pDefinition->dataDef );
         (*env)->SetObjectField( env, jdefinition, g_id_dataDef, jba );
      }
   }
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

