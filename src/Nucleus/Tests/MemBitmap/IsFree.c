/*
 * Copyright (C) 2006-2010 Daniel Prevost <dprevost@photonsoftware.org>
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

#include "Nucleus/MemBitmap.h"
#include "Nucleus/Tests/EngineTestCommon.h"

psonMemBitmap * pBitmap;
unsigned char * ptr;
psonSessionContext context;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void setup_test()
{
   initTest( &context );

   ptr = malloc( PSON_BLOCK_SIZE*10 );
   assert( ptr != NULL );
   context.pBaseAddress = ptr;
   
   pBitmap = (psonMemBitmap*) ptr;
   
   psonMemBitmapInit( pBitmap, 
                      SET_OFFSET(context.pBaseAddress, ptr),
                      10*PSON_BLOCK_SIZE,
                      8,
                      &context );

  psonSetBufferAllocated( pBitmap,
                          PSON_BLOCK_SIZE/4,  /* offset */
                          PSON_BLOCK_SIZE*2,  /* length */
                          &context );

   psonSetBufferFree( pBitmap,
                      PSON_BLOCK_SIZE/2,  /* offset */
                      PSON_BLOCK_SIZE/4,  /* length */
                      &context );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void teardown_test()
{
   if (ptr) free(ptr);
   ptr = NULL;
   context.pBaseAddress = NULL;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_bitmap( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonIsBufferFree( NULL, PSON_BLOCK_SIZE/2, &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_context( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonIsBufferFree( pBitmap, PSON_BLOCK_SIZE/2, NULL ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_offset( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonIsBufferFree( pBitmap, PSON_NULL_OFFSET, &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_pass( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   bool answer;
   psonSessionContext context;
   
   answer = psonIsBufferFree( pBitmap, PSON_BLOCK_SIZE/4, &context );
   assert_false( answer );
   
   answer = psonIsBufferFree( pBitmap, PSON_BLOCK_SIZE/2, &context );
   assert_true( answer );
   
   psonMemBitmapFini( pBitmap, &context );

#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   int rc = 0;
#if defined(PSO_UNIT_TESTS)
   const UnitTest tests[] = {
      unit_test_setup_teardown( test_null_bitmap,  setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_context, setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_offset,  setup_test, teardown_test ),
      unit_test_setup_teardown( test_pass,         setup_test, teardown_test )
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

