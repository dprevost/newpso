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
#include "API/Queue.h"
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

void test_null_data( void ** state )
{
   PSO_HANDLE sessionHandle, objHandle;
   int errcode;
   const char * data1 = "My Data1";
   psoObjectDefinition defQueue = { PSO_QUEUE, 0, 0 };
   psoFieldDefinition fields[1] = {
      { "Field_1", PSO_VARCHAR, {10} }
   };
   PSO_HANDLE dataDefHandle;
   unsigned int length;
   
   errcode = psoInit( "10701", NULL );
   assert_true( errcode == PSO_OK );
   
   errcode = psoInitSession( &sessionHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateFolder( sessionHandle,
                              "/api_queue_first_null_data",
                              strlen("/api_queue_first_null_data") );
   assert_true( errcode == PSO_OK );

   errcode = psoDataDefCreate( sessionHandle,
                               "api_queue_first_null_data",
                               strlen("api_queue_first_null_data"),
                               PSO_DEF_PHOTON_ODBC_SIMPLE,
                               (unsigned char *)fields,
                               sizeof(psoFieldDefinition),
                               &dataDefHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateQueue( sessionHandle,
                             "/api_queue_first_null_data/test",
                             strlen("/api_queue_first_null_data/test"),
                             &defQueue,
                             dataDefHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoQueueOpen( sessionHandle,
                           "/api_queue_first_null_data/test",
                           strlen("/api_queue_first_null_data/test"),
                           &objHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoQueuePush( objHandle, data1, strlen(data1) );
   assert_true( errcode == PSO_OK );

   expect_assert_failure( psoaQueueFirst( objHandle, NULL, &length ) );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_handle( void ** state )
{
   PSO_HANDLE sessionHandle, objHandle;
   int errcode;
   const char * data1 = "My Data1";
   psoObjectDefinition defQueue = { PSO_QUEUE, 0, 0 };
   psoFieldDefinition fields[1] = {
      { "Field_1", PSO_VARCHAR, {10} }
   };
   PSO_HANDLE dataDefHandle;
   unsigned char * buffer;
   unsigned int length;

   errcode = psoInit( "10701", NULL );
   assert_true( errcode == PSO_OK );
   
   errcode = psoInitSession( &sessionHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateFolder( sessionHandle,
                              "/api_queue_first_null_handle",
                              strlen("/api_queue_first_null_handle") );
   assert_true( errcode == PSO_OK );

   errcode = psoDataDefCreate( sessionHandle,
                               "api_queue_first_null_handle",
                               strlen("api_queue_first_null_handle"),
                               PSO_DEF_PHOTON_ODBC_SIMPLE,
                               (unsigned char *)fields,
                               sizeof(psoFieldDefinition),
                               &dataDefHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateQueue( sessionHandle,
                             "/api_queue_first_null_handle/test",
                             strlen("/api_queue_first_null_handle/test"),
                             &defQueue,
                             dataDefHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoQueueOpen( sessionHandle,
                             "/api_queue_first_null_handle/test",
                             strlen("/api_queue_first_null_handle/test"),
                             &objHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoQueuePush( objHandle, data1, strlen(data1) );
   assert_true( errcode == PSO_OK );

   expect_assert_failure( psoaQueueFirst( NULL, &buffer, &length ) );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_length( void ** state )
{
   PSO_HANDLE sessionHandle, objHandle;
   int errcode;
   const char * data1 = "My Data1";
   psoObjectDefinition defQueue = { PSO_QUEUE, 0, 0 };
   psoFieldDefinition fields[1] = {
      { "Field_1", PSO_VARCHAR, {10} }
   };
   PSO_HANDLE dataDefHandle;
   unsigned char * buffer;

   errcode = psoInit( "10701", NULL );
   assert_true( errcode == PSO_OK );
   
   errcode = psoInitSession( &sessionHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateFolder( sessionHandle,
                              "/api_queue_first_null_length",
                              strlen("/api_queue_first_null_length") );
   assert_true( errcode == PSO_OK );

   errcode = psoDataDefCreate( sessionHandle,
                               "api_queue_first_null_length",
                               strlen("api_queue_first_null_length"),
                               PSO_DEF_PHOTON_ODBC_SIMPLE,
                               (unsigned char *)fields,
                               sizeof(psoFieldDefinition),
                               &dataDefHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateQueue( sessionHandle,
                             "/api_queue_first_null_length/test",
                             strlen("/api_queue_first_null_length/test"),
                             &defQueue,
                             dataDefHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoQueueOpen( sessionHandle,
                           "/api_queue_first_null_length/test",
                           strlen("/api_queue_first_null_length/test"),
                           &objHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoQueuePush( objHandle, data1, strlen(data1) );
   assert_true( errcode == PSO_OK );

   expect_assert_failure( psoaQueueFirst( objHandle, &buffer, NULL ) );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_wrong_handle( void ** state )
{
   PSO_HANDLE sessionHandle, objHandle;
   int errcode;
   const char * data1 = "My Data1";
   psoObjectDefinition defQueue = { PSO_QUEUE, 0, 0 };
   psoFieldDefinition fields[1] = {
      { "Field_1", PSO_VARCHAR, {10} }
   };
   PSO_HANDLE dataDefHandle;
   unsigned char * buffer;
   unsigned int length;

   errcode = psoInit( "10701", NULL );
   assert_true( errcode == PSO_OK );
   
   errcode = psoInitSession( &sessionHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateFolder( sessionHandle,
                              "/api_queue_first_wrong_handle",
                              strlen("/api_queue_first_wrong_handle") );
   assert_true( errcode == PSO_OK );

   errcode = psoDataDefCreate( sessionHandle,
                               "api_queue_first_wrong_handle",
                               strlen("api_queue_first_wrong_handle"),
                               PSO_DEF_PHOTON_ODBC_SIMPLE,
                               (unsigned char *)fields,
                               sizeof(psoFieldDefinition),
                               &dataDefHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateQueue( sessionHandle,
                             "/api_queue_first_wrong_handle/test",
                             strlen("/api_queue_first_wrong_handle/test"),
                             &defQueue,
                             dataDefHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoQueueOpen( sessionHandle,
                           "/api_queue_first_wrong_handle/test",
                           strlen("/api_queue_first_wrong_handle/test"),
                           &objHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoQueuePush( objHandle, data1, strlen(data1) );
   assert_true( errcode == PSO_OK );

   expect_assert_failure( psoaQueueFirst( sessionHandle, &buffer, &length ) );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_pass( void ** state )
{
   PSO_HANDLE sessionHandle, objHandle;
   int errcode;
   const char * data1 = "My Data1";
   psoObjectDefinition defQueue = { PSO_QUEUE, 0, 0 };
   psoFieldDefinition fields[1] = {
      { "Field_1", PSO_VARCHAR, {10} }
   };
   PSO_HANDLE dataDefHandle;
   unsigned char * buffer;
   unsigned int length;

   errcode = psoInit( "10701", NULL );
   assert_true( errcode == PSO_OK );
   
   errcode = psoInitSession( &sessionHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateFolder( sessionHandle,
                              "/api_queue_first_pass",
                              strlen("/api_queue_first_pass") );
   assert_true( errcode == PSO_OK );

   errcode = psoDataDefCreate( sessionHandle,
                               "api_queue_first_pass",
                               strlen("api_queue_first_pass"),
                               PSO_DEF_PHOTON_ODBC_SIMPLE,
                               (unsigned char *)fields,
                               sizeof(psoFieldDefinition),
                               &dataDefHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateQueue( sessionHandle,
                             "/api_queue_first_pass/test",
                             strlen("/api_queue_first_pass/test"),
                             &defQueue,
                             dataDefHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoQueueOpen( sessionHandle,
                           "/api_queue_first_pass/test",
                           strlen("/api_queue_first_pass/test"),
                           &objHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoQueuePush( objHandle, data1, strlen(data1) );
   assert_true( errcode == PSO_OK );

   errcode = psoaQueueFirst( objHandle, &buffer, &length );
   assert_true( errcode == PSO_OK );
   assert_true( memcmp( buffer, data1, strlen(data1) ) == 0 );
   
   psoExit();
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   int rc = 0;
#if defined(PSO_UNIT_TESTS)
   const UnitTest tests[] = {
      unit_test_setup_teardown( test_null_data,    setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_handle,  setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_length,  setup_test, teardown_test ),
      unit_test_setup_teardown( test_wrong_handle, setup_test, teardown_test ),
      unit_test_setup_teardown( test_pass,         setup_test, teardown_test ),
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

