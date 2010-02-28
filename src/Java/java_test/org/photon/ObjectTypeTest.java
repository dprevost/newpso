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

import org.junit.*;
import static org.junit.Assert.*;

import java.util.*;

public class ObjectTypeTest {

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   static {
      System.loadLibrary("photon_jni");
      System.loadLibrary("photon_jni_test");
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   @Test
   public void testFolder() {
      ObjectType obj = ObjectType.FOLDER;
      assertTrue(obj.getType() == getFolder());
   }
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   @Test
   public void testHashMap() {
      ObjectType obj = ObjectType.HASH_MAP;
      assertTrue(obj.getType() == getHashMap());
   }
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   @Test
   public void testFastMap() {
      ObjectType obj = ObjectType.FAST_MAP;
      assertTrue(obj.getType() == getFastMap());
   }
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   @Test
   public void testLifo() {
      ObjectType obj = ObjectType.LIFO;
      assertTrue(obj.getType() == getLifo());
   }
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   @Test
   public void testQueue() {
      ObjectType obj = ObjectType.QUEUE;
      assertTrue(obj.getType() == getQueue());
   }
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public static void main(String args[]) {
      org.junit.runner.JUnitCore.main("org.photon.ObjectTypeTest");
    
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   private native int getFolder();
   private native int getHashMap();
   private native int getFastMap();
   private native int getLifo();
   private native int getQueue();

}
