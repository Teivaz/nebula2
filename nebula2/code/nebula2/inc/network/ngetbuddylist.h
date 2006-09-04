#ifndef N_GETBUDDYLIST_H
#define N_GETBUDDYLIST_H
//------------------------------------------------------------------------------
/**
    @class nGetBuddylist
    @ingroup Network

    getbuddylist command for buddyclient

    (C) 2006 RadonLabs GmbH
*/
#include "kernel/nroot.h"
#include "kernel/nautoref.h"
#include "network/ncommand.h"


//------------------------------------------------------------------------------
class nGetBuddylist : public nCommand
{
public:
    /// constructor
    nGetBuddylist();
    /// destructor
   ~nGetBuddylist();

   bool Execute();
   void EvaluateResult(nStream& result);

   nArray<nString>& GetBuddyList();

private:

   nArray<nString> BuddyList;

};



//------------------------------------------------------------------------------
/**
*/
inline
nArray<nString>&
nGetBuddylist::GetBuddyList()
{
    return this->BuddyList;
}



#endif