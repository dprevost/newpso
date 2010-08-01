/*
 * This file was generated by the program errorParser
 * using the input file photon.xml.
 * Date: Fri Jul 30 23:12:52 2010.
 *
 * The version of the XML error file is 0.5.
 *
 * Copyright (C) 2006-2008 Daniel Prevost
 *
 * This file is part of Photon (photonsoftware.org).
 *
 * This file may be distributed and/or modified under the terms of the 
 * GNU General Public License version 2 or version 3 as published by 
 * the Free Software Foundation and appearing in the file COPYING.GPL2 
 * and COPYING.GPL3 included in the packaging of this software.
 *
 * Licensees holding a valid Photon Commercial license can use this 
 * file in accordance with the terms of their license.
 *
 * This software is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#ifndef PSOERRORHANDLER_H
#define PSOERRORHANDLER_H

#ifdef __cplusplus
extern "C" {
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#if defined(WIN32)
#  if defined(BULDING_ERROR_MESSAGE)
#    define ERROR_MESSAGE_EXPORT __declspec ( dllexport )
#  else
#    define ERROR_MESSAGE_EXPORT __declspec ( dllimport )
#  endif
#else
#  define ERROR_MESSAGE_EXPORT
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Use this function to access the error messages (defined in the xml
 * input file).
 * 
 * Parameters:
 *   - errnum    The error number
 *
 * Return values:
 *   - the error message if errnum is valid (exists)
 *   - NULL otherwise
 */
ERROR_MESSAGE_EXPORT
const char * pson_ErrorMessage( int errnum );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#ifdef __cplusplus
}
#endif

#endif /* PSOERRORHANDLER_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

