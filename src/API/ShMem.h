/*
 * Copyright (C) 2010 Daniel Prevost <dprevost@photonsoftware.org>
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

#ifndef PSO_API_SHMEM_H
#define PSO_API_SHMEM_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "API/api.h"
#include "Nucleus/Engine.h"
#include "Nucleus/SessionContext.h"
#include <photon/psoCommon.h>
#include "API/Connector.h"
#include "Common/ThreadLock.h"
#include "Common/MemoryFile.h"

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* Forward declarations */
struct psonMemoryHeader;
struct psonProcess;
struct psonObjectContext;
struct psoProxyObject;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

typedef struct psoaShMem
{
   psoaObjetType type;

   /** Pointer to the header of the shared memory. */
   struct psonMemoryHeader* pHeader;

   /** 
    * Pointer to our own cleanup object in shared memory. This object is used by 
    * the CleanupManager to hold process-specific information that might
    * be needed in case of a crash (current state of transactions, etc.).
    */
   struct psonProcess* pCleanup;

   /** The pid of the process is stored in this member. */
   pso_lock_T lockValue;

   /** The name of the directory for log files (used by transactions). */
   char logDirName[PATH_MAX];

   /** This object encapsulates the task of talking with quasar. */
   psoaConnector connector;
   
   /** If true, we are not using the server (quasar). */
   bool standalone;
   
   /** 
    * Encapsulates access to the shared memory file (and the differences
    * in how it is accessed between Windows and Posix systems).
    */
   psocMemoryFile memoryFile;
   
} psoaShMem;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#if 0

PHOTON_API_EXPORT
int psoaProcessInit( psoaProcess * process,
                     const char  * qsrAddress,
                     const char  * processName );

PHOTON_API_EXPORT
void psoaProcessFini();

PHOTON_API_EXPORT
bool AreWeTerminated();

PHOTON_API_EXPORT
int psoaOpenMemory( psoaProcess        * process,
                    const char        * memoryFileName,
                    size_t              memorySizekb,
                    psonSessionContext * pSession );
                        
PHOTON_API_EXPORT
void psoaCloseMemory( psoaProcess        * process,
                      psonSessionContext * pSession );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#ifndef __cplusplus
static 
#endif
__inline
bool psoaProcessLock()
{
   bool ok;
   
   ok = psocTryAcquireThreadLock( &g_ProcessMutex, PSON_LOCK_TIMEOUT );
   PSO_POST_CONDITION( ok == true || ok == false );
   
   return ok;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** Unlock the current process. */
#ifndef __cplusplus
static 
#endif
__inline
void psoaProcessUnlock()
{
   psocReleaseThreadLock( &g_ProcessMutex );
}
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSO_API_SHMEM_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

