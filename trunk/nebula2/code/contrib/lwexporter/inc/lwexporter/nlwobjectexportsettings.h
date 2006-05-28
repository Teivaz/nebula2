#ifndef N_LW_OBJECT_EXPORT_SETTINGS_H
#define N_LW_OBJECT_EXPORT_SETTINGS_H
//----------------------------------------------------------------------------
extern "C" 
{
#include <lwsdk/lwserver.h>
#include <lwsdk/lwcustobj.h>
#include <lwsdk/lwgeneric.h>
}

#include "lwwrapper/nlwcustomobjecthandler.h"
#include "util/nstring.h"
#include "util/narray.h"
#include "lwexporter/nlwanimationstate.h"
#include "tools/nmeshbuilder.h"

class nLWObjectExportSettingsPanel;

//----------------------------------------------------------------------------
/**
    @class nLWObjectExportSettings
    @brief A CustomObjectHandler Lightwave plugin that provides access to
           per-object export settings.
*/
class nLWObjectExportSettings : public nLWCustomObjectHandlerBase
{
public:
    nLWObjectExportSettings(LWItemID lwItemId);
    virtual ~nLWObjectExportSettings();

    LWError DisplaySettingsPanel();
    nString GetItemName() const;
    LWItemID GetItemId() const;

    // object settings

    void SetExportNodeType(const nString&);
    const nString& GetExportNodeType() const;
    void SetExportSceneNodeHierarchy(bool);
    bool GetExportSceneNodeHierarchy() const;
    void SetAnimationVarName(const nString&);
    const nString& GetAnimationVarName() const;
    void SetAnimationStates(const nArray<nLWAnimationState>&);
    const nArray<nLWAnimationState>& GetAnimationStates() const;
    
    // plugin activation and stuff

    /// load settings from scene file
    virtual LWError OnLoad(const LWLoadState*);
    /// save settings to scene file
    virtual LWError OnSave(const LWSaveState*);
    virtual const char* OnGetDescription();
    virtual void OnChangeID(const LWItemID* idList);
    virtual LWError OnDisplayUI();

    XCALL_(static int) Activate_Handler(long version, 
                                        GlobalFunc* global,
                                        LWCustomObjHandler* local, 
                                        void* serverData);

    XCALL_(static int) Activate_Interface(long version, 
                                          GlobalFunc* global,
                                          LWInterface* local, 
                                          void* serverData);

    XCALL_(static int) Activate_SettingsPanel(long version, 
                                              GlobalFunc* global, 
                                              LWLayoutGeneric* local, 
                                              void* /*serverData*/);

    static const char HANDLER_NAME[];
    static const char SHORTCUT_GENERIC_NAME[];

private:
    static bool HasHandler(LWItemID lwItemId);

    XCALL_(static LWInstance) OnLWCreate(void* priv, void* context, LWError*);
    XCALL_(static LWError)    OnLWCopy(LWInstance dest, LWInstance source);

    LWItemID lwItemId;

    nString exportNodeType;
    bool exportSceneNodeHierarchy;
    nString animationVarName;
    nArray<nLWAnimationState> animationStateArray;
};

//----------------------------------------------------------------------------
/**
*/
inline
void 
nLWObjectExportSettings::SetExportNodeType(const nString& nodeType)
{
    this->exportNodeType = nodeType;
}

//----------------------------------------------------------------------------
/**
*/
inline
const nString&
nLWObjectExportSettings::GetExportNodeType() const
{
    return this->exportNodeType;
}

//----------------------------------------------------------------------------
/**
*/
inline
void 
nLWObjectExportSettings::SetExportSceneNodeHierarchy(bool exportHierarchy)
{
    this->exportSceneNodeHierarchy = exportHierarchy;
}

//----------------------------------------------------------------------------
/**
*/
inline
bool 
nLWObjectExportSettings::GetExportSceneNodeHierarchy() const
{
    return this->exportSceneNodeHierarchy;
}

//----------------------------------------------------------------------------
/**
*/
inline
void 
nLWObjectExportSettings::SetAnimationVarName(const nString& animVar)
{
    this->animationVarName = animVar;
}

//----------------------------------------------------------------------------
/**
*/
inline
const nString& 
nLWObjectExportSettings::GetAnimationVarName() const
{
    return this->animationVarName;
}

//----------------------------------------------------------------------------
/**
*/
inline
void 
nLWObjectExportSettings::SetAnimationStates(const nArray<nLWAnimationState>& a)
{
    this->animationStateArray.Clear();
    this->animationStateArray = a;
}

//----------------------------------------------------------------------------
/**
*/
inline
const nArray<nLWAnimationState>& 
nLWObjectExportSettings::GetAnimationStates() const
{
    return this->animationStateArray;
}

//----------------------------------------------------------------------------
#endif // N_LW_OBJECT_EXPORT_SETTINGS_H
