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

#ifndef PSON_PROCESS_H
#define PSON_PROCESS_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Nucleus/Engine.h"
#include "Nucleus/LinkedList.h"
#include "Nucleus/Session.h"
#include "Nucleus/MemoryObject.h"
#include "Nucleus/BlockGroup.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * This object is part of a group of objects which are used for handling 
 * crashes (process crashes, mainly). They keep track of the current status
 * and operations done by each process and by doing so enables the server
 * to cleanup if a process die unexpectedly: undo the current transaction(s),
 * closed objects (->decrease access counter), unlocked objects, etc.
 *
 */
struct psonProcess
{
   /** Always first */
   struct psonMemObject memObject;

   psonLinkNode node;
   
   pid_t pid;

   psonLinkedList listOfSessions;

   bool processIsTerminating;

   /** Variable size struct - always put at the end */
   struct psonBlockGroup blockGroup;

};

typedef struct psonProcess psonProcess;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonProcessAddSession( psonProcess        * pProcess,
                            void               * pApiSession,
                            psonSession       ** pSession,
                            psonSessionContext * pContext );

#if defined(PSO_USE_TRACE)
void psonProcessDump( psonProcess * process, int indent );
#endif

void psonProcessFini( psonProcess        * pProcess,
                      psonSessionContext * pContext );

bool psonProcessGetFirstSession( psonProcess        * pProcess,
                                 psonSession       ** ppSession,
                                 psonSessionContext * pContext );

bool psonProcessGetNextSession( psonProcess        * pProcess,
                                psonSession        * pCurrent,
                                psonSession       ** ppNext,
                                psonSessionContext * pContext );

bool psonProcessInit( psonProcess        * pProcess,
                      pid_t                pid,
                      psonSessionContext * pContext );
   
/*
 * Takes a lock on the current object. Not on the psonSession itself. 
 */
bool psonProcessRemoveSession( psonProcess        * pProcess,
                               psonSession        * pSession,
                               psonSessionContext * pContext );

static inline
void psonProcessNoMoreSessionAllowed( psonProcess        * pProcess,
                                      psonSessionContext * pContext )
{
   PSO_TRACE_ENTER( pContext );

   if ( psonLock( &pProcess->memObject, pContext ) ) {
      pProcess->processIsTerminating = true;
      psonUnlock( &pProcess->memObject, pContext );
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSON_PROCESS_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

