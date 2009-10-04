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

public class Test {

   public static void main( String args[] ) {
      
      Session session;

      try {
         Photon.init( "10701", "Test-Java" );

         SessionTest.tests();
         
         session = new Session();

         FolderTest.test1( session );
         FolderTest.test2( session );
         FolderTest.createFolders( session );
         
         QueueTest.prepareTests( session );
         
         QueueTest.test1( session );
         QueueTest.test2( session );

         try {
            QueueTest.cleanupTests( session );
         } catch( Exception e ) {
            System.out.println("QueueTest.cleanupTests threw an exception!");
         }

      } catch ( Exception e ) {
         System.out.println("Test failed!");
         e.printStackTrace();
      }

      /* The GC might not call finalize() - safer to cleanup ourselves */
      Photon.exit();
   }
}

