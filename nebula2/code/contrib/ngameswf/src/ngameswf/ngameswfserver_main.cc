//------------------------------------------------------------------------------
//  ngameswfserver_main.cc
//  (C) 2003, 2004 George McBay, Kim, Hyoun Woo
//------------------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "ngameswf/ngameswfserver.h"
#include "base/tu_file.h"

nNebulaScriptClass(nGameSwfServer, "nresourceloader");

//-----------------------------------------------------------------------------
/**
    Print internal log message of gameswf out.
*/
static 
void LogCallback (bool error, const char* message)
{
    n_printf ("gameswf log: %s\n", message);
}

//-----------------------------------------------------------------------------
/**
    
*/
static 
tu_file *OpenFile (const char* url)
{
    return new tu_file(url, "rb");
}


//-----------------------------------------------------------------------------
/**
    Constructor.
*/
nGameSwfServer::nGameSwfServer() : 
    refGfxServer("/sys/servers/gfx"),
    ccByteToFloat(1.0f / 255.0f),
    current_texture_id(0),
    strShaderRoot ("gswf:")
{
    int i=0;
}

//-----------------------------------------------------------------------------
/**
    Destructor.  
*/
nGameSwfServer::~nGameSwfServer()
{
    gameswf::clear ();
}

//-----------------------------------------------------------------------------
/**
    overrided from nRoot
*/
void nGameSwfServer::Initialize()
{
    nRoot::Initialize ();

    // Note: It is assumed nGameSwfServer will be a pseudo-singleton object
    // and only one will be initialized per process!
    gameswf::register_file_opener_callback(OpenFile);
    gameswf::register_log_callback(LogCallback);

    gameswf::set_render_handler(this);
}

//-----------------------------------------------------------------------------
/**
    Specify external callback function for responding 'fscommand' of 
    action-script.
*/
void nGameSwfServer::SetFSCommandCallback (gameswf::fscommand_callback handler)
{
    gameswf::register_fscommand_callback (handler);
}

//-----------------------------------------------------------------------------
/**
    Specify antialiasing.
*/
void nGameSwfServer::set_antialiased(bool enable)
{ 
    // Not yet supported.
}

//-----------------------------------------------------------------------------
/**
    Delete the given bitmap info struct.
*/
void nGameSwfServer::delete_bitmap_info(gameswf::bitmap_info* bi)
{
    delete bi;
}

//-----------------------------------------------------------------------------
/**
    Given an image, returns a pointer to a bitmap_info struct that can later 
    be passed to fill_styleX_bitmap(), to set a bitmap fill style.
*/
gameswf::bitmap_info* 
nGameSwfServer::create_bitmap_info_rgb(image::rgb* im)
{
    char resource_loader_path[N_MAXPATH];

    GetFullName(resource_loader_path, sizeof(resource_loader_path));

    bitmap_info_nebula *new_bi = new bitmap_info_nebula(im, refGfxServer.get(), 
                                                        resource_loader_path, 
                                                        current_texture_id++);

    bitmap_info_array.PushBack(new_bi);

    return new_bi;
}

//-----------------------------------------------------------------------------
/**
    Given an image, returns a pointer to a bitmap_info struct that can later 
    be passed to fill_style_bitmap(), to set a bitmap fill style.
*/
gameswf::bitmap_info* 
nGameSwfServer::create_bitmap_info_rgba(image::rgba* im)
{
    char resource_loader_path[N_MAXPATH];

    GetFullName(resource_loader_path, sizeof(resource_loader_path));

    bitmap_info_nebula *new_bi = new bitmap_info_nebula(im, refGfxServer.get(), 
                                                        resource_loader_path,
                                                        current_texture_id++);

    bitmap_info_array.PushBack(new_bi);

    return new_bi;
}

//-----------------------------------------------------------------------------
/**
    Create a placeholder bitmap_info.  Used when DO_NOT_LOAD_BITMAPS is set; 
    then later on the host program can use movie_definition::get_bitmap_info_count() 
    and movie_definition::get_bitmap_info() to stuff precomputed textures into 
    these bitmap infos.
*/
gameswf::bitmap_info* 
nGameSwfServer::create_bitmap_info_empty()
{
    //return new bitmap_info_nebula;

    bitmap_info_nebula* new_bi = new bitmap_info_nebula;
    bitmap_info_array.PushBack(new_bi);

    return new_bi;
}

//-----------------------------------------------------------------------------
/**
    Create a bitmap_info so that it contains an alpha texture with 
    the given data (1 byte per texel).
*/
gameswf::bitmap_info* 
nGameSwfServer::create_bitmap_info_alpha(int w, int h, uchar* data)
{
    char resource_loader_path[N_MAXPATH];

    GetFullName(resource_loader_path, sizeof(resource_loader_path));

    bitmap_info_nebula* new_bi;
    new_bi = new bitmap_info_nebula(w, h, data, refGfxServer.get (), 
                                    resource_loader_path, 
                                    current_texture_id++);

    bitmap_info_array.PushBack(new_bi);
    return new_bi;
}

//-----------------------------------------------------------------------------
/**
    Set the current transform for mesh & line-strip rendering.
*/
void nGameSwfServer::set_matrix(const gameswf::matrix& m)
{
    matrix44 cur_matrix;

    cur_matrix.M11 = m.m_[0][0];
    cur_matrix.M12 = m.m_[1][0];
    cur_matrix.M21 = m.m_[0][1];
    cur_matrix.M22 = m.m_[1][1];
    cur_matrix.M41 = m.m_[0][2];
    cur_matrix.M42 = m.m_[1][2];

    cur_matrix = cur_matrix * this->modelViewMatrix;

    matrix44 coordTransform = cur_matrix * this->projMatrix;

    refShapeShader->SetMatrix (nShader2::Parameter::ModelViewProjection, coordTransform);
    refShapeTexWrapShader->SetMatrix (nShader2::Parameter::ModelViewProjection, coordTransform);
    refShapeTexClampShader->SetMatrix(nShader2::Parameter::ModelViewProjection, coordTransform);
    refLineShader->SetMatrix (nShader2::Parameter::ModelViewProjection, coordTransform);
}

//-----------------------------------------------------------------------------
/**
    Set the current color transform for mesh & line-strip rendering.
*/
void nGameSwfServer::set_cxform(const gameswf::cxform& cx)
{
    current_cxform = cx;
}

//-----------------------------------------------------------------------------
/**
    Set fill style for the left interior of the shape.  If enable is false, 
    turn off fill for the left interior.
*/
void nGameSwfServer::fill_style_color(int fill_side, gameswf::rgba color)
{
    current_styles[fill_side].set_color (current_cxform.transform(color));

    if (fill_side == LEFT_STYLE)
    {
        float4 shapeColor;
        shapeColor[0] = current_styles[LEFT_STYLE].m_color.m_r * ccByteToFloat;
        shapeColor[1] = current_styles[LEFT_STYLE].m_color.m_g * ccByteToFloat;
        shapeColor[2] = current_styles[LEFT_STYLE].m_color.m_b * ccByteToFloat;
        shapeColor[3] = current_styles[LEFT_STYLE].m_color.m_a * ccByteToFloat;

        refShapeShader->SetFloatArray (nShader2::Parameter::MatDiffuse, shapeColor, 4);
    }
}

//-----------------------------------------------------------------------------
/**
    
*/
void nGameSwfServer::fill_style_bitmap(int fill_side,
                                       const gameswf::bitmap_info* bi,
                                       const gameswf::matrix& m,
                                       bitmap_wrap_mode wm)
{
    current_styles[fill_side].set_bitmap (bi, m, wm, current_cxform);

    if (fill_side == LEFT_STYLE)
    {
        const gameswf::matrix &m = current_styles[LEFT_STYLE].m_bitmap_matrix;

        float invWidth = (1.0f / current_styles[LEFT_STYLE].m_bitmap_info->m_original_width);
        float invHeight = 1.0f / current_styles[LEFT_STYLE].m_bitmap_info->m_original_height;

        float4 texGenS = { 0.0f, 0.0f, 0.0f, 0.0f };
        texGenS[0] = m.m_[0][0] * invWidth;
        texGenS[1] = m.m_[0][1] * invWidth;
        texGenS[3] = m.m_[0][2] * invWidth;

        float4 texGenT = { 0.0f, 0.0f, 0.0f, 0.0f };

        texGenT[0] = m.m_[1][0] * invHeight;
        texGenT[1] = m.m_[1][1] * invHeight;
        texGenT[3] = m.m_[1][2] * invHeight;

        float4 addColor;

        addColor[0] = current_styles[LEFT_STYLE].m_bitmap_color_transform.m_[0][0];
        addColor[1] = current_styles[LEFT_STYLE].m_bitmap_color_transform.m_[1][0];
        addColor[2] = current_styles[LEFT_STYLE].m_bitmap_color_transform.m_[2][0];
        addColor[3] = current_styles[LEFT_STYLE].m_bitmap_color_transform.m_[3][0];

        if (wm == WRAP_CLAMP)
        {
            refShapeTexClampShader->SetTexture    (nShader2::Parameter::DiffMap0, ((bitmap_info_nebula*)bi)->ref_texture.get());
            refShapeTexClampShader->SetFloatArray (nShader2::Parameter::TexGenS, texGenS, 4);
            refShapeTexClampShader->SetFloatArray (nShader2::Parameter::TexGenT, texGenT, 4);
            refShapeTexClampShader->SetFloatArray (nShader2::Parameter::MatDiffuse, addColor, 4); 
        }
        else
        {
            refShapeTexWrapShader->SetTexture    (nShader2::Parameter::DiffMap0, ((bitmap_info_nebula*)bi)->ref_texture.get());
            refShapeTexWrapShader->SetFloatArray (nShader2::Parameter::TexGenS, texGenS, 4);
            refShapeTexWrapShader->SetFloatArray (nShader2::Parameter::TexGenT, texGenT, 4);
            refShapeTexWrapShader->SetFloatArray (nShader2::Parameter::MatDiffuse, addColor, 4);
        }
    }
}

//-----------------------------------------------------------------------------
/**
    
*/
void nGameSwfServer::fill_style_disable(int fill_side)
{
    current_styles[fill_side].disable ();
}

//-----------------------------------------------------------------------------
/**
     Set the line style of the shape.  If enable is false, turn off lines for 
     following curve segments.
*/
void nGameSwfServer::line_style_color(gameswf::rgba color)
{
    current_styles[LINE_STYLE].set_color (current_cxform.transform(color));

    float4 lineColor;

    lineColor[0] = current_styles[LINE_STYLE].m_color.m_r * ccByteToFloat;
    lineColor[1] = current_styles[LINE_STYLE].m_color.m_g * ccByteToFloat;
    lineColor[2] = current_styles[LINE_STYLE].m_color.m_b * ccByteToFloat;
    lineColor[3] = current_styles[LINE_STYLE].m_color.m_a * ccByteToFloat;

    refLineShader->SetFloatArray (nShader2::MatDiffuse, lineColor, 4);
}

//-----------------------------------------------------------------------------
void nGameSwfServer::line_style_width(float width)
{
    // Not yet supported
}

//-----------------------------------------------------------------------------
void nGameSwfServer::line_style_disable()
{
    current_styles[LINE_STYLE].disable ();
}

//-----------------------------------------------------------------------------
void nGameSwfServer::begin_submit_mask ()
{
    // Not yet supported.
}

//-----------------------------------------------------------------------------
void nGameSwfServer::end_submit_mask ()
{
    // Not yet supported.
}

//-----------------------------------------------------------------------------
void nGameSwfServer::disable_mask ()
{
    // Not yet supported.
}


