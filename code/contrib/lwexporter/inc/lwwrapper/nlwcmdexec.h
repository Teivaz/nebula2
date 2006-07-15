#ifndef N_LW_CMD_EXEC_H
#define N_LW_CMD_EXEC_H
//----------------------------------------------------------------------------
extern "C" 
{
#include <lwsdk/lwgeneric.h>
}

#include "util/nstring.h"

//----------------------------------------------------------------------------
/**
    @class nLWCmdExec
    @brief Provides the ability to execute Lightwave commands.
*/
class nLWCmdExec
{
public:
    nLWCmdExec(LWLayoutGeneric*);
    ~nLWCmdExec();

    bool ExecuteCmd(const nString& cmdStr);

private:
    LWLayoutGeneric* generic;
};

//----------------------------------------------------------------------------
/**
*/
inline
nLWCmdExec::nLWCmdExec(LWLayoutGeneric* generic) :
    generic(generic)
{
    // empty
}

//----------------------------------------------------------------------------
/**
*/
inline
nLWCmdExec::~nLWCmdExec()
{
    this->generic = 0;
}

//----------------------------------------------------------------------------
/**
    @brief Execute a command in Lightwave (Layout).
    @return true if the command was executed successfully, false otherwise.
*/
inline
bool
nLWCmdExec::ExecuteCmd(const nString& cmdStr)
{
    n_assert(this->generic);
    if (this->generic && this->generic->evaluate)
    {
        return this->generic->evaluate(this->generic->data, 
                                       cmdStr.Get()) == 1;
    }
    return false;
}

//----------------------------------------------------------------------------
#endif // N_LW_CMD_EXEC_H
