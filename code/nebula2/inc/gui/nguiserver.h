#ifndef N_GUISERVER_H
#define N_GUISERVER_H
//-----------------------------------------------------------------------------
/**
    @class nGuiServer
    @ingroup Gui
    @brief The central server object of the Nebula2 user interface subsystem.

    (C) 2001 RadonLabs GmbH
*/
#include "kernel/nroot.h"
#include "kernel/ndynautoref.h"
#include "mathlib/vector.h"
#include "mathlib/rectangle.h"
#include "gfx2/ndynamicmesh.h"
#include "gfx2/nshader2.h"
#include "gui/nguievent.h"
#include "gui/nguiskin.h"
#if __NEBULA_STATS__
#include "kernel/nprofiler.h"
#endif

class nGuiWindow;
class nGuiWidget;
class nGfxServer2;
class nInputServer;
class nGuiToolTip;
class nScriptServer;
class nGuiDragBox;
class nGuiMouseCursor;

//------------------------------------------------------------------------------
class nGuiServer : public nRoot
{
public:
    /// constructor
    nGuiServer();
    /// destructor
    virtual ~nGuiServer();
    /// get instance pointer
    static nGuiServer* Instance();

    /// set display resolution to use for layout computations
    void SetDisplaySize(const vector2& s);
    /// get display resolution
    const vector2& GetDisplaySize() const;
    /// set the texel mapping ratio (default 1.0)
    void SetTexelMappingRatio(float r);
    /// get the texel mapping ratio
    float GetTexelMappingRatio() const;
    /// create a window of given class name
    nGuiWindow* NewWindow(const char* className, bool visible);
    /// create a new skin
    nGuiSkin* NewSkin(const char* skinName);
    /// immediatly release all windows under the current root window
    void DiscardWindows(const char* className);
    /// find window of given class under root window
    nGuiWindow* FindWindowByClass(const char* className, nGuiWindow* curWindow);
    /// set gui root directory in Nebula object hierarchy
    void SetRootPath(const char* name);
    /// get gui root directory in Nebula object hierarchy
    const char* GetRootPath() const;
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
    void DrawBrush(const rectangle& r, nGuiBrush& brush);
    /// compute brush size in screen space, assume 1:1 texel/pixel mapping
    vector2 ComputeScreenSpaceBrushSize(const char* brushName);
    /// return true iff the named brush exists in the current skin
    bool BrushExists(const char* brushName);
    /// push clipping rectangle into stack
    void PushClipRect(rectangle& cr);
    /// remove clipping rectangle from the top of stack
    void PopClipRect();
    /// get clipping rectangle from the top of stack
    bool GetClipRect(rectangle& cr) const;
    /// get the size of 1 pixel
    vector2 GetPixelSize() const;
    /// check if mouse is over a GUI element
    bool IsMouseOverGui() const;
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
    /// play a gui sound
    void PlaySound(const char* name);
    /// draw text, call this instead of nGfxServer2::DrawText()!
    void DrawText(const char* text, const vector4& color, const rectangle& rect, uint flags);
    /// move the rectangle so that it is fully visible
    void MoveRectToVisibleArea(rectangle& r) const;
    /// set pointer to optional global drag box object
    void SetDragBox(nGuiDragBox* dragBox);
    /// get pointer to optional global drag box object
    nGuiDragBox* GetDragBox() const;
    /// set tool tip enable
    void SetToolTipEnable(bool enable);
    /// is tool tip enabled ?
    bool IsToolTipEnabled() const;
    /// get tool tip activation time
    nTime GetToolTipActivationTime() const;
    /// get tool tip fade in time
    nTime GetToolTipFadeInTime() const;
    /// display tooltip window
    void ShowToolTip(const char* text, const vector4& textColor);
    /// is gui system window enabled ?
    bool IsSystemGuiEnabled() const;
    /// set gui mouse cursor
    void SetGuiMouseCursor(nGuiMouseCursor* cursor);
    /// get gui mouse cursor
    nGuiMouseCursor* GetGuiMouseCursor();

private:
    friend class nRpPass;

    /// render the current gui
    virtual void Render();
    /// validate embedded rectangle mesh
    void ValidateMesh();
    /// validate embedded shader
    void ValidateShader();
    /// update the mesh geometry
    void UpdateMesh(const rectangle& uvs);
    /// return true if tooltip window currently visible
    bool IsToolTipShown() const;
    /// hide tooltip window
    void HideToolTip();
    /// flush brush rendering (before rendering text)
    void FlushBrushes();

    static nGuiServer* Singleton;

    nDynAutoRef<nRoot>      refGui;
    nAutoRef<nInputServer>  refInputServer;
    nAutoRef<nScriptServer> refScriptServer;
    nRef<nGuiWindow>        refCurrentRootWindow;
    nRef<nGuiToolTip>       refToolTip;
    nRef<nFont2>            refDefaultFont;
    nRef<nFont2>            refSmallFont;
    nRef<nRoot>             refSkins;
    nRef<nGuiSkin>          refCurrentSkin;
    nRef<nGuiDragBox>       refDragBox;
    nRef<nGuiMouseCursor>   refMouseCursor;
    
    nClass* guiWindowClass;

    uint uniqueId;
    bool isOpen;
    vector2 curMousePos;
    nTime curTime;
    vector4 globalColor;

    bool toolTipEnabled;
    bool toolTipSet;
    const nTime toolTipFadeInTime;
    const nTime toolTipActivationTime;

    bool guiMouseCursorEnabled;

    nArray< nRef<nGuiWidget> > eventListeners;

    // system GUI stuff
    nRef<nGuiWindow> refSystemRootWindow;       // the default system root window
    nRef<nGuiSkin>   refSystemSkin;             // the system skin
    nRef<nGuiWindow> refUserRootWindow;         // used as backup while system gui visible
    nRef<nGuiSkin>   refUserSkin;               // used as backup while system gui visible
    bool systemGuiActive;

    // dynamic mesh rendering
    nDynamicMesh dynMesh;
    nRef<nShader2> refShader;
    nTexture2* curTexture;
    vector4 curColor;
    int curMaxNumVertices;
    float* curVertexPointer;
    int curVertexIndex;

    vector2 displaySize;
    float texelMappingRatio;

    nArray<rectangle> clipRectStack;

#if __NEBULA_STATS__
    nProfiler profGUIDrawBrush;
    nProfiler profGUIDrawTexture;
    nProfiler profGUIDrawText;
#endif
};

//-----------------------------------------------------------------------------
/**
*/
inline
nGuiServer*
nGuiServer::Instance()
{
    n_assert(Singleton);
    return Singleton;
}

//-----------------------------------------------------------------------------
/**
*/
inline
void
nGuiServer::SetDisplaySize(const vector2& s)
{
    this->displaySize = s;
}

//-----------------------------------------------------------------------------
/**
*/
inline
const vector2&
nGuiServer::GetDisplaySize() const
{
    return this->displaySize;
}

//-----------------------------------------------------------------------------
/**
*/
inline
void
nGuiServer::SetTexelMappingRatio(float r)
{
    this->texelMappingRatio = r;
}

//-----------------------------------------------------------------------------
/**
*/
inline
float
nGuiServer::GetTexelMappingRatio() const
{
    return this->texelMappingRatio;
}

//-----------------------------------------------------------------------------
/**
*/
inline
nGuiDragBox*
nGuiServer::GetDragBox() const
{
    return this->refDragBox.isvalid() ? this->refDragBox.get() : 0;
}

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
/**
*/
inline
void
nGuiServer::SetToolTipEnable(bool enable)
{
    this->toolTipEnabled = enable;
}

//-----------------------------------------------------------------------------
/**
*/
inline
bool
nGuiServer::IsToolTipEnabled() const
{
    return this->toolTipEnabled;
}

//-----------------------------------------------------------------------------
/**
*/
inline
nTime
nGuiServer::GetToolTipActivationTime() const
{
    return this->toolTipActivationTime;
}

//-----------------------------------------------------------------------------
/**
*/
inline
nTime
nGuiServer::GetToolTipFadeInTime() const
{
    return this->toolTipFadeInTime;
}

//-----------------------------------------------------------------------------
/**
*/
inline
void
nGuiServer::PopClipRect()
{
    n_assert(this->clipRectStack.Size() > 0);

    int topRectIdx;
    topRectIdx = this->clipRectStack.Size() - 1;
    this->clipRectStack.Erase(topRectIdx);
}

//-----------------------------------------------------------------------------
/**
*/
inline
bool
nGuiServer::GetClipRect(rectangle& cr) const
{
    if (this->clipRectStack.Size() > 0)
    {
        cr = this->clipRectStack.Back();
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------
/**
*/
inline
bool
nGuiServer::IsSystemGuiEnabled() const
{
    return this->systemGuiActive;
}

//-----------------------------------------------------------------------------
/**
*/
inline
void
nGuiServer::SetGuiMouseCursor(nGuiMouseCursor* cursor)
{
    this->refMouseCursor = cursor;
}

//-----------------------------------------------------------------------------
#endif
