//------------------------------------------------------------------------------
//  ngameswfserver.h
//  (C) 2003, 2004 George McBay, Kim, Hyoun Woo
//------------------------------------------------------------------------------
#ifndef N_GAMESWFSERVER_H
#define N_GAMESWFSERVER_H

//-----------------------------------------------------------------------------
/**
    @class nGameSwfServer
    @ingroup nGameSwfGroup

    @brief provide renderer interface which is overrided from 
           gameswf::render_handler

    @note It is assumed nGameSwfServer will be a pseudo-singleton object
          and only one will be initialized per process!
*/

#include "ngameswf/ngameswf_bmpinfo.h"

class nGameSwfServer : public nResourceLoader, 
                       public gameswf::render_handler
{
public:
    nGameSwfServer();
    virtual ~nGameSwfServer();
    
    virtual bool SaveCmds(nPersistServer* ps);
    
    virtual void Initialize();
    virtual void InitResources();

    virtual void SetFSCommandCallback (gameswf::fscommand_callback callback_handler);

    virtual bool Load(const char* filename, nResource* callingResource);

    virtual void begin_display(gameswf::rgba bkgColor,
                       int viewportX0, int viewportY0,
                       int viewportWidth, int viewportHeight,
                       float x0, float x1, float y0, float y1);
    virtual void end_display();

    virtual void set_antialiased(bool enable);
    virtual void set_matrix(const gameswf::matrix& m);
    virtual void set_cxform(const gameswf::cxform& cx);

    virtual void draw_mesh_strip(const void* coords, int vertex_count);
    virtual void draw_line_strip(const void* coords, int vertex_count);
    
    virtual void fill_style_disable(int fill_side);
    virtual void fill_style_color(int fill_side, gameswf::rgba color);
    virtual void fill_style_bitmap(int fill_side,
                                   const gameswf::bitmap_info* bi,
                                   const gameswf::matrix& m, 
                                   gameswf::render_handler::bitmap_wrap_mode wm);
    
    virtual void line_style_disable();
    virtual void line_style_color(gameswf::rgba color);
    virtual void line_style_width(float width);

    virtual void begin_submit_mask();
    virtual void end_submit_mask();
    virtual void disable_mask();

    virtual void draw_bitmap(const gameswf::matrix& m,
                             const gameswf::bitmap_info* bi,
                             const gameswf::rect& coords,
                             const gameswf::rect& uv_coords,
                             gameswf::rgba color);

    void delete_bitmap_info(gameswf::bitmap_info* bi);

    gameswf::bitmap_info* create_bitmap_info_rgb(image::rgb* im);
    gameswf::bitmap_info* create_bitmap_info_rgba(image::rgba* im);
    gameswf::bitmap_info* create_bitmap_info_empty();
    gameswf::bitmap_info* create_bitmap_info_alpha(int w, int h, uchar* data);

    void SetShaderPath (const char* path);
    const char* GetShaderPath () const;

    void SetUseCacheFile (bool bUse);

protected:
    enum {
        VERTEX_BUFFER_INITIAL_SIZE = 1000
    };

    nArray<bitmap_info_nebula*> bitmap_info_array;

    nAutoRef<nGfxServer2> refGfxServer;

    nRef<nShader2> refShapeShader; //< shader for shape object.
    nRef<nMesh2>   refShapeMesh; //< mesh for shape object.

    nRef<nMesh2>   refBkgMesh; //< mesh for background object.

    nRef<nShader2> refShapeTexClampShader; //< shader for clamped texture object.
    nRef<nShader2> refShapeTexWrapShader;  //< shader for wrapped texture object.

    nRef<nShader2> refBitmapShader; //< shader for bitmap object.
    nRef<nMesh2>   refBitmapMesh; //< mesh for bitmap object.

    nRef<nShader2> refLineShader; //< shader for line object.
    nRef<nMesh2>   refLineMesh; //< mesh for line object.

    matrix44    projMatrix; //< projection matrix.
    matrix44    modelViewMatrix; //< modelview matrix.

    float ccByteToFloat;

    enum style_index
    {
        LEFT_STYLE = 0,
        RIGHT_STYLE,
        LINE_STYLE,
        STYLE_COUNT
    };

    struct fill_style
    {
        enum mode 
        {
            INVALID,
            COLOR,
            BITMAP_WRAP,
            BITMAP_CLAMP,
            LINEAR_GRADIENT,
            RADIAL_GRADIENT,
        };

        mode m_mode;
        gameswf::rgba m_color;

        const gameswf::bitmap_info* m_bitmap_info;

        gameswf::matrix m_bitmap_matrix;
        gameswf::cxform m_bitmap_color_transform;

        fill_style() : m_mode (INVALID)
        {
        }
        
        void disable()
        {
            m_mode = INVALID;
        }

        void set_color(gameswf::rgba color)
        {
            m_mode  = COLOR;
            m_color = color;
        }

        void set_bitmap(const gameswf::bitmap_info* bi,
                        const gameswf::matrix& m,
                        bitmap_wrap_mode wm,
                        const gameswf::cxform& color_transform)
        {
            m_mode = (wm == WRAP_REPEAT) ? BITMAP_WRAP : BITMAP_CLAMP;
            m_color = gameswf::rgba();
            m_bitmap_info = bi;
            m_bitmap_matrix = m;
            m_bitmap_color_transform = color_transform;
        }
    };// end of fill_style

    fill_style current_styles[STYLE_COUNT];
    gameswf::cxform current_cxform;
    matrix44 current_matrix;

    int current_texture_id; //< internal texture identity.

    nString strShaderRoot; //< the mangle path where ngameswf shaders are.
};

//-----------------------------------------------------------------------------
/**
    Specify the mangle path of ngameswf shader repository.

    @param path path where ngameswf shaders are.
*/
inline
void 
nGameSwfServer::SetShaderPath (const char* path)
{
    this->strShaderRoot = path;
}

//-----------------------------------------------------------------------------
/**
    Retrieve the mangle path of ngameswf shader repository.

    @return path where ngameswf shaders are.
*/
inline
const char* 
nGameSwfServer::GetShaderPath () const
{
    return this->strShaderRoot.Get ();
}

//-----------------------------------------------------------------------------
/**
    Enable/disable attempts to read cache files (.gsc) when loading movies.

    @param bUse set to false if you do not want to use cached data.
*/
inline
void 
nGameSwfServer::SetUseCacheFile (bool bUse)
{
    gameswf::set_use_cache_files (bUse);
}

#endif /*N_GAMESWFSERVER_H*/
