/*
 * Copyright (C) 2007-2010 Daniel Prevost <dprevost@photonsoftware.org>
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
#include "API/Tests/quasar-run.h"

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
   PSO_HANDLE sessionHandle;
   PSO_HANDLE shmemHandle;
   int errcode;
   psoObjectDefinition * def = NULL;
   psoObjectDefinition returnedDef;
   psoKeyDefinition keyDef = { PSO_DEF_USER_DEFINED, 0, '\0' };
   uint32_t lengthDef;
   uint32_t retLengthDef;

   psoFieldDefinition fields[5] = {
      { "field1", PSO_TINYINT,  { 0  } },
      { "field2", PSO_INTEGER,  { 0  } },
      { "field3", PSO_CHAR,     { 30 } },
      { "field4", PSO_SMALLINT, { 0  } },
      { "field5", PSO_LONGVARBINARY, { 0 } }
   };

   lengthDef = offsetof(psoObjectDefinition, dataDef) + 
      5*sizeof(psoFieldDefinition);

   def = (psoObjectDefinition*) malloc( lengthDef );
   assert_false( def == NULL );

   errcode = psoInit( "10701", NULL );
   assert_true( errcode == PSO_OK );
   
   memset( def, 0, lengthDef );
   def->type = PSO_HASH_MAP;
   def->minNumBlocks = 1;
   def->dataDefType = PSO_DEF_PHOTON_ODBC_SIMPLE;
   def->dataDefLength = 5*sizeof(psoFieldDefinition);
   memcpy( def->dataDef, fields, 5*sizeof(psoFieldDefinition) );
   
   errcode = psoInitSession( shmemHandle, &sessionHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateMap( sessionHandle,
                           "/api_session_definition",
                           strlen("/api_session_definition"),
                           def,
                           &keyDef );
   assert_true( errcode == PSO_OK );

   /* Invalid arguments to tested function. */

   errcode = psoGetDefLength( NULL,
                              "/api_session_definition",
                              strlen("/api_session_definition"),
                              &retLengthDef );
   assert_true( errcode == PSO_NULL_HANDLE );

   errcode = psoGetDefLength( sessionHandle,
                              NULL,
                              strlen("/api_session_definition"),
                              &retLengthDef );
   assert_true( errcode == PSO_INVALID_OBJECT_NAME );

   errcode = psoGetDefLength( sessionHandle,
                              "/api_session_definition",
                              0,
                              &retLengthDef );
   assert_true( errcode == PSO_INVALID_LENGTH );

   errcode = psoGetDefLength( sessionHandle,
                              "/api_session_definition",
                              strlen("/api_session_definition"),
                              NULL );
   assert_true( errcode == PSO_NULL_POINTER );

   /* End of invalid args. This call should succeed. */
   errcode = psoGetDefLength( sessionHandle,
                              "/api_session_definition",
                              strlen("/api_session_definition"),
                              &retLengthDef );
   assert_true( errcode == PSO_OK );

   assert_true( retLengthDef == lengthDef );

   /* Close the process and try to act on the session */

   psoExit();
   
   errcode = psoGetDefLength( sessionHandle,
                              "/api_session_definition",
                              strlen("/api_session_definition"),
                              &retLengthDef );
   assert_true( errcode == PSO_SESSION_IS_TERMINATED );
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

