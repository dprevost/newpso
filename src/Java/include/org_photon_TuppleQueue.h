/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class org_photon_TuppleQueue */

#ifndef _Included_org_photon_TuppleQueue
#define _Included_org_photon_TuppleQueue
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     org_photon_TuppleQueue
 * Method:    findConstructor
 * Signature: ([Ljava/lang/reflect/Constructor;)Ljava/lang/reflect/Constructor;
 */
JNIEXPORT jobject JNICALL Java_org_photon_TuppleQueue_findConstructor
  (JNIEnv *, jobject, jobjectArray);

/*
 * Class:     org_photon_TuppleQueue
 * Method:    getFirstTupple
 * Signature: (J)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_org_photon_TuppleQueue_getFirstTupple
  (JNIEnv *, jobject, jlong);

/*
 * Class:     org_photon_TuppleQueue
 * Method:    getNextTupple
 * Signature: (J)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_org_photon_TuppleQueue_getNextTupple
  (JNIEnv *, jobject, jlong);

/*
 * Class:     org_photon_TuppleQueue
 * Method:    popTupple
 * Signature: (J)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_org_photon_TuppleQueue_popTupple
  (JNIEnv *, jobject, jlong);

/*
 * Class:     org_photon_TuppleQueue
 * Method:    pushTupple
 * Signature: (J[Ljava/lang/Object;)I
 */
JNIEXPORT jint JNICALL Java_org_photon_TuppleQueue_pushTupple
  (JNIEnv *, jobject, jlong, jobjectArray);

/*
 * Class:     org_photon_TuppleQueue
 * Method:    pushNowTupple
 * Signature: (J[Ljava/lang/Object;)I
 */
JNIEXPORT jint JNICALL Java_org_photon_TuppleQueue_pushNowTupple
  (JNIEnv *, jobject, jlong, jobjectArray);

/*
 * Class:     org_photon_TuppleQueue
 * Method:    validateConstructor
 * Signature: (Ljava/lang/reflect/Constructor;)Ljava/lang/reflect/Constructor;
 */
JNIEXPORT jobject JNICALL Java_org_photon_TuppleQueue_validateConstructor
  (JNIEnv *, jobject, jobject);

#ifdef __cplusplus
}
#endif
#endif