/*
 * Copyright (C) 2008-2009 Daniel Prevost <dprevost@photonsoftware.org>
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

#include "Common/Common.h"
#include <photon/photon.h>
#include "API/FastMap.h"
#include "API/Tests/quasar-run.h"

struct dummy {
   char c;
   uint32_t u32;
   char str[30];
   uint16_t u16;
   unsigned char bin[1];
};

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void setup_test()
{
   assert( startQuasar() );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void teardown_test()
{
   assert( stopQuasar() );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_pass( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   PSO_HANDLE sessionHandle, objHandle;
   int errcode;
   struct dummy * data1 = NULL;
   char key[] = "My Key";
   size_t lenData;

   psoObjectDefinition hashMapDef = { PSO_FAST_MAP, 0, 0 };
   psoKeyFieldDefinition keyDef = { "MyKey", PSO_KEY_LONGVARCHAR, 0 };

   psoFieldDefinition fields[5] = {
      { "field1", PSO_TINYINT,   {0} },
      { "field2", PSO_INTEGER,   {0} },
      { "field3", PSO_CHAR,      {30} },
      { "field4", PSO_SMALLINT,  {0} },
      { "field5", PSO_LONGVARBINARY, {0} }
   };
   PSO_HANDLE keyDefHandle, dataDefHandle;
   PSO_HANDLE retKeyDefHandle = NULL, retDataDefHandle = NULL;

   psoFieldDefinition retFields[5];
   psoObjectDefinition retDef;
   psoKeyFieldDefinition retKeyDef;
   
   memset( &retDef,    0, sizeof(psoObjectDefinition) );
   memset( &retKeyDef, 0, sizeof(psoKeyFieldDefinition) );
   memset( &retFields, 0, 5*sizeof(psoFieldDefinition) );

   lenData = offsetof(struct dummy, bin) + 10;
   data1 = (struct dummy *)malloc( lenData );
   
   errcode = psoInit( "10701", "Definition" );
   assert_true( errcode == PSO_OK );
   
   errcode = psoInitSession( &sessionHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateFolder( sessionHandle,
                              "/api_fast_map_definition",
                              strlen("/api_fast_map_definition") );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateMap( sessionHandle,
                           "/api_fast_map_definition/test",
                           strlen("/api_fast_map_definition/test"),
                           &hashMapDef,
                           pKeyDefinition );
   assert_true( errcode == PSO_OK );

   errcode = psoFastMapEdit( sessionHandle,
                             "/api_fast_map_definition/test",
                             strlen("/api_fast_map_definition/test"),
                             &objHandle );
   assert_true( errcode == PSO_OK );

   /* Test of the data definition with inserts . */
   errcode = psoFastMapInsert( objHandle, key, strlen(key), data1, lenData );
   assert_true( errcode == PSO_OK );
fprintf(stderr, "ok 1 \n" );
   /* Invalid arguments to tested function. */

   errcode = psoFastMapDefinition( NULL, 
                                   &retKeyDefHandle,
                                   &retDataDefHandle );
   assert_true( errcode == PSO_NULL_HANDLE );
   
   errcode = psoFastMapDefinition( objHandle,
                                   NULL,
                                   &retDataDefHandle );
   assert_true( errcode == PSO_NULL_POINTER );

   errcode = psoFastMapDefinition( objHandle,
                                   &retKeyDefHandle,
                                   NULL );
   assert_true( errcode == PSO_NULL_POINTER );

fprintf(stderr, "ok 2 \n" );
   /* End of invalid args. This call should succeed. */
   errcode = psoFastMapDefinition( objHandle, 
                                   &retKeyDefHandle,
                                   &retDataDefHandle );
   assert_true( errcode == PSO_OK );

//   if ( memcmp( &hashMapDef, &retDef, sizeof(psoObjectDefinition) ) != 0 ) {
//      ERROR_EXIT( expectedToPass, NULL, ; );
//   }
//   if ( memcmp( fields, retFields, 5*sizeof(psoFieldDefinition) ) != 0 ) {
//      ERROR_EXIT( expectedToPass, NULL, ; );
//   }

//   if ( memcmp( &hashMapDef, &retDef, sizeof(psoObjectDefinition) ) != 0 ) {
//      ERROR_EXIT( expectedToPass, NULL, ; );
//   }
//   if ( memcmp( &keyDef, &retKeyDef, sizeof(psoKeyFieldDefinition) ) != 0 ) {
//      ERROR_EXIT( expectedToPass, NULL, ; );
//   }

   /* Close the session and try to act on the object */

fprintf(stderr, "ok 3 \n" );
   errcode = psoExitSession( sessionHandle );
   assert_true( errcode == PSO_OK );
fprintf(stderr, "ok 5 \n" );

   errcode = psoFastMapDefinition( objHandle,
                                   &retKeyDefHandle,
                                   &retDataDefHandle );
   assert_true( errcode == PSO_SESSION_IS_TERMINATED );

fprintf(stderr, "ok 4 \n" );
   psoExit();
   
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   int rc = 0;
#if defined(PSO_UNIT_TESTS)
   const UnitTest tests[] = {
      unit_test_setup_teardown( test_pass, setup_test, teardown_test ),
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */


