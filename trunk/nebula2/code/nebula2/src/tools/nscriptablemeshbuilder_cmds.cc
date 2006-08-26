//------------------------------------------------------------------------------
//  nscriptablemeshbuilder_cmds.cc
//
//  (C)2005 James Mastro / Kim, Hyoun Woo
//------------------------------------------------------------------------------
#include "kernel/ncmdproto.h"
#include "kernel/nkernelserver.h"
#include "kernel/nfileserver2.h"
#include "tools/nscriptablemeshbuilder.h"

static void n_save(void* slf, nCmd* cmd);
static void n_beginaddvertex(void* slf, nCmd* cmd);
static void n_addcoord(void* slf, nCmd* cmd);
static void n_addnormal(void* slf, nCmd* cmd);
static void n_addcolor(void* slf, nCmd* cmd);
static void n_adduv(void* slf, nCmd* cmd);
static void n_addtangent(void* slf, nCmd* cmd);
static void n_addbinormal(void* slf, nCmd* cmd);
static void n_addjointindices(void* slf, nCmd* cmd);
static void n_addweights(void* slf, nCmd* cmd);
static void n_endaddvertex(void* slf, nCmd* cmd);

static void n_addtriangle(void* slf, nCmd* cmd);
static void n_transform(void* slf, nCmd* cmd);
static void n_flipuvs(void* slf, nCmd* cmd);
static void n_buildtrianglenormals(void* slf, nCmd* cmd);
static void n_buildvertexnormals(void* slf, nCmd* cmd);
static void n_buildvertextangents(void* slf, nCmd* cmd);
static void n_cleanup(void* slf, nCmd* cmd);
static void n_optimize(void* slf, nCmd* cmd);
static void n_createedges(void* slf, nCmd* cmd);
static void n_append(void* slf, nCmd* cmd);
static void n_clear(void* slf, nCmd* cmd);
static void n_checkforgeometryerror(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
*/
void
n_initcmds(nClass* cl)
{
    cl->BeginCmds();

    cl->AddCmd("b_save_os",  'GTNV', n_save);

    // vertex adding
    cl->AddCmd("i_beginaddvertex_v",      'BADV', n_beginaddvertex);
    cl->AddCmd("v_addcoord_ifff",         'ACOD', n_addcoord);
    cl->AddCmd("v_addnormal_ifff",        'ANRM', n_addnormal);
    cl->AddCmd("v_addcolor_iffff",        'ACLR', n_addcolor);
    cl->AddCmd("v_adduv_iiff",            'AUVS', n_adduv);
    cl->AddCmd("v_addtangent_ifff",       'ATNG', n_addtangent);
    cl->AddCmd("v_addbinormal_ifff",      'ABNM', n_addbinormal);
    cl->AddCmd("v_addjointindices_iffff", 'AJID', n_addjointindices);
    cl->AddCmd("v_addweights_iffff",      'AWGH', n_addweights);
    cl->AddCmd("v_endaddvertex_v",        'EADV', n_endaddvertex);

    // triangle adding
    cl->AddCmd("b_addtriangle_iiii",  'ADDT', n_addtriangle);

    cl->AddCmd("v_transform_ffffffffffffffff",  'XFRM', n_transform);
    cl->AddCmd("v_flipuvs_v",  'FLIP', n_flipuvs);

    // building mesh components
    cl->AddCmd("v_buildtrianglenormals_v",  'BDTN', n_buildtrianglenormals);
    cl->AddCmd("v_buildvertexnormals_v",    'BDVN', n_buildvertexnormals);
    cl->AddCmd("v_buildvertextangents_b",   'BDVT', n_buildvertextangents);

    cl->AddCmd("v_cleanup_v",  'CLUP', n_cleanup);
    cl->AddCmd("v_optimize_v",  'OPTI', n_optimize);
    cl->AddCmd("v_createedges_v",  'CRED', n_createedges);

    cl->AddCmd("i_append_o", 'APND', n_append);
    cl->AddCmd("v_clear_v",  'CLEA', n_clear);

    cl->AddCmd("v_checkforgeometryerror_v",  'CHKG', n_checkforgeometryerror);

    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
*/
static void
n_save(void* slf, nCmd* cmd)
{
    nScriptableMeshBuilder* self = static_cast<nScriptableMeshBuilder*>(slf);
    void* server = cmd->In()->GetO();
    const char* filename = cmd->In()->GetS();
    nFileServer2*   fileServer = static_cast<nFileServer2*>(server);

    bool    ret = self->nMeshBuilder::Save(fileServer, filename);

    cmd->Out()->SetB(ret);
}

//------------------------------------------------------------------------------
/**
*/
static
void n_beginaddvertex(void* slf, nCmd* cmd)
{
    nScriptableMeshBuilder* self = static_cast<nScriptableMeshBuilder*>(slf);

    int index = self->BeginAddVertex();
    cmd->Out()->SetI(index);
}

//------------------------------------------------------------------------------
/**
*/
static
void n_addcoord(void* slf, nCmd* cmd)
{
    nScriptableMeshBuilder* self = static_cast<nScriptableMeshBuilder*>(slf);

    int index = cmd->In()->GetI();
    float x = cmd->In()->GetF();
    float y = cmd->In()->GetF();
    float z = cmd->In()->GetF();
    self->AddCoord(index, x, y, z);
}

//------------------------------------------------------------------------------
/**
*/
static
void n_addnormal(void* slf, nCmd* cmd)
{
    nScriptableMeshBuilder* self = static_cast<nScriptableMeshBuilder*>(slf);

    int index = cmd->In()->GetI();
    float x = cmd->In()->GetF();
    float y = cmd->In()->GetF();
    float z = cmd->In()->GetF();
    self->AddNormal(index, x, y, z);
}

//------------------------------------------------------------------------------
/**
*/
static
void n_addcolor(void* slf, nCmd* cmd)
{
    nScriptableMeshBuilder* self = static_cast<nScriptableMeshBuilder*>(slf);

    int index = cmd->In()->GetI();
    float r = cmd->In()->GetF();
    float g = cmd->In()->GetF();
    float b = cmd->In()->GetF();
    float a = cmd->In()->GetF();
    self->AddColor(index, r, g, b, a);
}

//------------------------------------------------------------------------------
/**
*/
static
void n_adduv(void* slf, nCmd* cmd)
{
    nScriptableMeshBuilder* self = static_cast<nScriptableMeshBuilder*>(slf);

    int index = cmd->In()->GetI();
    int layer = cmd->In()->GetI();
    float u = cmd->In()->GetF();
    float v = cmd->In()->GetF();
    self->AddUv(index, layer, u, v);
}

//------------------------------------------------------------------------------
/**
*/
static
void n_addtangent(void* slf, nCmd* cmd)
{
    nScriptableMeshBuilder* self = static_cast<nScriptableMeshBuilder*>(slf);

    int index = cmd->In()->GetI();
    float x = cmd->In()->GetF();
    float y = cmd->In()->GetF();
    float z = cmd->In()->GetF();
    self->AddTangent(index, x, y, z);
}

//------------------------------------------------------------------------------
/**
*/
static
void n_addbinormal(void* slf, nCmd* cmd)
{
    nScriptableMeshBuilder* self = static_cast<nScriptableMeshBuilder*>(slf);

    int index = cmd->In()->GetI();
    float x = cmd->In()->GetF();
    float y = cmd->In()->GetF();
    float z = cmd->In()->GetF();
    self->AddBinormal(index, x, y, z);
}

//------------------------------------------------------------------------------
/**
*/
static
void n_addjointindices(void* slf, nCmd* cmd)
{
    nScriptableMeshBuilder* self = static_cast<nScriptableMeshBuilder*>(slf);

    int index = cmd->In()->GetI();
    float a = cmd->In()->GetF();
    float b = cmd->In()->GetF();
    float c = cmd->In()->GetF();
    float d = cmd->In()->GetF();
    self->AddJointIndices(index, a, b, c, d);
}

//------------------------------------------------------------------------------
/**
*/
static
void n_addweights(void* slf, nCmd* cmd)
{
    nScriptableMeshBuilder* self = static_cast<nScriptableMeshBuilder*>(slf);

    int index = cmd->In()->GetI();
    float a = cmd->In()->GetF();
    float b = cmd->In()->GetF();
    float c = cmd->In()->GetF();
    float d = cmd->In()->GetF();
    self->AddWeights(index, a, b, c, d);
}

//------------------------------------------------------------------------------
/**
*/
static
void n_endaddvertex(void* slf, nCmd* cmd)
{
    nScriptableMeshBuilder* self = static_cast<nScriptableMeshBuilder*>(slf);
    self->EndAddVertex();
}

//------------------------------------------------------------------------------
/**
*/
static void
n_addtriangle(void* slf, nCmd* cmd)
{
    nScriptableMeshBuilder* self = static_cast<nScriptableMeshBuilder*>(slf);
    int     group = cmd->In()->GetI();

    int     vert0 = cmd->In()->GetI();
    int     vert1 = cmd->In()->GetI();
    int     vert2 = cmd->In()->GetI();

    nMeshBuilder::Triangle    t;

    t.SetGroupId(group);
    t.SetVertexIndices(vert0, vert1, vert2);

    self->AddTriangle(t);
}

//------------------------------------------------------------------------------
/**
*/
static void
n_transform(void* slf, nCmd* cmd)
{
    nScriptableMeshBuilder* self = static_cast<nScriptableMeshBuilder*>(slf);

    matrix44    worldMat;

    worldMat.m[0][0] = cmd->In()->GetF();
    worldMat.m[0][1] = cmd->In()->GetF();
    worldMat.m[0][2] = cmd->In()->GetF();
    worldMat.m[0][3] = cmd->In()->GetF();
    worldMat.m[1][0] = cmd->In()->GetF();
    worldMat.m[1][1] = cmd->In()->GetF();
    worldMat.m[1][2] = cmd->In()->GetF();
    worldMat.m[1][3] = cmd->In()->GetF();
    worldMat.m[2][0] = cmd->In()->GetF();
    worldMat.m[2][1] = cmd->In()->GetF();
    worldMat.m[2][2] = cmd->In()->GetF();
    worldMat.m[2][3] = cmd->In()->GetF();
    worldMat.m[3][0] = cmd->In()->GetF();
    worldMat.m[3][1] = cmd->In()->GetF();
    worldMat.m[3][2] = cmd->In()->GetF();
    worldMat.m[3][3] = cmd->In()->GetF();

    self->Transform(worldMat);
}

//------------------------------------------------------------------------------
/**
*/
static void
n_flipuvs(void* slf, nCmd* cmd)
{
    nScriptableMeshBuilder* self = static_cast<nScriptableMeshBuilder*>(slf);
    self->FlipUvs();
}

//------------------------------------------------------------------------------
/**
*/
static void
n_buildtrianglenormals(void* slf, nCmd* cmd)
{
    nScriptableMeshBuilder* self = static_cast<nScriptableMeshBuilder*>(slf);
    self->BuildTriangleNormals();
}

//------------------------------------------------------------------------------
/**
*/
static void
n_buildvertexnormals(void* slf, nCmd* cmd)
{
    nScriptableMeshBuilder* self = static_cast<nScriptableMeshBuilder*>(slf);
    self->BuildVertexNormals();
}

//------------------------------------------------------------------------------
/**
*/
static void
n_buildvertextangents(void* slf, nCmd* cmd)
{
    nScriptableMeshBuilder* self = static_cast<nScriptableMeshBuilder*>(slf);
    self->BuildVertexTangents(cmd->In()->GetB());
}

//------------------------------------------------------------------------------
/**
*/
static void
n_append(void* slf, nCmd* cmd)
{
    nScriptableMeshBuilder* self = static_cast<nScriptableMeshBuilder*>(slf);

    nScriptableMeshBuilder* src = static_cast<nScriptableMeshBuilder*>(cmd->In()->GetO());

    int index = self->Append(*src);
    cmd->Out()->SetI(index);
}

//------------------------------------------------------------------------------
/**
*/
static void
n_cleanup(void* slf, nCmd* cmd)
{
    nScriptableMeshBuilder* self = static_cast<nScriptableMeshBuilder*>(slf);
    self->Cleanup(NULL);
}

//------------------------------------------------------------------------------
/**
*/
static void
n_optimize(void* slf, nCmd* cmd)
{
    nScriptableMeshBuilder* self = static_cast<nScriptableMeshBuilder*>(slf);
    self->Optimize();
}

//------------------------------------------------------------------------------
/**
*/
static void
n_createedges(void* slf, nCmd* cmd)
{
    nScriptableMeshBuilder* self = static_cast<nScriptableMeshBuilder*>(slf);
    self->CreateEdges();
}

//------------------------------------------------------------------------------
/**
*/
static void
n_clear(void* slf, nCmd* cmd)
{
    nScriptableMeshBuilder* self = static_cast<nScriptableMeshBuilder*>(slf);
    self->Clear();
}

//------------------------------------------------------------------------------
/**
*/
static void
n_checkforgeometryerror(void* slf, nCmd* cmd)
{
    nScriptableMeshBuilder* self = static_cast<nScriptableMeshBuilder*>(slf);

    nArray<nString> results = self->CheckForGeometryError();

    for (int x = 0; x < results.Size(); x++)
    {
        nString msg = results[x];
        printf("%s", msg.Get());
    }
}
//------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------
