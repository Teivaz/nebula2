#ifndef N_JAVALANGUAGEWRAPPER_H
#define N_JAVALANGUAGEWRAPPER_H
//------------------------------------------------------------------------------
/**
    @class nJavaLanguageWrapper

    This file is licensed under the terms of the Nebula License.
    (C) 2003 Bruce Mitchener, Jr.
*/
#include "wrapper/nlanguagewrapper.h"
#include "kernel/nfile.h"

//------------------------------------------------------------------------------
class nJavaLanguageWrapper : public nLanguageWrapper
{
public:
    /// constructor
    nJavaLanguageWrapper();
    /// destructor
    virtual ~nJavaLanguageWrapper();

    virtual void OpenClass(nClass *);
    virtual void CloseClass(nClass *);
    virtual void WriteCommand(nClass *, nCmdProto *);

private:
    nFile* javaFile;
    nFile* headerFile;
    nFile* implFile;
};

//------------------------------------------------------------------------------
#endif
