#ifndef N_LW_LOAD_STATE_H
#define N_LW_LOAD_STATE_H
//----------------------------------------------------------------------------
extern "C" 
{
#include <lwsdk/lwio.h>
}

#include "util/nstring.h"

//----------------------------------------------------------------------------
/**
    @class nLWLoadState
    @brief Encapsulates Lightwave's LWLoadState structure.
*/
class nLWLoadState
{
public:
    nLWLoadState(const LWLoadState*);
    ~nLWLoadState();

    bool ReadInt(int& out);
    bool ReadFloat(float& out);
    bool ReadFloat4(nFloat4& out);
    bool ReadBool(bool& out);
    bool ReadString(nString& out);
    bool BeginBlock(const LWBlockIdent& block);
    void EndBlock();
    int GetBlockDepth() const;

private:
    const LWLoadState* lwLoadState;
};

//----------------------------------------------------------------------------
/**
*/
inline
nLWLoadState::nLWLoadState(const LWLoadState* state) :
    lwLoadState(state)
{

}

//----------------------------------------------------------------------------
/**
*/
inline
nLWLoadState::~nLWLoadState()
{
    this->lwLoadState = 0;
}

//----------------------------------------------------------------------------
/**
    @return true if an int was read in, false otherwise.
*/
inline
bool
nLWLoadState::ReadInt(int& out)
{
    long val = 0;
    if (!LWLOAD_I4(this->lwLoadState, &val, 1))
        return false;

    out = static_cast<int>(val);
    return true;
}

//----------------------------------------------------------------------------
/**
    @return true if a float was read in, false otherwise.
*/
inline
bool 
nLWLoadState::ReadFloat(float& out)
{
    if (!LWLOAD_FP(this->lwLoadState, &out, 1))
        return false;

    return true;
}

//----------------------------------------------------------------------------
/**
    @return true if an nFloat4 was read in, false otherwise.
*/
inline
bool 
nLWLoadState::ReadFloat4(nFloat4& out)
{
    if (!LWLOAD_FP(this->lwLoadState, &out.x, 1))
        return false;

    if (!LWLOAD_FP(this->lwLoadState, &out.y, 1))
        return false;

    if (!LWLOAD_FP(this->lwLoadState, &out.z, 1))
        return false;

    if (!LWLOAD_FP(this->lwLoadState, &out.w, 1))
        return false;

    return true;
}

//----------------------------------------------------------------------------
/**
    @return true if a bool was read in, false otherwise.
*/
inline
bool
nLWLoadState::ReadBool(bool& out)
{
    char val = 0;
    if (!LWLOAD_I1(this->lwLoadState, &val, 1))
        return false;
    
    out = ((0 == val) ? false : true);
    return true;
}

//----------------------------------------------------------------------------
/**
    @return true if a string was read in, false otherwise.
*/
inline
bool
nLWLoadState::ReadString(nString& str)
{
    char strbuf[256] = { 0 };
    if (!LWLOAD_STR(this->lwLoadState, strbuf, sizeof(strbuf)))
        return false;

    str = strbuf;
    return true;
}

//----------------------------------------------------------------------------
/**
*/
inline
bool 
nLWLoadState::BeginBlock(const LWBlockIdent& block)
{
    LWBlockIdent blockList[] = { block, 0 };
    return (LWLOAD_FIND(this->lwLoadState, blockList) == block.id);
}

//----------------------------------------------------------------------------
/**
*/
inline
void 
nLWLoadState::EndBlock()
{
    LWLOAD_END(this->lwLoadState);
}

//----------------------------------------------------------------------------
/**
*/
inline
int 
nLWLoadState::GetBlockDepth() const
{
    return LWLOAD_DEPTH(this->lwLoadState);
}

//----------------------------------------------------------------------------
#endif // N_LW_LOAD_STATE_H
