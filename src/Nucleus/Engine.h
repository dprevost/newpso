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

#ifndef PSON_ENGINE_H
#define PSON_ENGINE_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Common/Common.h"
#include "Common/ProcessLock.h"
#include <photon/psoErrors.h>
#include <photon/psoCommon.h>
#include "Common/ErrorHandler.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * How do you identify NULL values using offsets instead of pointers? 
 * Tricky...
 * You cannot use the trick SET_OFFSET(NULL) because this set the value
 * to -base_address - which is only valid for a given process. 
 * You cannot use the value "0" because it is a valid offset. 
 * However the offset -1 (or 0xffffffff on 32 bits systems) can never
 * be reached (if ptrdiff_t is always the same number of bits as
 * a pointer - which is guaranteed by definition ...).
 */

 /* MAJOR concern: you cannot check if a reconstructed pointer is
  * NULL or not - this makes no sense anymore. You have to test 
  * the offset for PSON_NULL_OFFSET !!! 
  */
#define PSON_NULL_OFFSET ( (ptrdiff_t) -1 )

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 *  The base address of the shared memory as seen for each process (each 
 *  process having their own copy of this global). This pointer is used
 *  everywhere to recover the real pointer addresses from our offsets
 */
extern unsigned char* g_pBaseAddr;

extern psocErrMsgHandle g_psoErrorHandle;

#define SET_OFFSET(BASE_ADDRESS,PTR) ( \
   (ptrdiff_t) ( (unsigned char*)(PTR) - (unsigned char*)(BASE_ADDRESS) ) )

/* Only use this macro when you know, for a fact, that the offset cannot
 * be the PSON_NULL_OFFSET (for example, in the LinkedList "class", the links
 * are never set to PSON_NULL_OFFSET...). 
 */
#define GET_PTR_FAST(BASE_ADDRESS,OFF,TYPE) ( \
   (TYPE *) ( (unsigned char*)BASE_ADDRESS + (ptrdiff_t) (OFF) ) )

#define GET_PTR_OPT(BASE_ADDRESS,TARGET,OFFSET,TYPE) { \
   TARGET = (TYPE *) ( (unsigned char*) (BASE_ADDRESS) + (ptrdiff_t) (OFFSET) ); \
}

#define GET_PTR_DBG(BASE_ADDRESS,TARGET,OFFSET,TYPE) { \
   PSO_INV_CONDITION( (OFFSET) != 0 ); \
   PSO_INV_CONDITION( (OFFSET) != PSON_NULL_OFFSET ); \
   TARGET = (TYPE *) ( (unsigned char*) (BASE_ADDRESS) + (ptrdiff_t) (OFFSET) ); \
}

#if defined(USE_DBC)
#  define GET_PTR(BASE_ADDRESS,TARGET,OFFSET,TYPE) \
   GET_PTR_DBG(BASE_ADDRESS,TARGET,OFFSET,TYPE)
#else
#  define GET_PTR(BASE_ADDRESS,TARGET,OFFSET,TYPE) \
   GET_PTR_OPT(BASE_ADDRESS,TARGET,OFFSET,TYPE)
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#define PSON_LOCK_TIMEOUT 10000 /* in milliseconds */

#define PSON_MAX_LOCK_DEPTH 3

/** Pages allocations will be done as multiples of PSON_BLOCK_SIZE. */
#if ! defined(PSON_BLOCK_SIZE)
#  define PSON_BLOCK_SIZE   8192
#  define PSON_BLOCK_SHIFT    13
#endif

/** Memory allocation will be done as multiples of PSON_ALLOCATION_UNIT. */
#if SIZEOF_VOID_P == 4
#  define PSON_ALLOCATION_UNIT 16
#elif SIZEOF_VOID_P == 8
#  define PSON_ALLOCATION_UNIT 32
#else
#  error "Without a known SIZEOF_VOID_P (4 or 8) I cannot calculate PSON_ALLOCATION_UNIT"
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * List of possible "type" identifiers for all objects allocated
 * in shared memory. Each of these objects "inherits" from MemoryObject
 * which sets the object identifier (as part of the initializer).
 * This is used, in part, by the crash recovery process
 * to validate/invalidate recovered buffers.
 *
 * Note: all the identifiers are odd numbers to distinguish them from
 * stray pointers/offsets (for debugging, if needed).
 */

enum psonMemObjIdentifier
{
   /**
    * Special identifiers - it is set by the allocator when a group of blocks
    * is allocated (under lock). An "allocated" group with PSON_IDENT_LIMBO
    * is a group in limbo while the same group with PSON_IDENT_ALLOCATED
    * is not.
    */
   PSON_IDENT_CLEAR           = 0xc1ea9001,
   PSON_IDENT_LIMBO           = 0xccaaffee,
   PSON_IDENT_ALLOCATED       = 0xeeffaacc,
   
   PSON_IDENT_FIRST           = 0x34220101,
   PSON_IDENT_ALLOCATOR       = 0x34220103,

   PSON_IDENT_FOLDER          = 0x34220105,
   PSON_IDENT_HASH_MAP        = 0x34220107,
   PSON_IDENT_PROCESS         = 0x34220109,
   PSON_IDENT_PROCESS_MGR     = 0x3422010b,
   PSON_IDENT_QUEUE           = 0x3422010d,
   PSON_IDENT_SESSION         = 0x3422010f,
   PSON_IDENT_TRANSACTION     = 0x34220111,
   PSON_IDENT_MAP             = 0x34220113,
   PSON_IDENT_CURSOR          = 0x34220115,
   
   PSON_IDENT_LAST            = 0x34220125

};

typedef enum psonMemObjIdentifier psonMemObjIdent;

#define PSON_IDENT_PAGE_GROUP   0x80000000

#if defined(PSO_USE_TRACE)

struct psonSessionContext;

void psonMemObjIdentifierDump( psonMemObjIdent             identifier,
                               int                         indent,
                               struct psonSessionContext * pContext );
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum psonItemTypeEnum
{
   PSON_FIRST_ITEM,
   PSON_HASH_ITEM,
   PSON_HASH_TX_ITEM,
   PSON_LAST_ITEM
};

typedef enum psonItemTypeEnum psonItemTypeEnum;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * This enum allows to count the number of objects and the number of 
 * extensions (additional groups of blocks added to an object). 
 */
enum psonAllocTypeEnum
{
   /** When allocating/freeing the initial group of blocks of an API object */ 
   PSON_ALLOC_API_OBJ,
   
   /** Any other group of blocks */
   PSON_ALLOC_ANY
   
};

typedef enum psonAllocTypeEnum psonAllocTypeEnum;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#if defined(PSO_USE_TRACE)

#  define DO_INDENT(CONTEXT,LOOP) {                                 \
   int ijk_loop_kji;                                                \
   for( ijk_loop_kji = 0; ijk_loop_kji < (LOOP); ijk_loop_kji++ ) { \
      fprintf( (CONTEXT)->tracefp, " " );                           \
   }                                                                \
}

#  define PSO_TRACE_DUMP_API(CONTEXT,TRACES)        \
if ( ( (CONTEXT)->traceFlags & PSO_TRACE_DUMP ) &&  \
     ( (CONTEXT)->traceFlags & PSO_TRACE_API  ) ) { \
   TRACES                                           \
}

#  define PSO_TRACE_DUMP_NUCLEUS(CONTEXT,TRACES)       \
if ( ( (CONTEXT)->traceFlags & PSO_TRACE_DUMP ) &&     \
     ( (CONTEXT)->traceFlags & PSO_TRACE_NUCLEUS ) ) { \
   TRACES                                              \
}

#  define PSO_TRACE_ENTER_API(CONTEXT)                                  \
if ( ( (CONTEXT)->traceFlags & PSO_TRACE_FUNCTION ) &&                  \
     ( (CONTEXT)->traceFlags & PSO_TRACE_API ) ) {                      \
   (CONTEXT)->indent += 2;                                              \
   DO_INDENT( (CONTEXT), (CONTEXT)->indent );                           \
   fprintf( (CONTEXT)->tracefp, "Entering function %s()\n", __func__ ); \
}

#  define PSO_TRACE_ENTER_NUCLEUS(CONTEXT)                              \
if ( ( (CONTEXT)->traceFlags & PSO_TRACE_FUNCTION ) &&                  \
     ( (CONTEXT)->traceFlags & PSO_TRACE_NUCLEUS ) ) {                  \
   (CONTEXT)->indent += 2;                                              \
   DO_INDENT( (CONTEXT), (CONTEXT)->indent );                           \
   fprintf( (CONTEXT)->tracefp, "Entering function %s()\n", __func__ ); \
}

#  define PSO_TRACE_EXIT_API(CONTEXT,OK)                                   \
if ( ( (CONTEXT)->traceFlags & PSO_TRACE_FUNCTION ) &&                     \
     ( (CONTEXT)->traceFlags & PSO_TRACE_API ) ) {                         \
   DO_INDENT( (CONTEXT), (CONTEXT)->indent );                              \
   if ( OK ) {                                                             \
      fprintf( (CONTEXT)->tracefp, "Exiting  function %s()\n", __func__ ); \
   }                                                                       \
   else {                                                                  \
      fprintf( (CONTEXT)->tracefp, "Exiting function %s() on error\n",     \
         __func__ );                                                       \
   }                                                                       \
   (CONTEXT)->indent -= 2;                                                 \
}

#  define PSO_TRACE_EXIT_NUCLEUS(CONTEXT,OK)                               \
if ( ( (CONTEXT)->traceFlags & PSO_TRACE_FUNCTION ) &&                     \
     ( (CONTEXT)->traceFlags & PSO_TRACE_NUCLEUS ) ) {                     \
   DO_INDENT( (CONTEXT), (CONTEXT)->indent );                              \
   if ( OK ) {                                                             \
      fprintf( (CONTEXT)->tracefp, "Exiting  function %s()\n", __func__ ); \
   }                                                                       \
   else {                                                                  \
      fprintf( (CONTEXT)->tracefp, "Exiting function %s() on error\n",     \
         __func__ );                                                       \
   }                                                                       \
   (CONTEXT)->indent -= 2;                                                 \
}

#else

#  define DO_INDENT(CONTEXT,LOOP)
#  define PSO_TRACE_DUMP_API(CONTEXT,TRACES)
#  define PSO_TRACE_DUMP_NUCLEUS(CONTEXT,TRACES)
#  define PSO_TRACE_ENTER_API(CONTEXT)
#  define PSO_TRACE_ENTER_NUCLEUS(CONTEXT)
#  define PSO_TRACE_EXIT_API(CONTEXT,OK)
#  define PSO_TRACE_EXIT_NUCLEUS(CONTEXT,OK)

#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
   
END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSON_ENGINE_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

