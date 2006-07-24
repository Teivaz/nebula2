//----------------------------------------------------------------------------
// (c) 2006    Vadim Macagon
//----------------------------------------------------------------------------
#include "lwexporter/nlwobjectexportsettings.h"
#include "lwexporter/nlwobjectexportsettings_ui.h"
#include "lwexporter/nlwsettingsregistry.h"
#include "lwwrapper/nlwglobals.h"
#include "lwwrapper/nlwsavestate.h"
#include "lwwrapper/nlwloadstate.h"
#include "lwwrapper/nlwitemidchangelist.h"
#include "lwexporter/nlwexportermaster.h"

//----------------------------------------------------------------------------
/**
*/
nLWObjectExportSettings::nLWObjectExportSettings(LWItemID lwItemId) :
    lwItemId(lwItemId),
    exportSceneNodeHierarchy(true),
    animationVarName("chnCharState"),
    animationStateArray(0, 8)
{
    nLWExporterMaster::PluginCreated();

    nLWSettingsRegistry* registry = nLWSettingsRegistry::Instance();
    if (registry)
        registry->Add(this);
}

//----------------------------------------------------------------------------
/**
*/
nLWObjectExportSettings::~nLWObjectExportSettings()
{
    nLWSettingsRegistry* registry = nLWSettingsRegistry::Instance();
    if (registry)
        registry->Remove(this);

    nLWExporterMaster::PluginDestroyed();
}

//----------------------------------------------------------------------------
/**
*/
LWError
nLWObjectExportSettings::DisplaySettingsPanel()
{
    HostDisplayInfo* hdi = nLWGlobals::GetHostDisplayInfo();
    if (!hdi)
        return "Failed to obtain HostDisplayInfo.";

    wxWindow parent;
    parent.SetHWND(hdi->window);
    parent.Enable(false);
    nLWObjectExportSettingsPanel settingsPanel(&parent, this);
    settingsPanel.SetItemName(this->GetItemName());
    settingsPanel.ShowModal();
    parent.Enable(true);
    parent.SetHWND(0);

    // no errors
    return 0;
}

//----------------------------------------------------------------------------
/**
*/
nString
nLWObjectExportSettings::GetItemName() const
{
    nLWGlobals::ItemInfo itemInfo;
    n_assert(itemInfo.IsValid());
    if (itemInfo.IsValid())
    {
        return itemInfo.Get()->name(this->lwItemId);
    }

    static nString emptyStr;
    return emptyStr;
}

//----------------------------------------------------------------------------
/**
*/
LWItemID
nLWObjectExportSettings::GetItemId() const
{
    return this->lwItemId;
}

//----------------------------------------------------------------------------
/**
    Block ids used to save and load plugin data in the Lightwave file.
*/
namespace FileBlockId
{
    const LWBlockIdent ANIMATION_DATA  = { LWID_('A','N','I','M'), "AnimationData" };
    const LWBlockIdent ANIM_VAR        = { LWID_('A','V','A','R'), "AnimVar" };
    const LWBlockIdent ANIM_STATE      = { LWID_('S','T','A','T'), "AnimState" };
    const LWBlockIdent ANIM_STATE_NAME = { LWID_('A','S','N','M'), "AnimStateName" };
    const LWBlockIdent ANIM_STATE_SF   = { LWID_('A','S','S','F'), "AnimStateSF" };
    const LWBlockIdent ANIM_STATE_EF   = { LWID_('A','S','E','F'), "AnimStateEF" };
    const LWBlockIdent ANIM_STATE_FIT  = { LWID_('A','S','F','T'), "AnimStateFT" };
    const LWBlockIdent ANIM_STATE_LOOP = { LWID_('A','S','L','P'), "AnimStateLoop" };
}

//----------------------------------------------------------------------------
/**
*/
LWError
nLWObjectExportSettings::OnLoad(const LWLoadState* lwLoadState)
{
    nLWLoadState state(lwLoadState);

    this->exportNodeType = "None";
    if (!state.ReadString(this->exportNodeType))
    {
        n_assert2(false, "Failed to read in Export Node Type.");
    }

    this->exportSceneNodeHierarchy = false;
    if (!state.ReadBool(this->exportSceneNodeHierarchy))
    {
        n_assert2(false, "Failed to read in Export Hierarchy.");
    }

    // animation stuff
    this->animationVarName.Clear();
    this->animationStateArray.Clear();

    if (state.BeginBlock(FileBlockId::ANIMATION_DATA))
    {
        this->animationVarName.Clear();
        if (state.BeginBlock(FileBlockId::ANIM_VAR))
        {
            state.ReadString(this->animationVarName);
            state.EndBlock();
        }

        nLWAnimationState animState;
        while (state.BeginBlock(FileBlockId::ANIM_STATE))
        {
            animState.name.Clear();
            if (state.BeginBlock(FileBlockId::ANIM_STATE_NAME))
            {
                state.ReadString(animState.name);
                state.EndBlock();
            }

            animState.startFrame = 0;
            if (state.BeginBlock(FileBlockId::ANIM_STATE_SF))
            {
                state.ReadInt(animState.startFrame);
                state.EndBlock();
            }

            animState.endFrame = animState.startFrame;
            if (state.BeginBlock(FileBlockId::ANIM_STATE_EF))
            {
                state.ReadInt(animState.endFrame);
                state.EndBlock();
            }

            animState.fadeInTime = 0.0f;
            if (state.BeginBlock(FileBlockId::ANIM_STATE_FIT))
            {
                state.ReadFloat(animState.fadeInTime);
                state.EndBlock();
            }

            animState.repeat = false;
            if (state.BeginBlock(FileBlockId::ANIM_STATE_LOOP))
            {
                state.ReadBool(animState.repeat);
                state.EndBlock();
            }

            this->animationStateArray.PushBack(animState);

            state.EndBlock(/*ANIM_STATE*/);
        }

        state.EndBlock(/*ANIMATION_DATA*/);
    }

    return 0; // no errors
}

//----------------------------------------------------------------------------
/**
*/
LWError
nLWObjectExportSettings::OnSave(const LWSaveState* lwSaveState)
{
    nLWSaveState state(lwSaveState);
    state.WriteString(this->exportNodeType);
    state.WriteBool(this->exportSceneNodeHierarchy);

    // animation stuff
    state.BeginBlock(FileBlockId::ANIMATION_DATA, false);

    state.BeginBlock(FileBlockId::ANIM_VAR, true);
    state.WriteString(this->animationVarName);
    state.EndBlock();

    for (int i = 0; i < this->animationStateArray.Size(); i++)
    {
        const nLWAnimationState& animState = this->animationStateArray[i];
        state.BeginBlock(FileBlockId::ANIM_STATE, false);

        state.BeginBlock(FileBlockId::ANIM_STATE_NAME, true);
        state.WriteString(animState.name);
        state.EndBlock();

        state.BeginBlock(FileBlockId::ANIM_STATE_SF, true);
        state.WriteInt(animState.startFrame);
        state.EndBlock();

        state.BeginBlock(FileBlockId::ANIM_STATE_EF, true);
        state.WriteInt(animState.endFrame);
        state.EndBlock();

        state.BeginBlock(FileBlockId::ANIM_STATE_FIT, true);
        state.WriteFloat(animState.fadeInTime);
        state.EndBlock();

        state.BeginBlock(FileBlockId::ANIM_STATE_LOOP, true);
        state.WriteBool(animState.repeat);
        state.EndBlock();

        state.EndBlock(/*FileBlockId::ANIM_STATE*/);
    }
    state.EndBlock(/*FileBlockId::ANIMATION_DATA*/);

    return 0; // no errors
}

//----------------------------------------------------------------------------
/**
*/
const char*
nLWObjectExportSettings::OnGetDescription()
{
    return "N2 Exporter Object Export Settings Handler";
}

//----------------------------------------------------------------------------
/**
*/
void
nLWObjectExportSettings::OnChangeID(const LWItemID* idList)
{
    nLWItemIDChangeList changeList(idList);
    LWItemID newItemId = LWITEM_NULL;
    if (changeList.GetNewId(this->lwItemId, newItemId))
    {
        // update the registry
        nLWSettingsRegistry* registry = nLWSettingsRegistry::Instance();
        if (registry)
        {
            registry->Update(this->lwItemId, newItemId);
        }
        this->lwItemId = newItemId;
    }
}

//----------------------------------------------------------------------------
/**
*/
LWError
nLWObjectExportSettings::OnDisplayUI()
{
    return this->DisplaySettingsPanel();
}

//----------------------------------------------------------------------------
/**
*/
XCALL_(LWInstance)
nLWObjectExportSettings::OnLWCreate(void* /*priv*/, void* context, LWError* error)
{
    // warning: in 7.5c at least Lightwave always passes in NULL for priv, even if
    //          specifically tell it to pass something else!

    LWItemID lwItemID = (LWItemID)context;
    n_assert(lwItemID);
    if (!lwItemID)
    {
        *error = "Item ID not supplied to nLWObjectExportSettings::OnCreate()";
        return 0;
    }

    // check if an object export settings handler is already attached to
    // this item, there should only be one per item
    if (nLWObjectExportSettings::HasHandler(lwItemID))
    {
        *error = "Only one Object Export Settings handler can be attached per item.";
        return 0;
    }

    return n_new(nLWObjectExportSettings(lwItemID));
}

//----------------------------------------------------------------------------
/**
*/
XCALL_(LWError)
nLWObjectExportSettings::OnLWCopy(LWInstance dest, LWInstance source)
{
    // TODO
    return 0;
}

//----------------------------------------------------------------------------
// EOF
//----------------------------------------------------------------------------
