//------------------------------------------------------------------------------
//  njavautils.cc
//  This file is licensed under the terms of the Nebula License.
//  (C) 2003 Bruce Mitchener, Jr.
//------------------------------------------------------------------------------
#include "java/njavautils.h"
#include "kernel/nkernelserver.h"

void
quickInit(void)
{
    // create minimal Nebula runtime
    nKernelServer* kernelServer = new nKernelServer;
}

nRoot *
unwrapJavaObject(JNIEnv * env, jobject obj)
{
    jclass jcls = env->GetObjectClass(obj);
    jfieldID fieldID;
    jstring jNOHName;
    const char *nohName;
    nKernelServer * kernelServer = nKernelServer::ks;
    n_assert(kernelServer);
    nRoot * nobj;

    fieldID = env->GetFieldID(jcls, "nohName", "Ljava/lang/String;");
    if (0 == fieldID)
    {
        // XXX: Error out here
        return NULL;
    }
    jNOHName = (jstring)env->GetObjectField(obj, fieldID);
    nohName = env->GetStringUTFChars(jNOHName, 0);
    nobj = kernelServer->Lookup(nohName);
    env->ReleaseStringUTFChars(jNOHName, nohName);
    return nobj;
}

jobject
wrapNebulaObject(JNIEnv * env, nRoot * nobj)
{
    jobject result = NULL;
    if (NULL == nobj)
    {
        return NULL;
    }
    char jclsName[256];
    sprintf(jclsName, "org/cubik/nebuladevice/%s", nobj->GetClass()->GetName());
    jclass jcls = env->FindClass(jclsName);
    if (0 == jcls)
    {
        return NULL;
    }
    result = env->AllocObject(jcls);
    jfieldID nameField = env->GetFieldID(jcls, "nohName", "Ljava/lang/String;");
    if (0 == nameField)
    {
        return NULL;
    }
    char buf[N_MAXPATH];
    nobj->GetFullName(buf, sizeof(buf));
    jstring nohName = env->NewStringUTF(buf);
    env->SetObjectField(result, nameField, nohName);
    return result;
}

jobjectArray
wrapNebulaArgList(JNIEnv * env, nArg * list)
{
    jobjectArray result;
    nArg * listContents;

    jclass objectCls = env->FindClass("java/lang/Object");
    int listLen = list->GetL(listContents);
    result = env->NewObjectArray(listLen, objectCls, 0); 
    for(int i = 0; i < listLen; i++)
    {
        jobject value = getJavaObjectFromArg(env, &listContents[i]);
        env->SetObjectArrayElement(result, i, value);
    }
    return result;
}

#define convertHelper(resultClassName, signature, setter, theValue) \
{                                                             \
    jclass jcls = env->FindClass(resultClassName);            \
    if (0 == jcls)                                            \
    {                                                         \
        result = NULL;                                        \
    }                                                         \
    result = env->AllocObject(jcls);                          \
    jfieldID valueField;                                      \
    valueField = env->GetFieldID(jcls, "value", signature);   \
    if (0 == valueField)                                      \
    {                                                         \
        result = NULL;                                        \
    }                                                         \
    env->##setter(result, valueField, theValue);              \
}

jobject
getJavaObjectFromArg(JNIEnv * env, nArg * value)
{
    jobject result;

    switch (value->GetType())
    {
        case nArg::ARGTYPE_INT:
            convertHelper("L/java/lang/Integer;", "I", SetIntField, value->GetI())
            break;
        case nArg::ARGTYPE_FLOAT:
            convertHelper("L/java/lang/Float;", "F", SetFloatField, value->GetF())
            break;
        case nArg::ARGTYPE_STRING:
            convertHelper("L/java/lang/String;", "L/java/langString;", SetObjectField, (jstring)env->NewStringUTF(value->GetS()))
            break;
        case nArg::ARGTYPE_CODE:
            n_error("We don't support ARGTYPE_CODE");
            break;
        case nArg::ARGTYPE_BOOL:
            convertHelper("L/java/lang/Boolean;", "Z", SetBooleanField, value->GetB())
            break;
        case nArg::ARGTYPE_OBJECT:
            result = wrapNebulaObject(env, (nRoot*)value->GetO());
            break;
        case nArg::ARGTYPE_VOID:
            // No op .. shouldn't get here
            break;
        case nArg::ARGTYPE_LIST:
            result = wrapNebulaArgList(env, value);
            break;
    }

    return result;
}

void
setArgFromJavaString(JNIEnv * env, nArg * value, jstring stringObject)
{
    const char * stringValue = env->GetStringUTFChars(stringObject, 0);
    value->SetS(stringValue);
    env->ReleaseStringUTFChars(stringObject, stringValue);
}

