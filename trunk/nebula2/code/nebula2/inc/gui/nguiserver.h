#ifndef N_GUISERVER_H
#define N_GUISERVER_H
//-----------------------------------------------------------------------------
/**
    @class nGuiServer
    @ingroup NebulaGuiSystem
    @brief Handle user interfaces.

    (C) 2001 RadonLabs GmbH
*/
#include "kernel/nroot.h"
#include "kernel/ndynautoref.h"
#include "mathlib/vector.h"
#include "mathlib/rectangle.h"
#include "gfx2/nmesh2.h"
#include "gfx2/nshader2.h"
#include "gui/nguievent.h"

class nGuiWindow;
class nGuiWidget;
class nGfxServer2;
class nInputServer;
class nGuiToolTip;
class nScriptServer;
class nGuiSkin;

//------------------------------------------------------------------------------
class nGuiServer : public nRoot
{
public:
    /// constructor
    nGuiServer();
    /// destructor
    virtual ~nGuiServer();

    /// create a window of given class name
    nGuiWindow* NewWindow(const char* className, bool visible);
    /// create a new skin
    nGuiSkin* NewSkin(const char* skinName);
    /// set gui root directory in Nebula object hierarchy
    void SetRootPath(const char* name);
    /// get gui root directory in Nebula object hierarchy
    const char* GetRootPath() const;
    /// set optionsl gui resource postfix
    void SetResourcePostfix(const char* post);
    /// get optional gui resource postfix
    const char* GetResourcePostfix() const;
    /// set the current skin
    void SetSkin(nGuiSkin* skin);
    /// get the current skin
    nGuiSkin* GetSkin() const;
    /// set the system skin
    void SetSystemSkin(nGuiSkin* skin);
    /// get the system skin
    nGuiSkin* GetSystemSkin() const;
    /// open the gui server
    bool Open();
    /// close the gui server
    void Close();
    /// set reference display size
    void SetReferenceSize(const vector2& s);
    /// get reference display size
    const vector2& GetReferenceSize() const;
    /// set the current root window by its name
    void SetRootWindow(const char* name);
    /// get the name of current root window
    const char* GetRootWindow() const;
    /// set pointer to current root window
    void SetRootWindowPointer(nGuiWindow* window);
    /// get a pointer to the current root window
    nGuiWindow* GetRootWindowPointer() const;
    /// set current time
    void SetTime(nTime t);
    /// get current time
    nTime GetTime() const;
    /// Per frame actions.
    virtual void Trigger();
    /// render the current gui
    virtual void Render();
    /// render audio effects
    virtual void RenderAudio();
    /// add a font definition using a system font
    void AddSystemFont(const char* fontName, const char* typeFace, int height, bool bold, bool italic, bool underline);
    /// add a font definition using a custom font
    void AddCustomFont(const char* fontName, const char* fontFile, const char* typeFace, int height, bool bold, bool italic, bool underline);
    /// convert a screen space coord to a view space coord
    vector3 ConvertScreenToViewSpace(const vector2& screenCoord);
    /// convert a screen space rectangle to a view space matrix
    matrix44 GetViewSpaceMatrix(const rectangle& r);
    /// directly draw a textured rectangle
    void DrawTexture(const rectangle& rect, const rectangle& uvRect, const vector4& color, nTexture2* tex);
    /// draw a brush of the current skin
    void DrawBrush(const rectangle& r, const char* brushName);
    /// convert a ref space rectangle to screen space
    rectangle ConvertRefToScreenSpace(const rectangle& src);
    /// convert a screen space rectangle to ref space
    rectangle ConvertScreenToRefSpace(const rectangle& src);
    /// compute brush size in screen space, assume 1:1 texel/pixel mapping
    vector2 ComputeScreenSpaceBrushSize(const char* brushName);
    /// check if mouse is over a GUI element
    bool IsMouseOverGui() const;
    /// display tooltip window
    void ShowToolTip(const char* text, const vector4& textColor);
    /// get current mouse pos
    const vector2& GetMousePos() const;
    /// run a widget script command
    bool RunCommand(nGuiWidget* who, const nString& cmd);
    /// put an event, will be distributed to all listeners
    void PutEvent(const nGuiEvent& event);
    /// register an event listener
    void RegisterEventListener(nGuiWidget* w);
    /// unregister an event listener
    void UnregisterEventListener(nGuiWidget* w);
    /// toggle the system GUI
    void ToggleSystemGui();
    /// set the current overall modulation color
    void SetGlobalColor(const vector4& c);
    /// get the current overall modulation color
    const vector4& GetGlobalColor() const;

private:
    /// validate embedded rectangle mesh
    void ValidateMesh();
    /// validate embedded shader
    void ValidateShader();
    /// update the mesh geometry
    void UpdateMesh(const rectangle& uvs);
    /// hide tooltip window
    void HideToolTip();
    /// return true if tooltip window currently visible
    bool IsToolTipShown() const;

    nAutoRef<nGfxServer2>   refGfxServer;
    nDynAutoRef<nRoot>      refGui;
    nAutoRef<nInputServer>  refInputServer;
    nAutoRef<nScriptServer> refScriptServer;
    nRef<nGuiWindow>        refCurrentRootWindow;
    nRef<nGuiToolTip>       refToolTip;
    nRef<nFont2>            refDefaultFont;
    nRef<nFont2>            refSmallFont;
    nRef<nRoot>             refSkins;
    nRef<nGuiSkin>          refCurrentSkin;

    uint uniqueId;
    bool isOpen;
    vector2 curMousePos;
    vector2 referenceSize;
    nTime curTime;
    nRef<nMesh2> refMesh;
    nRef<nShader2> refShader;
    nString resourcePostfix;
    vector4 globalColor;

    nArray< nRef<nGuiWidget> > eventListeners;

    // system GUI stuff
    nRef<nGuiWindow> refSystemRootWindow;       // the default system root window
    nRef<nGuiSkin>   refSystemSkin;             // the system skin
    nRef<nGuiWindow> refUserRootWindow;         // used as backup while system gui visible
    nRef<nGuiSkin>   refUserSkin;               // used as backup while system gui visible
    bool systemGuiActive;
};

//-----------------------------------------------------------------------------
/**
*/
inline
void
nGuiServer::SetGlobalColor(const vector4& c)
{
    this->globalColor = c;
}

//-----------------------------------------------------------------------------
/**
*/
inline
const vector4&
nGuiServer::GetGlobalColor() const
{
    return this->globalColor;
}

//-----------------------------------------------------------------------------
/**
*/
inline
const vector2&
nGuiServer::GetMousePos() const
{
    return this->curMousePos;
}

//-----------------------------------------------------------------------------
/**
*/
inline
void
nGuiServer::SetResourcePostfix(const char* pre)
{
    this->resourcePostfix = pre;
}

//-----------------------------------------------------------------------------
/**
*/
inline
const char*
nGuiServer::GetResourcePostfix() const
{
    return this->resourcePostfix.Get();
}

//-----------------------------------------------------------------------------
/**
    Set the current skin (can be 0).
*/
inline
void
nGuiServer::SetSkin(nGuiSkin* skin)
{
    n_assert(skin);
    this->refCurrentSkin = skin;
}

//-----------------------------------------------------------------------------
/**
    Get pointer to the current skin object.
*/
inline
nGuiSkin*
nGuiServer::GetSkin() const
{
    return this->refCurrentSkin.isvalid() ? this->refCurrentSkin.get() : 0;
}

//-----------------------------------------------------------------------------
/**
    Set the system skin.
*/
inline
void
nGuiServer::SetSystemSkin(nGuiSkin* skin)
{
    n_assert(skin);
    this->refSystemSkin = skin;
}

//-----------------------------------------------------------------------------
/**
    Get the system skin.
*/
inline
nGuiSkin*
nGuiServer::GetSystemSkin() const
{
    return this->refSystemSkin.isvalid() ? this->refSystemSkin.get() : 0;
}

//-----------------------------------------------------------------------------
#endif
