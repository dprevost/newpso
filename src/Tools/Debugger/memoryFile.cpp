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

#include "Tools/Debugger/memoryFile.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

MyMemoryFile::MyMemoryFile( const char * filename )
   : m_address ( NULL )
{
   memset( &m_memFile, 0, sizeof m_memFile );
   
   psocInitMemoryFile( &m_memFile, 1, filename );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

MyMemoryFile::~MyMemoryFile()
{
   psocFiniMemoryFile( &m_memFile );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool MyMemoryFile::Open()
{
   psocMemoryFileStatus status;
   psocErrorHandler errorHandler;

   psocInitErrorHandler( &errorHandler );
   psocBackStoreStatus( &m_memFile, &status );

   if ( ! status.fileExist || status.actualLLength == 0 ) {
//      fprintf( stderr, "
      return false;
   }
   m_memFile.length = status.actualLLength;
   
   if ( ! psocOpenMemFile( &m_memFile, &m_address, &errorHandler ) ) return false;

   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

