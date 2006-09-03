//------------------------------------------------------------------------------
//  nanimbuilder.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "tools/nanimbuilder.h"
#include "kernel/nfileserver2.h"
#include "kernel/nfile.h"
#include "util/nstring.h"
#include "anim2/nanimation.h"

//------------------------------------------------------------------------------
/**
*/
nAnimBuilder::nAnimBuilder()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nAnimBuilder::~nAnimBuilder()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nAnimBuilder::Clear()
{
    this->groupArray.Clear();
}

//------------------------------------------------------------------------------
/**
    Return the actual number of keys (not including the 'keys' in collapsed
    curves).
*/
int
nAnimBuilder::GetNumKeys()
{
    int numKeys = 0;

    int groupIndex;
    int numGroups = this->GetNumGroups();
    for (groupIndex = 0; groupIndex < numGroups; groupIndex++)
    {
        const Group& group = this->GetGroupAt(groupIndex);
        int curveIndex;
        int numCurves = group.GetNumCurves();
        for (curveIndex = 0; curveIndex < numCurves; curveIndex++)
        {
            const Curve& curve = group.GetCurveAt(curveIndex);
            if (!curve.IsCollapsed())
            {
                numKeys += group.GetNumKeys();
            }
        }
    }
    return numKeys;
}

//------------------------------------------------------------------------------
/**
*/
bool
nAnimBuilder::Save(nFileServer2* fileServer, const char* filename)
{
    n_assert(filename);
    n_assert(fileServer);

    nString path(filename);
    if (path.CheckExtension("nanim2"))
    {
        return this->SaveNanim2(fileServer, filename);
    }
    else if (path.CheckExtension("nax2"))
    {
        return this->SaveNax2(fileServer, filename);
    }
    else
    {
        n_printf("nAnimBuilder::Save(): unsupported file extension in '%s'\n", filename);
        return false;
    }
}

//------------------------------------------------------------------------------
/**
    Save ascii nanim2 file.
    Check animation/nanimation.h for file format specification.
*/
bool
nAnimBuilder::SaveNanim2(nFileServer2* fileServer, const char* filename)
{
    n_assert(fileServer && filename);

    // open the file
    nFile* file = fileServer->NewFileObject();
    n_assert(file);
    if (file->Open(filename, "w"))
    {
        char line[1024];
        const int numGroups = this->GetNumGroups();

        // write header
        file->PutS("type nanim2\n");
        sprintf(line, "numgroups %d\n", numGroups);
        file->PutS(line);
        sprintf(line, "numkeys %d\n", this->GetNumKeys());
        file->PutS(line);

        // for each group...
        int groupIndex;
        for (groupIndex = 0; groupIndex < numGroups; groupIndex++)
        {
            const Group& group = this->GetGroupAt(groupIndex);
            const int numCurves = group.GetNumCurves();
            const int startKey  = group.GetStartKey();
            const int numKeys   = group.GetNumKeys();
            const int keyStride = group.GetKeyStride();
            const float keyTime = group.GetKeyTime();
            const float fadeInFrames = group.GetFadeInFrames();
            const char* loopTypeString = Group::LoopType2String(group.GetLoopType());
            sprintf(line, "group %d %d %d %d %f %f %s\n", numCurves, startKey, numKeys, keyStride, keyTime, fadeInFrames, loopTypeString);
            file->PutS(line);

            // for each curve...
            int curveIndex;
            for (curveIndex = 0; curveIndex < numCurves; curveIndex++)
            {
                const Curve& curve = group.GetCurveAt(curveIndex);
                const char* ipolTypeString = Curve::IpolType2String(curve.GetIpolType());
                const int isAnimated = curve.IsAnimated();
                const int firstKeyIndex = curve.GetFirstKeyIndex();
                const vector4& collapsedKey = curve.GetCollapsedKey();
                sprintf(line, "curve %s %d %d %f %f %f %f\n", ipolTypeString, firstKeyIndex, isAnimated, collapsedKey.x, collapsedKey.y, collapsedKey.z, collapsedKey.w);
                file->PutS(line);
            }

            // write interleaved keys of the non-collapsed keys...
            int keyIndex;
            for (keyIndex = 0; keyIndex < numKeys; keyIndex++)
            {
                for (curveIndex = 0; curveIndex < numCurves; curveIndex++)
                {
                    const Curve& curve = group.GetCurveAt(curveIndex);
                    if (!curve.IsCollapsed())
                    {
                        const vector4& val = group.GetCurveAt(curveIndex).GetKeyAt(keyIndex).Get();
                        sprintf(line, "key %f %f %f %f\n", val.x, val.y, val.z, val.w);
                        file->PutS(line);
                    }
                }
            }
        }

        // cleanup
        file->Close();
    }
    file->Release();
    return true;
}

//------------------------------------------------------------------------------
/**
    Save binary animation data into existing file.
*/
bool
nAnimBuilder::SaveNax2(nFile* file)
{
    const int numGroups = this->GetNumGroups();
    const int numKeys   = this->GetNumKeys();

    // write header
    file->PutInt('NAX2');
    file->PutInt(numGroups);
    file->PutInt(numKeys);

    // write group headers...
    int groupIndex;
    for (groupIndex = 0; groupIndex < numGroups; groupIndex++)
    {
        const Group& group = this->GetGroupAt(groupIndex);
        const int numCurves = group.GetNumCurves();
        const int startKey  = group.GetStartKey();
        const int numKeys   = group.GetNumKeys();
        const int keyStride = group.GetKeyStride();
        const float keyTime = group.GetKeyTime();
        const float fadeInFrames = group.GetFadeInFrames();
        nAnimation::Group::LoopType loopType = nAnimation::Group::Clamp;
        if (nAnimation::Group::LoopType(group.GetLoopType()) == nAnimation::Group::Repeat)
        {
            loopType = nAnimation::Group::Repeat;
        }

        file->PutInt(numCurves);
        file->PutInt(startKey);
        file->PutInt(numKeys);
        file->PutInt(keyStride);
        file->PutFloat(keyTime);
        file->PutFloat(fadeInFrames);
        file->PutInt(int(loopType));
    }

    // write curve headers
    for (groupIndex = 0; groupIndex < numGroups; groupIndex++)
    {
        const Group& group = this->GetGroupAt(groupIndex);
        int numCurves = group.GetNumCurves();
        int curveIndex;
        for (curveIndex = 0; curveIndex < numCurves; curveIndex++)
        {
            const Curve& curve = group.GetCurveAt(curveIndex);
            nAnimation::Curve::IpolType ipolType = nAnimation::Curve::None;
            switch (curve.GetIpolType())
            {
                case Curve::STEP:   ipolType = nAnimation::Curve::Step; break;
                case Curve::LINEAR: ipolType = nAnimation::Curve::Linear; break;
                case Curve::QUAT:   ipolType = nAnimation::Curve::Quat; break;
                default:            ipolType = nAnimation::Curve::None; break;
            }

            const int firstKeyIndex = curve.GetFirstKeyIndex();
            const int isAnim = curve.IsAnimated();
            const vector4& collapsedKey = curve.GetCollapsedKey();

            file->PutInt(int(ipolType));
            file->PutInt(firstKeyIndex);
            file->PutInt(isAnim);
            file->PutFloat(collapsedKey.x);
            file->PutFloat(collapsedKey.y);
            file->PutFloat(collapsedKey.z);
            file->PutFloat(collapsedKey.w);
        }
    }

    // write keys
    for (groupIndex = 0; groupIndex < numGroups; groupIndex++)
    {
        const Group& group = this->GetGroupAt(groupIndex);
        int numCurves = group.GetNumCurves();
        int numKeys   = group.GetNumKeys();
        int keyIndex;
        for (keyIndex = 0; keyIndex < numKeys; keyIndex++)
        {
            int curveIndex;
            for (curveIndex = 0; curveIndex < numCurves; curveIndex++)
            {
                const Curve& curve = group.GetCurveAt(curveIndex);
                if (!curve.IsCollapsed())
                {
                    const vector4& val = group.GetCurveAt(curveIndex).GetKeyAt(keyIndex).Get();
                    file->PutFloat(val.x);
                    file->PutFloat(val.y);
                    file->PutFloat(val.z);
                    file->PutFloat(val.w);
                }
            }
        }
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    Save animation data as binary NAX2 file.
    Check animation/nanimation.h for file format specification.
*/
bool
nAnimBuilder::SaveNax2(nFileServer2* fileServer, const char* filename)
{
    n_assert(fileServer && filename);
    bool retval = false;

    // open the file
    nFile* file = fileServer->NewFileObject();
    n_assert(file);
    if (file->Open(filename, "wb"))
    {
        if (this->SaveNax2(file))
        {
            retval = true;
        }
        file->Close();
    }
    file->Release();
    return retval;
}

//------------------------------------------------------------------------------
/**
    Optimize curves. At the moment this will just collapse curves where
    all keys are identical. Returns number of collapsed curves.
*/
int
nAnimBuilder::Optimize()
{
    int numOptimizedCurves = 0;
    int numGroups = this->GetNumGroups();
    int groupIndex;
    for (groupIndex = 0; groupIndex < numGroups; groupIndex++)
    {
        Group& group = this->GetGroupAt(groupIndex);
        int numCurves = group.GetNumCurves();
        int curveIndex;
        for (curveIndex = 0; curveIndex < numCurves; curveIndex++)
        {
            Curve& curve = group.GetCurveAt(curveIndex);
            if (curve.Optimize())
            {
                numOptimizedCurves++;
            }
        }
    }
    return numOptimizedCurves;
}

//------------------------------------------------------------------------------
/**
    Fixes the first key index and key stride members in the
    contained groups and curves.
*/
void
nAnimBuilder::FixKeyOffsets()
{
    int numGroups = this->GetNumGroups();
    int groupIndex;
    int groupFirstKeyIndex = 0;
    for (groupIndex = 0; groupIndex < numGroups; groupIndex++)
    {
        Group& group = this->GetGroupAt(groupIndex);

        // count non-collapsed curves
        int numCurves = group.GetNumCurves();
        int numNonCollapsedCurves = 0;
        int curveIndex;
        for (curveIndex = 0; curveIndex < numCurves; curveIndex++)
        {
            Curve& curve = group.GetCurveAt(curveIndex);
            if (!curve.IsCollapsed())
            {
                curve.SetFirstKeyIndex(groupFirstKeyIndex + numNonCollapsedCurves);
                numNonCollapsedCurves++;
            }
        }

        // the key stride in each group is identical to the number of non-collapsed curves
        group.SetKeyStride(numNonCollapsedCurves);

        // update the groupFirstKeyIndex for the next group
        groupFirstKeyIndex += (numNonCollapsedCurves * group.GetNumKeys());;
    }
}

//------------------------------------------------------------------------------
/**
    Load animation from nanim2 or nax2 file.
*/
bool
nAnimBuilder::Load(nFileServer2* fileServer, const char* filename)
{
    n_assert(filename);
    n_assert(fileServer);

    nString path(filename);
    if (path.CheckExtension("nanim2"))
    {
        return this->LoadNanim2(fileServer, filename);
    }
    else if (path.CheckExtension("nax2"))
    {
        return this->LoadNax2(fileServer, filename);
    }
    else
    {
        n_printf("nAnimBuilder::Load(): unsupported file extension in '%s'\n", filename);
        return false;
    }
}

//------------------------------------------------------------------------------
/**
    Load animation data from ascii nanim2 file.
*/
bool
nAnimBuilder::LoadNanim2(nFileServer2* fileServer, const char* filename)
{
    n_assert(filename);
    n_assert(fileServer);

    bool retval = false;
    nFile* file = fileServer->NewFileObject();
    n_assert(file);

    nArray<vector4> keyArray(4096, 4096);

    int numGroups = 0;
    int numKeys = 0;
    if (file->Open(filename, "r"))
    {
        char line[1024];
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
                    file->Close();
                    file->Release();
                    return false;
                }
            }
            else if (0 == strcmp(keyWord, "numgroups"))
            {
                const char* numGroupsString = strtok(0, N_WHITESPACE);
                n_assert(numGroupsString);
                numGroups = atoi(numGroupsString);
            }
            else if (0 == strcmp(keyWord, "numkeys"))
            {
                const char* numKeysString = strtok(0, N_WHITESPACE);
                n_assert(numKeysString);
                numKeys = atoi(numKeysString);
            }
            else if (0 == strcmp(keyWord, "group"))
            {
                // add a group object
                const char* numCurvesString = strtok(0, N_WHITESPACE);
                const char* startKeyString  = strtok(0, N_WHITESPACE);
                const char* numKeysString   = strtok(0, N_WHITESPACE);
                const char* keyStrideString = strtok(0, N_WHITESPACE);
                const char* keyTimeString   = strtok(0, N_WHITESPACE);
                const char* loopTypeString  = strtok(0, N_WHITESPACE);
                n_assert(numCurvesString);
                n_assert(startKeyString);
                n_assert(numKeysString);
                n_assert(keyStrideString);
                n_assert(keyTimeString);
                n_assert(loopTypeString);

                Group group;
                group.SetLoopType(Group::String2LoopType(loopTypeString));
                group.SetStartKey(atoi(startKeyString));
                group.SetNumKeys(atoi(numKeysString));
                group.SetKeyStride(atoi(keyStrideString));
                group.SetKeyTime(float(atof(keyTimeString)));
                this->AddGroup(group);
            }
            else if (0 == strcmp(keyWord, "curve"))
            {
                // add curve object to current group
                const char* ipolTypeString = strtok(0, N_WHITESPACE);
                const char* firstKeyIndexString = strtok(0, N_WHITESPACE);
                const char* collapsedKeyXString = strtok(0, N_WHITESPACE);
                const char* collapsedKeyYString = strtok(0, N_WHITESPACE);
                const char* collapsedKeyZString = strtok(0, N_WHITESPACE);
                const char* collapsedKeyWString = strtok(0, N_WHITESPACE);
                n_assert(ipolTypeString);
                n_assert(firstKeyIndexString);
                n_assert(collapsedKeyXString);
                n_assert(collapsedKeyYString);
                n_assert(collapsedKeyZString);
                n_assert(collapsedKeyWString);

                static vector4 collapsedKey;
                collapsedKey.set(float(atof(collapsedKeyXString)),
                                 float(atof(collapsedKeyYString)),
                                 float(atof(collapsedKeyZString)),
                                 float(atof(collapsedKeyWString)));

                Curve curve;
                curve.SetIpolType(Curve::String2IpolType(ipolTypeString));
                curve.SetFirstKeyIndex(atoi(firstKeyIndexString));
                if (curve.GetFirstKeyIndex() == -1)
                {
                    curve.SetCollapsed(true);
                }
                curve.SetCollapsedKey(collapsedKey);
                this->GetGroupAt(this->GetNumGroups() - 1).AddCurve(curve);
            }
            else if (0 == strcmp(keyWord, "key"))
            {
                // read new key and add to key array
                const char* keyXString = strtok(0, N_WHITESPACE);
                const char* keyYString = strtok(0, N_WHITESPACE);
                const char* keyZString = strtok(0, N_WHITESPACE);
                const char* keyWString = strtok(0, N_WHITESPACE);
                n_assert(keyXString && keyYString && keyZString && keyWString);

                static vector4 key;
                key.set(float(atof(keyXString)),
                        float(atof(keyYString)),
                        float(atof(keyZString)),
                        float(atof(keyWString)));
                keyArray.Append(key);
            }
        }
        file->Close();
        retval = true;
    }
    file->Release();

    if (retval)
    {
        // transfer keys into curves
        int groupIndex;
        int numGroups = this->GetNumGroups();
        for (groupIndex = 0; groupIndex < numGroups; groupIndex++)
        {
            Group& group = this->GetGroupAt(groupIndex);
            int curveIndex;
            const int numCurves = group.GetNumCurves();
            const int groupNumKeys = group.GetNumKeys();
            const int keyStride = group.GetKeyStride();
            for (curveIndex = 0; curveIndex < numCurves; curveIndex++)
            {
                Curve& curve = group.GetCurveAt(curveIndex);
                if (!curve.IsCollapsed())
                {
                    int firstKey = curve.GetFirstKeyIndex();
                    int keyIndex;
                    for (keyIndex = 0; keyIndex < groupNumKeys; keyIndex++)
                    {
                        Key key;
                        key.Set(keyArray[firstKey + (keyIndex * keyStride)]);
                        curve.SetKey(keyIndex, key);
                    }
                }
            }
        }
    }
    return retval;
}

//------------------------------------------------------------------------------
/**
    Load animation data from ascii nanim2 file.
*/
bool
nAnimBuilder::LoadNax2(nFileServer2* fileServer, const char* filename)
{
    n_assert(filename);
    n_assert(fileServer);

    bool retval = false;
    nFile* file = fileServer->NewFileObject();
    n_assert(file);

    if (file->Open(filename, "rb"))
    {
        // read header, make sure it's a NAX2 file
        int magic = file->GetInt();
        if (magic != 'NAX2')
        {
            // magic number mismatch
            file->Close();
            file->Release();
            return false;
        }
        int numGroups = file->GetInt();
        int numKeys   = file->GetInt();

        // read groups
        int groupIndex;
        for (groupIndex = 0; groupIndex < numGroups; groupIndex++)
        {
            int numCurves = file->GetInt();
            int startKey  = file->GetInt();
            int numKeys   = file->GetInt();
            int keyStride = file->GetInt();
            float keyTime = file->GetFloat();
            int loopType  = file->GetInt();

            Group group;

            // add empty curve objects to the group
            Curve curve;
            int curveIndex;
            for (curveIndex = 0; curveIndex < numCurves; curveIndex++)
            {
                group.AddCurve(curve);
            }
            group.SetLoopType((Group::LoopType) loopType);
            group.SetStartKey(startKey);
            group.SetNumKeys(numKeys);
            group.SetKeyTime(keyTime);
            group.SetKeyStride(keyStride);
            this->AddGroup(group);
        }

        // read curves
        for (groupIndex = 0; groupIndex < numGroups; groupIndex++)
        {
            Group& group = this->GetGroupAt(groupIndex);
            int numCurves = group.GetNumCurves();
            int curveIndex;
            for (curveIndex = 0; curveIndex < numCurves; curveIndex++)
            {
                Curve& curve = group.GetCurveAt(curveIndex);

                static vector4 collapsedKey;
                short ipolType = file->GetShort();
                int firstKeyIndex = file->GetInt();
                collapsedKey.x = file->GetFloat();
                collapsedKey.y = file->GetFloat();
                collapsedKey.z = file->GetFloat();
                collapsedKey.w = file->GetFloat();

                curve.SetIpolType((Curve::IpolType) ipolType);
                curve.SetFirstKeyIndex(firstKeyIndex);
                if (-1 == firstKeyIndex)
                {
                    curve.SetCollapsed(true);
                }
                curve.SetCollapsedKey(collapsedKey);
            }
        }

        // read keys
        int bufferSize = numKeys * 4 * sizeof(float);
        float* keyBuffer = (float*) n_malloc(bufferSize);
        int numRead = file->Read(keyBuffer, bufferSize);
        n_assert(numRead == bufferSize);

        // transfer keys into curves
        for (groupIndex = 0; groupIndex < numGroups; groupIndex++)
        {
            Group& group = this->GetGroupAt(groupIndex);
            int curveIndex;
            const int numCurves    = group.GetNumCurves();
            const int groupNumKeys = group.GetNumKeys();
            const int keyStride    = group.GetKeyStride();
            for (curveIndex = 0; curveIndex < numCurves; curveIndex++)
            {
                Curve& curve = group.GetCurveAt(curveIndex);
                if (!curve.IsCollapsed())
                {
                    int firstKey = curve.GetFirstKeyIndex();
                    int keyIndex;
                    for (keyIndex = 0; keyIndex < groupNumKeys; keyIndex++)
                    {
                        Key key;
                        int bufIndex = (firstKey + (keyIndex * keyStride)) * 4;
                        key.SetX(keyBuffer[bufIndex++]);
                        key.SetY(keyBuffer[bufIndex++]);
                        key.SetZ(keyBuffer[bufIndex++]);
                        key.SetW(keyBuffer[bufIndex++]);
                        curve.SetKey(keyIndex, key);
                    }
                }
            }
        }
        n_free(keyBuffer);
        file->Close();
        retval = true;
    }
    file->Release();
    return retval;
}
