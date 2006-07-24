
#include "kernel/nkernelserver.h"
#include "tools/nscriptableanimbuilder.h"

//-----------------------------------------------------------------------------
/**
*/
nScriptableAnimBuilder::nScriptableAnimBuilder() :
    begin_group(false)
{
}

//-----------------------------------------------------------------------------
/**
*/
nScriptableAnimBuilder::~nScriptableAnimBuilder()
{
}

//-----------------------------------------------------------------------------
/**
*/
void nScriptableAnimBuilder::SetLoopType(int groupIndex, const char* loopType)
{
    nString str = loopType;
    str.ToLower();

    nAnimBuilder::Group group = nAnimBuilder::GetGroupAt(groupIndex);

    if (str == "clamp")
    {
        group.SetLoopType(nAnimBuilder::Group::CLAMP);
    }
    else
    if (str == "repeat")
    {
        group.SetLoopType(nAnimBuilder::Group::REPEAT);
    }
    else
    {
        n_printf("Wrong animation group loop type : %s", loopType);
        return;
    }
}

//-----------------------------------------------------------------------------
/**
*/
int nScriptableAnimBuilder::BeginGroup()
{
    n_assert (false == this->begin_group);

    nAnimBuilder::Group group;
    nAnimBuilder::AddGroup(group);

    this->begin_group = true;

    // return index of the added group.
    return (nAnimBuilder::GetNumGroups() - 1);
}

//-----------------------------------------------------------------------------
/**
*/
void nScriptableAnimBuilder::EndGroup()
{
    this->begin_group = false;
}

//-----------------------------------------------------------------------------
/**
*/
int nScriptableAnimBuilder::AddCurve(int index)
{
    n_assert (this->begin_group);

    nAnimBuilder::Group& group = nAnimBuilder::GetGroupAt(index);

    nAnimBuilder::Curve curve;
    group.AddCurve(curve);

    // return index of the added curve.
    return (group.GetNumCurves() - 1);
}

//-----------------------------------------------------------------------------
/**
*/
void nScriptableAnimBuilder::AddKey(int groupIndex, int curveIndex, int keyIndex,
                                   float x, float y, float z, float w)
{
    n_assert (this->begin_group);

    nAnimBuilder::Group& group = nAnimBuilder::GetGroupAt(groupIndex);
    nAnimBuilder::Curve& curve = group.GetCurveAt(curveIndex);

    curve.SetKey(keyIndex, nAnimBuilder::Key(vector4(x, y, z, w)));
}
