#ifndef N_JAVALANGUAGEWRAPPER_H
#define N_JAVALANGUAGEWRAPPER_H
//------------------------------------------------------------------------------
/**
    @class nJavaLanguageWrapper

    This file is licensed under the terms of the Nebula License.
    (C) 2003 Bruce Mitchener, Jr.
*/

#ifndef N_LANGUAGEWRAPPER_H
#include "wrapper/nlanguagewrapper.h"
#endif

#ifndef N_FILE_H
#include "kernel/nfile.h"
#endif

#undef N_DEFINES
#define N_DEFINES nJavaLanguageWrapper
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class N_PUBLIC nJavaLanguageWrapper : public nLanguageWrapper
{
public:
    /// constructor
    nJavaLanguageWrapper();
    /// destructor
    virtual ~nJavaLanguageWrapper();

    virtual void OpenClass(nClass *);
    virtual void CloseClass(nClass *);
    virtual void WriteCommand(nClass *, nCmdProto *);

    static nKernelServer* kernelServer;

private:
    nFile* javaFile;
    nFile* headerFile;
    nFile* implFile;
};

//------------------------------------------------------------------------------
#endif
