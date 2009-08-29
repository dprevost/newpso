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
#include "API/Tests/quasar-run.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */


//#define BASE_DIR newpso.git

void setup_test()
{
#if 0
   char w[PATH_MAX];
   FILE * fp;
   int i, j, k;
   
   fp = fopen( "/home/project/Photon/newpso.git/out.txt", "w" );
   if ( fp == NULL ) {
      fprintf( stderr, "ok\n" );
      return;
   }
   getcwd(w, PATH_MAX);
   for ( i = 0; i < strlen(w) ) {
      if ( w[i] == '/' ) {
 //        if ( strcmp( w[i+1], 
   }
   fprintf( fp, "path: %s\n", w );
  ///home/project/Photon/newpso.git/build/default/src/Quasar/quasar  
#endif
   startQuasar();
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void teardown_test()
{
   stopQuasar();
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_pass( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   int errcode;
   
   errcode = psoInit( NULL, "Init" );
   assert_true( errcode == PSO_INVALID_QUASAR_ADDRESS );

   errcode = psoInit( "12345", "" );
   assert_true( errcode == PSO_CONNECT_ERROR );

   sleep( 30 );
   errcode = psoInit( "10701", NULL );
   assert_true( errcode == PSO_OK );
   
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

