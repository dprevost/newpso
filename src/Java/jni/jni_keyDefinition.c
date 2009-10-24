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
#include "org_photon_KeyDefinition.h"
#include "API/KeyDefinition.h"

jfieldID g_id_type;
jfieldID g_id_keyDef;
jfieldID g_id_keyDefStr;
jfieldID g_id_keyDefLength;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Class:     org_photon_KeyDefinition
 * Method:    initIDs
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_org_photon_KeyDefinition_initIDs( JNIEnv * env,
                                       jclass   classDefinition )
{
   g_id_type = (*env)->GetFieldID( env, 
      classDefinition, "type", "I" );
   if ( g_id_type == NULL ) return;
   g_id_keyDef = (*env)->GetFieldID( env, 
      classDefinition, "keyDef", "[B" );
   if ( g_id_keyDef == NULL ) return;
   g_id_keyDefStr = (*env)->GetFieldID( env, 
      classDefinition, "keyDefStr", "Ljava/lang/String;" );
   if ( g_id_keyDefStr == NULL ) return;
   g_id_keyDefLength = (*env)->GetFieldID( env, 
      classDefinition, "keyDefLength", "I" );
   if ( g_id_keyDefLength == NULL ) return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Internal to the library.
 */
psoKeyDefinition * Java2C_KeyDefinition( JNIEnv  * env,
                                         jobject   jdefinition )
{
   int length;
   psoKeyDefinition * pDefinition;
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
   length = (*env)->GetIntField( env, jdefinition, g_id_keyDefLength );
   if ( length  == 0 ) {
      pDefinition = (psoKeyDefinition *) malloc( sizeof(psoKeyDefinition) );
   }
   else if ( length > 0 ) {
      pDefinition = (psoKeyDefinition *)
         malloc( offsetof(struct psoKeyDefinition, definition) + length );
   }
   else {
      pDefinition = (psoKeyDefinition *)
         malloc( offsetof(psoKeyDefinition, definition) - length );
   }
   if ( pDefinition == NULL ) return NULL;
   
   pDefinition->type  = (*env)->GetIntField( env, jdefinition, g_id_type );

   pDefinition->definitionLength = 0;
   if ( length  > 0 ) {
      pDefinition->definitionLength = length;
      jdefArray = (*env)->GetObjectField( env, jdefinition, g_id_keyDef );
      if ( jdefArray == NULL ) return NULL;
      jdef = (*env)->GetByteArrayElements( env, jdefArray, NULL );
      if ( jdef == NULL ) return NULL;

      memcpy( pDefinition->definition, jdef, pDefinition->definitionLength );
      (*env)->ReleaseByteArrayElements( env, jdefArray, jdef, JNI_ABORT );
   }
   else if ( length < 0 ) {
      pDefinition->definitionLength = -length;
      jstr = (*env)->GetObjectField( env, jdefinition, g_id_keyDefStr );
      if ( jstr == NULL ) return NULL;
      str = (*env)->GetStringUTFChars( env, jstr, NULL );
      if ( str == NULL ) return NULL;
      
      memcpy( pDefinition->definition, str, pDefinition->definitionLength );
      (*env)->ReleaseStringUTFChars( env, jstr, str );
   }

   return pDefinition;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Internal to the library.
 */
int C2Java_KeyDefinition( JNIEnv           * env,
                          jobject            jdefinition,
                          psoKeyDefinition * pDefinition )
{
   int i;
   mbstate_t state;
   int num, is_string = 0;
   jbyteArray jba;
   jstring jstr;
   char * str;

   (*env)->SetIntField( env, jdefinition, g_id_type, pDefinition->type );

   if ( pDefinition->definitionLength == 0 ) {
       (*env)->SetIntField( env, jdefinition, g_id_keyDefLength, 0 );
   }
   else {
      memset( &state, 0, sizeof(state) );
      num = mbrtowc( (wchar_t *)NULL,
                       (const char *)pDefinition->definition,
                       pDefinition->definitionLength,
                       &state );
      if ( num >= 0 ) {
         // Probably a utf-8 string - no conversion error.
         (*env)->SetIntField( env,
            jdefinition, g_id_keyDefLength, -pDefinition->definitionLength );

         str = malloc( pDefinition->definitionLength + 1 );
         if ( str == NULL ) return PSO_NOT_ENOUGH_HEAP_MEMORY;
      
         memcpy( str, pDefinition->definition, pDefinition->definitionLength );
         str[pDefinition->definitionLength] = 0;
         jstr = (*env)->NewStringUTF( env, str );
         free( str );
         if ( jstr == NULL ) return PSO_NOT_ENOUGH_HEAP_MEMORY;

         (*env)->SetObjectField( env, jdefinition, g_id_keyDefStr, jstr );
      }
      else {
         (*env)->SetIntField( env,
            jdefinition, g_id_keyDefLength, pDefinition->definitionLength );

         jba = (*env)->NewByteArray( env, pDefinition->definitionLength );
         if ( jba == NULL ) {
            return PSO_NOT_ENOUGH_HEAP_MEMORY;
         }
         (*env)->SetByteArrayRegion( env, jba, 0, pDefinition->definitionLength,
            (jbyte *)pDefinition->definition );
         (*env)->SetObjectField( env, jdefinition, g_id_keyDef, jba );
      }
   }
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

