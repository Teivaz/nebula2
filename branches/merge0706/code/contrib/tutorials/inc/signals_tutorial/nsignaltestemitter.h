#include "kernel/nroot.h"
#include "signals/nsignalnative.h"
#include "signals/nsignalemitter.h"

class nSignalTestEmitter : public nRoot
{
public:
    nSignalTestEmitter()
    {
        /// empty
    }

    ~nSignalTestEmitter()
    {
        /// empty
    }

    // signals
    NSIGNAL_DECLARE('SIG0', bool, Testbii, 1, (int), 0, ());
    NSIGNAL_DECLARE('SIG1', bool, Testbi2, 1, (int), 0, ());
    NSIGNAL_DECLARE('SIG2', float, Testffff, 3, (float, float, float), 0, ());
    NSIGNAL_DECLARE('SIG3', float, Testbbifs, 4, (bool, int, float, const char *), 0, ());
    NSIGNAL_DECLARE('SIG4', bool,  Testvv, 0, (), 0, ());
    NSIGNAL_DECLARE('SIG5', bool,  Testiv, 1, (int), 0, ());
    NSIGNAL_DECLARE('SIG6', bool,  Testfv, 1, (int), 0, ());
    NSIGNAL_DECLARE('SIG7', void,  Testvbbbb, 4, (bool, bool, bool, bool), 0, ());
    NSIGNAL_DECLARE('SIG8', void,  Testv3, 1, (vector3 &), 0, ());

};
