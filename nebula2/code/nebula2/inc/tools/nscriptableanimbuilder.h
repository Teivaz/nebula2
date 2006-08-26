#ifndef N_SCRIPTABLE_ANIMBUILDER_H
#define N_SCRIPTABLE_ANIMBUILDER_H
//-----------------------------------------------------------------------------
/**
    @class nScriptableAnimBuilder
    @ingroup Tools

    The following lua script shows the way to add animation group to the
    Nebula animation builder:
    @verbatim
    groupIndex = animBuilder:begingroup();

    for k, v in bones do
        ...
        -- curve for translation
        curveTran = animBuilder:addcurve();
        -- curve for quaternion
        curveRot = animBuilder:addcurve();
        -- curve for scale
        curveScale = animBuilder:addcurve();
        ...
        -- add keys
        for k, v in clips do
            animBuilder:addkey(groupIndex, curveTrans, tx, ty, tz, 0.0);
            animBuilder:addkey(groupIndex, curveRot,   rx, ry, rz, rw);
            animBuilder:addkey(groupIndex, curveTrans, sx, sy, sz, 0.0);
        end
    end
    animBulder:endgroup();
    @endverbatim

    (C)2005 Kim, Hyoun Woo
*/
#include "tools/nanimbuilder.h"

//-----------------------------------------------------------------------------
class nScriptableAnimBuilder : public nRoot
                             , public nAnimBuilder
{
public:
    /// constructor.
    nScriptableAnimBuilder();
    /// destructor.
    virtual ~nScriptableAnimBuilder();

    void SetLoopType(int groupIndex, const char* loopType);

    /// @name Curve Adding Functions
    /// @{
    int  BeginGroup();
    int  AddCurve(int index);
    void AddKey(int groupIndex, int curveIndex, int keyIndex,
                float x, float y, float z, float w);
    void EndGroup();
    /// @}

private:
    bool begin_group;

};

#endif
