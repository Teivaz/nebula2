//------------------------------------------------------------------------------
//  ntextshapenode_main.cc
//  (C) 2006 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "scene/ntextshapenode.h"
#include "gfx2/nfontdesc.h"

nNebulaClass(nTextShapeNode, "nshapenode");

//------------------------------------------------------------------------------
/**
*/
nTextShapeNode::nTextShapeNode()
{
    this->transformNodeClass = nKernelServer::Instance()->FindClass("ntransformnode");
    this->fontFlags = nFont2::NoClip | nFont2::Center | nFont2::VCenter;
}

//------------------------------------------------------------------------------
/**
*/
nTextShapeNode::~nTextShapeNode()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Load the resources needed by this object.
*/
bool
nTextShapeNode::LoadResources()
{
    if (nShapeNode::LoadResources())
    {
        return this->LoadFont();
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Unload the resources if refcount has reached zero.
*/
void
nTextShapeNode::UnloadResources()
{
    nShapeNode::UnloadResources();
    this->UnloadFont();
}

//------------------------------------------------------------------------------
/**
    Load font resource
    Resource name:  TypeFace_Height_Weight_ItalicUnderlineAntiAliased
                    i.E.:   Arial_10_3_001
                            Arial, Height = 10, Weight = Normal,
                            Italic = false, Underline = false, AntiAliased = true
*/
bool
nTextShapeNode::LoadFont()
{
    if (!this->refFont.isvalid())
    {
        // get render flags
        this->fontFlags = nFont2::StringToRenderFlag(this->GetStringAttr("rlGuiHAlignment"));
        this->fontFlags |= nFont2::StringToRenderFlag(this->GetStringAttr("rlGuiVAlignment"));

        // append mesh usage to mesh resource name
        nString resourceName;

        nFontDesc fontDesc;
        fontDesc.SetItalic(this->GetBoolAttr("rlGuiItalic"));
        fontDesc.SetAntiAliased(this->GetBoolAttr("rlGuiAntiAliased"));
        fontDesc.SetUnderline(this->GetBoolAttr("rlGuiUnderline"));
        fontDesc.SetHeight(this->GetIntAttr("rlGuiHeight"));
        fontDesc.SetTypeFace(this->GetStringAttr("rlGuiTypeFace").Get());
        fontDesc.SetWeight(nFontDesc::StringToWeight(this->GetStringAttr("rlGuiWeight")));
        fontDesc.SetFilename(this->GetStringAttr("rlGuiFilePath").Get());

        // FIXME:
        // Filename is not included in the resource name
        resourceName.Format("%s_%u_%u_%u%u%u",
                            fontDesc.GetTypeFace(),
                            fontDesc.GetHeight(),
                            fontDesc.GetWeight(),
                            fontDesc.GetItalic(),
                            fontDesc.GetUnderline(),
                            fontDesc.GetAntiAliased());

        // get a new or shared font
        nFont2* font = nGfxServer2::Instance()->NewFont(resourceName.Get(), fontDesc);
        n_assert(font);
        if (!font->IsLoaded())
        {
            if (!font->Load())
            {
                n_printf("nMeshNode: Error loading font '%s'\n", resourceName.Get());
                font->Release();
                return false;
            }
        }
        this->refFont = font;
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    Unload font resource if valid.
*/
void
nTextShapeNode::UnloadFont()
{
    if (this->refFont.isvalid())
    {
        this->refFont->Release();
        this->refFont.invalidate();
    }
}

//------------------------------------------------------------------------------
/**
    Perform pre-instancing actions needed for rendering geometry. This
    is called once before multiple instances of this shape node are
    actually rendered.
*/
bool
nTextShapeNode::ApplyGeometry(nSceneServer* sceneServer)
{
    n_assert(this->refFont->IsValid());

    // set font
    nGfxServer2::Instance()->SetFont(this->refFont);

    return nShapeNode::ApplyGeometry(sceneServer);
}


//------------------------------------------------------------------------------
/**
    Update the screen space rectangle. This gets the bounding box from
    our Nebula2 node, resolves the hierarchy transforms, and transforms
    the result to screen space.
*/
void
nTextShapeNode::UpdateScreenSpaceRect()
{
    // compute flattened transformation matrix
    matrix44 m = this->GetTransform();
    nTransformNode* parentNode = (nTransformNode*) this->GetParent();
    while (parentNode && parentNode->IsA(this->transformNodeClass))
    {
        m = m * parentNode->GetTransform();
        parentNode = (nTransformNode*) parentNode->GetParent();
    }

    // multiply by orthogonal projection matrix, this must be the same
    // as used by the GUI shader!
    const static matrix44 orthoProj(0.5f, 0.0f,    0.0f, 0.0f,
                                    0.0f, 0.6667f, 0.0f, 0.0f,
                                    0.0f, 0.0f,   -0.5f, 0.0f,
                                    0.0f, 0.0f,    0.5f, 1.0f);
    m = m * orthoProj;

    // get local bounding box and transform to screen space
    bbox3 box = this->GetLocalBox();
    box.transform_divw(m);

    // build screen space rectangle from result, move the range
    // from (-1.0, 1.0) to (0.0, 1.0)
    vector2 vmin, vmax;
    vmin.x = (box.vmin.x + 1.0f) * 0.5f;
    vmin.y = 1.0f - ((box.vmax.y + 1.0f) * 0.5f);
    vmax.x = (box.vmax.x + 1.0f) * 0.5f;
    vmax.y = 1.0f - ((box.vmin.y + 1.0f) * 0.5f);
    this->screenSpaceRect.set(vmin, vmax);
}

//------------------------------------------------------------------------------
/**
    Render the text Immediate with the given font.
    Reset the font to the original font after rendering.
*/
bool
nTextShapeNode::RenderGeometry(nSceneServer* sceneServer, nRenderContext* renderContext)
{
    // update text element rectangle
    this->UpdateScreenSpaceRect();

    nString text = this->GetStringAttr("rlGuiText");
    vector4 color = this->GetVector4Attr("rlGuiColor");
    // FIXME: Alpha value not exported
    color.w = 1.0f;

    nGfxServer2::Instance()->DrawText(text, color, this->screenSpaceRect, this->fontFlags, true);

    return nShapeNode::RenderGeometry(sceneServer, renderContext);
}