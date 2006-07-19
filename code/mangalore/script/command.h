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
    /// parse arguments from command line args object execute the Command
    virtual bool Execute(const Util::CmdLineArgs& args);
};

} // namespace Script
//------------------------------------------------------------------------------
#endif
