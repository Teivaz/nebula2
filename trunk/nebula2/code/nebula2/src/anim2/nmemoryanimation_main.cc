#define N_DEFINES nMemoryAllocation
//------------------------------------------------------------------------------
//  nmemoryanimation_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "anim2/nmemoryanimation.h"
#include "kernel/nfileserver2.h"
#include "kernel/nfile.h"
#include "mathlib/quaternion.h"

nNebulaClass(nMemoryAnimation, "nanimation");

//------------------------------------------------------------------------------
/**
*/
nMemoryAnimation::nMemoryAnimation() :
    keyArray(0, 0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nMemoryAnimation::~nMemoryAnimation()
{
    if (this->IsValid())
    {
        this->Unload();
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
nMemoryAnimation::Load()
{
    n_assert(!this->valid);

    if (nAnimation::Load())
    {
        if (this->filename.CheckExtension("nanim2"))
        {
            return this->LoadNanim2(this->filename.Get());
        }
        else if (this->filename.CheckExtension("nax2"))
        {
            return this->LoadNax2(this->filename.Get());
        }
        else
        {
            n_error("Could not load anim file %s\n", this->filename.Get());
        }
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void
nMemoryAnimation::Unload()
{
    if (this->valid)
    {
        nAnimation::Unload();
        this->keyArray.Clear();
    }
}

//------------------------------------------------------------------------------
/**
    Loads animation data from an ASCII nanim2 file.
*/
bool
nMemoryAnimation::LoadNanim2(const char* filename)
{
    n_assert(!this->valid);

    nFile* file = this->refFileServer->NewFileObject();
    n_assert(file);

    // open the file
    if (!file->Open(this->filename.Get(), "r"))
    {
        n_printf("nMemoryAnimation::LoadNanim2(): Could not open file %s\n", this->filename.Get());
        return false;
    }

    // read file line by line
    char line[1024];
    int groupIndex = 0;
    int curveIndex = 0;
    int keyIndex   = 0;
    Group* curGroup = 0;
    Curve* curCurve = 0;
    static vector4 vec4;
    while (file->GetS(line, sizeof(line)))
    {
        // get keyword
        char* keyWord = strtok(line, N_WHITESPACE);
        if (0 == keyWord)
        {
            continue;
        }
        else if (0 == strcmp(keyWord, "type"))
        {
            // type must be 'nanim2'
            const char* typeString = strtok(0, N_WHITESPACE);
            n_assert(typeString);
            if (0 != strcmp(typeString, "nanim2"))
            {
                n_error("nMemoryAnimation::LoadNanim2(): Invalid type %s, must be 'nanim2'\n", this->filename.Get(), typeString);
                file->Close();
                delete file;
                return false;
            }
        }
        else if (0 == strcmp(keyWord, "numgroups"))
        {
            const char* numGroupsString = strtok(0, N_WHITESPACE);
            n_assert(numGroupsString);
            this->SetNumGroups(atoi(numGroupsString));
        }
        else if (0 == strcmp(keyWord, "numkeys"))
        {
            const char* numKeysString = strtok(0, N_WHITESPACE);
            n_assert(numKeysString);
            this->keyArray.SetFixedSize(atoi(numKeysString));
        }
        else if (0 == strcmp(keyWord, "group"))
        {
            const char* numCurvesString = strtok(0, N_WHITESPACE);
            const char* startKeyString  = strtok(0, N_WHITESPACE);
            const char* numKeysString   = strtok(0, N_WHITESPACE);
            const char* keyStrideString = strtok(0, N_WHITESPACE);
            const char* keyTimeString   = strtok(0, N_WHITESPACE);
            const char* loopTypeString  = strtok(0, N_WHITESPACE);
            n_assert(numCurvesString && startKeyString && keyStrideString && numKeysString && keyTimeString && loopTypeString);

            curveIndex = 0;
            curGroup = &(this->GetGroupAt(groupIndex++));
            curGroup->SetNumCurves(atoi(numCurvesString));
            curGroup->SetStartKey(atoi(startKeyString));
            curGroup->SetNumKeys(atoi(numKeysString));
            curGroup->SetKeyStride(atoi(keyStrideString));
            curGroup->SetKeyTime(float(atof(keyTimeString)));
            curGroup->SetLoopType(curGroup->StringToLoopType(loopTypeString));
        }
        else if (0 == strcmp(keyWord, "curve"))
        {
            const char* ipolTypeString      = strtok(0, N_WHITESPACE);
            const char* firstKeyIndexString = strtok(0, N_WHITESPACE);
            const char* constXString        = strtok(0, N_WHITESPACE);
            const char* constYString        = strtok(0, N_WHITESPACE);
            const char* constZString        = strtok(0, N_WHITESPACE);
            const char* constWString        = strtok(0, N_WHITESPACE);
            n_assert(ipolTypeString && firstKeyIndexString && constXString && constYString && constZString && constWString);

            n_assert(curGroup);
            curCurve = &(curGroup->GetCurveAt(curveIndex++));
            curCurve->SetIpolType(curCurve->StringToIpolType(ipolTypeString));
            curCurve->SetFirstKeyIndex(atoi(firstKeyIndexString));
            vec4.x = float(atof(constXString));
            vec4.y = float(atof(constYString));
            vec4.z = float(atof(constZString));
            vec4.w = float(atof(constWString));
            curCurve->SetConstValue(vec4);
        }
        else if (0 == strcmp(keyWord, "key"))
        {
            const char* keyXString = strtok(0, N_WHITESPACE);
            const char* keyYString = strtok(0, N_WHITESPACE);
            const char* keyZString = strtok(0, N_WHITESPACE);
            const char* keyWString = strtok(0, N_WHITESPACE);
            n_assert(keyXString && keyYString && keyZString && keyWString);

            vec4.x = float(atof(keyXString));
            vec4.y = float(atof(keyYString));
            vec4.z = float(atof(keyZString));
            vec4.w = float(atof(keyWString));
            this->keyArray[keyIndex++] = vec4;
        }
        else
        {
            n_error("nMemoryAnimation::LoadNanim2(): Unknown keyword %s in nanim2 file %s\n", keyWord, this->filename.Get());
            file->Close();
            delete file;
            return false;
        }
    }

    // cleanup
    file->Close();
    delete file;
    return true;
}

//------------------------------------------------------------------------------
/**
    Loads animation data from a binary nax2 file.
*/
bool
nMemoryAnimation::LoadNax2(const char* filename)
{
    // FIXME!
    return false;
}

//------------------------------------------------------------------------------
/**
    Samples the current values for a number of curves in the given
    animation group. The sampled values will be written to a client provided 
    vector4 array.

    @param  time                a point in time
    @param  groupIndex          index of animation group to sample from
    @param  firstCurveIndex     group-relative curve index of first curve to sample
    @param  numCurves           number of curves to sample
    @param  dstKeyArray         pointer to vector4 array with numCurves element which
                                will be filled with the results
*/
void 
nMemoryAnimation::SampleCurves(float time, int groupIndex, int firstCurveIndex, int numCurves, vector4* dstKeyArray)
{
    // convert the time into 2 global key indices and an inbetween value
    const Group& group = this->GetGroupAt(groupIndex);
    int keyIndex[2];
    float inbetween;
    group.TimeToIndex(time, keyIndex[0], keyIndex[1], inbetween);

    int i;
    static quaternion q0;
    static quaternion q1;
    static quaternion q;
    for (i = 0; i < numCurves; i++)
    {
        const Curve& curve            = group.GetCurveAt(i + firstCurveIndex);
        Curve::IpolType curveIpolType = curve.GetIpolType();

        switch (curveIpolType)
        {
            case Curve::None:
                dstKeyArray[i] = curve.GetConstValue();
                break;

            case Curve::Step:
                {
                    int index0 = curve.GetFirstKeyIndex() + keyIndex[0];
                    dstKeyArray[i] = this->keyArray[index0];
                }
                break;

            case Curve::Quat:
                {
                    int curveFirstKeyIndex = curve.GetFirstKeyIndex();
                    int index0 = curveFirstKeyIndex + keyIndex[0];
                    int index1 = curveFirstKeyIndex + keyIndex[1];
                    q0.set(this->keyArray[index0].x, this->keyArray[index0].y, this->keyArray[index0].z, this->keyArray[index0].w);
                    q1.set(this->keyArray[index1].x, this->keyArray[index1].y, this->keyArray[index1].z, this->keyArray[index1].w);
                    q.slerp(q0, q1, inbetween);
                    dstKeyArray[i].set(q.x, q.y, q.z, q.w);
                }
                break;

            default:
                {
                    int curveFirstKeyIndex = curve.GetFirstKeyIndex();
                    int index0 = curveFirstKeyIndex + keyIndex[0];
                    int index1 = curveFirstKeyIndex + keyIndex[1];
                    const vector4& v0 = this->keyArray[index0];
                    const vector4& v1 = this->keyArray[index1];
                    dstKeyArray[i] = v0 + ((v1 - v0) * inbetween);
                }
                break;
        }
    }
}

