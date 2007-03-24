#ifndef N_ADDBUDDY_H
#define N_ADDBUDDY_H
//------------------------------------------------------------------------------
/**
    @class nAddBuddy
    @ingroup Network

    addbuddy command for buddyclient

    (C) 2006 RadonLabs GmbH
*/
#include "kernel/nroot.h"
#include "kernel/nautoref.h"
#include "network/ncommand.h"


//------------------------------------------------------------------------------
class nAddBuddy : public nCommand
{
public:
    /// constructor
    nAddBuddy();
    /// destructor
   ~nAddBuddy();

   bool Execute();
   void EvaluateResult(nStream& result);

   void SetBuddy(const nString& b);

private:

   nString buddy;

};

//------------------------------------------------------------------------------
/**
*/
inline
void
nAddBuddy::SetBuddy(const nString& b)
{
    this->buddy = b;
}


#endif