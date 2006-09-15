//------------------------------------------------------------------------------
/**
    @ingroup OpenGL

    OpenGL extension declarations
    
    2006 Haron (Oleg Kreptul)
*/

//-----------------------------------------------------------
// OpenGL2 extentions
//-----------------------------------------------------------

//-----------------------------------------------------------
// GL_ARB_shading_language_100
//-----------------------------------------------------------
EXTENSION_BEGIN(GL_ARB_shading_language_100)
//#if defined(GL_ARB_shading_language_100) && !defined(GL_GLEXT_PROTOTYPES)
//#endif
EXTENSION_END(GL_ARB_shading_language_100)


//-----------------------------------------------------------
// GL_ARB_vertex_program
//-----------------------------------------------------------
EXTENSION_BEGIN(GL_ARB_vertex_program)
  #if defined(GL_ARB_vertex_program) && !defined(GL_GLEXT_PROTOTYPES)
    DECL_GLEXT_PROC( PFNGLVERTEXATTRIB1DARBPROC,             glVertexAttrib1dARB)
    DECL_GLEXT_PROC( PFNGLVERTEXATTRIB1DVARBPROC,            glVertexAttrib1dvARB)
    DECL_GLEXT_PROC( PFNGLVERTEXATTRIB1FARBPROC,             glVertexAttrib1fARB)
    DECL_GLEXT_PROC( PFNGLVERTEXATTRIB1FVARBPROC,            glVertexAttrib1fvARB)
    DECL_GLEXT_PROC( PFNGLVERTEXATTRIB1SARBPROC,             glVertexAttrib1sARB)
    DECL_GLEXT_PROC( PFNGLVERTEXATTRIB1SVARBPROC,            glVertexAttrib1svARB)
    DECL_GLEXT_PROC( PFNGLVERTEXATTRIB2DARBPROC,             glVertexAttrib2dARB)
    DECL_GLEXT_PROC( PFNGLVERTEXATTRIB2DVARBPROC,            glVertexAttrib2dvARB)
    DECL_GLEXT_PROC( PFNGLVERTEXATTRIB2FARBPROC,             glVertexAttrib2fARB)
    DECL_GLEXT_PROC( PFNGLVERTEXATTRIB2FVARBPROC,            glVertexAttrib2fvARB)
    DECL_GLEXT_PROC( PFNGLVERTEXATTRIB2SARBPROC,             glVertexAttrib2sARB)
    DECL_GLEXT_PROC( PFNGLVERTEXATTRIB2SVARBPROC,            glVertexAttrib2svARB)
    DECL_GLEXT_PROC( PFNGLVERTEXATTRIB3DARBPROC,             glVertexAttrib3dARB)
    DECL_GLEXT_PROC( PFNGLVERTEXATTRIB3DVARBPROC,            glVertexAttrib3dvARB)
    DECL_GLEXT_PROC( PFNGLVERTEXATTRIB3FARBPROC,             glVertexAttrib3fARB)
    DECL_GLEXT_PROC( PFNGLVERTEXATTRIB3FVARBPROC,            glVertexAttrib3fvARB)
    DECL_GLEXT_PROC( PFNGLVERTEXATTRIB3SARBPROC,             glVertexAttrib3sARB)
    DECL_GLEXT_PROC( PFNGLVERTEXATTRIB3SVARBPROC,            glVertexAttrib3svARB)
    DECL_GLEXT_PROC( PFNGLVERTEXATTRIB4NBVARBPROC,           glVertexAttrib4NbvARB)
    DECL_GLEXT_PROC( PFNGLVERTEXATTRIB4NIVARBPROC,           glVertexAttrib4NivARB)
    DECL_GLEXT_PROC( PFNGLVERTEXATTRIB4NSVARBPROC,           glVertexAttrib4NsvARB)
    DECL_GLEXT_PROC( PFNGLVERTEXATTRIB4NUBARBPROC,           glVertexAttrib4NubARB)
    DECL_GLEXT_PROC( PFNGLVERTEXATTRIB4NUBVARBPROC,          glVertexAttrib4NubvARB)
    DECL_GLEXT_PROC( PFNGLVERTEXATTRIB4NUIVARBPROC,          glVertexAttrib4NuivARB)
    DECL_GLEXT_PROC( PFNGLVERTEXATTRIB4NUSVARBPROC,          glVertexAttrib4NusvARB)
    DECL_GLEXT_PROC( PFNGLVERTEXATTRIB4BVARBPROC,            glVertexAttrib4bvARB)
    DECL_GLEXT_PROC( PFNGLVERTEXATTRIB4DARBPROC,             glVertexAttrib4dARB)
    DECL_GLEXT_PROC( PFNGLVERTEXATTRIB4DVARBPROC,            glVertexAttrib4dvARB)
    DECL_GLEXT_PROC( PFNGLVERTEXATTRIB4FARBPROC,             glVertexAttrib4fARB)
    DECL_GLEXT_PROC( PFNGLVERTEXATTRIB4FVARBPROC,            glVertexAttrib4fvARB)
    DECL_GLEXT_PROC( PFNGLVERTEXATTRIB4IVARBPROC,            glVertexAttrib4ivARB)
    DECL_GLEXT_PROC( PFNGLVERTEXATTRIB4SARBPROC,             glVertexAttrib4sARB)
    DECL_GLEXT_PROC( PFNGLVERTEXATTRIB4SVARBPROC,            glVertexAttrib4svARB)
    DECL_GLEXT_PROC( PFNGLVERTEXATTRIB4UBVARBPROC,           glVertexAttrib4ubvARB)
    DECL_GLEXT_PROC( PFNGLVERTEXATTRIB4UIVARBPROC,           glVertexAttrib4uivARB)
    DECL_GLEXT_PROC( PFNGLVERTEXATTRIB4USVARBPROC,           glVertexAttrib4usvARB)
    DECL_GLEXT_PROC( PFNGLVERTEXATTRIBPOINTERARBPROC,        glVertexAttribPointerARB)
    DECL_GLEXT_PROC( PFNGLENABLEVERTEXATTRIBARRAYARBPROC,    glEnableVertexAttribArrayARB)
    DECL_GLEXT_PROC( PFNGLDISABLEVERTEXATTRIBARRAYARBPROC,   glDisableVertexAttribArrayARB)
    DECL_GLEXT_PROC( PFNGLPROGRAMSTRINGARBPROC,              glProgramStringARB)
    DECL_GLEXT_PROC( PFNGLBINDPROGRAMARBPROC,                glBindProgramARB)
    DECL_GLEXT_PROC( PFNGLDELETEPROGRAMSARBPROC,             glDeleteProgramsARB)
    DECL_GLEXT_PROC( PFNGLGENPROGRAMSARBPROC,                glGenProgramsARB)
    DECL_GLEXT_PROC( PFNGLPROGRAMENVPARAMETER4DARBPROC,      glProgramEnvParameter4dARB)
    DECL_GLEXT_PROC( PFNGLPROGRAMENVPARAMETER4DVARBPROC,     glProgramEnvParameter4dvARB)
    DECL_GLEXT_PROC( PFNGLPROGRAMENVPARAMETER4FARBPROC,      glProgramEnvParameter4fARB)
    DECL_GLEXT_PROC( PFNGLPROGRAMENVPARAMETER4FVARBPROC,     glProgramEnvParameter4fvARB)
    DECL_GLEXT_PROC( PFNGLPROGRAMLOCALPARAMETER4DARBPROC,    glProgramLocalParameter4dARB)
    DECL_GLEXT_PROC( PFNGLPROGRAMLOCALPARAMETER4DVARBPROC,   glProgramLocalParameter4dvARB)
    DECL_GLEXT_PROC( PFNGLPROGRAMLOCALPARAMETER4FARBPROC,    glProgramLocalParameter4fARB)
    DECL_GLEXT_PROC( PFNGLPROGRAMLOCALPARAMETER4FVARBPROC,   glProgramLocalParameter4fvARB)
    DECL_GLEXT_PROC( PFNGLGETPROGRAMENVPARAMETERDVARBPROC,   glGetProgramEnvParameterdvARB)
    DECL_GLEXT_PROC( PFNGLGETPROGRAMENVPARAMETERFVARBPROC,   glGetProgramEnvParameterfvARB)
    DECL_GLEXT_PROC( PFNGLGETPROGRAMLOCALPARAMETERDVARBPROC, glGetProgramLocalParameterdvARB)
    DECL_GLEXT_PROC( PFNGLGETPROGRAMLOCALPARAMETERFVARBPROC, glGetProgramLocalParameterfvARB)
    DECL_GLEXT_PROC( PFNGLGETPROGRAMIVARBPROC,               glGetProgramivARB)
    DECL_GLEXT_PROC( PFNGLGETPROGRAMSTRINGARBPROC,           glGetProgramStringARB)
    DECL_GLEXT_PROC( PFNGLGETVERTEXATTRIBDVARBPROC,          glGetVertexAttribdvARB)
    DECL_GLEXT_PROC( PFNGLGETVERTEXATTRIBFVARBPROC,          glGetVertexAttribfvARB)
    DECL_GLEXT_PROC( PFNGLGETVERTEXATTRIBIVARBPROC,          glGetVertexAttribivARB)
    DECL_GLEXT_PROC( PFNGLGETVERTEXATTRIBPOINTERVARBPROC,    glGetVertexAttribPointervARB)
    DECL_GLEXT_PROC( PFNGLISPROGRAMARBPROC,                  glIsProgramARB)
  #endif
EXTENSION_END(GL_ARB_vertex_program)


//-----------------------------------------------------------
// GL_ARB_fragment_program
//-----------------------------------------------------------
EXTENSION_BEGIN(GL_ARB_fragment_program)
//#if defined(GL_ARB_fragment_program) && !defined(GL_GLEXT_PROTOTYPES)
//#endif
EXTENSION_END(GL_ARB_fragment_program)


//-----------------------------------------------------------
// GL_ARB_shader_objects
//-----------------------------------------------------------
EXTENSION_BEGIN(GL_ARB_shader_objects)
  #if defined(GL_ARB_shader_objects) && !defined(GL_GLEXT_PROTOTYPES)
    DECL_GLEXT_PROC( PFNGLATTACHOBJECTARBPROC,         glAttachObjectARB)
    DECL_GLEXT_PROC( PFNGLCOMPILESHADERARBPROC,        glCompileShaderARB)
    DECL_GLEXT_PROC( PFNGLCREATEPROGRAMOBJECTARBPROC,  glCreateProgramObjectARB)
    DECL_GLEXT_PROC( PFNGLCREATESHADEROBJECTARBPROC,   glCreateShaderObjectARB)
    DECL_GLEXT_PROC( PFNGLDELETEOBJECTARBPROC,         glDeleteObjectARB)
    DECL_GLEXT_PROC( PFNGLDETACHOBJECTARBPROC,         glDetachObjectARB)
    DECL_GLEXT_PROC( PFNGLGETACTIVEUNIFORMARBPROC,     glGetActiveUniformARB)
    DECL_GLEXT_PROC( PFNGLGETATTACHEDOBJECTSARBPROC,   glGetAttachedObjectsARB)
    DECL_GLEXT_PROC( PFNGLGETHANDLEARBPROC,            glGetHandleARB)
    DECL_GLEXT_PROC( PFNGLGETINFOLOGARBPROC,           glGetInfoLogARB)
    DECL_GLEXT_PROC( PFNGLGETOBJECTPARAMETERIVARBPROC, glGetObjectParameterivARB)
    DECL_GLEXT_PROC( PFNGLGETOBJECTPARAMETERFVARBPROC, glGetObjectParameterfvARB)
    DECL_GLEXT_PROC( PFNGLGETSHADERSOURCEARBPROC,      glGetShaderSourceARB)
    DECL_GLEXT_PROC( PFNGLGETUNIFORMIVARBPROC,         glGetUniformivARB)
    DECL_GLEXT_PROC( PFNGLGETUNIFORMFVARBPROC,         glGetUniformfvARB)
    DECL_GLEXT_PROC( PFNGLGETUNIFORMLOCATIONARBPROC,   glGetUniformLocationARB)
    DECL_GLEXT_PROC( PFNGLLINKPROGRAMARBPROC,          glLinkProgramARB)
    DECL_GLEXT_PROC( PFNGLSHADERSOURCEARBPROC,         glShaderSourceARB)
    DECL_GLEXT_PROC( PFNGLUNIFORM1IARBPROC,            glUniform1iARB)
    DECL_GLEXT_PROC( PFNGLUNIFORM2IARBPROC,            glUniform2iARB)
    DECL_GLEXT_PROC( PFNGLUNIFORM3IARBPROC,            glUniform3iARB)
    DECL_GLEXT_PROC( PFNGLUNIFORM4IARBPROC,            glUniform4iARB)
    DECL_GLEXT_PROC( PFNGLUNIFORM1FARBPROC,            glUniform1fARB)
    DECL_GLEXT_PROC( PFNGLUNIFORM2FARBPROC,            glUniform2fARB)
    DECL_GLEXT_PROC( PFNGLUNIFORM3FARBPROC,            glUniform3fARB)
    DECL_GLEXT_PROC( PFNGLUNIFORM4FARBPROC,            glUniform4fARB)
    DECL_GLEXT_PROC( PFNGLUNIFORM1IVARBPROC,           glUniform1ivARB)
    DECL_GLEXT_PROC( PFNGLUNIFORM2IVARBPROC,           glUniform2ivARB)
    DECL_GLEXT_PROC( PFNGLUNIFORM3IVARBPROC,           glUniform3ivARB)
    DECL_GLEXT_PROC( PFNGLUNIFORM4IVARBPROC,           glUniform4ivARB)
    DECL_GLEXT_PROC( PFNGLUNIFORM1FVARBPROC,           glUniform1fvARB)
    DECL_GLEXT_PROC( PFNGLUNIFORM2FVARBPROC,           glUniform2fvARB)
    DECL_GLEXT_PROC( PFNGLUNIFORM3FVARBPROC,           glUniform3fvARB)
    DECL_GLEXT_PROC( PFNGLUNIFORM4FVARBPROC,           glUniform4fvARB)
    DECL_GLEXT_PROC( PFNGLUNIFORMMATRIX2FVARBPROC,     glUniformMatrix2fvARB)
    DECL_GLEXT_PROC( PFNGLUNIFORMMATRIX3FVARBPROC,     glUniformMatrix3fvARB)
    DECL_GLEXT_PROC( PFNGLUNIFORMMATRIX4FVARBPROC,     glUniformMatrix4fvARB)
    DECL_GLEXT_PROC( PFNGLUSEPROGRAMOBJECTARBPROC,     glUseProgramObjectARB)
    DECL_GLEXT_PROC( PFNGLVALIDATEPROGRAMARBPROC,      glValidateProgramARB)
  #endif
EXTENSION_END(GL_ARB_shader_objects)


//-----------------------------------------------------------
// GL_ARB_vertex_shader
//-----------------------------------------------------------
EXTENSION_BEGIN(GL_ARB_vertex_shader)
  #if defined(GL_ARB_vertex_shader) && !defined(GL_GLEXT_PROTOTYPES)
    DECL_GLEXT_PROC( PFNGLBINDATTRIBLOCATIONARBPROC, glBindAttribLocationARB)
    DECL_GLEXT_PROC( PFNGLGETACTIVEATTRIBARBPROC,    glGetActiveAttribARB)
    DECL_GLEXT_PROC( PFNGLGETATTRIBLOCATIONARBPROC,  glGetAttribLocationARB)
  #endif
EXTENSION_END(GL_ARB_vertex_shader)


//-----------------------------------------------------------
// OpenGL extentions
//-----------------------------------------------------------

//-----------------------------------------------------------
// GL_ARB_vertex_buffer_object
//-----------------------------------------------------------
EXTENSION_BEGIN(GL_ARB_vertex_buffer_object)
  #if defined(GL_ARB_vertex_buffer_object) && !defined(GL_GLEXT_PROTOTYPES)
    DECL_GLEXT_PROC( PFNGLBINDBUFFERARBPROC,           glBindBufferARB)
    DECL_GLEXT_PROC( PFNGLDELETEBUFFERSARBPROC,        glDeleteBuffersARB)
    DECL_GLEXT_PROC( PFNGLGENBUFFERSARBPROC,           glGenBuffersARB)
    DECL_GLEXT_PROC( PFNGLISBUFFERARBPROC,             glIsBufferARB)
    DECL_GLEXT_PROC( PFNGLBUFFERDATAARBPROC,           glBufferDataARB)
    DECL_GLEXT_PROC( PFNGLBUFFERSUBDATAARBPROC,        glBufferSubDataARB)
    DECL_GLEXT_PROC( PFNGLGETBUFFERSUBDATAARBPROC,     glGetBufferSubDataARB)
    DECL_GLEXT_PROC( PFNGLMAPBUFFERARBPROC,            glMapBufferARB)
    DECL_GLEXT_PROC( PFNGLUNMAPBUFFERARBPROC,          glUnmapBufferARB)
    DECL_GLEXT_PROC( PFNGLGETBUFFERPARAMETERIVARBPROC, glGetBufferParameterivARB)
    DECL_GLEXT_PROC( PFNGLGETBUFFERPOINTERVARBPROC,    glGetBufferPointervARB)
  #endif
EXTENSION_END(GL_ARB_vertex_buffer_object)


//-----------------------------------------------------------
// WGL_ARB_render_texture
//-----------------------------------------------------------
#ifdef __WIN32__
EXTENSION_BEGIN(WGL_ARB_render_texture)
  #if defined(WGL_ARB_render_texture) && !defined(WGL_WGLEXT_PROTOTYPES)
    // WGL_ARB_render_texture
    DECL_GLEXT_PROC( PFNWGLBINDTEXIMAGEARBPROC,           wglBindTexImageARB)
    DECL_GLEXT_PROC( PFNWGLRELEASETEXIMAGEARBPROC,        wglReleaseTexImageARB)
    // WGL_ARB_pbuffer
    DECL_GLEXT_PROC( PFNWGLCREATEPBUFFERARBPROC,          wglCreatePbufferARB)
    DECL_GLEXT_PROC( PFNWGLGETPBUFFERDCARBPROC,           wglGetPbufferDCARB)
    DECL_GLEXT_PROC( PFNWGLRELEASEPBUFFERDCARBPROC,       wglReleasePbufferDCARB)
    DECL_GLEXT_PROC( PFNWGLDESTROYPBUFFERARBPROC,         wglDestroyPbufferARB)
    DECL_GLEXT_PROC( PFNWGLQUERYPBUFFERARBPROC,           wglQueryPbufferARB)
    DECL_GLEXT_PROC( PFNWGLSETPBUFFERATTRIBARBPROC,       wglSetPbufferAttribARB)
    // WGL_ARB_pixel_format
    DECL_GLEXT_PROC( PFNWGLCHOOSEPIXELFORMATARBPROC,      wglChoosePixelFormatARB)
    DECL_GLEXT_PROC( PFNWGLGETPIXELFORMATATTRIBIVARBPROC, wglGetPixelFormatAttribivARB)
    DECL_GLEXT_PROC( PFNWGLGETPIXELFORMATATTRIBFVARBPROC, wglGetPixelFormatAttribfvARB)
    // WGL_ARB_make_current_read
    DECL_GLEXT_PROC( PFNWGLMAKECONTEXTCURRENTARBPROC,     wglMakeContextCurrentARB)
    DECL_GLEXT_PROC( PFNWGLGETCURRENTREADDCARBPROC,       wglGetCurrentReadDCARB)
  #endif
EXTENSION_END(WGL_ARB_render_texture)
#endif // __WIN32__


//-----------------------------------------------------------
// GL_ARB_texture_compression
//-----------------------------------------------------------
EXTENSION_BEGIN(GL_ARB_texture_compression)
  #if defined(GL_ARB_texture_compression) && !defined(GL_GLEXT_PROTOTYPES)
    DECL_GLEXT_PROC( PFNGLCOMPRESSEDTEXIMAGE3DARBPROC,    glCompressedTexImage3DARB)
    DECL_GLEXT_PROC( PFNGLCOMPRESSEDTEXIMAGE2DARBPROC,    glCompressedTexImage2DARB)
    DECL_GLEXT_PROC( PFNGLCOMPRESSEDTEXIMAGE1DARBPROC,    glCompressedTexImage1DARB)
    DECL_GLEXT_PROC( PFNGLCOMPRESSEDTEXSUBIMAGE3DARBPROC, glCompressedTexSubImage3DARB)
    DECL_GLEXT_PROC( PFNGLCOMPRESSEDTEXSUBIMAGE2DARBPROC, glCompressedTexSubImage2DARB)
    DECL_GLEXT_PROC( PFNGLCOMPRESSEDTEXSUBIMAGE1DARBPROC, glCompressedTexSubImage1DARB)
    DECL_GLEXT_PROC( PFNGLGETCOMPRESSEDTEXIMAGEARBPROC,   glGetCompressedTexImageARB)
  #endif
EXTENSION_END(GL_ARB_texture_compression)


//-----------------------------------------------------------
// GL_EXT_texture_compression_s3tc
//-----------------------------------------------------------
EXTENSION_BEGIN(GL_EXT_texture_compression_s3tc)
//#if defined(GL_EXT_texture_compression_s3tc) && !defined(GL_GLEXT_PROTOTYPES)
//#endif
EXTENSION_END(GL_EXT_texture_compression_s3tc)


//-----------------------------------------------------------
// GL_ARB_texture_cube_map
//-----------------------------------------------------------
EXTENSION_BEGIN(GL_ARB_texture_cube_map)
//#ifdef GL_ARB_texture_cube_map
//#endif
EXTENSION_END(GL_ARB_texture_cube_map)


//-----------------------------------------------------------
// GL_ARB_multitexture
//-----------------------------------------------------------
EXTENSION_BEGIN(GL_ARB_multitexture)
  #if defined(GL_ARB_multitexture) && !defined(GL_GLEXT_PROTOTYPES)
    DECL_GLEXT_PROC( PFNGLCLIENTACTIVETEXTUREARBPROC, glActiveTextureARB)
    DECL_GLEXT_PROC( PFNGLMULTITEXCOORD2FARBPROC,     glMultiTexCoord2fARB)
    DECL_GLEXT_PROC( PFNGLACTIVETEXTUREARBPROC,       glClientActiveTextureARB)
  #endif
EXTENSION_END(GL_ARB_multitexture)
