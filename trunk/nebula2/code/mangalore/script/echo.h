#ifndef SCRIPT_COMMAND_ECHO_H
#define SCRIPT_COMMAND_ECHO_H
//------------------------------------------------------------------------------
/**
    @class Script::Echo
    
    
    (C) 2005 Radon Labs GmbH
*/

#include "foundation/refcounted.h"
#include "util/cmdlineargs.h"
#include "script/command.h"

//------------------------------------------------------------------------------
namespace Script
{
class Echo : public Command
{
    DeclareRtti;
	DeclareFactory(Echo);

public:
    /// constructor
    Echo();
    /// destructor
    virtual ~Echo();
    /// get instance pointer
    static Echo* Instance();
    /// parse arguments from command line args object execute the Command
    virtual int Execute(const Util::CmdLineArgs& args);
    /// get the command name
    virtual nString GetName() const;


private:

    static Echo* Singleton;

};
RegisterFactory(Echo);

//------------------------------------------------------------------------------
/**
*/
inline
Echo*
Echo::Instance()
{
    n_assert(0 != Singleton);
    return Singleton;
}


}; // namespace Script
//------------------------------------------------------------------------------
#endif
    