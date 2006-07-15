#ifndef N_LW_SKIN_ANIM_EXPORT_H
#define N_LW_SKIN_ANIM_EXPORT_H
//----------------------------------------------------------------------------
extern "C"
{
#include <lwsdk/lwrender.h>
}

#include "util/nstring.h"
#include "tools/nanimbuilder.h"
#include "lwexporter/nlwbonejointmap.h"

class nLWLayoutMonitor;
class nSkinAnimator;
class nLWObjectExportSettings;
class nLWCmdExec;

//----------------------------------------------------------------------------
/**
    @class nLWSkinAnimExport
    @brief Handles exporting of animation data from bone-animated Lightwave
           characters.
*/
class nLWSkinAnimExport
{
public:
    nLWSkinAnimExport(nLWObjectExportSettings*, nLWCmdExec*);
    ~nLWSkinAnimExport();

    bool Run(nLWLayoutMonitor*);

    static bool GetAnimCurvesForBone(LWItemID boneId, 
                                     int& startFrame,
                                     nAnimBuilder::Curve** transCurve, 
                                     nAnimBuilder::Curve** rotCurve, 
                                     nAnimBuilder::Curve** scaleCurve);

private:
    bool AddJointsToSkinAnimator(nSkinAnimator*, LWItemID boneId, 
                                 int parentJointIdx, LWBoneInfo*, LWItemInfo*, 
                                 float scale);
    bool AddJointsToSkinAnimator(nSkinAnimator*, LWItemID rootBoneId,
                                 LWBoneInfo*, LWItemInfo*);
    bool CollectJoints(nSkinAnimator*, nLWLayoutMonitor*);
    bool CollectAnimStates(nSkinAnimator*, nLWLayoutMonitor*);
    bool ConstructAnimFileName(nString& animFileName, nLWLayoutMonitor*) const;
    bool AttachFrameHandler(LWItemID boneId, LWItemInfo*);
    void BeginFrameGrab(int startFrame, nAnimBuilder::Group*);
    bool GrabFrame(int frameNum);
    void EndFrameGrab();
    bool DetachFrameHandler(LWItemID boneId, LWItemInfo*);
    bool ExportAnimation(const nString& animFileName, nLWLayoutMonitor*);

    nLWObjectExportSettings* objSettings;
    nLWCmdExec* cmdExec;
    nLWBoneJointMap boneJointMap;

    static const nLWBoneJointMap* curBoneJointMap;
    static int curAnimGroupStartFrame;
    static nAnimBuilder::Group* curAnimGroup;
};

//----------------------------------------------------------------------------
#endif // N_LW_SKIN_ANIM_EXPORT_H
