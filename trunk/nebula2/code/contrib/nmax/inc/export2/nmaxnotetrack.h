//---------------------------------------------------------------------------
//  nmaxnotetrack.h
//
//  (c)2004 Kim, Hyoun Woo
//---------------------------------------------------------------------------
#ifndef N_MAXNOTETRACK_H
#define N_MAXNOTETRACK_H 
//---------------------------------------------------------------------------
/**
    @class nMaxNoteTack
    @ingroup

    @brief
*/
#include "export2/nmaxinterface.h"

#include "util/nstring.h"

//---------------------------------------------------------------------------
class nMaxAnimState 
{
public:
    nMaxAnimState();

    void AddClip(const char* weightChannelName);

    const nString& GetClip(int index) const;

    int firstFrame;
    int duration;

    float fadeInTime;

    nString name;

    nArray<nString> clipArray;
};

//---------------------------------------------------------------------------
class nMaxNoteTrack
{
public:
    nMaxNoteTrack();
    virtual ~nMaxNoteTrack();

    void CreateAnimState(INode* inode);

    int GetNumStates() const;

    const nMaxAnimState& GetState(int index);

protected:
    int GetNextFrame(int index, DefNoteTrack* n);

protected:
    nArray<nMaxAnimState> stateArray;

};
//---------------------------------------------------------------------------
inline
int nMaxNoteTrack::GetNumStates() const
{
    return this->stateArray.Size();
}
//---------------------------------------------------------------------------
inline
int nMaxNoteTrack::GetNextFrame(int index, DefNoteTrack* n)
{
    return (index < n->keys.Count() - 1 ? 
        n->keys[index+1]->time/GetTicksPerFrame() : 
    nMaxInterface::Instance()->GetNumFrames());
}
//---------------------------------------------------------------------------
inline
const nMaxAnimState& 
nMaxNoteTrack::GetState(int index)
{
    n_assert(index >= 0);
    return this->stateArray[index];
}
//---------------------------------------------------------------------------
#endif
