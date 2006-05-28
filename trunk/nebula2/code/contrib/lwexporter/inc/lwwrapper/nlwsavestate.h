#ifndef N_LW_SAVE_STATE_H
#define N_LW_SAVE_STATE_H
//----------------------------------------------------------------------------
extern "C" 
{
#include <lwsdk/lwio.h>
}

#include "util/nstring.h"

//----------------------------------------------------------------------------
/**
    @class nLWSaveState
    @brief Encapsulates Lightwave's LWSaveState structure.
*/
class nLWSaveState
{
public:
    nLWSaveState(const LWSaveState*);
    ~nLWSaveState();

    void WriteInt(int);
    void WriteFloat(float);
    void WriteFloat4(const nFloat4&);
    void WriteBool(bool);
    void WriteString(const nString&);
    void BeginBlock(const LWBlockIdent& block, bool leaf);
    void EndBlock();
    int GetBlockDepth() const;

private:
    const LWSaveState* lwSaveState;
};

//----------------------------------------------------------------------------
/**
*/
inline
nLWSaveState::nLWSaveState(const LWSaveState* state) :
    lwSaveState(state)
{

}

//----------------------------------------------------------------------------
/**
*/
inline
nLWSaveState::~nLWSaveState()
{
    this->lwSaveState = 0;
}

//----------------------------------------------------------------------------
/**
*/
inline
void 
nLWSaveState::WriteInt(int val)
{
    long out = val;
    LWSAVE_I4(this->lwSaveState, &out, 1);
}

//----------------------------------------------------------------------------
/**
*/
inline
void 
nLWSaveState::WriteFloat(float val)
{
    LWSAVE_FP(this->lwSaveState, &val, 1);
}

//----------------------------------------------------------------------------
/**
*/
inline
void 
nLWSaveState::WriteFloat4(const nFloat4& val)
{
    LWSAVE_FP(this->lwSaveState, &val.x, 1);
    LWSAVE_FP(this->lwSaveState, &val.y, 1);
    LWSAVE_FP(this->lwSaveState, &val.z, 1);
    LWSAVE_FP(this->lwSaveState, &val.w, 1);
}

//----------------------------------------------------------------------------
/**
*/
inline
void 
nLWSaveState::WriteBool(bool val)
{
    char out = val;
    LWSAVE_I1(this->lwSaveState, &out, 1);
}

//----------------------------------------------------------------------------
/**
*/
inline
void 
nLWSaveState::WriteString(const nString& val)
{
    LWSAVE_STR(this->lwSaveState, val.Get());
}

//----------------------------------------------------------------------------
/**
    @brief Start writing a new block.
*/
inline
void 
nLWSaveState::BeginBlock(const LWBlockIdent& block, bool leaf)
{
    int leafInt = leaf ? 1 : 0;
    LWSAVE_BEGIN(this->lwSaveState, &block, leafInt);
}

//----------------------------------------------------------------------------
/**
    @brief End the current block.
*/
inline
void 
nLWSaveState::EndBlock()
{
    LWSAVE_END(this->lwSaveState);
}

//----------------------------------------------------------------------------
/**
    @brief Get the current block nesting level, where zero means you've 
           entered no blocks.
*/
inline
int 
nLWSaveState::GetBlockDepth() const
{
    return LWSAVE_DEPTH(this->lwSaveState);
}

//----------------------------------------------------------------------------
#endif // N_LW_SAVE_STATE_H
