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
import java.util.*;
import java.lang.reflect.*;
import org.photon.*;
import org.photon.serializer.*;

public class QueueTest {

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public static void main( String args[] ) {
      
      int i;
      long before, after;
      int loop = 30000;
      double elapsedTime;

      Session session;
      JavaSerializer<Order> s;
      org.photon.Queue<Order, JavaSerializer<Order>> queue;
      TuppleQueue<Order> tuppleQueue;

      try {
         Photon.init( "10701", "Test-Java" );
         session = new Session();

         ObjectDefinition definition = new ObjectDefinition( 
            ObjectType.QUEUE, 1, 0, DefinitionType.JAVA );
      
         session.createQueue( "/JavaPerfQueue1", definition );
         session.commit();
      
         s = new JavaSerializer<Order>();
      
         queue = new org.photon.Queue<Order, JavaSerializer<Order>>( 
            session, 
            "/JavaPerfQueue1",
            s );
      
         Order order1 = new Order( "Un bidule",
                                   "Mon magasin",
                                   2,
                                   (float)56.35,
                                   12345,
                                   "And another string as a comment" );
         for ( i = 0; i < loop; ++i )
            queue.push( order1 );

         session.commit();
  
         System.gc();
         before = System.currentTimeMillis();
         for ( i = 0; i < loop; ++ i ) {
            order1 = queue.next();
         }
         after = System.currentTimeMillis();

         elapsedTime = (double)(after-before) / (double) (loop) * 1000;
         System.out.printf("queue.next, average time elapsed for each call: %.2f milliseconds\n", 
            elapsedTime );

         queue.close();

         Constructor<Order> ctr = Order.class.getConstructor( String.class,
                                                              String.class,
                                                              int.class,
                                                              float.class,
                                                              long.class,
                                                              String.class );
 
         tuppleQueue = new TuppleQueue<Order>( ctr );

         session.destroyObject( "/JavaPerfQueue1" );
         session.commit();

      }
      catch ( Exception e ) {
         System.out.println("Test failed!");
         e.printStackTrace();
      }

      Photon.exit();
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
};

