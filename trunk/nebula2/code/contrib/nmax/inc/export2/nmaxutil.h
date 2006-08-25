//-----------------------------------------------------------------------------
//  nmaxutil.h
//
//  (c)2004 Kim, Hyoun Woo
//-----------------------------------------------------------------------------
#ifndef N_MAXUTIL_H
#define N_MAXUTIL_H
//-----------------------------------------------------------------------------
/**
    @class nMaxUtil
    @ingroup NebulaMaxExport2Contrib

    @brief
*/
#include "nmax.h"

class nString;
class nMeshBuilder;

//-----------------------------------------------------------------------------
class nMaxUtil
{
public:
    static Object* GetBaseObject(INode* inode, TimeValue time);

    static Modifier* FindMorpherModifier(Object *pObj);
    static Modifier* FindModifier(Object *obj, Class_ID kModID);

    static SClass_ID GetSuperClassID(Object* obj);
    static bool IsMorphObject(INode* inode);
    static bool IsParticle(INode* inode);
    static Modifier* FindPhysique(INode* inode);
    //static ISkin* FindSkin(INode* inode);
    static Modifier* FindSkin(INode* inode);
    static int GetNumMaterials(INode* inode);

    static nString CorrectName(nString &string);
    static char* CorrectName(const char* string);
    static char* CorrectName(char* string);

    static void PutVertexInfo(nMeshBuilder& meshBuilder);

    /// assign name for resource types.
    enum nAssignType
    {
        Anim,
        Gfx,
        Mesh,
        Texture
    };

    static nString RelacePathToAssign(nAssignType type, nString& path, nString& fileName);

protected:
    ///
    static nString GetAssignFromType (nAssignType type);
    ///
    static nString GetPathFromType(nAssignType type);

};
//-----------------------------------------------------------------------------
#endif