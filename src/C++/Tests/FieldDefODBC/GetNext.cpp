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

#include "Common/Common.h"
#include <photon/photon>
#include "Tests/PrintError.h"
#include <iostream>

using namespace std;
using namespace pso;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main( int argc, char * argv[] )
{
   FieldDefinitionODBC def(2);
   string name;
   psoFieldDefinition fields[6] = {
      { "abcd1", PSO_INTEGER,   {0}  },
      { "abcd2", PSO_CHAR,      {12} },
      { "abcd3", PSO_BINARY,    {12} },
      { "abcd4", PSO_TINYINT,   {0}  },
      { "abcd5", PSO_NUMERIC,   {0}  },
      { "abcd6", PSO_VARBINARY, {20} }
   };
   
   fields[4].vals.decimal.precision = PSO_FIELD_MAX_PRECISION;
   fields[4].vals.decimal.scale = PSO_FIELD_MAX_PRECISION;
   
   FieldDefinitionODBC defOK( (unsigned char *) fields, 
      6*sizeof(psoFieldDefinition));

   name = "abcd1";
   try {
      def.AddField( name, PSO_INTEGER, 0, 0, 0 );
   }
   catch( pso::Exception exc ) {
      cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
      return 1;
   }

   name = "abcd2";
   try {
      def.AddField( name, PSO_CHAR, 12, 0, 0 );
   }
   catch( pso::Exception exc ) {
      cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
      return 1;
   }

   try {
      def.GetNext();
      // Should never come here
      cerr << "Test failed - line " << __LINE__ << endl;
      return 1;
   }
   catch( pso::Exception exc ) {
      if ( exc.ErrorCode() != PSO_INVALID_DEF_OPERATION ) {
         cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
         return 1;
      }
   }

   try {
      string s;
      do {
         s = defOK.GetNext();
         cout << s << endl;
      } while (s.length() > 0 );
   }
   catch( pso::Exception exc ) {
      cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
      return 1;
   }
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
