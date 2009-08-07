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

/*
 * This test is very similar to the LockConcurrency test... except that
 * this program does not always lock properly (on purpose)!!! The rate of 
 * failure of our locks is controlled by the #define DEFAULT_FAILURE_RATE.
 *
 * In a way, it insures that we would be able to detect a failure in
 * our locking mechanism using the standard LockConcurrency test. 
 *
 * The amount of time we will try to observe a concurrency failure is
 * determined by the first argument to the program.
 */
 
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Common/Common.h"
#include "Common/MemoryFile.h"
#include "Common/Timer.h"
#include "Common/ProcessLock.h"
#include "Common/Options.h"
#if defined(_WIN32)
#  include <Process.h>
#else
#  include <sys/wait.h>
#endif

#if defined(_MSC_VER) && (_MSC_VER >= 1400)
#  define unlink(a) _unlink(a)
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#define US_PER_SEC     1000000

struct localData
{
   psocProcessLock lock;
   int exitFlag;
   volatile char dum1[150];
   volatile char dum2[250];
};

/*
 * Misnamed define... the failure rate is 1/DEFAULT_FAILURE_RATE...
 *
 * I would prefer a much lower failure rate but it would mean
 * being very patient... (failures can only be seen when context
 * switches are done and with time slices of 1/1000 sec. (on many
 * modern OSes), it would force us to run the test for a lot longer).
 *
 * [DEFAULT_FAILURE_RATE 1000 --> 0.1% failure]
 */
#define DEFAULT_FAILURE_RATE   1000
#define DEFAULT_TIME            300

#define CHECK_TIMER 1345 /* Check the time every CHECK_TIMER loops */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main( int argc, char* argv[] )
{
   pid_t mypid;
   unsigned long sec, nanoSec;
   psocTimer timer;
   bool foundError = false;
   
   unsigned long elapsedTime = 0, maxTime = 0;
   unsigned long loop = 1, failureRate;
   void* ptr = NULL;   
   char filename[PATH_MAX];
   struct localData *data = NULL;
   int errcode;
   bool ok;
   psocMemoryFile memFile;
   psocErrorHandler errorHandler;
   
   char dum3[100];
   int dumId;
   psocOptionHandle handle;
   char *argument;
   struct psocOptStruct opts[2] = { 
      { 'r', "rate",       1, "rateOfFailure", "Inverse rate: 1000 means a rate of 0.1%" },
      { 't', "time",       1, "timeInSecs",    "Time to run the tests" }
   };

   psocInitErrorDefs();
   psocInitErrorHandler( &errorHandler );
   psocInitTimer( &timer );   

   ok = psocSetSupportedOptions( 2, opts, &handle );
   assert( ok );
   
   errcode = psocValidateUserOptions( handle, argc, argv, 1 );
   assert( errcode >= 0 );
   if ( errcode > 0 ) {
      psocShowUsage( handle, "LockShouldFailStd", "" );
      return 1;
   }
   
   if ( psocGetShortOptArgument( handle, 't', &argument ) ) {
      maxTime = strtol( argument, NULL, 0 );
      assert( maxTime > 0 );
   }
   else {
      maxTime = DEFAULT_TIME; /* in seconds */
   }
   
   strcpy( filename, "Memfile.mem" );
   
   if ( psocGetShortOptArgument( handle, 'r', &argument ) ) {
      failureRate = strtol( argument, NULL, 0 );
      assert( failureRate > 0 );
   }
   else {
      failureRate = DEFAULT_FAILURE_RATE;
   }
   
   psocInitMemoryFile( &memFile, 10, filename );
   
   psocBeginTimer( &timer );
   maxTime *= US_PER_SEC;

   mypid = getpid();
   ok = psocOpenMemFile( &memFile, &ptr, &errorHandler );
   assert( ok );
   data = (struct localData*) ptr;
   
   for (;;) {
      if ( (loop%failureRate) != 0 ) {
         psocAcquireProcessLock( &data->lock, mypid );
      }
         
      sprintf( (char *)data->dum2, "dumStr2 %d  ", mypid );
      memcpy( (void *)data->dum1, (void *)data->dum2, 100 );
            
      sscanf( (char *)data->dum1, "%s %d", dum3, &dumId );

      if ( dumId != mypid ) {

         psocEndTimer( &timer );
         psocCalculateTimer( &timer, &sec, &nanoSec );

         fprintf( stderr, "%s %d) - time = %u.%03u secs, \n",
                  "Ok! We got our expected error (pid =",
                  mypid,
                  (unsigned int)sec,
                  (unsigned int)(nanoSec/1000/1000) );
         data->exitFlag = 1;
         if ( (loop%failureRate) != 0 ) {
            psocReleaseProcessLock( &data->lock );
         }
            
         return 0;
      }
         
      if ( (loop%failureRate) != 0 ) {
         psocReleaseProcessLock( &data->lock );
      }
         
      if ( data->exitFlag == 1 ) break;
   
      loop++;

      if ( (loop%CHECK_TIMER) != 0 ) {
         psocEndTimer( &timer );
         psocCalculateTimer( &timer, &sec, &nanoSec );
         
         elapsedTime = sec*US_PER_SEC + nanoSec/1000;
         if ( elapsedTime > maxTime ) break;
      }
   } /* For loop */

   psocFiniErrorHandler( &errorHandler );
   psocFiniErrorDefs();

   return 1;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

