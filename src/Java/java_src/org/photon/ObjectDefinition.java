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

package org.photon;

public class ObjectDefinition {
   
   /** Type of the object */
   int type;
   
   /**
    * Optimization feature - not implemented yet.
    * <p>
    * The expected minimum number of data records. This is used internally
    * to avoid shrinking the internal "holder" of the data beyond what is
    * needed to hold this minimum number of data records.
    */
   long minNumOfDataRecords = 0;
   
   /**
    * Optimization feature - not implemented yet
    * <p>
    * The expected minimum number of blocks. This is used internally
    * to avoid shrinking the shared-memory object beyond a certain predefined
    * minimum size. 
    * <p>
    * Potential issue: the amount of overhead used by Photon will vary;
    * some potential factors includes the type of object, the number of 
    * data records, the length of the data records (and keys, if used).
    * <p>
    * You might want to retrieve the status of the object and evaluate
    * the minimum number of blocks needed from it..
    */
   long minNumBlocks = 1;

   /**
    * Type of definition of the data fields.
    */
   int dataDefType = DefinitionType.USER_DEFINED.getType();
   
   /**
    * Definition of the data fields.
    * <p>
    * This definition can be used as a comment to describe the content
    * of the object. It can also be used to pack and unpack the items
    * (serialized objects) contain in the object container.
    * <p>
    * It can be NULL if not needed.
    */
   byte [] dataDef;

   /**
    * Definition of the data fields as a string
    * <p>
    * This definition can be used as a comment to describe the content
    * of the object. It can also be used to pack and unpack the items
    * (serialized objects) contain in the object container.
    * <p>
    * It can be NULL if not needed.
    */
   String dataDefStr;
   
   /*
    * A bit of a hack. 
    *
    * The dataDef can be either an array of bytes or a string. Or nothing.
    *
    * If zero, no dataDef. If < 0, a string. A byte[] otherwise.
    *
    */
   int dataDefLength = 0; 
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   static {
      initIDs();
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public ObjectDefinition( ObjectType     type,
                            long           minNumOfDataRecords,
                            long           minNumBlocks,
                            DefinitionType defType ) {
      this.type = type.getType();
      this.minNumOfDataRecords = minNumOfDataRecords;
      this.minNumBlocks = minNumBlocks;
      dataDefType = defType.getType();
      dataDefLength = 0;
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public ObjectDefinition( ObjectType     type,
                            long           minNumOfDataRecords,
                            long           minNumBlocks,
                            DefinitionType defType,
                            byte[]         dataDefinition ) {
      this.type = type.getType();
      this.minNumOfDataRecords = minNumOfDataRecords;
      this.minNumBlocks = minNumBlocks;
      dataDefType = defType.getType();
      dataDef = dataDefinition;
      dataDefLength = dataDef.length;
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public ObjectDefinition( ObjectType     type,
                            long           minNumOfDataRecords,
                            long           minNumBlocks,
                            DefinitionType defType,
                            String         dataDefinition ) {
      this.type = type.getType();
      this.minNumOfDataRecords = minNumOfDataRecords;
      this.minNumBlocks = minNumBlocks;
      dataDefType = defType.getType();
      dataDefStr = dataDefinition;
      dataDefLength = -1 * dataDefStr.length();
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   /*
    * This constructor is not public...
    */
   ObjectDefinition() {}

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public ObjectType getType() { return ObjectType.getEnum(type); }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public long getMinNumOfDataRecords() { return minNumOfDataRecords; }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public long getMinNumBlocks() { return minNumBlocks; }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public byte[] getDataDefBytes() { return dataDef; }
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public String getDataDefString() { return dataDefStr; }
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public DefinitionType getDataType() { 
      return DefinitionType.getEnum( dataDefType ); 
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   private static native void initIDs();

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
}
