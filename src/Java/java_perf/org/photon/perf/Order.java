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

package org.photon.perf;

import java.lang.*;
//import java.util.*;

import java.io.Serializable;

public class Order implements Serializable {

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
   
   String  productName;
   String  storeName;
   int     numberOrdered;
   float   price;
   long    productCode;
   String  comments;
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public Order( String productName,
                 String storeName,
                 int    numberOrdered,
                 float  price,
                 long   productCode,
                 String comments ) {

      this.productName   = productName;
      this.storeName     = storeName;
      this.numberOrdered = numberOrdered;
      this.price         = price;
      this.productCode   = productCode;
      this.comments      = comments;
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
};
