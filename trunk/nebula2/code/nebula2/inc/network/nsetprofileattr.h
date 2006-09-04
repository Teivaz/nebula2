#ifndef N_SETPROFILEATTR_H
#define N_SETPROFILEATTR_H
//------------------------------------------------------------------------------
/**
    @class nSetProfileAttr
    @ingroup Network

    sets a profile attr - for buddyclient

    (C) 2006 RadonLabs GmbH
*/
#include "kernel/nroot.h"
#include "kernel/nautoref.h"
#include "network/ncommand.h"


//------------------------------------------------------------------------------
class nSetProfileAttr : public nCommand
{
public:
    /// constructor
    nSetProfileAttr();
    /// destructor
   ~nSetProfileAttr();

   bool Execute();
   void EvaluateResult(nStream& result);

   void SetProfilAttrName(nString& b);
   void SetProfilAttrValue(nString& b);

private:

   nString key;
   nString value;

};

//------------------------------------------------------------------------------
/**
*/
inline
void
nSetProfileAttr::SetProfilAttrName(nString& b)
{
    this->key = b;
}
//------------------------------------------------------------------------------
/**
*/
inline
void
nSetProfileAttr::SetProfilAttrValue(nString& b)
{
    this->value = b;
}


#endif