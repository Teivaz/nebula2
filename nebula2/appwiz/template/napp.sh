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
#include "application/napplication.h"

class %(appStateName)s;

//-----------------------------------------------------------------------------
class %(className)s : public nApplication
{
public:
    %(className)s();
    virtual ~%(className)s();

    // @name nApplication override functions
    // @{
    virtual bool Open();
    virtual void Close();    
    // @}

protected:
    /// create script server object, override in subclass as needed
    virtual nScriptServer* CreateScriptServer();

protected:
    /// app state
    nRef<%(appStateName)s>    %(appStateObjName)s;

};
//------------------------------------------------------------------------------
#endif // %(classPrefixU)s_%(classNameNoPrefixU)s_H 
