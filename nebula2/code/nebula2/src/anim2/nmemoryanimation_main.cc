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
nMemoryAnimation::LoadResource()
{
    n_assert(!this->IsValid());

    bool success = false;
    nPathString filename = this->GetFilename().Get();
    if (filename.CheckExtension("nanim2"))
    {
        success = this->LoadNanim2(filename.Get());
    }
    else if (filename.CheckExtension("nax2"))
    {
        success = this->LoadNax2(filename.Get());
    }
    if (success)
    {
        this->SetValid(true);
    }
    return success;
}

//------------------------------------------------------------------------------
/**
*/
void
nMemoryAnimation::UnloadResource()
{
    if (this->IsValid())
    {
        nAnimation::UnloadResource();
        this->keyArray.Clear();
        this->SetValid(false);
    }
}

//------------------------------------------------------------------------------
/**
    Loads animation data from an ASCII nanim2 file.
*/
bool
nMemoryAnimation::LoadNanim2(const char* filename)
{
    n_assert(!this->IsValid());

    nFile* file = this->refFileServer->NewFileObject();
    n_assert(file);

    // open the file
    if (!file->Open(filename, "r"))
    {
        n_error("nMemoryAnimation::LoadNanim2(): Could not open file %s\n", filename);
        file->Release();
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
                n_error("nMemoryAnimation::LoadNanim2(): Invalid type %s, must be 'nanim2'\n", filename, typeString);
                file->Close();
                file->Release();
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
            n_error("nMemoryAnimation::LoadNanim2(): Unknown keyword %s in nanim2 file %s\n", keyWord, filename);
            file->Close();
            file->Release();
            return false;
        }
    }

    // cleanup
    file->Close();
    file->Release();
    return true;
}

//------------------------------------------------------------------------------
/**
    Loads animation data from a binary nax2 file.
*/
bool
nMemoryAnimation::LoadNax2(const char* filename)
{
    n_assert(!this->IsValid());

    nFile* file = this->refFileServer->NewFileObject();
    n_assert(file);

    // open the file
    if (!file->Open(filename, "rb"))
    {
        n_error("nMemoryAnimation::LoadNax2(): Could not open file %s!", filename);
        file->Release();
        return false;
    }

    // read header
    int magic = file->GetInt();
    if (magic != 'NAX2')
    {
        n_error("nMemoryAnimation::LoadNax2(): File %s is not a NAX2 file!", filename);
        file->Close();
        file->Release();
        return false;
    }
    int numGroups = file->GetInt();
    int numKeys = file->GetInt();

    this->SetNumGroups(numGroups);
    this->keyArray.SetFixedSize(numKeys);

    // read groups
    int groupIndex = 0;
    for (groupIndex = 0; groupIndex < numGroups; groupIndex++)
    {
        int numCurves = file->GetInt();
        int startKey  = file->GetInt();
        int numKeys   = file->GetInt();
        int keyStride = file->GetInt();
        float keyTime = file->GetFloat();
        int loopType  = file->GetInt();

        Group& group = this->GetGroupAt(groupIndex);
        group.SetNumCurves(numCurves);
        group.SetStartKey(startKey);
        group.SetNumKeys(numKeys);
        group.SetKeyStride(keyStride);
        group.SetKeyTime(keyTime);
        group.SetLoopType((Group::LoopType) loopType);
    }

    // read curves
    for (groupIndex = 0; groupIndex < numGroups; groupIndex++)
    {
        Group& group = this->GetGroupAt(groupIndex);
        int numCurves = group.GetNumCurves();
        int curveIndex;
        for (curveIndex = 0; curveIndex < numCurves; curveIndex++)
        {
            static vector4 collapsedKey;
            short ipolType = file->GetShort();
            int firstKeyIndex = file->GetInt();
            collapsedKey.x = file->GetFloat();
            collapsedKey.y = file->GetFloat();
            collapsedKey.z = file->GetFloat();
            collapsedKey.w = file->GetFloat();

            Curve& curve = group.GetCurveAt(curveIndex);
            curve.SetIpolType((Curve::IpolType) ipolType);
            curve.SetConstValue(collapsedKey);
            curve.SetFirstKeyIndex(firstKeyIndex);
        }
    }

    // read keys
    int keyArraySize = numKeys * sizeof(vector4);
    file->Read(&(this->keyArray[0]), keyArraySize);
    
    // cleanup
    file->Close();
    file->Release();
    return true;
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

//------------------------------------------------------------------------------
/**
*/
int
nMemoryAnimation::GetByteSize()
{
    return this->keyArray.Size() * sizeof(vector4);
}
