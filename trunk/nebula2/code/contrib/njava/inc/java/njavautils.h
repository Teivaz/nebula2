#ifndef N_JAVAUTILS_H
#define N_JAVAUTILS_H

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
