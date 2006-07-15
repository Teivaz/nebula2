#ifndef SCRIPT_COMMAND_H
#define SCRIPT_COMMAND_H
//------------------------------------------------------------------------------
/**
    @class Script::Command


    (C) 2005 Radon Labs GmbH
*/

#include "foundation/refcounted.h"
#include "util/cmdlineargs.h"

//------------------------------------------------------------------------------
namespace Script
{
class Command : public Foundation::RefCounted
{
    DeclareRtti;
	DeclareFactory(Command);

public:
    /// constructor
    Command();
    /// destructor
    virtual ~Command();
    /// get instance pointer
    static Command* Instance();
    /// parse arguments from command line args object execute the Command
    virtual int Execute(const Util::CmdLineArgs& args);
    /// get the command name
    virtual nString GetName() const;

private:
    static Command* Singleton;
};
RegisterFactory(Command);

//------------------------------------------------------------------------------
/**
*/
inline
Command*
Command::Instance()
{
    n_assert(0 != Singleton);
    return Singleton;
}


} // namespace Script
//------------------------------------------------------------------------------
#endif
