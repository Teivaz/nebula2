#ifndef N_DOTNETLANGUAGEWRAPPER_H
#define N_DOTNETLANGUAGEWRAPPER_H
//------------------------------------------------------------------------------
/**
    @class nDotNetLanguageWrapper

    This file is licensed under the terms of the Nebula License.
    (C) 2003 Alton Goerby aka PerfectCell.
*/

#ifndef N_LANGUAGEWRAPPER_H
#include "wrapper/nlanguagewrapper.h"
#endif

#ifndef N_FILE_H
#include "kernel/nfile.h"
#endif

#undef N_DEFINES
#define N_DEFINES nDotNetLanguageWrapper
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class N_PUBLIC nDotNetLanguageWrapper : public nLanguageWrapper
{
public:
    /// constructor
    nDotNetLanguageWrapper();
    /// destructor
    virtual ~nDotNetLanguageWrapper();

    virtual void OpenClass(nClass *);
    virtual void CloseClass(nClass *);
    virtual void WriteCommand(nClass *, nCmdProto *);

    static nKernelServer* kernelServer;

private:
    std::vector<nString> classNames;
    nFile csharpFile;
};

//------------------------------------------------------------------------------
#endif
