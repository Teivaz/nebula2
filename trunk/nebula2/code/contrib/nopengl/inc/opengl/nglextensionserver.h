#ifndef N_GLEXTENSIONSERVER_H
#define N_GLEXTENSIONSERVER_H
//------------------------------------------------------------------------------
/**
    @class nGLExtensionServer
    @ingroup OpenGL

    @brief OpenGL extension management

    2004 Haron
*/
#include "opengl/nglincludes.h"

#define EXTENSION_BEGIN(extname)
#define DECL_GLEXT_PROC(type, procname) extern type procname;
#define EXTENSION_END(extname)
#include "opengl\nglextensions.h"
#undef EXTENSION_BEGIN
#undef DECL_GLEXT_PROC
#undef EXTENSION_END

#define N_GL_EXTENSION_SUPPORTED(extname) nGLExtensionServer::IsSupported_##extname()

//------------------------------------------------------------------------------
class nGLExtensionServer //: public nRoot
{
public:
    /// GL Extensition Lookup
    static bool HasExtension(nString extName);
    /// print extensions to Log file
    static void PrintExtensions(nString extStr);

private:
    /// constructor
    nGLExtensionServer();
    /// destructor
    ~nGLExtensionServer();

protected:
    friend class nGLServer2;

    /// initialize all possible extention that we are need
    static void InitExtensions();

#define EXTENSION_BEGIN(extname) private: static bool support_##extname;\
                                 public: static bool IsSupported_##extname() {return support_##extname;}
#define DECL_GLEXT_PROC(type, procname)
#define EXTENSION_END(extname)
#include "opengl\nglextensions.h"
#undef EXTENSION_BEGIN
#undef DECL_GLEXT_PROC
#undef EXTENSION_END
};

//------------------------------------------------------------------------------
#endif
