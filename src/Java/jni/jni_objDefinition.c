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
   g_id_type = (*env)->GetFieldID( env, defClass, "type", "I" );
   if ( g_id_type == NULL ) return;
   g_id_minNumOfDataRecords = (*env)->GetFieldID( env, defClass, "minNumOfDataRecords", "J" );
   if ( g_id_minNumOfDataRecords == NULL ) return;
   g_id_minNumBlocks = (*env)->GetFieldID( env, defClass, "minNumBlocks", "J" );
   if ( g_id_minNumBlocks == NULL ) return;

   g_id_dataDefType = (*env)->GetFieldID( env, defClass, "dataDefType", "I" );
   if ( g_id_dataDefType = NULL ) return;

   g_id_dataDef = (*env)->GetFieldID( env, defClass, "dataDef", "[B" );
   if ( g_id_dataDef = NULL ) return;
   g_id_dataDefStr = (*env)->GetFieldID( env, defClass, "dataDefStr", "Ljava/lang/String;" );
   if ( g_id_dataDefStr = NULL ) return;
   g_id_dataDefLength = (*env)->GetFieldID( env, defClass, "dataDefLength", "I" );
   if ( g_id_dataDefLength = NULL ) return;

}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Internal to the library.
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

   /*
    * A bit of a hack. 
    *
    * dataDef can be either an array of bytes or a string. Or nothing.
    *
    * If zero, no dataDef. If < 0, a string. A byte[] otherwise.
    *
    */
   length = (*env)->GetIntField( env, jdefinition, g_id_dataDefLength );
   if ( length  == 0 ) {
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
   if ( pDefinition == NULL ) return NULL;
   
   pDefinition->type  = (*env)->GetIntField( env, jdefinition, g_id_type );
   pDefinition->minNumOfDataRecords = (size_t) (*env)->GetLongField( env,
      jdefinition, g_id_minNumOfDataRecords );
   pDefinition->minNumBlocks = (size_t) (*env)->GetLongField( env,
      jdefinition, g_id_minNumBlocks );
   pDefinition->dataDefType = (*env)->GetIntField( env,
      jdefinition, g_id_dataDefType );

   pDefinition->dataDefLength = 0;
   if ( length  > 0 ) {
      pDefinition->dataDefLength = length;
      jdefArray = (*env)->GetObjectField( env, jdefinition, g_id_dataDef );
      if ( jdefArray == NULL ) return NULL;
      jdef = (*env)->GetByteArrayElements( env, jdefArray, NULL );
      if ( jdef == NULL ) return NULL;

      memcpy( pDefinition->dataDef, jdef, pDefinition->dataDefLength );
      (*env)->ReleaseByteArrayElements( env, jdefArray, jdef, JNI_ABORT );
   }
   else if ( length < 0 ) {
      pDefinition->dataDefLength = -length;
      jstr = (*env)->GetObjectField( env, jdefinition, g_id_dataDefStr );
      if ( jstr == NULL ) return NULL;
      str = (*env)->GetStringUTFChars( env, jstr, NULL );
      if ( str == NULL ) return NULL;
      
      memcpy( pDefinition->dataDef, str, pDefinition->dataDefLength );
   (*env)->ReleaseStringUTFChars( env, jstr, str );
   }

   return pDefinition;
#if 0

   /** Type of the object */
   int type;
   
   /**
    * Optimization feature - not implemented yet.
    * <p>
    * The expected minimum number of data records. This is used internally
    * to avoid shrinking the internal "holder" of the data beyond what is
    * needed to hold this minimum number of data records.
    */
   long minNumOfDataRecords = 0;
   
   /**
    * Optimization feature - not implemented yet
    * <p>
    * The expected minimum number of blocks. This is used internally
    * to avoid shrinking the shared-memory object beyond a certain predefined
    * minimum size. 
    * <p>
    * Potential issue: the amount of overhead used by Photon will vary;
    * some potential factors includes the type of object, the number of 
    * data records, the length of the data records (and keys, if used).
    * <p>
    * You might want to retrieve the status of the object and evaluate
    * the minimum number of blocks needed from it..
    */
   long minNumBlocks = 1;

   /**
    * Type of definition of the data fields.
    */
   int dataDefType = DefinitionType.USER_DEFINED.getType();
   
   /**
    * Definition of the data fields.
    * <p>
    * This definition can be used as a comment to describe the content
    * of the object. It can also be used to pack and unpack the items
    * (serialized objects) contain in the object container.
    * <p>
    * It can be NULL if not needed.
    */
   byte [] dataDef;

   /**
    * Definition of the data fields as a string
    * <p>
    * This definition can be used as a comment to describe the content
    * of the object. It can also be used to pack and unpack the items
    * (serialized objects) contain in the object container.
    * <p>
    * It can be NULL if not needed.
    */
   String dataDefStr;
   
   /*
    * A bit of a hack. 
    *
    * The dataDef can be either an array of bytes or a string. Or nothing.
    *
    * If zero, no dataDef. If < 0, a string. A byte[] otherwise.
    *
    */
   int dataDefLength = 0; 
#endif

}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Internal to the library.
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

