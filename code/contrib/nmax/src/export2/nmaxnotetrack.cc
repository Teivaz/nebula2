//---------------------------------------------------------------------------
//  nmaxnotetrack.cc
//
//  (c)2004 Kim, Hyoun Woo
//---------------------------------------------------------------------------
#include "export2/nmax.h"

#include "export2/nmaxnotetrack.h"
#include "export2/nmaxutil.h"
#include "export2/nmaxbones.h"
#include "pluginlibs/nmaxdlg.h"
#include "pluginlibs/nmaxlogdlg.h"

#include "kernel/ntypes.h"
#include "tools/nanimbuilder.h"

//---------------------------------------------------------------------------
/**
*/
nMaxAnimState::nMaxAnimState() :
    firstFrame(0),
    duration(0),
    fadeInTime(0.0f),
    loopType(nAnimBuilder::Group::REPEAT)
{
    // default clip.
    this->AddClip("one");
}

//---------------------------------------------------------------------------
/**
*/
void nMaxAnimState::AddClip(const char* weightChannelName)
{
    n_assert(weightChannelName);

    this->clipArray.Append(nString(weightChannelName));
}

//---------------------------------------------------------------------------
/**
*/
const nString& 
nMaxAnimState::GetClip(int index) const
{
    n_assert(index >= 0);

    return this->clipArray[index];
}

//---------------------------------------------------------------------------
/**
*/
nMaxNoteTrack::nMaxNoteTrack()
{
}

//---------------------------------------------------------------------------
/**
*/
nMaxNoteTrack::~nMaxNoteTrack()
{
}

//---------------------------------------------------------------------------
/**
    Extract note name.
    e.g) get a word 'walk' with the given string, "walk : repeat".
*/
nString nMaxNoteTrack::ExtractNoteName(nString& rhs)
{
    nString tmp = rhs;
    tmp.TrimRight(":");
    return nMaxUtil::CorrectName(tmp);
}

//---------------------------------------------------------------------------
/**
    Extract loop type.
    e.g) get a word 'repeat' with the given string, "walk : repeat".
*/
nString nMaxNoteTrack::ExtractLoopType(nString& rhs)
{
    nString tmp = rhs;
    tmp.TrimLeft(":");
    return nMaxUtil::CorrectName(tmp);
}

//---------------------------------------------------------------------------
/**
    Collect note key from the given node and creates animation states.

    A note key is used for representing animation state.

    -20-Feb-06  kims  Fixed to get correct index of ExtractRange().
                      Thanks Ivan Tivonenko for the patch.

    @param inode A node which we collect animation states from.
                 The node should be bone.
*/
void nMaxNoteTrack::GetAnimState(INode* inode)
{
    n_assert(inode);

    for (int i=0; i<inode->NumNoteTracks(); i++)
    {
        DefNoteTrack* noteTrack = static_cast<DefNoteTrack*>(inode->GetNoteTrack(i));

        for (int j=0; j<noteTrack->keys.Count(); j++)
        {
            NoteKey* noteKey = noteTrack->keys[j];

            int frame = noteKey->time / GetTicksPerFrame();
            int nextFrame = GetNextFrame(j, noteTrack);

            nString note = noteKey->note.data();
            if (!note.IsEmpty())
            {
                int duration = 0;
                nString strNote, strNoteName, strDuration, strLoopType;

                int idx = note.FindCharIndex('\n', 0);
                if (idx > 0) // we have both note key and duration for the property
                {
                    strNote = nMaxUtil::CorrectName(note.ExtractRange(0, idx));
                    if (strNote.ContainsCharFromSet(":"))
                    {
                        strNoteName = this->ExtractNoteName(strNote);
                        strLoopType = this->ExtractLoopType(strNote);
                    }
                    else
                    {
                        strNoteName = strNote;
                    }

                    int durLen = note.Length() - idx - 1;
                    if (durLen > 0) 
                    {
                        strDuration = nMaxUtil::CorrectName(note.ExtractRange(idx+1, durLen));
                        duration = strDuration.AsInt();//duration = atoi(strDuration.Get());
                    }

                    if (duration <= 0)
                    {
                        duration = nextFrame - frame;
                    }
                }
                else // we only have note key name
                {
                    strNote = nMaxUtil::CorrectName(note);
                    if (strNote.ContainsCharFromSet(":"))
                    {
                        strNoteName = this->ExtractNoteName(strNote);
                        strLoopType = this->ExtractLoopType(strNote);
                    }
                    else
                    {
                        strNoteName = strNote;
                    }

                    duration = nextFrame - frame;
                }

                // add animation state.
                nMaxAnimState animState;

                animState.name       = strNoteName;
                animState.firstFrame = frame;
                animState.duration   = duration;
                animState.fadeInTime = 0.0f;

                strLoopType.ToLower();
                if (strLoopType == "repeat")
                {
                    animState.loopType = nAnimBuilder::Group::REPEAT;
                }
                else
                if (strLoopType == "clamp")
                {
                    animState.loopType = nAnimBuilder::Group::CLAMP;
                }
                else
                {
                    // unknown loop type. just specify it to repeat type.
                    animState.loopType = nAnimBuilder::Group::REPEAT;
                }

                this->stateArray.Append(animState);
            }
        }// end of for each key in note track.
    }// end of for each note track.
}

