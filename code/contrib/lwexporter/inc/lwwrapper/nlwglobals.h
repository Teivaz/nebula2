#ifndef N_LW_GLOBALS_H
#define N_LW_GLOBALS_H
//----------------------------------------------------------------------------
extern "C" 
{
#include <lwsdk/lwpanel.h>
#include <lwsdk/lwdisplay.h>
#include <lwsdk/lwhost.h>
#include <lwsdk/lwmonitor.h>
#include <lwsdk/lwrender.h>
#include <lwsdk/lwtxtr.h>
#include <lwsdk/lwsurf.h>
}

#include "kernel/ntypes.h"
#include "util/nstring.h"

namespace nLWGlobals
{

    /// check if the Lightwave global function has been set
    bool IsGlobalFuncSet();

    /// set the Lightwave global function
    void SetGlobalFunc(GlobalFunc*);

    /// get the Lightwave global function
    GlobalFunc* GetGlobalFunc();
    
    //----------------------------------------------------------------------------
    /**
    */
    enum GlobalName
    {
        GN_BoneInfo = 0,
        GN_MessageFuncs,
        GN_TextureFuncs,
        GN_SurfaceFuncs,
        GN_ObjectInfo,
        GN_InterfaceInfo,
        GN_MonFuncs,
        GN_PanelFuncs,
        GN_ItemInfo,
    };

    //----------------------------------------------------------------------------
    /**
    */
    inline
    const char* 
    NameEnumToString(GlobalName globalName)
    {
        const char* nameStr = 0;
        switch (globalName)
        {
            case GN_BoneInfo:
                nameStr = LWBONEINFO_GLOBAL;
                break;

            case GN_MessageFuncs:
                nameStr = LWMESSAGEFUNCS_GLOBAL;
                break;

            case GN_TextureFuncs:
                nameStr = LWTEXTUREFUNCS_GLOBAL;
                break;
                    
            case GN_SurfaceFuncs:
                nameStr = LWSURFACEFUNCS_GLOBAL;
                break;

            case GN_ObjectInfo:
                nameStr = LWOBJECTINFO_GLOBAL;
                break;
            
            case GN_InterfaceInfo:
                nameStr = LWINTERFACEINFO_GLOBAL;
                break;

            case GN_MonFuncs:
                nameStr = LWLMONFUNCS_GLOBAL;
                break;

            case GN_PanelFuncs:
                nameStr = LWPANELFUNCS_GLOBAL;
                break;
                    
            case GN_ItemInfo:
                nameStr = LWITEMINFO_GLOBAL;
                break;

            default:
                n_assert2(false, "Invalid or unhandled GlobalName enum member!");
        }
        return nameStr;
    }

    //----------------------------------------------------------------------------
    /**
    */
    template<class GLOBAL_TYPE, GlobalName GLOBAL_NAME>
    class AcquiredGlobal
    {
    public:
        AcquiredGlobal();
        ~AcquiredGlobal();
        bool IsValid() const;
        GLOBAL_TYPE* Get() const;

    private:
        GLOBAL_TYPE* global;
    };

    //----------------------------------------------------------------------------
    /**
    */
    template<class GLOBAL_TYPE, GlobalName GLOBAL_NAME>
    AcquiredGlobal<GLOBAL_TYPE, GLOBAL_NAME>::AcquiredGlobal() :
        global(0)
    {
        GlobalFunc* globalFunc = GetGlobalFunc();
        n_assert(globalFunc);
        if (globalFunc)
        {
            this->global = (GLOBAL_TYPE*)globalFunc(NameEnumToString(GLOBAL_NAME), 
                                                    GFUSE_ACQUIRE);
        }
    }

    //----------------------------------------------------------------------------
    /**
    */
    template<class GLOBAL_TYPE, GlobalName GLOBAL_NAME>
    AcquiredGlobal<GLOBAL_TYPE, GLOBAL_NAME>::~AcquiredGlobal()
    {
        GlobalFunc* globalFunc = GetGlobalFunc();
        n_assert(globalFunc);
        if (globalFunc)
        {
            globalFunc(NameEnumToString(GLOBAL_NAME), GFUSE_RELEASE);
        }
        this->global = 0;
    }

    //----------------------------------------------------------------------------
    /**
    */
    template<class GLOBAL_TYPE, GlobalName GLOBAL_NAME>
    bool 
    AcquiredGlobal<GLOBAL_TYPE, GLOBAL_NAME>::IsValid() const
    {
        return (this->global != 0);
    }

    //----------------------------------------------------------------------------
    /**
    */
    template<class GLOBAL_TYPE, GlobalName GLOBAL_NAME>
    GLOBAL_TYPE* 
    AcquiredGlobal<GLOBAL_TYPE, GLOBAL_NAME>::Get() const
    {
        n_assert(this->global);
        return this->global;
    }

    //----------------------------------------------------------------------------
    /**
    */
    typedef AcquiredGlobal<LWBoneInfo,      GN_BoneInfo>      BoneInfo;
    typedef AcquiredGlobal<LWMessageFuncs,  GN_MessageFuncs>  MessageFuncs;
    typedef AcquiredGlobal<LWTextureFuncs,  GN_TextureFuncs>  TextureFuncs;
    typedef AcquiredGlobal<LWSurfaceFuncs,  GN_SurfaceFuncs>  SurfaceFuncs;
    typedef AcquiredGlobal<LWObjectInfo,    GN_ObjectInfo>    ObjectInfo;
    typedef AcquiredGlobal<LWInterfaceInfo, GN_InterfaceInfo> InterfaceInfo;
    typedef AcquiredGlobal<LWLMonFuncs,     GN_MonFuncs>      LMonFuncs;
    typedef AcquiredGlobal<LWPanelFuncs,    GN_PanelFuncs>    PanelFuncs;
    typedef AcquiredGlobal<LWItemInfo,      GN_ItemInfo>      ItemInfo;

    //----------------------------------------------------------------------------
    /**
    */
    class SystemID
    {
    public:
        static bool IsModeler();
        static bool IsLayout();
        static bool IsScreamernet();
    };

    //----------------------------------------------------------------------------
    /**
        @brief Check if we're running in an interactive Modeler session.
        @return true if running in Modeler, false otherwise.
    */
    inline
    bool 
    SystemID::IsModeler()
    {
        GlobalFunc* globalFunc = GetGlobalFunc();
        unsigned long sysid = (unsigned long)globalFunc(LWSYSTEMID_GLOBAL, 
                                                        GFUSE_TRANSIENT);
        return ((sysid & LWSYS_TYPEBITS) == LWSYS_MODELER);
    }

    //----------------------------------------------------------------------------
    /**
        @brief Check if we're running in an interactive Layout session.
        @return true if running in Layout, false otherwise.
    */
    inline
    bool 
    SystemID::IsLayout()
    {
        GlobalFunc* globalFunc = GetGlobalFunc();
        unsigned long sysid = (unsigned long)globalFunc(LWSYSTEMID_GLOBAL, 
                                                        GFUSE_TRANSIENT);
        return ((sysid & LWSYS_TYPEBITS) == LWSYS_LAYOUT);
    }

    //----------------------------------------------------------------------------
    /**
        @brief Check if we're running in a non-interactive Scremernet session.
    */
    inline
    bool 
    SystemID::IsScreamernet()
    {
        GlobalFunc* globalFunc = GetGlobalFunc();
        unsigned long sysid = (unsigned long)globalFunc(LWSYSTEMID_GLOBAL, 
                                                        GFUSE_TRANSIENT);
        return ((sysid & LWSYS_TYPEBITS) == LWSYS_SCREAMERNET);
    }
    
    //----------------------------------------------------------------------------
    /**
    */
    class ColorPicker
    {
    public:
        ColorPicker();
        void SetTitle(const nString&);
        void SetCallback(LWHotColorFunc*, void* userData);
        void SetColor(float r, float g, float b);
        bool GetColor(float* r, float* g, float* b);

    private:
        nString title;
        LWColorPickLocal local;
    };

    //----------------------------------------------------------------------------
    /**
    */
    inline
    ColorPicker::ColorPicker()
    {
        this->local.title = 0;
        this->local.red = 0.0f;
        this->local.green = 0.0f;
        this->local.blue = 0.0f;
        this->local.hotFunc = 0;
        this->local.data = 0;
    }
    
    //----------------------------------------------------------------------------
    /**
    */
    inline
    void 
    ColorPicker::SetTitle(const nString& t)
    {
        this->title = t;
        this->local.title = this->title.Get();
    }

    //----------------------------------------------------------------------------
    /**
    */
    inline
    void 
    ColorPicker::SetCallback(LWHotColorFunc* func, void* userData)
    {
        this->local.hotFunc = func;
        this->local.data = userData;
    }

    //----------------------------------------------------------------------------
    /**
    */
    inline
    void 
    ColorPicker::SetColor(float r, float g, float b)
    {
        this->local.red = r;
        this->local.green = g;
        this->local.blue = b;
    }

    //----------------------------------------------------------------------------
    /**
        @brief Display the color picker and allow the user to pick a color.
        @return true if the user picked a color, false otherwise.
    */
    inline
    bool 
    ColorPicker::GetColor(float* r, float* g, float* b)
    {
        GlobalFunc* globalFunc = GetGlobalFunc();
        LWColorActivateFunc* colorFunc = 0;
        colorFunc = (LWColorActivateFunc*)globalFunc(LWCOLORACTIVATEFUNC_GLOBAL, 
                                                     GFUSE_TRANSIENT);
        if (colorFunc(LWCOLORPICK_VERSION, &this->local) == AFUNC_OK)
        {
            if (1 == this->local.result)
            {
                if (r)
                {
                    *r = this->local.red;
                }
                if (g)
                {
                    *g = this->local.green;
                }
                if (b)
                {
                    *b = this->local.blue;
                }
                return true;
            }
        }
        return false;
    }

    //----------------------------------------------------------------------------
    /**
    */
    class SceneInfo
    {
    public:
        SceneInfo();
        bool IsValid() const;
        LWSceneInfo* Get() const;

    private:
        GlobalFunc* globalFunc;
    };

    //----------------------------------------------------------------------------
    /**
    */
    inline
    SceneInfo::SceneInfo()
    {
        this->globalFunc = GetGlobalFunc();
        n_assert(globalFunc);
    }

    //----------------------------------------------------------------------------
    /**
    */
    inline
    bool 
    SceneInfo::IsValid() const
    {
        return (this->globalFunc != 0);
    }

    //----------------------------------------------------------------------------
    /**
    */
    inline
    LWSceneInfo* 
    SceneInfo::Get() const
    {
        return (LWSceneInfo*)this->globalFunc(LWSCENEINFO_GLOBAL, GFUSE_TRANSIENT);
    }
        
    //----------------------------------------------------------------------------
    /**
    */
    class DirInfoFunc
    {
    public:
        nString GetContentDir() const;
        nString GetObjectsDir() const;
        nString GetImagesDir() const;
        nString GetScenesDir() const;
    };

    //----------------------------------------------------------------------------
    /**
        @brief Get the absolute path to the current Lightwave Content directory.
    */
    inline
    nString 
    DirInfoFunc::GetContentDir() const
    {
        GlobalFunc* globalFunc = GetGlobalFunc();
        n_assert(globalFunc);
        if (globalFunc)
        {
            LWDirInfoFunc* func = (LWDirInfoFunc*)globalFunc(LWDIRINFOFUNC_GLOBAL, 
                                                             GFUSE_TRANSIENT);
            if (func)
            {
                return func(LWFTYPE_CONTENT);
            }
        }
        
        return nString(); // empty string
    }

    //----------------------------------------------------------------------------
    /**
        @brief Get the absolute path to the current Lightwave Objects directory.
    */
    inline
    nString 
    DirInfoFunc::GetObjectsDir() const
    {
        GlobalFunc* globalFunc = GetGlobalFunc();
        n_assert(globalFunc);
        if (globalFunc)
        {
            LWDirInfoFunc* func = (LWDirInfoFunc*)globalFunc(LWDIRINFOFUNC_GLOBAL, 
                                                             GFUSE_TRANSIENT);
            if (func)
            {
                return func(LWFTYPE_OBJECT);
            }
        }

        return nString(); // empty string
    }

    //----------------------------------------------------------------------------
    /**
        @brief Get the absolute path to the current Lightwave Images directory.
    */
    inline
    nString 
    DirInfoFunc::GetImagesDir() const
    {
        GlobalFunc* globalFunc = GetGlobalFunc();
        n_assert(globalFunc);
        if (globalFunc)
        {
            LWDirInfoFunc* func = (LWDirInfoFunc*)globalFunc(LWDIRINFOFUNC_GLOBAL, 
                                                             GFUSE_TRANSIENT);
            if (func)
            {
                return func(LWFTYPE_IMAGE);
            }
        }

        return nString(); // empty string
    }

    //----------------------------------------------------------------------------
    /**
        @brief Get the absolute path to the current Lightwave Scenes directory.
    */
    inline
    nString 
    DirInfoFunc::GetScenesDir() const
    {
        GlobalFunc* globalFunc = GetGlobalFunc();
        n_assert(globalFunc);
        if (globalFunc)
        {
            LWDirInfoFunc* func = (LWDirInfoFunc*)globalFunc(LWDIRINFOFUNC_GLOBAL, 
                                                             GFUSE_TRANSIENT);
            if (func)
            {
                return func(LWFTYPE_SCENE);
            }
        }

        return nString(); // empty string
    }
    
    //----------------------------------------------------------------------------
    /**
        @brief Get host display information from Lightwave.
        @warning May return NULL if called in a non-interactive context 
                 (i.e. no app window).
        @return Pointer to a host display structure, or NULL if called in a
                non-interactive context.
    */
    inline
    HostDisplayInfo*
    GetHostDisplayInfo()
    {
        GlobalFunc* globalFunc = GetGlobalFunc();
        n_assert(globalFunc);
        if (globalFunc)
            return (HostDisplayInfo*)globalFunc(LWHOSTDISPLAYINFO_GLOBAL, GFUSE_TRANSIENT);

        return 0;
    }

} // namespace nLWGlobals

//----------------------------------------------------------------------------
#endif // N_LW_GLOBALS_H
