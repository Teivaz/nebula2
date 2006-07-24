//---------------------------------------------------------------------------
//
//
//  (c)2004 Kim, Hyoun Woo.
//---------------------------------------------------------------------------
#include "export2/nmax.h"
#include "export2/nmaxvertexinfluence.h"

//---------------------------------------------------------------------------
/**
*/
nMaxVertexInfluence::nMaxVertexInfluence()
{
}

//---------------------------------------------------------------------------
/**
*/
nMaxVertexInfluence::~nMaxVertexInfluence()
{
}

//---------------------------------------------------------------------------
/**
*/
int
nMaxVertexInfluence::GetNumInfuences()
{
    return this->boneArray.Size();
}

//---------------------------------------------------------------------------
/**
*/
vector4
nMaxVertexInfluence::GetWeightsVector()
{
    vector4 vec(0, 0, 0, 0);
    for (int i=0; i<min(this->boneArray.Size(), 4); i++)
    {
        BoneInfluence& influence = this->boneArray[i];
        switch ( i )
        {
        case 0:
            vec.x = influence.weight;
            break;
        case 1:
            vec.y = influence.weight;
            break;
        case 2:
            vec.z = influence.weight;
            break;
        case 3:
            vec.w = influence.weight;
            break;
        }
    }

    return vec;
}

//---------------------------------------------------------------------------
/**
*/
vector4
nMaxVertexInfluence::GetIndicesVector()
{
    vector4 vec(0, 0, 0, 0);
    for (int i=0; i<min(this->boneArray.Size(), 4); i++)
    {
        BoneInfluence& influence = this->boneArray[i];
        switch (i)
        {
        case 0:
            vec.x = (float)influence.bone;
            break;
        case 1:
            vec.y = (float)influence.bone;
            break;
        case 2:
            vec.z = (float)influence.bone;
            break;
        case 3:
            vec.w = (float)influence.bone;
            break;
        }
    }

    return vec;
}

//---------------------------------------------------------------------------
/**
*/
void
nMaxVertexInfluence::AddBoneInfluence(int iBone, float fWeight)
{
    BOOL bAddFlag = TRUE;

    for (int i=0; i<this->boneArray.Size(); i++)
    {
        BoneInfluence& influence = this->boneArray[i];
        if (influence.bone == iBone)
        {
            influence.weight += fWeight;
            bAddFlag = FALSE;
            break;
        }
    }

    if (bAddFlag)
    {
        BoneInfluence Infliuence;
        Infliuence.bone = iBone;
        Infliuence.weight = fWeight;

        this->boneArray.Append(Infliuence);
    }
}

//---------------------------------------------------------------------------
/**
*/
void
nMaxVertexInfluence::AdjustBonesInfluence(float fWeightTreshold)
{
    int i;

    for (i = 0; true; i++)
    {
        if (i >= this->boneArray.Size())
            break;

        BoneInfluence& influence = this->boneArray[i];

        if (influence.weight < fWeightTreshold)
        {
            this->boneArray.Erase( i );
        }
    }

    while (this->boneArray.Size() > 4)
    {
        // find min influence
        float fMinInfluence = 1.0f;
        int MinInfluencePos;

        for (i = 0; true; i++)
        {
            if (i >= this->boneArray.Size())
                break;

            BoneInfluence& influence = this->boneArray[i];

            if ( influence.weight < fMinInfluence )
            {
                fMinInfluence = influence.weight;
                MinInfluencePos = i;
            }
        }

        this->boneArray.Erase(MinInfluencePos);
    }

    // normalize influences
    float fTotalWeight = 0.0f;
    for ( i = 0; i < this->boneArray.Size(); i++ )
    {
        BoneInfluence& influence = this->boneArray[i];
        fTotalWeight += influence.weight;
    }

    if ((fTotalWeight<TINY) && (this->boneArray.Size() > 0))
    {
        fTotalWeight = 1.0f / (float)this->boneArray.Size();
    }

    for (i=0; i<this->boneArray.Size(); i++)
    {
        BoneInfluence& influence = this->boneArray[i];
        influence.weight /= fTotalWeight;
    }
}