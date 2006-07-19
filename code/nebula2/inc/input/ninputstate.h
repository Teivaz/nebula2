#ifndef N_INPUTSTATE_H
#define N_INPUTSTATE_H
//-------------------------------------------------------------------
/**
    @class nInputState
    @ingroup Input

    nInputState objects are named input 'slots' which are
    bound to input events (or a simple combination of ie's)
    through a user defined mapping table.
*/
//-------------------------------------------------------------------
#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifndef N_HASHNODE_H
#include "util/nhashnode.h"
#endif

#ifndef N_INPUTEVENT_H
#include "input/ninputevent.h"
#endif

//-------------------------------------------------------------------
class nInputState : public nHashNode
{
    int refCount;
    bool  btn;
    float val;

public:
    nInputState(const char* name) 
        : nHashNode(name),
          refCount(0),
          btn(false),
          val(0.0f)
    {
    };
    ~nInputState() {
        // n_printf("~nInputState(%s)\n",this->GetName());
    };
    void AddRef(void) {
        this->refCount++;
    };
    void Release(void) {
        n_assert(this->refCount > 0);
        this->refCount--;
    };
    int GetRefCount(void) {
        return this->refCount;
    };
    void SetButton(bool b) {
        this->btn = b;
    };
    void SetSlider(float f) {
        this->val = f;
    };
    void AddSlider(float f) {
        this->val += f;
        if (this->val > 1.0f)      this->val = 1.0f;
        else if (this->val < 0.0f) this->val = 0.0f;
    };
    bool GetButton(void) {
        return this->btn;
    };
    float GetSlider(void) {
        return this->val;
    };
    /// clear input state
    void Clear()
    {
        this->btn = false;
        this->val = 0.0f;
    };
};

//-------------------------------------------------------------------
#endif
