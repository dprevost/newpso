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

#ifndef PSOC_ARCH_PROCESS_LOCK_H
#define PSOC_ARCH_PROCESS_LOCK_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

inline void
psocAcquireProcessLock( psocProcessLock * pLock,
                        pid_t             pid_locker )
{
   unsigned int out = 0xff;

   PSO_PRE_CONDITION( pLock != NULL );
   PSO_INV_CONDITION( pLock->initialized == PSOC_LOCK_SIGNATURE );
   PSO_PRE_CONDITION( pid_locker != 0 );

   for (;;) {
      if ( pLock->lock == 0 ) {
         __asm__ __volatile__ (
            "xchgl %0, (%1)"
            : "=r"(out)
            : "r"(&pLock->lock), "0"(out)
            : "memory" );
      }
      if ( out == 0 ) break;
      psocLockSleep( &g_timeOut );
   }

   /* Failure to get the lock should not occured!!! */
   PSO_POST_CONDITION( out == 0 );

   /* There is a "race condition" when saving the pid of the caller
    * this way -> it is possible to have lock out-of-synch with
    * the actual condition of the object.
    */
   pLock->pid = pid_locker;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

inline bool
psocTryAcquireProcessLock( psocProcessLock * pLock,
                           pid_t             pid_locker,
                           unsigned int      milliSecs )
{
   unsigned int out = 0xff;

   PSO_PRE_CONDITION( pLock != NULL );
   PSO_INV_CONDITION( pLock->initialized == PSOC_LOCK_SIGNATURE );
   PSO_PRE_CONDITION( pid_locker != 0 );

   if ( pLock->lock == 0 ) {
      __asm__ __volatile__ (
         "xchgl %0, (%1)"
         : "=r"(out)
         : "r"(&pLock->lock), "0"(out)
         : "memory" );
   }

   if ( out > 0 ) {
      int iterations = milliSecs/g_timeOutinMilliSecs;
      int i;
      
      for ( i = 0; i < iterations; ++i ) {
         psocLockSleep( &g_timeOut );
         if ( pLock->lock == 0 ) {
            __asm__ __volatile__ (
               "xchgl %0, (%1)"
               : "=r"(out)
               : "r"(&pLock->lock), "0"(out)
               : "memory" );
         }
         if ( out == 0 ) break;
      }
   }
 
   /* 
    * If the condition is true, the time slice was not enough, 
    * we did not get the lock.
    */
   if ( out > 0 ) return false;

   /* There is a "race condition" when saving the pid of the caller
    * this way -> it is possible to have lock out-of-synch with
    * the actual condition of the object.
    */
   pLock->pid = pid_locker;

   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

inline void
psocReleaseProcessLock( psocProcessLock * pLock )
{
   PSO_PRE_CONDITION( pLock != NULL );
   PSO_INV_CONDITION( pLock->initialized == PSOC_LOCK_SIGNATURE );

   pLock->pid = 0;

   /* 
    * We might need to change the template to "lock; movl $0, %0" on 
    * some cpus (erratum of some Pentium PRO (1995-1997)) and the 
    * Winchips (1997 to 1999). 
    */ 
   __asm__ __volatile__ (
      "movl $0, %0"
      :"=m"(pLock->lock)
      :
      :"memory" );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSOC_ARCH_PROCESS_LOCK_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

