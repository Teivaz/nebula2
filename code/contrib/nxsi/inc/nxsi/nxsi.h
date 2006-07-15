//-----------------------------------------------------------------------------
/**
    @class nXSI
    @ingroup NXSIContribModule

    @brief dotXSI to nebula2 converter.

    (C) 2004 Ville Ruusutie
*/
//-----------------------------------------------------------------------------
// See the file "nxsi_license.txt" for information on usage and redistribution
// of this file, and for a DISCLAIMER OF ALL WARRANTIES.
//-----------------------------------------------------------------------------
#ifndef N_XSI_H
#define N_XSI_H
//-----------------------------------------------------------------------------
#include "nxsi/nxsioptions.h"
#include "nxsi/nxsitypes.h"
#include <scene/nshapenode.h>
#include <tools/nmeshbuilder.h>
#include <tools/nanimbuilder.h>
#include <tools/nskinpartitioner.h>
#include <mathlib/vector.h>
#include <kernel/nkernelserver.h>
#include <kernel/nscriptserver.h>
#include <kernel/nref.h>

#define __XSI_APPLICATION_
#define XSI_STATIC_LINK
#include <FTK/SemanticLayer.h>
//-----------------------------------------------------------------------------

#define NXSI_MAX_UVSETS 16

class nXSI
{
public:
    /// constructor
    nXSI(int argc, char *argv[]);
    /// destructor
    ~nXSI();

    /// export scene
    bool Export();

private:
    bool InitXSIParser();
    void DeinitXSIParser();

    //@{
    /// base type handling
    void HandleSIModel(CSLModel* templ);
    void HandleSICamera(CSLCamera* templ);
    void HandleSILight(CSLLight* templ);
    //@}

    //@{
    /// mesh handling
    void HandleSIMesh(CSLMesh* templ);
    void HandleSIMeshSkeleton(CSLMesh* templ, nString& skinName, nArray<nXSIWeight>& weightList);
    void HandleSITriangleList(CSLTriangleList* templ, CSLShape* shape, const nArray<nXSIWeight>& weightList, uint groupId);
    //@}

    //@{
    /// animation handling
    void BuildJointAnimations(const nArray<CSLModel*>& joint_list, int jointCount);
    bool BuildTransformAnimation(CSLTransform* transform, nString& animName);
    //@}

    //@{
    /// material handling
    void HandleXSIMaterialVariables(CSLXSIMaterial* templ, nShapeNode* node, bool isSkinned);
    void HandleSIMaterialVariables(CSLMaterial* templ, nShapeNode* node, bool isSkinned);
    //@}

    //@{
    /// data conversion
    void ConvertSIPolygonList(CSLPolygonList* templ);
    void ConvertSITriangleStripList(CSLTriangleStripList* templ);
    //@}

private:
    nXSIOptions             options;

    CSLScene                xsiScene;

    nMeshBuilder            meshBuilder;
    nAnimBuilder            animBuilder;
    nSkinPartitioner        skinPartioner;

    nKernelServer           kernelServer;
    nRef<nScriptServer>     scriptServer;
    nRef<nVariableServer>   variableServer;

    uint                    meshGroupId;
    uint                    animGroupId;
};

//-----------------------------------------------------------------------------
#endif
