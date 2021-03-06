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

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

/**
 * Sessions exist mainly to support multi-threaded (MT) programs in C,
 * C++, Java, C#, Python, etc.
 * <p>
 * Each thread of an MT program should have its own session, making it
 * independent of the other threads. You must not share Photon objects 
 * across threads.
 * <p>
 * If you need to access the same Photon object in multiple threads,
 * simply create multiple instances of it, one for each session/thread
 * of interest.
 * <p>
 * Explanation: Photon Java objects are just proxies for the real 
 * objects sitting in shared memory. Proper synchronization is already done 
 * in shared memory and it is best to avoid the additional synchronization 
 * of these proxy objects.
 */

public class Session {

   /* To save the native pointer/handle of the C struct. */
   long handle = 0;
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   static {
      initIDs();
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
   
   /**
    * This function initializes a Photon session. 
    * <p>
    * This function will also initiate a new transaction.
    * <p>
    * Upon normal termination, the current transaction is rolled back. You
    * MUST explicitly call commit to save your changes.
    *
    * @exception PhotonException On an error with the Photon library.
    */
   public Session() throws PhotonException {

      int errcode;
      
      errcode = psoInit();
      if ( errcode != 0 ) {
         throw new PhotonException( PhotonErrors.getEnum(errcode) );
      }
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   /**
    * Terminate the current session. 
    * 
    * An implicit call to rollback is executed by this method. You must
    * call commit to save any pending operations.
    */
   public void close() { 
      psoFini(handle); 
      handle = 0;
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   /**
    * Commit all insertions and deletions (of the current session) executed 
    * since the previous call to commit or rollback.
    * <p>
    * Insertions and deletions subjected to this call include both data items
    * inserted and deleted from data containers (maps, etc.) and objects 
    * themselves created with createObject and/or destroyed with 
    * destroyObject.
    * <p>
    * Note: the internal calls executed by the engine to satisfy this 
    * request cannot fail. As such, you CANNOT find yourself with an 
    * ugly situation where some operations were committed and others 
    * not. If this methos throws an exception, nothing was committed.
    *
    * @exception PhotonException On an error with the Photon library.
    */
   public void commit() throws PhotonException {
      
      int errcode;
      
      if ( handle == 0 ) {
         throw new PhotonException( PhotonErrors.NULL_HANDLE );
      }

      errcode = psoCommit( handle );
      if ( errcode != 0 ) {
         throw new PhotonException( PhotonErrors.getEnum(errcode) );
      }
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   /**
    * Create a new folder in shared memory.
    * <p>
    * The creation of the object only becomes permanent after a call to 
    * commit.
    * <p>
    * This function does not provide a handle to the newly created object. 
    * You must use org.photon.Folder.open() to access the newly created object.
    *
    * @param folderName The fully qualified name of the object.
    *
    * @exception PhotonException On an error with the Photon library.
    */
   public void createFolder( String folderName ) throws PhotonException {

      int errcode;
      
      if ( handle == 0 ) {
         throw new PhotonException( PhotonErrors.NULL_HANDLE );
      }

      errcode = psoCreateFolder( handle, folderName );
      if ( errcode != 0 ) {
         throw new PhotonException( PhotonErrors.getEnum(errcode) );
      }
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   /**
    * Create a new queue (LIFO, FIFO) in shared memory.
    * <p>
    * The creation of the object only becomes permanent after a call to 
    * Session.commit.
    * <p>
    * This function does not return a Java object linked to the newly 
    * created object. You must use org.photon.Queue.open and similar to 
    * access the newly created object.
    *
    * @param objectName The fully qualified name of the queue.
    * @param definition The object definition (its type, etc.).
    * @exception PhotonException On an error with the Photon library.
    */
   public void createQueue( String           objectName,
                            ObjectDefinition definition ) throws PhotonException {
      int errcode;
      
      if ( handle == 0 ) {
         throw new PhotonException( PhotonErrors.NULL_HANDLE );
      }

      errcode = psoCreateQueue( handle, 
                                objectName, 
                                definition );
      if ( errcode != 0 ) {
         throw new PhotonException( PhotonErrors.getEnum(errcode) );
      }
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   /**
    * Create a new map in shared memory.
    * <p>
    * The creation of the object only becomes permanent after a call to 
    * Session.commit.
    * <p>
    * This function does not return a Java object linked to the newly 
    * created object. You must use org.photon.Hashmap and similar to 
    * access the newly created object.
    *
    * @param objectName The fully qualified name of the object.
    * @param definition The object definition (its type, etc.)
    * @param keyDef     The definition of the key.
    * @exception PhotonException On an error with the Photon library.
    */
   public void createMap( String           objectName,
                          ObjectDefinition definition, 
                          KeyDefinition    keyDef ) throws PhotonException {
      int errcode;
      
      if ( handle == 0 ) {
         throw new PhotonException( PhotonErrors.NULL_HANDLE );
      }

      errcode = psoCreateMap( handle, 
                              objectName, 
                              definition,
                              keyDef );
      if ( errcode != 0 ) {
         throw new PhotonException( PhotonErrors.getEnum(errcode) );
      }
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   /**
    * Destroy an existing object in shared memory.
    * <p>
    * The destruction of the object only becomes permanent after a call to 
    * commit.
    *
    * @param objectName The fully qualified name of the object. 
    *
    * @exception PhotonException On an error with the Photon library.
    */
   public void destroyObject( String objectName ) throws PhotonException {
      
      int errcode;
      
      if ( handle == 0 ) {
         throw new PhotonException( PhotonErrors.NULL_HANDLE );
      }

      errcode = psoDestroyObject( handle, objectName );
      if ( errcode != 0 ) {
         throw new PhotonException( PhotonErrors.getEnum(errcode) );
      }
   }
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   protected void finalize() throws Throwable {     
      
      try {
         psoFini(handle);
      } finally {
         handle = 0;
         super.finalize();
      }
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   /**
    * Retrieve the basic definition of the named object.
    *
    * @param objectName The fully qualified name of the object.
    *
    * @return A new ObjectDefinition object for the named Photon object.
    *
    * @exception PhotonException On an error with the Photon library.
    */
   public ObjectDefinition getDefinition( String objectName ) throws PhotonException {

      int errcode;
      ObjectDefinition definition = new ObjectDefinition();
      
      if ( handle == 0 ) {
         throw new PhotonException( PhotonErrors.NULL_HANDLE );
      }

      errcode = psoGetDefinition( handle, objectName, definition );
      if ( errcode != 0 ) {
         throw new PhotonException( PhotonErrors.getEnum(errcode) );
      }

      return definition;
   }
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   /**
    * Retrieve the key definition of the named object.
    *
    * @param objectName The fully qualified name of the object. 
    * 
    * @return A new KeyDefinition object for the named Photon object.
    *
    * @exception PhotonException On an error with the Photon library.
    */
   public KeyDefinition getKeyDefinition( String objectName ) throws PhotonException {

      int errcode;
      KeyDefinition definition = new KeyDefinition();
      
      if ( handle == 0 ) {
         throw new PhotonException( PhotonErrors.NULL_HANDLE );
      }

      errcode = psoGetKeyDefinition( handle, objectName, definition );
      if ( errcode != 0 ) {
         throw new PhotonException( PhotonErrors.getEnum(errcode) );
      }

      return definition;
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   /**
    * Return information on the current status of the shared memory.
    * <p>
    * The fetched information is mainly about the current status of the memory 
    * allocator.
    *
    * @return A new info object.
    * @exception PhotonException On an error with the Photon library.
    */
   public Info getInfo() throws PhotonException {
      
      int errcode;
      Info info = new Info();
      
      if ( handle == 0 ) {
         throw new PhotonException( PhotonErrors.NULL_HANDLE );
      }

      errcode = psoGetInfo( handle, info );
      if ( errcode != 0 ) {
         throw new PhotonException( PhotonErrors.getEnum(errcode) );
      }

      return info;
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   /**
    * Return the status of the named object.
    *
    * @param objectName The fully qualified name of the object. 
    * @return A new status object.
    * @exception PhotonException On an error with the Photon library.
    */
   public ObjectStatus getStatus( String objectName ) throws PhotonException {
      
      int errcode;
      ObjectStatus status = new ObjectStatus();
      
      if ( handle == 0 ) {
         throw new PhotonException( PhotonErrors.NULL_HANDLE );
      }

      errcode = psoGetStatus( handle, objectName, status );
      if ( errcode != 0 ) {
         throw new PhotonException( PhotonErrors.getEnum(errcode) );
      }

      return status;
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   /**
    * Rollback all insertions and deletions (of the current session) executed 
    * since the previous call to commit or rollback.
    * <p>
    * Insertions and deletions subjected to this call include both data items
    * inserted and deleted from data containers (maps, etc.) and objects 
    * themselves created with createObject and/or destroyed with 
    * destroyObject.
    * <p>
    * Note: the internal calls executed by the engine to satisfy this 
    * request cannot fail. As such, you cannot find yourself with an 
    * ugly situation where some operations were rollbacked and others 
    * not. If an exception is thrown by this function, nothing was 
    * rollbacked.
    *
    * @exception PhotonException On an error with the Photon library.
    */
   public void rollback() throws PhotonException {
      
      int errcode;
      
      if ( handle == 0 ) {
         throw new PhotonException( PhotonErrors.NULL_HANDLE );
      }

      errcode = psoRollback( handle );
      if ( errcode != 0 ) {
         throw new PhotonException( PhotonErrors.getEnum(errcode) );
      }
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   private static native void initIDs();

   private native int psoCommit( long handle );

   private native int psoCreateFolder(
      long handle, String objectName );

   private native int psoCreateQueue(
      long             handle,
      String           objectName,
      ObjectDefinition definition );

   private native int psoCreateMap(
      long             handle,
      String           objectName,
      ObjectDefinition definition, 
      KeyDefinition    keyDef );

   private native int psoDestroyObject( long handle, String objectName );

   private native int psoFini( long handle );

   private native int psoGetDefinition( long             handle,
                                        String           objectName,
                                        ObjectDefinition objectDef );

   private native int psoGetInfo( long handle,
                                  Info info );

   private native int psoGetKeyDefinition( long          handle,
                                           String        objectName,
                                           KeyDefinition dataDef );

   private native int psoGetStatus( long      handle,
                                    String    objectName,
                                    ObjectStatus status );

   private native int psoInit();

   private native int psoRollback( long handle );

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
}

