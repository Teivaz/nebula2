/**
   @file receiver.h
   @author Luis Jose Cabellos Gomez <luis.cabellos@tragnarion.com>
   
   (C) 2005 Tragnarion Studios
*/
#ifndef RECEIVER_H
#define RECEIVER_H
#include "kernel/nobject.h"

//------------------------------------------------------------------------------
/**
    @class Receiver
    @ingroup SceneNodes

    @brief Objects from this class will receive signals notifications
*/
class Receiver : public nObject
{
public:
    Receiver():number(1){ }

    void Trigger();
    void Trigger( float v );
    void OnInt( int i );
    int GetKey( void );
    void SetNumber( int i );

private:
    int number;
};

//------------------------------------------------------------------------------
/**
*/
inline
void 
Receiver::Trigger()
{
    n_printf( "Trigger exec by Receiver %d\n", this->number );
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
Receiver::Trigger( float v)
{
    n_printf( "Trigger(double %f) exec by Receiver %d\n", v, this->number );
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
Receiver::OnInt( int i )
{
    n_printf( "OnInt( %d ) exec by Receiver %d\n", i, this->number );
}

//------------------------------------------------------------------------------
/**
*/
inline
int
Receiver::GetKey( void )
{
    return 42;
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
Receiver::SetNumber( int i )
{
    this->number = i;
}

#endif//RECEIVER_H
//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------
