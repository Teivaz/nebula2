#ifndef EMITTER_H
#define EMITTER_H
#include "kernel/nroot.h"
#include "signals/nsignalnative.h"

class Emitter : public nRoot
{
public:

    NSIGNAL_DECLARE( 'SN01', void, Trigger, 0, (), 0, () );
    NSIGNAL_DECLARE( 'SN02', void, Trigger2, 1, (float), 0, () );
    NSIGNAL_DECLARE( 'SN03', void, OnInt, 1, (int), 0, () );
    NSIGNAL_DECLARE( 'SN05', int , GetKey, 0, (), 0, () );

private:
};
#endif//EMITTER_H
