#ifndef N_WIN32PREFSERVER_H
#define N_WIN32PREFSERVER_H
//------------------------------------------------------------------------------
/**
    @class nWin32PrefServer
    @ingroup Misc

    @brief The Win32 version of nPrefServer. Uses the Windows registry to store
    persistent information under:

    HKCU/Software/[company]/[appname]/prefs

    (C) 2004 RadonLabs GmbH
*/
#include "misc/nprefserver.h"

#include <winreg.h>

//------------------------------------------------------------------------------
class nWin32PrefServer : public nPrefServer
{
public:
    /// constructor
    nWin32PrefServer();
    /// destructor
    virtual ~nWin32PrefServer();
    /// open the preferences server
    virtual bool Open();
    /// close the preferences server
    virtual void Close();
    /// check if a key exists
    virtual bool KeyExists(const nString& key);

protected:
    /// write a generic value in string form
    virtual bool WriteGeneric(const nString& key, const nString& value);
    /// read a generic value in string form
    virtual nString ReadGeneric(const nString& key);

    HKEY rootKey;
};
//------------------------------------------------------------------------------
#endif
