/*
 * Copyright (C) 2007-2009 Daniel Prevost <dprevost@photonsoftware.org>
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
#include "API/HashMap.h"
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
   PSO_HANDLE sessionHandle, objHandle;
   int errcode;
   struct dummy * data1 = NULL;
   char key[] = "My Key";
   size_t lenData;
   uint32_t lengthDef;
   psoKeyDefinition keyDef = { PSO_DEF_USER_DEFINED, 0, '\0' };
   uint32_t retLengthDef = 0;
   
   psoObjectDefinition * hashMapDef;

   psoFieldDefinition fields[5] = {
      { "field1", PSO_TINYINT,  { 0  } },
      { "field2", PSO_INTEGER,  { 0  } },
      { "field3", PSO_CHAR,     { 30 } },
      { "field4", PSO_SMALLINT, { 0  } },
      { "field5", PSO_LONGVARBINARY, { 0 } }
   };
   
   lengthDef = offsetof(psoObjectDefinition, dataDef) + 
      5*sizeof(psoFieldDefinition);

   hashMapDef = (psoObjectDefinition*) malloc( lengthDef );
   assert_false( hashMapDef == NULL );
   
   memset( hashMapDef, 0, lengthDef );
   hashMapDef->type = PSO_HASH_MAP;
   hashMapDef->minNumBlocks = 1;
   hashMapDef->dataDefType = PSO_DEF_PHOTON_ODBC_SIMPLE;
   hashMapDef->dataDefLength = 5*sizeof(psoFieldDefinition);
   memcpy( hashMapDef->dataDef, fields, 5*sizeof(psoFieldDefinition) );
   
   lenData = offsetof(struct dummy, bin) + 10;
   data1 = (struct dummy *)malloc( lenData );
   
   errcode = psoInit( "10701", NULL );
   assert_true( errcode == PSO_OK );
   
   errcode = psoInitSession( &sessionHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateFolder( sessionHandle,
                              "/api_hashmap_definition",
                              strlen("/api_hashmap_definition") );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateMap( sessionHandle,
                           "/api_hashmap_definition/test",
                           strlen("/api_hashmap_definition/test"),
                           hashMapDef,
                           &keyDef );
   assert_true( errcode == PSO_OK );

   errcode = psoHashMapOpen( sessionHandle,
                             "/api_hashmap_definition/test",
                             strlen("/api_hashmap_definition/test"),
                             &objHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoHashMapInsert( objHandle, key, strlen(key), data1, lenData );
   assert_true( errcode == PSO_OK );

   /* Invalid arguments to tested function. */

   errcode = psoHashMapDefLength( NULL, &retLengthDef );
   assert_true( errcode == PSO_NULL_HANDLE );

   errcode = psoHashMapDefLength( objHandle, NULL );
   assert_true( errcode == PSO_NULL_POINTER );

   /* End of invalid args. This call should succeed. */
   errcode = psoHashMapDefLength( objHandle, &retLengthDef );
   assert_true( errcode == PSO_OK );

   assert_true( retLengthDef == lengthDef );

   /* Close the session and try to act on the object */

   errcode = psoExitSession( sessionHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoHashMapDefLength( objHandle, &retLengthDef );
   assert_true( errcode == PSO_SESSION_IS_TERMINATED );

   psoExit();
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

