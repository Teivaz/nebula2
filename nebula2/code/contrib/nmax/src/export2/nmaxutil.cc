//-----------------------------------------------------------------------------
//  nmaxutil.cc
//
//  (c)2004 Kim, Hyoun Woo
//-----------------------------------------------------------------------------
#include "export2/nmaxutil.h"
#include "kernel/ntypes.h"
#include "util/nstring.h"

#include "pluginlibs/nmaxdlg.h"
#include "pluginlibs/nmaxlogdlg.h"

#include "tools/nmeshbuilder.h"

//-----------------------------------------------------------------------------
/**
    Retrieves Object from the given node.
    It checkes the returned object is derived object and if it is, 
    tri to get its referenced object until it is not a derived object.
*/
Object* nMaxUtil::GetBaseObject(INode* inode, TimeValue time)
{
    ObjectState objState = inode->EvalWorldState(time);
    Object* obj = objState.obj;

    while( obj->SuperClassID() == GEN_DERIVOB_CLASS_ID )
    {
        obj = ((IDerivedObject*)obj)->GetObjRef();
    }

    return obj;
}

//---------------------------------------------------------------------------
/*
static
Modifier* nMaxUtil::FindMorpherModifier(Object *pObj)
{
    int i;

    if (!pObj)
        return(NULL);

    IDerivedObject *pDerived = NULL;
    if (pObj->SuperClassID() == GEN_DERIVOB_CLASS_ID)
    {
        pDerived = (IDerivedObject *) pObj;

        for (i = 0; i < pDerived->NumModifiers(); i++)
        {
            Modifier *pMod;
            Class_ID cID;

            pMod = pDerived->GetModifier(i);

            cID = pMod->ClassID();
            if (cID == MR3_CLASS_ID)
               return(pMod);
        }
    }

    if (pDerived)
        return FindMorpherModifier(pDerived->GetObjRef());
    else
        return(NULL);
}
*/
//---------------------------------------------------------------------------
/**
    It is used to find physique modifier or skin modifier.

    @code
    Object *obj = node->GetObjectRef();

    Modifier* mod;
    // find physique modifier
    mod = nMaxUtil::FindModifier(obj, Class_ID(PHYSIQUE_CLASS_ID_A, PHYSIQUE_CLASS_ID_B));

    // find skin modifier
    mod = nMaxUtil::FindModifier(obj, SKIN_CLASSID);
    @endcode
*/
Modifier* nMaxUtil::FindModifier(Object *obj, Class_ID classID)
{
    if (!obj)
        return NULL;

/*
    if (obj->SuperClassID() == GEN_DERIVOB_CLASS_ID)
    {
        IDerivedObject *pDerived = (IDerivedObject*)obj;

        for (int i=0; i<pDerived->NumModifiers(); i++)
        {
            Modifier* modifier = pDerived->GetModifier(i);

            // found the modifier.
            Class_ID cId = modifier->ClassID();
            if (cId == classID)
               return modifier;
        }
    }
*/
    Modifier *mod;

    while (obj->SuperClassID() == GEN_DERIVOB_CLASS_ID)
    {
        IDerivedObject* derivObj = static_cast<IDerivedObject*>(obj);

        int modStack = 0;

        // iterate modifier stack.
        while (modStack < derivObj->NumModifiers())
        {
            // Get current modifier.
            mod = derivObj->GetModifier(modStack);
            // See this modifier is skin.
            //if (mod->ClassID() == SKIN_CLASSID)
            Class_ID cId = mod->ClassID();
            if (cId == classID)
            {
                // found the modifier
                return mod;
            }
            modStack++;
        }
        obj = derivObj->GetObjRef();
    }

    return NULL;
}
//-----------------------------------------------------------------------------
/**
    Retrieves super class id of the given Object.

    @param obj pointer to the Object class instance.
    @return super class id of the Object.
*/
SClass_ID nMaxUtil::GetSuperClassID(Object* obj)
{
    n_assert(obj != 0);

    SClass_ID sID;

    sID = obj->SuperClassID();

    while(sID == GEN_DERIVOB_CLASS_ID)
    {
        obj = ((IDerivedObject*)obj)->GetObjRef();
        sID = obj->SuperClassID();
    }

    return sID;
}

//-----------------------------------------------------------------------------
/**
*/
bool nMaxUtil::IsMorphObject(INode* inode)
{
    /*
    Object *objRef = inode->GetObjectRef();
    Class_ID cID = objRef->ClassID();

    Modifier *morpher = FindMorpherModifier(inode->GetObjectRef());
    if (morpher)
        return true;

    if (cID == Class_ID(DERIVOB_CLASS_ID, 0))
    {
        IDerivedObject *derivedObj = (IDerivedObject*)objRef;
        Object *objref2 = derivedObj->GetObjRef();
        Class_ID cID2 = objref2->ClassID();
        
        if (cID2 == Class_ID(MORPHOBJ_CLASS_ID, 0))
            return true;
    }
    else 
    if (cID == Class_ID(MORPHOBJ_CLASS_ID, 0))
        return true;
    */
    return false ;
}

//-----------------------------------------------------------------------------
/**
*/
bool nMaxUtil::IsParticle(INode* inode)
{
/*
    Class_ID eID = inode->GetObjectRef()->ClassID();

    return ((eID == Class_ID(RAIN_CLASS_ID, 0) ||
            eID == Class_ID(SNOW_CLASS_ID, 0) ||
            eID == PCLOUD_CLASS_ID ||
            eID == SUPRSPRAY_CLASS_ID ||
            eID == BLIZZARD_CLASS_ID ||
            eID == PArray_CLASS_ID));
*/
    return false;
}

//-----------------------------------------------------------------------------
/**
*/
Modifier* nMaxUtil::FindPhysique(INode* inode)
{
    Object *obj = inode->GetObjectRef();
    if (!obj)
        return NULL;

    //Modifier* modifier = 0;
    //modifier = FindModifier(obj, Class_ID(PHYSIQUE_CLASS_ID_A, PHYSIQUE_CLASS_ID_B));

    // Is derived object ?
    if (obj->SuperClassID() == GEN_DERIVOB_CLASS_ID)
    {
        // Yes -> Cast.
        IDerivedObject* deriveObj = static_cast<IDerivedObject*>(obj);

        // Iterate over all entries of the modifier stack.
        int modStack = 0;
        while (modStack < deriveObj->NumModifiers())
        {
            // Get current modifier.
            Modifier* modifier = deriveObj->GetModifier(modStack);

            // Is this Physique ?
            if (modifier->ClassID() == Class_ID(PHYSIQUE_CLASS_ID_A, PHYSIQUE_CLASS_ID_B))
            {
                // Yes -> Exit.
                return modifier;
            }

            // Next modifier stack entry.
            modStack++;
        }
    }

    return NULL;
}

//-----------------------------------------------------------------------------
/**
*/
Modifier* nMaxUtil::FindSkin(INode* inode)
{
    //Object *obj = inode->GetObjectRef();

    //Modifier* modifier = FindModifier(obj, SKIN_CLASSID);

    //ISkin* skinModifier = 0;
    //
    //if (modifier)
    //    skinModifier = (ISkin*)modifier->GetInterface(I_SKIN);

    //return skinModifier;

    // Get object from node. Abort if no object.
    Object* obj = inode->GetObjectRef();
    if (!obj) 
        return NULL;

    // Is derived object ?
    while (obj->SuperClassID() == GEN_DERIVOB_CLASS_ID)
    {
        // Yes -> Cast.
        IDerivedObject* derObj = static_cast<IDerivedObject*>(obj);

        // Iterate over all entries of the modifier stack.
        int ModStackIndex = 0;
        while (ModStackIndex < derObj->NumModifiers())
        {
            // Get current modifier.
            Modifier* mod = derObj->GetModifier(ModStackIndex);

            // Is this Skin ?
            if (mod->ClassID() == SKIN_CLASSID )
            {
                // Yes -> Exit.
                return mod;
            }

            // Next modifier stack entry.
            ModStackIndex++;
        }

        obj = derObj->GetObjRef();
    }

    // Not found.
    return NULL;
}

//-----------------------------------------------------------------------------
/**
*/
int nMaxUtil::GetNumMaterials(INode* inode)
{
    Mtl* material = 0;
    material = inode->GetMtl();
    if (!material)
        return 1;

    if (material->ClassID() == Class_ID(MULTI_CLASS_ID, 0))
    {
        MultiMtl *mm = (MultiMtl*)material;

        return mm->NumSubMtls();
    }

    return 1;
}

//------------------------------------------------------------------------------
/**
*/
nString
nMaxUtil::CorrectName(nString &string)
{
    char* temp = n_new_array(char, string.Length() + 1);
    strcpy(temp, string.Get());
    string.Set(CorrectName(temp));
    return string;
}


//------------------------------------------------------------------------------
/**
*/
char*
nMaxUtil::CorrectName(const char* string)
{
    char* temp = n_new_array(char, strlen(string) + 1);
    strcpy(temp, string);
    temp = CorrectName(temp);
    return temp;
}

//------------------------------------------------------------------------------
/**
*/
char*
nMaxUtil::CorrectName(char* string)
{
    char *t = string;
    char *s = t;
    while(*t)
    {
        switch(*t)
        {
        case '>':	//bad character
        case '<':	//bad character
        case '\\':	//bad character
        case '/':	//bad character
        case ':':	//bad character
        case '[':   //bad character
        case ']':   //bad character
        case '(':   //bad character
        case ')':   //bad character
        case ' ':	//space
        case '\n':	//newline
        case '\r':	//carriage return
        case '\t':	//tab
            if(*(t+1) == 0 ) *s = 0;
            else *s = '_';
            s++;
            break;
        default:
            *s = *t;
            s++;
            break;
        }
        t++;
    }
    return string;
}

//------------------------------------------------------------------------------
/**
*/
void nMaxUtil::PutVertexInfo(nMeshBuilder& meshBuilder)
{
    const nMeshBuilder::Vertex& v = meshBuilder.GetVertexAt(0);

    nArray<nMeshBuilder::Group> groupMap;
    meshBuilder.BuildGroupMap(groupMap);

    n_maxlog(Low, "Groups: %i", groupMap.Size());
    n_maxlog(Low, "Triangles: %i", meshBuilder.GetNumTriangles());
    n_maxlog(Low, "Vertices: %i", meshBuilder.GetNumVertices());

    nString msg;

    if (v.HasComponent(nMeshBuilder::Vertex::COORD))
    {
        msg += "coord ";
    }
    if (v.HasComponent(nMeshBuilder::Vertex::NORMAL))
    {
        msg += "normal ";
    }
    if (v.HasComponent(nMeshBuilder::Vertex::TANGENT))
    {
        msg += "tangent ";
    }
    if (v.HasComponent(nMeshBuilder::Vertex::BINORMAL))
    {
        msg += "binormal ";
    }
    if (v.HasComponent(nMeshBuilder::Vertex::COLOR))
    {
        msg += "color ";
    }
    if (v.HasComponent(nMeshBuilder::Vertex::UV0))
    {
        msg += "uv0 ";
    }
    if (v.HasComponent(nMeshBuilder::Vertex::UV1))
    {
        msg += "uv1 ";
    }
    if (v.HasComponent(nMeshBuilder::Vertex::UV2))
    {
        msg += "uv2 ";
    }
    if (v.HasComponent(nMeshBuilder::Vertex::UV3))
    {
        msg += "uv3 ";
    }
    if (v.HasComponent(nMeshBuilder::Vertex::WEIGHTS))
    {
        msg += "weights ";
    }
    if (v.HasComponent(nMeshBuilder::Vertex::JINDICES))
    {
        msg += "jindices";
    }

    n_maxlog(Low, "Vertex components: %s", msg.Get());
}
