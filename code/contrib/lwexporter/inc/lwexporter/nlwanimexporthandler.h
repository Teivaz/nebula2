#ifndef N_LW_ANIM_EXPORT_HANDLER_H
#define N_LW_ANIM_EXPORT_HANDLER_H
//----------------------------------------------------------------------------
extern "C" 
{
#include <lwsdk/lwserver.h>
#include <lwsdk/lwrender.h>
#include <lwsdk/lwmotion.h>
}

class nLWCmdExec;

//----------------------------------------------------------------------------
/**
    @class nLWAnimExportHandler
    @brief An ItemMotionHandler Lightwave plugin for extracting bone animation.

    This plugin is attached to bones and is called back every frame to
    obtain the bone transform.
*/
class nLWAnimExportHandler
{
public:
    nLWAnimExportHandler(LWItemID);

    static bool AttachToBone(LWItemID, nLWCmdExec*);
    static bool DetachFromBone(LWItemID, nLWCmdExec*);

    XCALL_(static int) Activate_Handler(long version, 
                                        GlobalFunc* global,
                                        LWItemMotionHandler* local, 
                                        void* serverData);

    static const char HANDLER_NAME[];

private:
    XCALL_(static LWInstance)   Create(void* priv, void* boxedItemId, LWError*);
    XCALL_(static void)         Destroy(LWInstance);
    XCALL_(static LWError)      Copy(LWInstance dest, LWInstance src);
    XCALL_(static LWError)      Load(LWInstance, const LWLoadState*);
    XCALL_(static LWError)      Save(LWInstance, const LWSaveState*);
    XCALL_(static const char*)  Describe(LWInstance);
    XCALL_(static void)         Evaluate(LWInstance, const LWItemMotionAccess*);
    XCALL_(static unsigned int) Flags(LWInstance);

    LWItemID boneId;
};

//----------------------------------------------------------------------------
#endif // N_LW_ANIM_EXPORT_HANDLER_H
