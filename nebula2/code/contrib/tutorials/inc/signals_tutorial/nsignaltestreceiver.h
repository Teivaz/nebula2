#include "kernel/nobject.h"
#include "signals/nsignal.h"

class nSignalTestReceiver : public nRoot
{
public:
    nSignalTestReceiver()
    {
        /// empty
    }

    ~nSignalTestReceiver()
    {
        /// empty
    }

    bool Signaledbi(int par)
    {
        n_printf("%s: %x\n", this->GetName(), par);
        return true;
    }

    float Signaledffff(float x, float y, float z)
    {
        n_printf("%s: %f %f %f\n", this->GetName(), x, y, z);
        return (x + y + z);
    }

    int Signaledibifs(bool b, int i, float f, const char * s)
    {
        n_printf("%s: %d %i %f %s", this->GetName(), b, i, f, s);
        return i;
    }

    void Signaledbbbb(bool b1, bool b2, bool b3, bool b4)
    {
        n_printf("%s: %d %d %d %d", this->GetName(), b1, b2, b3, b4);
    }
};

