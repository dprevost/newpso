/*
 * Copyright (C) 2006-2009 Daniel Prevost <dprevost@photonsoftware.org>
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

psonSessionContext context;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void setup_test()
{
   initTest( &context );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void teardown_test()
{
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_context( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonGetBitmapLengthBytes( 1024, 8, NULL ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_poweroftwo7( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonGetBitmapLengthBytes( 1024, 7, &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_poweroftwo9( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonGetBitmapLengthBytes( 1024, 9, &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_zero_granu( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonGetBitmapLengthBytes( 1024, 0, &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_zero_length( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonGetBitmapLengthBytes( 0, 8, &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_pass( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   size_t calculatedSize;
   
   calculatedSize = psonGetBitmapLengthBytes( 1024, 8, &context );
   assert_true( calculatedSize == 1024/8/8 );
   
   calculatedSize = psonGetBitmapLengthBytes( 1023, 8, &context );
   assert_true( calculatedSize == 1024/8/8 );
   
   calculatedSize = psonGetBitmapLengthBytes( 1025, 8, &context );
   assert_true( calculatedSize == 1024/8/8 + 1 );
   
   calculatedSize = psonGetBitmapLengthBytes( 1016, 8, &context );
   assert_true( calculatedSize == 1024/8/8 );
   
   calculatedSize = psonGetBitmapLengthBytes( 1024+63, 8, &context );
   assert_true( calculatedSize == 1024/8/8 + 1 );
   
   calculatedSize = psonGetBitmapLengthBytes( 1024+64, 8, &context );
   assert_true( calculatedSize == 1024/8/8 + 1 );
   
   calculatedSize = psonGetBitmapLengthBytes( 1024+65, 8, &context );
   assert_true( calculatedSize == 1024/8/8 + 2 );
   
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   int rc = 0;
#if defined(PSO_UNIT_TESTS)
   const UnitTest tests[] = {
      unit_test_setup_teardown( test_null_context, setup_test, teardown_test ),
      unit_test_setup_teardown( test_poweroftwo7,  setup_test, teardown_test ),
      unit_test_setup_teardown( test_poweroftwo9,  setup_test, teardown_test ),
      unit_test_setup_teardown( test_zero_granu,   setup_test, teardown_test ),
      unit_test_setup_teardown( test_zero_length,  setup_test, teardown_test ),
      unit_test_setup_teardown( test_pass,         setup_test, teardown_test ),
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

