//-----------------------------------------------------------------------------
//  nguiserver_main.cc
//  (C) 2001 RadonLabs GmbH
//-----------------------------------------------------------------------------
#include "gui/nguiserver.h"
#include "gui/nguiwindow.h"
#include "gui/nguitooltip.h"
#include "kernel/nkernelserver.h"
#include "kernel/nscriptserver.h"
#include "input/ninputserver.h"
#include "gfx2/ngfxserver2.h"
#include "gfx2/nfontdesc.h"
#include "gfx2/nmesh2.h"
#include "gfx2/nshader2.h"
#include "gfx2/nfontdesc.h"
#include "gui/nguiskin.h"

nNebulaScriptClass(nGuiServer, "nroot");

//-----------------------------------------------------------------------------
/**
*/
nGuiServer::nGuiServer() :
    refScriptServer("/sys/servers/script"),
    refGfxServer("/sys/servers/gfx"),
    refGui("/res/gui"),
    refInputServer("/sys/servers/input"),
    isOpen(false),
    curTime(0.0),
    referenceSize(1.0f, 1.0f),
    uniqueId(0),
    systemGuiActive(false),
    globalColor(1.0f, 1.0f, 1.0f, 1.0f)
{
    // empty
}

//-----------------------------------------------------------------------------
/**
*/
nGuiServer::~nGuiServer()
{
    if (this->isOpen)
    {
        this->Close();
    }
}

//-----------------------------------------------------------------------------
/**
    Set the root directory name in Nebula's object hierarchy. This is
    where the window objects are located. Default is "/res/gui".
*/
void
nGuiServer::SetRootPath(const char* name)
{
    n_assert(name);
    this->refGui = name;
}

//-----------------------------------------------------------------------------
/**
    Get the root directory.
*/
const char*
nGuiServer::GetRootPath() const
{
    return this->refGui.getname();
}

//-----------------------------------------------------------------------------
/**
    Update the mesh's uv coordinates. FIXME: this is bad behaviour,
    since the mesh has been allocated with WriteOnce behaviour!
*/
void
nGuiServer::UpdateMesh(const rectangle& uvs)
{
    float* vPtr = this->refMesh->LockVertices();
    n_assert(vPtr);

    *vPtr++ = -0.5f; *vPtr++ = 0.5f; *vPtr++ = 0.0f; 
    *vPtr++ = 0.0f;  *vPtr++ = 0.0f; *vPtr++ = 1.0f;
    *vPtr++ = uvs.v0.x; *vPtr++ = 1.0f - uvs.v1.y;
    
    *vPtr++ = -0.5f; *vPtr++ = -0.5f; *vPtr++ = 0.0f; 
    *vPtr++ = 0.0f;  *vPtr++ = 0.0f; *vPtr++ = 1.0f;        
    *vPtr++ = uvs.v0.x; *vPtr++ = 1.0f - uvs.v0.y;
    
    *vPtr++ = 0.5f;  *vPtr++ =  0.5f; *vPtr++ = 0.0f; 
    *vPtr++ = 0.0f;  *vPtr++ = 0.0f; *vPtr++ = 1.0f;        
    *vPtr++ = uvs.v1.x; *vPtr++ = 1.0f - uvs.v1.y;
    
    *vPtr++ = 0.5f;  *vPtr++ = -0.5f; *vPtr++ = 0.0f; 
    *vPtr++ = 0.0f;  *vPtr++ = 0.0f; *vPtr++ = 1.0f;        
    *vPtr++ = uvs.v1.x; *vPtr++ = 1.0f - uvs.v0.y;

    this->refMesh->UnlockVertices();

    ushort* iPtr = this->refMesh->LockIndices();
    n_assert(iPtr);
    *iPtr++ = 0; *iPtr++ = 1; *iPtr++ = 2;
    *iPtr++ = 1; *iPtr++ = 3; *iPtr++ = 2;
    this->refMesh->UnlockIndices();
}

//-----------------------------------------------------------------------------
/**
    Initialize the rectangle mesh. This method should be called per
    frame because the mesh may become invalid (for instance when 
    switching display modes).
*/
void
nGuiServer::ValidateMesh()
{
    nMesh2* mesh = 0;
    if (!this->refMesh.isvalid())
    {
        mesh = this->refGfxServer->NewMesh(0);
        mesh->SetUsage(nMesh2::WriteOnly);
        mesh->SetRefillBuffersMode(nMesh2::Enabled);
        mesh->SetNumVertices(4);
        mesh->SetNumIndices(6);
        mesh->SetVertexComponents(nMesh2::Coord | nMesh2::Normal | nMesh2::Uv0);
        this->refMesh = mesh;
    }
    else
    {
        mesh = this->refMesh.get();
    }

    if (!mesh->IsValid() || (nMesh2::NeededNow == mesh->GetRefillBuffersMode()))
    {
        bool success = mesh->Load();
        n_assert(success);
        rectangle uvs(vector2(0.0f, 0.0f), vector2(1.0f, 1.0f));
        this->UpdateMesh(uvs);
        mesh->SetRefillBuffersMode(nMesh2::Enabled);
    }
}

//-----------------------------------------------------------------------------
/**
    Initialize the gui shader. This method must be called per frame
    because the shader may become invalid (for instance when switching
    display modes).
*/
void
nGuiServer::ValidateShader()
{
    nShader2* shader = 0;
    if (!this->refShader.isvalid())
    {
        shader = this->refGfxServer->NewShader(0);
        shader->SetFilename("shaders:gui.fx");
        this->refShader = shader;
    }
    else
    {
        shader = this->refShader.get();
    }
    if (!shader->IsValid())
    {
        bool guiShaderLoaded = shader->Load();
        n_assert(guiShaderLoaded);
    }
}

//-----------------------------------------------------------------------------
/**
    Create a new window object.
*/
nGuiWindow*
nGuiServer::NewWindow(const char* className, bool visible)
{
    n_assert(this->isOpen);
    n_assert(className);

    // tell old focus window that it is about to lose focus
    if (this->refCurrentRootWindow.isvalid())
    {
        nGuiWindow* curFocusWindow = this->refCurrentRootWindow->GetTopMostWindow();
        if (curFocusWindow)
        {
            curFocusWindow->OnLoseFocus();
        }
    }

    // create new window, if no root window is set, creates a new root window
    char windowName[N_MAXPATH];
    sprintf(windowName, "window%d", this->uniqueId++);
    if (this->refCurrentRootWindow.isvalid())
    {
        kernelServer->PushCwd(this->refCurrentRootWindow.get());
    }
    else
    {
        kernelServer->PushCwd(this->refGui.get());
    }
    nGuiWindow* window = (nGuiWindow*) kernelServer->New(className, windowName);
    kernelServer->PopCwd();
    if (window)
    {
        if (visible)
        {
            window->Show();
        }
        if (this->refCurrentRootWindow.isvalid())
        {
            this->refCurrentRootWindow->SetFocusWindow(window);
        }
    }
    return window;
}

//-----------------------------------------------------------------------------
/**
    Create a new skin object.
*/
nGuiSkin*
nGuiServer::NewSkin(const char* skinName)
{
    n_assert(this->isOpen);
    n_assert(skinName);

    if (this->refSkins->Find(skinName))
    {
        n_error("nGuiServer::NewSkin(): skin '%s' already exists!", skinName);
        return 0;
    }

    kernelServer->PushCwd(this->refSkins.get());
    nGuiSkin* newSkin = (nGuiSkin*) kernelServer->New("nguiskin", skinName);
    kernelServer->PopCwd();
    n_assert(newSkin);
    return newSkin;
}

//-----------------------------------------------------------------------------
/**
    Open the gui server. This will initialize any required resources.
*/
bool
nGuiServer::Open()
{
    n_assert(!this->isOpen);
    n_assert(!this->refMesh.isvalid());
    n_assert(!this->refShader.isvalid());

    // create a few default fonts
    nFontDesc defaultFont;
    defaultFont.SetAntiAliased(true);
    defaultFont.SetHeight(16);
    defaultFont.SetWeight(nFontDesc::Normal);
    defaultFont.SetTypeFace("Arial");
    this->refDefaultFont = this->refGfxServer->NewFont("GuiDefault", defaultFont);
    if ((!this->refDefaultFont->IsValid()) && (!this->refDefaultFont->Load()))
    {
        n_error("nGuiServer: Failed to load default gui font!");
    }
    nFontDesc smallFont;
    smallFont.SetAntiAliased(true);
    smallFont.SetHeight(14);
    smallFont.SetWeight(nFontDesc::Normal);
    smallFont.SetTypeFace("Arial");
    this->refSmallFont = this->refGfxServer->NewFont("GuiSmall", smallFont);
    if ((!this->refSmallFont->Load()) && (!this->refSmallFont->Load()))
    {
        n_error("nGuiServer: Failed to load small gui font!");
    }

    if (!this->refGui.isvalid())
    {
        kernelServer->PushCwd(kernelServer->Lookup("/"));
        kernelServer->New("nroot", this->GetRootPath());
        kernelServer->PopCwd();
        n_assert(this->refGui.isvalid());
    }

    // create skin location
    this->refSkins = kernelServer->New("nroot", "/sys/share/rsrc/skins");
    n_assert(this->refSkins.isvalid());

    // create a default root window
    kernelServer->PushCwd(this->refGui.get());
    this->refSystemRootWindow = (nGuiWindow*) kernelServer->New("nguiwindow", "SystemRootWindow");
    kernelServer->PopCwd();
    this->SetRootWindow("SystemRootWindow");

    // initialize mesh and shader
    this->ValidateMesh();
    this->ValidateShader();
    this->isOpen = true;

    // call the OnGuiServerOpen script function
    const char* scriptResult;
    this->refScriptServer->RunFunction("OnGuiServerOpen", scriptResult);

    // set root window to 0, this will hide the GUI
    this->refCurrentRootWindow->Hide();
    this->SetRootWindow(0);

    return true;
}

//-----------------------------------------------------------------------------
/**
    Close the gui server.
*/
void
nGuiServer::Close()
{
    n_assert(this->isOpen);

    // call the OnGuiServerClose script function
    const char* scriptResult;
    this->refScriptServer->RunFunction("OnGuiServerClose", scriptResult);

    // clear root window
    this->SetRootWindow(0);

    // clear default system root window
    if (this->refSystemRootWindow.isvalid())
    {
        this->refSystemRootWindow->Release();
        n_assert(!this->refSystemRootWindow.isvalid());
    }

    this->refGfxServer->SetMesh(0);
    this->refGfxServer->SetShader(0);
    this->refGfxServer->SetFont(0);
    if (this->refMesh.isvalid())
    {
        this->refMesh->Release();
        this->refMesh = 0;
    }
    if (this->refShader.isvalid())
    {
        this->refShader->Release();
        this->refShader = 0;
    }
    if (this->refDefaultFont.isvalid())
    {
        this->refDefaultFont->Release();
        this->refDefaultFont = 0;
    }
    if (this->refSmallFont.isvalid())
    {
        this->refSmallFont->Release();
        this->refSmallFont = 0;
    }
    if (this->refSkins.isvalid())
    {
        this->refSkins->Release();
        n_assert(!this->refSkins.isvalid());
    }
    this->isOpen = false;
}

//-----------------------------------------------------------------------------
/**
    Set a new root window.
*/
void
nGuiServer::SetRootWindow(const char* name)
{
    nGuiWindow* window = 0;
    if (name)
    {
        window = (nGuiWindow*) this->refGui->Find(name);
        if (!window)
        {
            n_error("nGuiServer: window '%s' not found!", name);
        }
    }
    this->SetRootWindowPointer(window);
}

//-----------------------------------------------------------------------------
/**
    Get current root window.
*/
const char*
nGuiServer::GetRootWindow() const
{
    return this->refCurrentRootWindow.isvalid() ? this->refCurrentRootWindow->GetName() : 0;
}

//-----------------------------------------------------------------------------
/**
*/
void
nGuiServer::SetRootWindowPointer(nGuiWindow* window)
{
    this->refCurrentRootWindow = window;
    if (window && !window->IsShown())
    {
        window->Show();
    }
}

//-----------------------------------------------------------------------------
/**
    Returns a pointer to the current root window. May return 0 if no
    root window is set.
*/
nGuiWindow*
nGuiServer::GetRootWindowPointer() const
{
    return this->refCurrentRootWindow.isvalid() ? this->refCurrentRootWindow.get() : 0;
}

//-----------------------------------------------------------------------------
/**
   Per frame actions.
*/
void
nGuiServer::Trigger()
{
    if (this->refCurrentRootWindow.isvalid())
    {
        // the root window may change during event handling...
        nGuiWindow* rootWindow = this->refCurrentRootWindow.get();
        for (nInputEvent* i = this->refInputServer->FirstEvent(); i != 0; i = this->refInputServer->NextEvent(i))
        {
            n_assert(i != 0);

            if (i->GetDeviceId() == N_INPUT_MOUSE(0))
            {
                // handle mouse movement
                if (i->GetType() == N_INPUT_MOUSE_MOVE)
                {
                    this->curMousePos.set(i->GetRelXPos(), i->GetRelYPos());
                    rootWindow->OnMouseMoved(this->curMousePos);
                    if (this->refToolTip.isvalid())
                    {
                        this->refToolTip->OnMouseMoved(this->curMousePos);
                    }
                }

                // handle button actions
                if (i->GetType() == N_INPUT_BUTTON_DOWN)
                {
                    this->curMousePos.set(i->GetRelXPos(), i->GetRelYPos());
                    if (i->GetButton() == 0)
                    {
                        rootWindow->OnButtonDown(this->curMousePos);
                    }
                    else if (i->GetButton() == 1)
                    {
                        rootWindow->OnRButtonDown(this->curMousePos);
                    }
                }
                if (i->GetType() == N_INPUT_BUTTON_UP)
                {
                    this->curMousePos.set(i->GetRelXPos(), i->GetRelYPos());
                    if (i->GetButton() == 0)
                    {
                        rootWindow->OnButtonUp(this->curMousePos);
                    }
                    else if (i->GetButton() == 1)
                    {
                        rootWindow->OnRButtonUp(this->curMousePos);
                    }
                }
            }
            if (i->GetDeviceId() == N_INPUT_KEYBOARD(0))
            {
                // handle character input
                switch (i->GetType())
                {
                    case N_INPUT_KEY_CHAR:
                        rootWindow->OnChar(i->GetChar());
                        break;

                    case N_INPUT_KEY_DOWN:
                        rootWindow->OnKeyDown(i->GetKey());
                        break;

                    case N_INPUT_KEY_UP:
                        rootWindow->OnKeyUp(i->GetKey());
                        break;

                    default:
                        break;
                }
            }
        }

        // call the per-frame handler method
        rootWindow->OnFrame();

        // check for dismissed windows, and release them
        nGuiWidget* curWidget = (nGuiWidget*) rootWindow->GetHead();
        nGuiWidget* nextWidget;
        while (curWidget)
        {
            nextWidget = (nGuiWidget*) curWidget->GetSucc();
            if (curWidget->IsDismissed())
            {
                curWidget->Release();
            }
            curWidget = nextWidget;
        }

        // check for invalid listener references (listeners which
        // have disappeared and did not unregister themselves)
        nArray< nRef<nGuiWidget> >::iterator iter = this->eventListeners.Begin();
        for (; iter != this->eventListeners.End();)
        {
            if (!iter->isvalid())
            {
                iter = this->eventListeners.Erase(iter);
            }
            else
            {
                iter++;
            }
        }
    }
}

//-----------------------------------------------------------------------------
/**
    Render the current GUI.
*/
void
nGuiServer::Render()
{
    // set the current time for all following render calls
    if (this->refCurrentRootWindow.isvalid())
    {
        this->ValidateMesh();
        this->ValidateShader();

        nShader2* shader = this->refShader.get();
        this->refGfxServer->SetShader(shader);
        int curPass;
        int numPasses = shader->Begin();
        for (curPass = 0; curPass < numPasses; curPass++)
        {
            shader->Pass(curPass);
            this->refCurrentRootWindow->Render();
        }

        // render optional tooltip
        if (this->IsToolTipShown())
        {
            this->refToolTip->Render();
            this->HideToolTip();
        }
        shader->End();
    }
}

//-----------------------------------------------------------------------------
/**
    Render the audio effects.
*/
void
nGuiServer::RenderAudio()
{
    // set the current time for all following render calls
    if (this->refCurrentRootWindow.isvalid())
    {
        this->refCurrentRootWindow->RenderAudio();
    }
}

//-----------------------------------------------------------------------------
/**
    Declare a UI font using a system font.

    @param  fontName    a font name which identifies this font in the UI system
    @param  typeFace    the typeface name (i.e. "Arial")
    @param  height      height in pixels
    @param  bold        bold flag
    @param  italic      italic flag
    @param  underline   underline flag
*/
void
nGuiServer::AddSystemFont(const char* fontName, const char* typeFace, int height, bool bold, bool italic, bool underline)
{
    n_assert(fontName && typeFace);

    // fill a font desc...
    nFontDesc fontDesc;
    fontDesc.SetTypeFace(typeFace);
    fontDesc.SetHeight(height);
    if (bold)
    {
        fontDesc.SetWeight(nFontDesc::Bold);
    }
    else
    {
        fontDesc.SetWeight(nFontDesc::Normal);
    }
    fontDesc.SetItalic(italic);
    fontDesc.SetUnderline(underline);
    fontDesc.SetAntiAliased(true);
    
    // ... and create the font
    this->refGfxServer->NewFont(fontName, fontDesc);
}


//-----------------------------------------------------------------------------
/**
    Declare a UI font using a custom font file.

    @param  fontName    a font name which identifies this font in the UI system
    @param  fontFile    font filename 
    @param  typeFace    the typeface name (i.e. "Arial")
    @param  height      height in pixels
    @param  bold        bold flag
    @param  italic      italic flag
    @param  underline   underline flag
*/
void
nGuiServer::AddCustomFont(const char* fontName, const char* fontFile, const char* typeFace, int height, bool bold, bool italic, bool underline)
{
    n_assert(fontName && fontFile && typeFace);

    // fill a font desc...
    nFontDesc fontDesc;
    fontDesc.SetFilename(fontFile);
    fontDesc.SetTypeFace(typeFace);
    fontDesc.SetHeight(height);
    if (bold)
    {
        fontDesc.SetWeight(nFontDesc::Bold);
    }
    else
    {
        fontDesc.SetWeight(nFontDesc::Normal);
    }
    fontDesc.SetItalic(italic);
    fontDesc.SetUnderline(underline);
    fontDesc.SetAntiAliased(true);
    
    // ... and create the font
    this->refGfxServer->NewFont(fontName, fontDesc);
}

//-----------------------------------------------------------------------------
/**
*/
void
nGuiServer::SetTime(nTime t)
{
    this->curTime = t;
}

//-----------------------------------------------------------------------------
/**
*/
nTime
nGuiServer::GetTime() const
{
    return this->curTime;
}

//-----------------------------------------------------------------------------
/**
    Convert a screen space coordinate into a valid view space
    coordinate.
*/
vector3
nGuiServer::ConvertScreenToViewSpace(const vector2& screenCoord)
{
    // get current inverted projection matrix
    matrix44 invProj = this->refGfxServer->GetTransform(nGfxServer2::Projection);
    invProj.invert();

    vector3 screenCoord3D((screenCoord.x - 0.5f) * 2.0f, (screenCoord.y - 0.5f) * 2.0f, 1.0f);
    vector3 viewCoord = invProj * screenCoord3D;

    // get near plane
    float nearZ = this->refGfxServer->GetCamera().GetNearPlane();
    return viewCoord * nearZ * 1.1f;
}

//-----------------------------------------------------------------------------
/**
    Get a 4x4 matrix representing this widgets screen space area in view space.
*/
matrix44
nGuiServer::GetViewSpaceMatrix(const rectangle& r)
{
    vector3 v0 = this->ConvertScreenToViewSpace(r.v0);
    vector3 v1 = this->ConvertScreenToViewSpace(r.v1);

    vector3 pos  = (v0 + v1) * 0.5f;
    pos.y = -pos.y;
    vector3 size = (v1 - v0);
    matrix44 m;
    m.scale(size);
    m.translate(pos);
    return m;
}

//-----------------------------------------------------------------------------
/**
    Directly draw a textured rectangle to the screen.
*/
void
nGuiServer::DrawTexture(const rectangle& rect, const rectangle& uvRect, const vector4& color, nTexture2* tex)
{
    n_assert(tex);

    nGfxServer2* gfxServer = this->refGfxServer.get();

    // update gfx server's modelview matrix
    static const matrix44 identity;
    gfxServer->PushTransform(nGfxServer2::View, identity);
    gfxServer->PushTransform(nGfxServer2::Model, this->GetViewSpaceMatrix(rect));

    // compute modulated color
    vector4 modColor(color.x * this->globalColor.x,
                     color.y * this->globalColor.y,
                     color.z * this->globalColor.z,
                     color.w * this->globalColor.w);

    // update shader parameters
    nShader2* shader = this->refShader.get();
    if (shader->IsParameterUsed(nShader2::DiffMap0))
    {
        shader->SetTexture(nShader2::DiffMap0, tex);
    }
    if (shader->IsParameterUsed(nShader2::MatDiffuse))
    {
        shader->SetVector4(nShader2::MatDiffuse, modColor);
    }

    // render the mesh
    gfxServer->SetMesh(0);
    this->UpdateMesh(uvRect);
    gfxServer->SetMesh(this->refMesh.get());
    gfxServer->SetVertexRange(0, 4);
    gfxServer->SetIndexRange(0, 6);
    gfxServer->DrawIndexedNS(nGfxServer2::TriangleList);

    gfxServer->PopTransform(nGfxServer2::View);
    gfxServer->PopTransform(nGfxServer2::Model);
}

//-----------------------------------------------------------------------------
/**
    Draw a brush of the current skin.
*/
void
nGuiServer::DrawBrush(const rectangle& rect, const char* brushName)
{
    // a null brush pointer is valid, in this case, nothing is rendered
    if (0 == brushName)
    {
        return;
    }

    nGfxServer2* gfxServer = this->refGfxServer.get();

    // resolve brush name into gui resource
    if (!this->GetSkin())
    {
        n_error("nGuiServer::DrawBrush: need skin for brush rendering!");
    }
    
    nGuiResource* guiResource = this->GetSkin()->FindBrush(brushName);
    
    if (!guiResource)
    {
        //toggle gui and retry
        this->ToggleSystemGui();
        guiResource = this->GetSkin()->FindBrush(brushName);
        this->ToggleSystemGui();
    }
    
    if (guiResource)
    {
        // make sure gui resource is loaded
        if (!guiResource->IsValid())
        {
            bool success = guiResource->Load();
            n_assert(success);
        }
        this->DrawTexture(rect, guiResource->GetUvRect(), guiResource->GetColor(), guiResource->GetTexture());
    }
    else
    {
        nGuiSkin* userSkin = this->refUserSkin.isvalid() ? this->refUserSkin.get() : 0;
        nGuiSkin* systemSkin = this->GetSystemSkin();
        n_error("nGuiServer: brush '%s' not found in user skin '%s' nor system skin '%s' !\n", brushName, userSkin ? userSkin->GetName() : "<NO SKIN>", systemSkin->GetName());
    }
}

//-----------------------------------------------------------------------------
/**
    Set a coordinate space reference size.
*/
void
nGuiServer::SetReferenceSize(const vector2& v)
{
    this->referenceSize = v;
}

//-----------------------------------------------------------------------------
/**
    Get the current coordinate space reference size.
*/
const vector2&
nGuiServer::GetReferenceSize() const
{
    return this->referenceSize;
}

//-----------------------------------------------------------------------------
/**
    Convert a reference size rectangle (i.e. 640x480) to a screen space
    rectangle (0..1).
*/
rectangle
nGuiServer::ConvertRefToScreenSpace(const rectangle& src)
{
    rectangle r;
    r.v0.x = src.v0.x / this->referenceSize.x;
    r.v0.y = src.v0.y / this->referenceSize.y;
    r.v1.x = src.v1.x / this->referenceSize.x;
    r.v1.y = src.v1.y / this->referenceSize.y;
    return r;
}

//-----------------------------------------------------------------------------
/**
    Convert a screen space rectangle to reference space.
*/
rectangle
nGuiServer::ConvertScreenToRefSpace(const rectangle& src)
{
    rectangle r;
    r.v0.x = src.v0.x * this->referenceSize.x;
    r.v0.y = src.v0.y * this->referenceSize.y;
    r.v1.x = src.v1.x * this->referenceSize.x;
    r.v1.y = src.v1.y * this->referenceSize.y;
    return r;
}

//-----------------------------------------------------------------------------
/**
    Check if the mouse is currently over an GUI element.
*/
bool
nGuiServer::IsMouseOverGui() const
{
    if (this->refCurrentRootWindow.isvalid())
    {
        // if current topmost window is modal, always return true
        nGuiWindow* topMostWindow = this->refCurrentRootWindow->GetTopMostWindow();
        if (topMostWindow && topMostWindow->IsModal())
        {
            return true;
        }

        // check child widgets (including windows)
        nGuiWidget* widget;
        for (widget = (nGuiWidget*) this->refCurrentRootWindow->GetHead();
             widget;
             widget = (nGuiWidget*) widget->GetSucc())
        {
            if (widget->IsShown() && (widget->Inside(this->curMousePos)))
            {
                return true;
            }
        }
    }
    // fallthrough: mouse not over any gui element
    return false;
}

//-----------------------------------------------------------------------------
/**
    Display a tooltip under the mouse.
*/
void
nGuiServer::ShowToolTip(const char* text, const vector4& textColor)
{
    n_assert(text);
    if (!this->refToolTip.isvalid())
    {
        // initialize the optional tooltip
        this->refToolTip = (nGuiToolTip*) this->refGui->Find("Tooltip");
    }
    if (this->refToolTip.isvalid())
    {
        this->HideToolTip();
        this->refToolTip->SetText(text);
        this->refToolTip->SetColor(textColor);
        this->refToolTip->Show();
    }
}

//-----------------------------------------------------------------------------
/**
    Hide tooltip.
*/
void
nGuiServer::HideToolTip()
{
    if (this->refToolTip.isvalid())
    {
        if (this->refToolTip->IsShown())
        {
            this->refToolTip->Hide();
        }
    }
}

//-----------------------------------------------------------------------------
/**
    Return true if tooltip currently shown.
*/
bool
nGuiServer::IsToolTipShown() const
{
    if (this->refToolTip.isvalid())
    {
        return this->refToolTip->IsShown();
    }
    else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
/**
    Runs a widget script command. This is a helper method for widgets.
    Before calling the command, the widget will be made current object.
*/
bool
nGuiServer::RunCommand(nGuiWidget* who, const nString& cmd)
{
    n_assert(who);
    if (!cmd.IsEmpty())
    {
        const char* res;
        kernelServer->PushCwd(who);
        this->refScriptServer->Run(cmd.Get(), res);
        kernelServer->PopCwd();
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------
/**
    Register an event listener. Event listener widgets are notified 
    through the OnEvent() method when the PutEvent() method is invoked.
*/
void
nGuiServer::RegisterEventListener(nGuiWidget* w)
{
    nRef<nGuiWidget> refWidget(w);
    n_assert(!this->eventListeners.Find(refWidget));
    this->eventListeners.Append(refWidget);
}

//-----------------------------------------------------------------------------
/**
    Unregister an event listener.
*/
void
nGuiServer::UnregisterEventListener(nGuiWidget* w)
{
    nRef<nGuiWidget> refWidget(w);
    nArray< nRef<nGuiWidget> >::iterator iter = this->eventListeners.Find(refWidget);
    n_assert(0 != iter);
    this->eventListeners.Erase(iter);
}

//-----------------------------------------------------------------------------
/**
    Put an event. The event will immediately distributed to all event
    listeners.
*/
void
nGuiServer::PutEvent(const nGuiEvent& event)
{
    int num = this->eventListeners.Size();
    int i;
    for (i = 0; i < num; i++)
    {
        this->eventListeners[i]->OnEvent(event);
    }
}

//-----------------------------------------------------------------------------
/**
    Toggle the system GUI. This will toggle between the SystemRootWindow
    and the user set root window.
*/
void
nGuiServer::ToggleSystemGui()
{
    if (this->systemGuiActive)
    {
        nGuiWindow* userRootWindow = this->refUserRootWindow.isvalid() ? this->refUserRootWindow.get() : 0;
        nGuiSkin* userSkin = this->refUserSkin.isvalid() ? this->refUserSkin.get() : 0;
        this->SetSkin(userSkin);
        this->SetRootWindowPointer(userRootWindow);
        this->systemGuiActive = false;
    }
    else
    {
        this->refUserSkin = this->GetSkin();
        this->refUserRootWindow = this->GetRootWindowPointer();
        this->SetSkin(this->GetSystemSkin());
        this->SetRootWindowPointer(this->refSystemRootWindow.get());
        this->systemGuiActive = true;
    }
}

//-----------------------------------------------------------------------------
/**
    Computes the screen space size of a brush if a 1:1 mapping
    is assumed between brush texels and screen pixels. If the brush
    does not exist, or no skin is set, a null vector will be returned.
*/
vector2
nGuiServer::ComputeScreenSpaceBrushSize(const char* brushName)
{
    vector2 size;
    if (this->GetSkin() && brushName)
    {
        nGuiResource* guiResource = this->GetSkin()->FindBrush(brushName);
        if (!guiResource)
        {
            //toggle gui and retry
            this->ToggleSystemGui();
            guiResource = this->GetSkin()->FindBrush(brushName);
            this->ToggleSystemGui();
        }
        
        if (guiResource)
        {
            const rectangle& uvRect = guiResource->GetUvRect();
            float texWidth   = (float) guiResource->GetTextureWidth();
            float texHeight  = (float) guiResource->GetTextureHeight();
            const nDisplayMode2& dispMode = this->refGfxServer->GetDisplayMode();
            float dispWidth  = (float) dispMode.GetWidth();
            float dispHeight = (float) dispMode.GetHeight();
            size.x = (uvRect.width() * texWidth) / dispWidth;
            size.y = (uvRect.height() * texHeight) / dispHeight;
        }
        //fallthrough: brush not found
    }
    return size;
}
