/**
   @file receiverscript.h
   @author Luis Jose Cabellos Gomez <luis.cabellos@tragnarion.com>
   
   (C) 2005 Tragnarion Studios
*/
#ifndef RECEIVERSCRIPT_H
#define RECEIVERSCRIPT_H
#include "kernel/nroot.h"

//------------------------------------------------------------------------------
/**
    @class ReceiverScript
    @ingroup SceneNodes

    @brief Objects from this class will receive signals notifications.
    This class has scripting side.
*/
class ReceiverScript : public nRoot
{
public:
    ReceiverScript():number(1){}

    void Trigger();
    void OnInt( int i );
    void SetNumber( int i );

private:
    int number;
};

//------------------------------------------------------------------------------
/**
*/
inline
void 
ReceiverScript::Trigger()
{
    n_printf( "Trigger exec by ReceiverScript %d\n", this->number );
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
ReceiverScript::OnInt( int i )
{
    n_printf( "OnInt( %d ) exec by ReceiverScript %d\n", i, this->number );
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
ReceiverScript::SetNumber( int i )
{
    this->number = i;
}

#endif//RECEIVERSCRIPT_H
//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------
