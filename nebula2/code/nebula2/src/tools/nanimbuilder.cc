//------------------------------------------------------------------------------
//  nanimbuilder.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "tools/nanimbuilder.h"
#include "kernel/nfileserver2.h"
#include "kernel/nfile.h"
#include "util/npathstring.h"

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

    nPathString path(filename);
    if (path.CheckExtension("nanim2"))
    {
        return this->SaveNanim2(fileServer, filename);
    }
    else if (path.CheckExtension("nax2"))
    {
// FIXME!        return this->SaveNax2(fileServer, filename);
        return this->SaveNanim2(fileServer, filename);
    }
    else
    {
        n_printf("nAnimBuilder::Save(): unsupported file extension in '%s'\n", filename);
        return false;
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
nAnimBuilder::SaveNanim2(nFileServer2* fileServer, const char* filename)
{
    n_assert(fileServer && filename);

    // fix the first key index and key strides
    this->FixKeyOffsets();

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
            const char* loopTypeString = Group::LoopType2String(group.GetLoopType());
            sprintf(line, "group %d %d %d %d %f %s\n", numCurves, startKey, numKeys, keyStride, keyTime, loopTypeString);
            file->PutS(line);

            // for each curve...
            int curveIndex;
            for (curveIndex = 0; curveIndex < numCurves; curveIndex++)
            {
                const Curve& curve = group.GetCurveAt(curveIndex);
                const char* ipolTypeString = Curve::IpolType2String(curve.GetIpolType());
                const vector4 collapsedKey = curve.GetCollapsedKey();
                const int firstKeyIndex = curve.GetFirstKeyIndex();
                sprintf(line, "curve %s %d %f %f %f %f\n", ipolTypeString, firstKeyIndex, collapsedKey.x, collapsedKey.y, collapsedKey.z, collapsedKey.w);
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
    delete file;
    return true;
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
