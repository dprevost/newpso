/*
 * Copyright (C) 2009-2010 Daniel Prevost <dprevost@photonsoftware.org>
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

package org.photon;

import java.lang.*;
import java.util.*;

public class KeyDefinition {

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   /**
    * Type of definition of the key fields.
    */
   int type = DefinitionType.USER_DEFINED.getType();

   /**
    * Definition of the key fields.
    * <p>
    * This definition can be used as a comment to describe the content
    * of the object. It can also be used to pack and unpack the items
    * (serialized objects) contain in the object container.
    * <p>
    * It can be NULL if not needed.
    */
   byte [] keyDef;

   /**
    * Definition of the key fields as a string
    * <p>
    * This definition can be used as a comment to describe the content
    * of the object. It can also be used to pack and unpack the items
    * (serialized objects) contain in the object container.
    * <p>
    * It can be NULL if not needed.
    */
   String keyDefStr;
   
   /*
    * A bit of a hack. 
    *
    * The definition can be either an array of bytes or a string. Or nothing.
    *
    * If zero, no dataDef. If < 0, a string. A byte[] otherwise.
    *
    */
   int keyDefLength = 0; 
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   static {
      initIDs();
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public KeyDefinition( DefinitionType defType ) {
      this.type = defType.getType();
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public KeyDefinition( DefinitionType defType,
                         byte[]         keyDefinition ) {
      
      this.type = defType.getType();
      keyDef = keyDefinition;
      keyDefLength = keyDef.length;
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public KeyDefinition( DefinitionType defType,
                         String         keyDefinition ) {

      this.type = defType.getType();
      keyDefStr = keyDefinition;
      keyDefLength = -keyDefStr.length();
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   /**
    * Default constructor. It is not public.
    */
   KeyDefinition() {}
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public byte[] getDefBytes() { return keyDef; }
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public String getDefString() { return keyDefStr; }
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public DefinitionType getType() throws PhotonException {

      return DefinitionType.getEnum(type);
   }
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   private static native void initIDs();

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
}
