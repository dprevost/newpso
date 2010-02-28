/*
 * Copyright (C) 2008-2010 Daniel Prevost <dprevost@photonsoftware.org>
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
#include "API/ListReaders.h"
#include "API/Tests/quasar-run.h"

#define MAX_READERS 50000

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
   int i;
   unsigned int sum1 = 0, sum2 = 0;

   psoaReader * readers, * dummy, * first;
   int * readerIsIn;
   psoaListReaders list;

   srand( 0x123456 );
   
   psoaListReadersInit( &list );
   
   readers = (psoaReader *)malloc( MAX_READERS*sizeof(psoaReader) );
   assert_false( readers == NULL );
   readerIsIn = (int *)malloc( MAX_READERS*sizeof(int) );
   assert_false( readerIsIn == NULL );

   for ( i = 0; i < MAX_READERS; ++i ) {
      readers[i].address = (void *) &readers[i];
      readerIsIn[i] = 0;
      if ( rand() > RAND_MAX/2 ) {
         psoaListReadersPut( &list, &readers[i] );
         readerIsIn[i] = 1;
         sum1++;
      }
   }
   
   assert_true( sum1 == list.currentSize );
   
   /* Test our loop */
   assert_true( psoaListReadersPeakFirst( &list, &dummy ) == true );
   first = dummy;
   do {
      assert_true( dummy->address == (void*) dummy );
      sum2++;
   } while ( psoaListReadersPeakNext( &list, dummy, &dummy ) );
   
   assert_true( sum2 == list.currentSize );
   assert_true( sum2 == sum1 );
   
   /* inverse our selection ! (to test removals) */
   for ( i = 0; i < MAX_READERS; ++i ) {
      if ( readerIsIn[i] == 0 ) {
         psoaListReadersPut( &list, &readers[i] );
         readerIsIn[i] = 1;
      }
      else {
         psoaListReadersRemove( &list, &readers[i] );
         readerIsIn[i] = 0;
      }
   }

   assert_true( (MAX_READERS-sum1) == list.currentSize );

   /* Retest our loop */
   sum2 = 0;
   assert_true( psoaListReadersPeakFirst( &list, &dummy ) == true );

   /* 
    * We cannot have the same first on the two loops since the 
    * selection was inversed.
    */
   assert_false( dummy == first );

   do {
      assert_true( dummy->address == (void*) dummy );
      sum2++;
   } while ( psoaListReadersPeakNext( &list, dummy, &dummy ) );
   
   assert_true( sum2 == list.currentSize );
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

