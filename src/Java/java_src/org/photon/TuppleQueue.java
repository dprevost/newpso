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
import java.lang.reflect.*;

/**
 * Queue class for the Photon library.
 */
public class TuppleQueue<O> extends BaseQueue implements Iterable<O>, Iterator<O> {

   /* For iterations */
   O dataBuffer;

   private boolean nextWasQueried = false;
   private boolean endIteration = true;

   Constructor<O> ctr;
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public TuppleQueue( Constructor<O> ctr ) {
//      super();

      int mod = ctr.getModifiers();
      if ( ! Modifier.isPublic( mod ) ) {
         // throw an exception
         System.out.println( "Invalid modifier" );
      }
      // String newMethodDescriptor = Type.getMethodDescriptor(targetMethod);
      
      System.out.println( ctr.toGenericString() );
      System.out.println( ctr.getParameterTypes()[2] );
//      System.out.println( ctr.getTypeParameters()[2] );
//      this.ctr = ctr;
   }
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public TuppleQueue( Session session, 
                       String name,
                       Constructor<O> ctr ) throws PhotonException {
      
      super (session, name );

      int mod = ctr.getModifiers();
      if ( ! Modifier.isPublic( mod ) ) {
         // throw an exception
         System.out.println( "Invalid modifier" );
      }
      // String newMethodDescriptor = Type.getMethodDescriptor(targetMethod);
      
      System.out.println( ctr.toGenericString() );

//      this.ctr = validateConstructor( ctr );
      
      
//      if ( serializer.getType() != definition.getDataType() ) {
//         close();
//         throw new PhotonException( PhotonErrors.INVALID_DATA_DEFINITION_TYPE );
//      }
//      System.out.println( definition.dataDefType );
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public TuppleQueue( Session session, 
                       String name,
                       Constructor<O> ctrs[] ) throws PhotonException {
      
      super (session, name );

      this.ctr = findConstructor( ctrs );
      
//      if ( serializer.getType() != definition.getDataType() ) {
//         close();
//         throw new PhotonException( PhotonErrors.INVALID_DATA_DEFINITION_TYPE );
//      }
//      System.out.println( definition.dataDefType );
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   // We cannot implement hasNext directly. What we do instead is to
   // get the next item into this.entry and set nextWasQueried to true
   public boolean hasNext() {
      // In case hasNext is called twice without a call to next()
      if ( nextWasQueried ) { return true; }
      
      try {
         nextWasQueried = getNextRecord();
      } catch (Exception e) {
         return false;
      }
      
      return nextWasQueried;
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public O next() {
      
      nextWasQueried = false;
      
      if ( nextWasQueried ) { return dataBuffer; }
      
      try {
         if ( getNextRecord() ) {
            return dataBuffer;
         }
      } catch (Exception e) { e.printStackTrace(); }

      throw new NoSuchElementException();
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   boolean getNextRecord() throws PhotonException, Exception {
      
      O buffer = null;
      
      if ( handle == 0 ) {
         throw new PhotonException( PhotonErrors.NULL_HANDLE );
      }

      if ( endIteration ) {
         endIteration = false;
         buffer = getFirstTupple( handle );
      } else {
         buffer = getNextTupple( handle );
      }
      if ( myerrcode == 0 ) {
         return true;
      }
      System.out.printf(" myerrcode = " + myerrcode );
      
      endIteration = true;
      if ( myerrcode == PhotonErrors.IS_EMPTY.getErrorNumber() || 
           myerrcode == PhotonErrors.REACHED_THE_END.getErrorNumber() ) {
         return false;
      }
      
      throw new PhotonException( PhotonErrors.getEnum(myerrcode) );
   }
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   /** This method implements the Iterable interface */
   public Iterator<O> iterator() {
      return this;
   }
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public O pop() throws PhotonException {
      
      O obj;
      
      if ( handle == 0 ) {
         throw new PhotonException( PhotonErrors.NULL_HANDLE );
      }

      myerrcode = 0;
      obj = popTupple( handle );
      if ( myerrcode == 0 ) return obj;

      throw new PhotonException( PhotonErrors.getEnum(myerrcode) );
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public void push( Object... arguments ) throws PhotonException {

      int errcode;
      
      if ( handle == 0 ) {
         throw new PhotonException( PhotonErrors.NULL_HANDLE );
      }

      errcode = pushTupple( handle, arguments );
      if ( errcode == 0 ) return;
      
      throw new PhotonException( PhotonErrors.getEnum(errcode) );
   }
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public void pushNow( Object... arguments ) throws PhotonException {

      int errcode;
      
      if ( handle == 0 ) {
         throw new PhotonException( PhotonErrors.NULL_HANDLE );
      }

      errcode = pushNowTupple( handle, arguments );
      if ( errcode == 0 ) return;

      throw new PhotonException( PhotonErrors.getEnum(errcode) );
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public void remove() {
      throw new UnsupportedOperationException();
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   private native Constructor<O> findConstructor( Constructor<O> ctrs[] );

   private native O getFirstTupple( long handle );

   private native O getNextTupple( long handle );
   
   private native O popTupple( long handle );

   private native int pushTupple( long handle, Object... arguments );

   private native int pushNowTupple( long handle, Object... arguments );

   private native Constructor<O> validateConstructor( Constructor<O> ctr );

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
}

