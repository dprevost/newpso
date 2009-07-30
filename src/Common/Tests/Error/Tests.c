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

#include "Common/Common.h"
#include "Common/ErrorHandler.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int msgErrorHandler( int errorCode, char* msg, unsigned int msgLength )
{
   errorCode = errorCode;
   msgLength = msgLength;

   strncpy( msg, "Dummy Handler", strlen("Dummy Handler") );

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test1( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   psocErrorHandler errorHandler;
   char msg[100];
   psocErrMsgHandle handle1, handle2;
   
   psocInitErrorDefs();
   psocInitErrorHandler( &errorHandler );
  
   handle1 = psocAddErrorMsgHandler( "Dummy1", &msgErrorHandler );
   assert_false( handle1 == PSOC_NO_ERRHANDLER );
   handle2 = psocAddErrorMsgHandler( "Dummy2", &msgErrorHandler );
   assert_false( handle2 == PSOC_NO_ERRHANDLER );
   
   psocSetError( &errorHandler, handle2, 7 );

   psocChainError( &errorHandler, handle1, 17 );

   /* This should work, although that would leave a single byte
    * to use for the error message...
    */
   psocGetErrorMsg( &errorHandler, msg, 81 );

   psocSetError( &errorHandler, PSOC_ERRNO_HANDLE, ENOENT );

   psocFiniErrorHandler( &errorHandler );
   psocFiniErrorDefs();
   
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   int rc = 0;
#if defined(PSO_UNIT_TESTS)
   const UnitTest tests[] = {
      unit_test( test1 ),
   };

   rc = run_tests(tests);
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

