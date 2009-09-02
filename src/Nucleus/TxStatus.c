/*
 * Copyright (C) 2009 Daniel Prevost <dprevost@photonsoftware.org>
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

#include "Nucleus/TxStatus.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#if defined(PSO_TRACE)

void psonTxStatusDump( psonTxStatus * pStatus )
{
   fprintf(stderr, "psonTXStatus (%p) offset = "PSO_PTRDIFF_T_FORMAT"\n",
      pStatus, SET_OFFSET(pStatus) );
   if ( pStatus == NULL ) return;

   fprintf( stderr, "  offset of the current transaction object: "PSO_PTRDIFF_T_FORMAT"\n",
      pStatus->txOffset );

   if ( pStatus->status == 0 )
      fprintf( stderr, "  Status is ok (no running transaction)\n" );
   if ( pStatus->status & PSON_TXS_DESTROYED )
      fprintf( stderr, "  Object is marked as destroyed\n" );
   if ( pStatus->status & PSON_TXS_ADDED )
      fprintf( stderr, "  Object is marked as added\n" );
   if ( pStatus->status & PSON_TXS_EDIT )
      fprintf( stderr, "  Object is marked as being edited\n" );
   if ( pStatus->status & PSON_TXS_REPLACED )
      fprintf( stderr, "  Object is marked as being replaced\n" );
   if ( pStatus->status & PSON_TXS_DESTROYED_COMMITTED )
      fprintf( stderr, "  Destruction of object is committed\n" );
   if ( pStatus->status & PSON_TXS_EDIT_COMMITTED )
      fprintf( stderr, "  Edition of object is committed\n" );

   fprintf( stderr, "  usage counter: %ud\n", pStatus->usageCounter );
   fprintf( stderr, "  parent counter %ud\n", pStatus->parentCounter );
}

#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
