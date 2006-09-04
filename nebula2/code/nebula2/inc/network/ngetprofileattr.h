#ifndef N_GETPROFILEATTR_H
#define N_GETPROFILEATTR_H
//------------------------------------------------------------------------------
/**
    @class nGetProfileAttr
    @ingroup Network

    gets a profile attr - for buddyclient

    (C) 2006 RadonLabs GmbH
*/
#include "kernel/nroot.h"
#include "kernel/nautoref.h"
#include "network/ncommand.h"


//------------------------------------------------------------------------------
class nGetProfileAttr : public nCommand
{
public:
    /// constructor
    nGetProfileAttr();
    /// destructor
   ~nGetProfileAttr();

   bool Execute();
   void EvaluateResult(nStream& result);

   void SetProfilAttrName(nString& b);

   nString GetProfilAttrValue();

private:

   nString key;
   nString value;

};

//------------------------------------------------------------------------------
/**
*/
inline
void
nGetProfileAttr::SetProfilAttrName(nString& b)
{
    this->key = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
nString
nGetProfileAttr::GetProfilAttrValue()
{
    return this->value;
}

#endif