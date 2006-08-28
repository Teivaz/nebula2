//-----------------------------------------------------------------------------
//  %(classNameL)s.h
//
//  (C)%(curYear)s %(author)s
//-----------------------------------------------------------------------------
#ifndef %(classPrefixU)s_%(classNameNoPrefixU)s_H
#define %(classPrefixU)s_%(classNameNoPrefixU)s_H
//-----------------------------------------------------------------------------
/**
    @class %(className)s
    @ingroup %(docGroup)s
    
    @brief %(appBriefDoc)s
    
*/
#include "application/app.h"

class %(appStateName)s;

//-----------------------------------------------------------------------------
class %(className)s : public Application::App
{
public:
    %(className)s();
    virtual ~%(className)s();

    // @name nApplication override functions
    // @{
    /// open application
    virtual bool Open();
    /// close the application
    virtual void Close();

    /// run the application
    virtual void Run();

    /// returns the application name (override in subclass)
    virtual nString GetAppName() const;
    /// returns the version string (override in subclass)
    virtual nString GetAppVersion() const;
    /// returns vendor name (override in subclass)
    virtual nString GetVendorName() const;
    // @}

protected:

};
//------------------------------------------------------------------------------
#endif // %(classPrefixU)s_%(classNameNoPrefixU)s_H 
