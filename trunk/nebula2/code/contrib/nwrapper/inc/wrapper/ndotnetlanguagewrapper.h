#ifndef N_DOTNETLANGUAGEWRAPPER_H
#define N_DOTNETLANGUAGEWRAPPER_H
//------------------------------------------------------------------------------
/**
    @class nDotNetLanguageWrapper

    This file is licensed under the terms of the Nebula License.
    (C) 2003 Alton Goerby aka PerfectCell.
*/

#include "wrapper/nlanguagewrapper.h"

class nFile;
//------------------------------------------------------------------------------
class nDotNetLanguageWrapper : public nLanguageWrapper
{
public:
    /// constructor
    nDotNetLanguageWrapper();
    /// destructor
    virtual ~nDotNetLanguageWrapper();

    virtual void OpenClass(nClass *);
    virtual void CloseClass(nClass *);
    virtual void WriteCommand(nClass *, nCmdProto *);

private:
    std::vector<nString> classNames;
    nFile * csharpFile;
};

//------------------------------------------------------------------------------
#endif
