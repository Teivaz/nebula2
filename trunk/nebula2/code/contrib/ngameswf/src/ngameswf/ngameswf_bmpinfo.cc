//------------------------------------------------------------------------------
//  ngameswf_bmpinfo.cc
//  (C) 2003, 2004 George McBay, Kim, Hyoun Woo
//------------------------------------------------------------------------------
#include "ngameswf/ngameswf_bmpinfo.h"

//-----------------------------------------------------------------------------
/**
    Constructor.
*/
bitmap_info_nebula::bitmap_info_nebula () : 
    gameswf::bitmap_info(),
    image_data (NULL)
{
}

//-----------------------------------------------------------------------------
/**
    @param width
    @param height
    @param data
    @param gfxserver
    @param rsrc_loader_path
    @param texture_id
*/
bitmap_info_nebula::bitmap_info_nebula (int width, int height, uchar* data,
                                        nGfxServer2* gfxserver,
                                        const char* rsrc_loader_path,
                                        int texture_id ) :
    gameswf::bitmap_info(),
    image_data (NULL)
{
    n_assert(data);

    strcpy(resource_loader_path, rsrc_loader_path);

    m_texture_id = texture_id;

    m_original_width  = width;
    m_original_height = height;

#ifdef _DEBUG
    int w = 1;
    while (w<width) { w <<= 1; }

    int h = 1;    
    while (h<height) { h <<= 1; }

    n_assert (w == width);
    n_assert (h == height);
#endif

    pixel_format = nTexture2::A8;

    image_width  = width;
    image_height = height;
    image_pitch  = width;

    image_data = (uchar*)n_malloc(image_width*image_height);
    n_assert (image_data != NULL);

    memcpy (image_data, data, image_width*image_height);

    char texRsrcName[N_MAXPATH];
    sprintf(texRsrcName, "gswf%d", m_texture_id);

    ref_texture = gfxserver->NewTexture(texRsrcName);
    n_assert(ref_texture.isvalid());

    load(gfxserver);
}

//-----------------------------------------------------------------------------
/**
    Constructor.

    @param im
    @param gfxserver
    @param rsrc_loader_path
    @param texture_id
*/
bitmap_info_nebula::bitmap_info_nebula(image::rgb* im,  nGfxServer2* gfxserver,
                                        const char* rsrc_loader_path, int texture_id) : 
    gameswf::bitmap_info(),
    image_data (NULL)
{
    n_assert(im);

    strcpy(resource_loader_path, rsrc_loader_path);

    m_texture_id = texture_id;

    m_original_width  = im->m_width;
    m_original_height = im->m_height;

    int w = 1;
    while (w < im->m_width) { w <<= 1; }

    int h = 1; 
    while ( h < im->m_height) { h <<= 1; }

    image::rgb* rescaled = image::create_rgb(w, h);
    image::resample (rescaled, 0, 0, w-1, h-1, im, 0, 0,
                     (float)im->m_width,(float)im->m_height);

    pixel_format = nTexture2::X8R8G8B8;

    image_width  = rescaled->m_width;
    image_height = rescaled->m_height;
    image_pitch  = 3 * rescaled->m_width;

    image_data = (uchar*)n_malloc(image_width*image_height*3);
    n_assert(image_data != 0);

    memcpy(image_data, rescaled->m_data, image_width*image_height*3);

    delete rescaled;

    char texRscName[N_MAXPATH];
    sprintf(texRscName, "gswf%d", m_texture_id);

    ref_texture = gfxserver->NewTexture(texRscName);
    n_assert(ref_texture.isvalid());

    load(gfxserver);
}

//-----------------------------------------------------------------------------
/**
    Constructor.
*/
bitmap_info_nebula::bitmap_info_nebula(image::rgba* im, nGfxServer2* gfxserver,
                                const char* rsrc_loader_path, int texture_id) : 
    gameswf::bitmap_info(),
    image_data (NULL)
{
     n_assert (im != NULL);

     pixel_format = nTexture2::A8R8G8B8;

     strcpy(resource_loader_path, rsrc_loader_path);

     m_texture_id = texture_id;

     m_original_width  = im->m_width;
     m_original_height = im->m_height;

     int w = 1;
     while (w < im->m_width) { w <<= 1; }

     int h = 1;
     while (h < im->m_height) { h <<= 1;}

     if (w != im->m_width || h != im->m_height)
     {
         image::rgba* rescaled = image::create_rgba(w, h);
         image::resample(rescaled, 0, 0, w-1, h-1, im, 0, 0,
                         (float)im->m_width, (float)im->m_height);

         image_width = rescaled->m_width;
         image_height= rescaled->m_height;
         image_pitch = 4 * rescaled->m_width;

         image_data = (uchar*)n_malloc(image_width*image_height*4);
         n_assert (image_data != NULL);

         memcpy(image_data, rescaled->m_data, image_width*image_height*4);

         delete rescaled;

         char texRsrcName[N_MAXPATH];
         sprintf(texRsrcName, "gswf%d", m_texture_id);

         ref_texture = gfxserver->NewTexture(texRsrcName);
         n_assert(ref_texture.isvalid());

         load(gfxserver);
     }
     else
     {
         pixel_format = nTexture2::Format::A8R8G8B8;

         image_width  = im->m_width;
         image_height = im->m_height;
         image_pitch  = 4 * im->m_width;

         image_data = (uchar*)n_malloc(image_width * image_height * 4);
         n_assert (image_data != NULL);

         memcpy(image_data, im->m_data, image_width*image_height*4);

         char texRscName[N_MAXPATH];
         sprintf(texRscName, "gswf%d", m_texture_id);

         ref_texture = gfxserver->NewTexture(texRscName);
         n_assert(ref_texture.isvalid());

         load(gfxserver);
     }
}

//-----------------------------------------------------------------------------
/**
    Destructor.
*/
bitmap_info_nebula::~bitmap_info_nebula()
{
    if (image_data != NULL)
    {
        n_free (image_data);
    }
}

//-----------------------------------------------------------------------------
/**
*/
void bitmap_info_nebula::load (nGfxServer2* gfxserver)
{
    n_assert(image_width != 0);
    n_assert(image_height != 0);
    n_assert(image_data != 0);

    // Create empty texture
    if(pixel_format == nTexture2::A8)
    {
        ref_texture->SetUsage (nTexture2::Usage::CreateEmpty);
        ref_texture->SetType  (nTexture2::Type::TEXTURE_2D);
        ref_texture->SetWidth (image_width);
        ref_texture->SetHeight(image_height);
        ref_texture->SetFormat(pixel_format);
        ref_texture->Load();
    }
    else
    {
        ref_texture->SetUsage (nTexture2::Usage::CreateEmpty);
        ref_texture->SetType  (nTexture2::Type::TEXTURE_2D);
        ref_texture->SetWidth (image_width);
        ref_texture->SetHeight(image_height);
        ref_texture->SetFormat(pixel_format);
        ref_texture->Load();
    }

    int dest_pitch;
    unsigned char *surface_data;

    nSurface *surface; 

    ref_texture->GetSurfaceLevel("/tmp/swfsurface", 0, &surface);

    unsigned char *src_scanline;
    unsigned char *dst_scanline;

    if(pixel_format == nTexture2::X8R8G8B8)
    {
        surface->Lock(dest_pitch, (void **)&surface_data); 		

        for(int y = 0; y<image_height; y++)
        {
            src_scanline = image_data + (y * image_pitch);
            dst_scanline = surface_data + (y * dest_pitch);

            for(int srcX=0, dstX=0; srcX<image_pitch; srcX += 3, dstX += 4)
            {
                dst_scanline[dstX]     = src_scanline[srcX + 2];
                dst_scanline[dstX + 1] = src_scanline[srcX + 1];
                dst_scanline[dstX + 2] = src_scanline[srcX]; 
            }
        } 

        surface->Unlock();
    }
    else if(pixel_format == nTexture2::A8R8G8B8)
    {
        surface->Lock(dest_pitch, (void **)&surface_data); 		

        for(int y=0; y<image_height; y++)
        {
            src_scanline = image_data + (y * image_pitch);
            dst_scanline = surface_data + (y * dest_pitch);

            for(int x=0; x<image_pitch; x += 4)
            {
                dst_scanline[x]     = src_scanline[x + 2];
                dst_scanline[x + 1] = src_scanline[x + 1];
                dst_scanline[x + 2] = src_scanline[x];
                dst_scanline[x + 3] = src_scanline[x + 3];
            }
        } 

        surface->Unlock();
    }
    else if(pixel_format == nTexture2::A8)
    {
        surface->LoadFromMemory(image_data, pixel_format, image_width, image_height, image_pitch);

        ref_texture->GenerateMipMaps();
    }

    surface->Release();

    ref_texture->SetResourceLoader(resource_loader_path);
}
//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------