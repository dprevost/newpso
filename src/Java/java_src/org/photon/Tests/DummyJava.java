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

package org.photon.Tests;

import java.io.Serializable;

public class DummyJava implements Serializable {

   int    dummy_i;
   String dummy_str1;
   double dummy_d;
   String dummy_str2;
   
   public DummyJava( int i, String str1, double d, String str2){
      
      dummy_i = i;
      dummy_str1 = str1;
      dummy_d = d;
      dummy_str2 = str2;
   }

}


