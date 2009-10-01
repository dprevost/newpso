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

import org.photon.*;
import org.photon.serializer.*;

/*
This could be used to implement a "home-made" serializer ??? 
   class myStruct {
      Integer i;
      String  str;
      Object[] a;
      
      myStruct( int i, String s ) {
         this.i = i;
         this.str = s;
         this.a = new Object[2];
         a[0] = this.i;
         a[1] = this.str;
      }
      
      public byte[] packObject() {
         byte[]x = new byte[10];
         return x;
      }
   
      public Object unpackObject( byte[] buffer ) { return buffer; }
   }
*/

public class QueueTest {

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public static void prepareTests( org.photon.Session session ) throws Exception {

      byte[] q = new byte[1];
      
      ObjectDefinition definition = new ObjectDefinition( ObjectType.FOLDER, 0, 0 );
      DataDefinition   dataDef = new DataDefinition( session,
         "QueueTest1", DefinitionType.USER_DEFINED, q );
      
      session.createQueue( "/JavaTestQueue1", definition, dataDef );
      
   }
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public static void test1( org.photon.Session session ) throws Exception {
      
      Queue<DummyJava, JavaSerializer<DummyJava>> queue = 
         new Queue<DummyJava, JavaSerializer<DummyJava>>();
      
      DummyJava dummy1 = new DummyJava( 123, "A little test", 99.56, "And another string to end it" );

      try {
         queue.push( dummy1 );
      } catch ( PhotonException e ) {
         System.out.println( e.getMessage() );
         if ( e.getErrorCode() != PhotonErrors.NULL_HANDLE ) {
            throw e;
         }
      }
      
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public static void test2( org.photon.Session session ) throws PhotonException {
      
//      JavaSerializer
      org.photon.Folder folder = new org.photon.Folder( session, "/" );
   }
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public static void createFolders( org.photon.Session session ) throws PhotonException {

      org.photon.Folder folder = new org.photon.Folder( session, "/" );
      
//      folder.createObject( "java1",
//                           new ObjectDefinition(ObjectType.FOLDER, 0),
//                           null,
//                           null );
//      folder.createObject( "java2",
//                           new ObjectDefinition(ObjectType.FOLDER, 0),
//                           null,
//                           null );
//      folder.createObject( "java3",
//                           new ObjectDefinition(ObjectType.FOLDER, 0),
//                           null,
//                           null );
      for (FolderEntry entry : folder) {
         System.out.println( "Name: " + entry.getName() + ", Type: " 
            + entry.getType() + " or " + entry.getType().getText() );
      }
   }

}
