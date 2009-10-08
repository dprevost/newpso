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
#include "API/Lifo.h"
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
   PSO_HANDLE sessionHandle, objHandle;
   PSO_HANDLE objHandle2, sessionHandle2;
   int errcode;
   const char * data1 = "My Data1";
   const char * data2 = "My Data 2";
   char buffer[200];
   uint32_t length;
   psoObjectDefinition defLifo = { PSO_LIFO, 0, 0 };
   psoFieldDefinition fields[1] = {
      { "Field_1", PSO_VARCHAR, {10} }
   };
   PSO_HANDLE dataDefHandle;

   errcode = psoInit( "10701", NULL );
   assert_true( errcode == PSO_OK );
   
   errcode = psoInitSession( &sessionHandle );
   assert_true( errcode == PSO_OK );
   errcode = psoInitSession( &sessionHandle2 );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateFolder( sessionHandle,
                              "/api_lifo_pop",
                              strlen("/api_lifo_pop") );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateQueue( sessionHandle,
                             "/api_lifo_pop/test",
                             strlen("/api_lifo_pop/test"),
                             &defLifo );
   assert_true( errcode == PSO_OK );

   errcode = psoCommit( sessionHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoLifoOpen( sessionHandle,
                           "/api_lifo_pop/test",
                           strlen("/api_lifo_pop/test"),
                           &objHandle );
   assert_true( errcode == PSO_OK );
   errcode = psoLifoOpen( sessionHandle2,
                          "/api_lifo_pop/test",
                          strlen("/api_lifo_pop/test"),
                          &objHandle2 );
   assert_true( errcode == PSO_OK );

   errcode = psoLifoPush( objHandle, data1, strlen(data1) );
   assert_true( errcode == PSO_OK );

   errcode = psoLifoPush( objHandle, data2, strlen(data2) );
   assert_true( errcode == PSO_OK );

   errcode = psoCommit( sessionHandle );
   assert_true( errcode == PSO_OK );
   
   /* Invalid arguments to tested function. */

   errcode = psoLifoPop( NULL,
                         buffer,
                         200,
                         &length );
   assert_true( errcode == PSO_NULL_HANDLE );
   
   errcode = psoLifoPop( objHandle,
                         NULL,
                         200,
                         &length );
   assert_true( errcode == PSO_NULL_POINTER );
   
   errcode = psoLifoPop( objHandle,
                         buffer,
                         2,
                         &length );
   assert_true( errcode == PSO_INVALID_LENGTH );
   
   errcode = psoLifoPop( objHandle,
                         buffer,
                         200,
                         NULL );
   assert_true( errcode == PSO_NULL_POINTER );
   
   /* End of invalid args. This call should succeed. */
   errcode = psoLifoPop( objHandle,
                         buffer,
                         200,
                         &length );
   assert_true( errcode == PSO_OK );
   assert_true( memcmp( buffer, data2, strlen(data2) ) == 0 );
   errcode = psoLifoPop( objHandle,
                         buffer,
                         200,
                         &length );
   assert_true( errcode == PSO_OK );
   assert_true( memcmp( buffer, data1, strlen(data1) ) == 0 );
   
   /*
    * Additional stuff to check while the Pop is uncommitted:
    *  - cannot get access to the item from first session.
    *  - can get access to the item from second session.
    *  - cannot modify it from second session.
    */
   errcode = psoLifoGetFirst( objHandle,
                               buffer,
                               200,
                               &length );
   assert_true( errcode == PSO_ITEM_IS_IN_USE );
   
   errcode = psoLifoGetFirst( objHandle2,
                               buffer,
                               200,
                               &length );
   assert_true( errcode == PSO_OK );
   errcode = psoLifoPop( objHandle2,
                          buffer,
                          200,
                          &length );
   assert_true( errcode == PSO_ITEM_IS_IN_USE );
   
   /*
    * Additional stuff to check after the commit:
    *  - cannot get access to the item from first session.
    *  - cannot get access to the item from second session.
    *  And that the error is PSO_EMPTY.
    *
    * Note to make sure that the deleted item is still in shared memory,
    * we first call GetFirst to get a pointer to the item from
    * session 2 (the failed call to Pop just above release that
    * internal pointer).
    */
   errcode = psoLifoGetFirst( objHandle2,
                              buffer,
                              200,
                              &length );
   assert_true( errcode == PSO_OK );
   errcode = psoCommit( sessionHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoLifoGetFirst( objHandle,
                              buffer,
                              200,
                              &length );
   assert_true( errcode == PSO_IS_EMPTY );
   
   errcode = psoLifoGetFirst( objHandle2,
                              buffer,
                              200,
                              &length );
   assert_true( errcode == PSO_IS_EMPTY );

   /* Close the session and try to act on the object */

   errcode = psoExitSession( sessionHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoLifoPop( objHandle,
                         buffer,
                         200,
                         &length );
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

