//----------------------------------------------------------------------------
//   %(classNameL)s_main.cc
//
//  (C)%(curYear)s %(author)s
//----------------------------------------------------------------------------
#include "%(subDirL)s/%(classNameL)s.h"
#include "%(subDirL)s/%(appStateNameL)s.h"

//----------------------------------------------------------------------------
/**
*/
%(className)s::%(className)s()
{
}

//----------------------------------------------------------------------------
/**
*/
%(className)s::~%(className)s()
{
}

//----------------------------------------------------------------------------
/**
*/
bool %(className)s::Open()
{
    if (!Application::App::Open())
    {
        return false;
    }

    //TODO: Add code for opening the application.

    // create edit state and specifies it to application.
    Ptr<%(appStateName)s> %(appStateObjName)s = n_new(%(appStateName)s);
    %(appStateObjName)s->SetName("%(appStateObjName)s");
    AddStateHandler(%(appStateObjName)s);

    this->SetState("%(appStateObjName)s");

    return true;
}

//----------------------------------------------------------------------------
/**
*/
void %(className)s::Close()
{
    //TODO: Add code for closing the application.

    Application::App::Close();
}

//----------------------------------------------------------------------------
/**
*/
void %(className)s::Run()
{
    //TODO: Add code for running the application.

    Application::App::Run();
}

//------------------------------------------------------------------------------
/**
    Override this method in subclasses to return a different application name.
*/
nString
%(className)s::GetAppName() const
{
    return "%(appName)s";
}

//------------------------------------------------------------------------------
/**
    Override this method in subclasses to return a different version string.
*/
nString
%(className)s::GetAppVersion() const
{
    return "0.1.0";
}

//------------------------------------------------------------------------------
/**
    Get the application vendor. This is usually the publishers company name.
*/
nString
%(className)s::GetVendorName() const
{
    return "%(companyName)s";
}

//----------------------------------------------------------------------------
// EOF
//----------------------------------------------------------------------------
