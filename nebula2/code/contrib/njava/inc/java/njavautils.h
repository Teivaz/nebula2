#ifndef N_JAVAUTILS_H
#define N_JAVAUTILS_H
//------------------------------------------------------------------------------
/**
    @brief Some utility functions for data conversion for nJava.

    This file is licensed under the terms of the Nebula License.
    (C) 2003 Bruce Mitchener, Jr.
*/
//------------------------------------------------------------------------------
#include <jni.h>

class nArg;
class nRoot;

void quickInit(void);
nRoot * unwrapJavaObject(JNIEnv *, jobject);
jobject wrapNebulaObject(JNIEnv *, nRoot *);
jobjectArray wrapNebulaArgList(JNIEnv *, nArg *);
jobject getJavaObjectFromArg(JNIEnv *, nArg *);
void setArgFromJavaString(JNIEnv *, nArg *, jstring);

#endif
