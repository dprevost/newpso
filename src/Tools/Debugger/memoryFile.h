/* :mode=c++:  - For jedit, previous line for emacs */
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

#ifndef PSO_DBG_MEMORY_FILE_H
#define PSO_DBG_MEMORY_FILE_H

#include "Common/MemoryFile.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

class MyMemoryFile
{
public:
   
   MyMemoryFile( const char * filename );

   ~MyMemoryFile();

   bool Open();
   
   void * BaseAddress() { return m_address; }
   
private:

   void * m_address;

   struct psocMemoryFile m_memFile;
   
};

#if 0

/** This struct holds all data needed for accessing the memory file. */
struct psocMemoryFile
{
   /** Set to PSOC_MEMFILE_SIGNATURE at initialization. */
   unsigned int initialized;
   
   /**
    * Base address of the memory-mapped file.
    * The content of this variable is determined when the shared
    * memory segment is initialized (when mmap or the Win32 
    * equivalent is called).
    */
   void * baseAddr;

   /** Length of the mapped file in bytes. */
   size_t length;

   /** Name of the backstore file. */
   char name[PATH_MAX];
   
#if defined(_WIN32)
   /** Win32 specific - the underlying file handle (a void*). */
   HANDLE fileHandle;
#else
   /** Unix/linux specific - the underlying file handle. */
   int fileHandle;
#endif

#if defined(_WIN32)
   /** Win32 specific - the underlying map handle (a void*). */
   HANDLE mapHandle;
   
   /** Win32 specific - the security attribute of the map. */
   SECURITY_ATTRIBUTES sa;
   /** Win32 specific - the security descriptor of the map. */
   SECURITY_DESCRIPTOR sd;
#endif
   
};


/*! \brief Return the status of the backstore file. */
void psocBackStoreStatus( psocMemoryFile       * pMem,
                          psocMemoryFileStatus * pStatus );
   
/*! \brief Create the backstore file */
bool psocCreateBackstore( psocMemoryFile   * pMem,
                          int                filePerms,
                          psocErrorHandler * pError );
   
/*! 
 *  \brief "Open" an access to the memory file (this might load the 
 *  backstore in memory).
 */
bool psocOpenMemFile( psocMemoryFile   * pMem,
                      void            ** ppAddr,
                      psocErrorHandler * pError );
   
/*! \brief Create a copy of the backstore file */
bool psocCopyBackstore( psocMemoryFile   * pMem,
                        int                filePerms,
                        psocErrorHandler * pError );

/*!
 *  \brief Close our access the memory file (possibly removing the memory
 *  file itself from memory).
 */
void psocCloseMemFile( psocMemoryFile   * pMem,
                       psocErrorHandler * pError );

/*! \brief Synchronize the memory file to the backstore (disk) */
bool psocSyncMemFile( psocMemoryFile   * pMem,
                      psocErrorHandler * pError );

/*! \brief Set the shared memory to read-only. */
static inline
bool psocSetReadOnly( psocMemoryFile   * pMem,
                      psocErrorHandler * pError );

/*! \brief Set the shared memory to be writable. */
static inline
bool psocSetReadWrite( psocMemoryFile   * pMem,
                       psocErrorHandler * pError );

#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif // PSO_DBG_MEMORY_FILE_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
