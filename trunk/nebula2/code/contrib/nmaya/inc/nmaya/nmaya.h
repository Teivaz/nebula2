#ifndef NMAYA_H
#define NMAYA_H

// Original maya->Nebula 1 exporter copyright 2003 Sane Asylum Studios, Inc.  All Rights Reserved.
// Nebula2 port copyright 2004 Rafael Van Daele-Hunt, but Johannes Kellner deserves credit
// for his 3DSMax->Nebula 2 exporter as well.
//--------------------------------------------------------------------------------
// You may choose to accept and redistribute this program under any of the following licenses:
//   * Nebula License (http://nebuladevice.sourceforge.net/doc/source/license.txt)
//   * MIT License (http://www.opensource.org/licenses/mit-license.php)
//   * GPL License (http://www.gnu.org/copyleft/gpl.html)
//   * LGPL License (http://www.gnu.org/copyleft/lgpl.html)

#include "nmaya/options.h"
#include <maya/MVectorArray.h>
#include <maya/MAnimControl.h>
#include <maya/MFnMesh.h>
#include <tools/nmeshbuilder.h>
#include <tools/nanimbuilder.h>
#include <mathlib/vector.h>
#include "kernel/nkernelserver.h"
#include "kernel/nscriptserver.h"
#include "kernel/nref.h"

class MFnSkinCluster;
class MItMeshPolygon;
class MFnDagNode;
class nAnimBuilder;
class MFnTransform;

/**
    @class nMaya
    @ingroup NMayaContribModule
*/
class nMaya
{
public:
    nMaya(int argc, char *argv[]);
    ~nMaya();

    MStatus Export();

private:

    int m_iVertProcessed; // our own vertex-ids
    int m_iGroupProcessed; // our own group-ids
    int m_iMaterialsProcessed; // our own material-ids

    MStringArray m_szGroupName;
    
    Options options;
    nMeshBuilder m_MeshBuilder;

    nKernelServer kernelServer;
    nRef<nPersistServer> persistServer;
    nRef<nFileServer2> fileServer;
    nRef<nScriptServer> scriptServer;

    // Analyze the scene, for datachecks
    MStatus Analyze();
    // Used for Weightmap extraction
    MStatus GetClusterMembership( MObject &obCluster, MSelectionList &obMembers);
  
    MStatus openMayaFile();
    MStatus parse();   
    
    MStatus InitVerticesAndFaces( const MDagPath& vDagPath);
    // Rewritten to work independently from Maya and Nebula vertex-indices
    MStatus ExtractVerticesAndTriangles( MItMeshPolygon& vPolyIterator , MFnMesh& fnMesh);

    // Extracts static geometry. Static geometry is a model without joints.
    MStatus ExportSimpleMesh();
    // Extracts rigged geometry. Has joints.
    MStatus ExportRiggedMesh();
    
    // Save the meshdata
    MStatus WriteMeshFile();
    // rewritten to use the scriptserver
    void WriteCreationScript();
    
// Older functions which need to be reviewed and improved for the new design using a scriptserver
    MStatus WriteAnimKeys();
    MStatus WriteCurves( nAnimBuilder::Group& OUT_animGroup, int& INOUT_numUncompressedCurves, const MAnimControl& vAnimCtrl, const MFnTransform& vJointFuncSet, int startFrame, int endFrame );
    MStatus SetWeights( nMeshBuilder::Vertex& vertex, const MDagPath& vDagPath, const MObject& vGeoObject, const MFnSkinCluster& vSkinClustFuncSet );
    MStatus WriteRiggedAnimKeys();
    bool isVisible( MFnDagNode&, MStatus& ) ;
    nAnimBuilder::Group CreateAnimGroup( int numFrames, nAnimBuilder::Group::LoopType loopType );
    int CalcStartFrame( int minFrame );
    int CalcEndFrame( int maxFrame, int totalLen );
    nAnimBuilder::Group::LoopType CalcPlaybackMode( const MAnimControl& vAnimCtrl );

    void WriteSkinAnimator_TCL( std::ofstream& fpScript );
    MStatus WriteJointTree_TCL( std::ofstream& fpScript );
    MStatus WriteJoint_TCL(int ourIdx, std::ostream&);
    void WriteSkinShapeNode_TCL( std::ofstream& fpScript );
    void WriteSkinAnimator( std::ofstream& fpScript );
    void WriteSkinShapeNode( std::ofstream& fpScript );
    MStatus WriteJointTree( std::ofstream& fpScript );
    MStatus SaveAnim( nAnimBuilder::Group& animGroup );

    vector4 GetRotVector( const MFnTransform& fnTransform );
    vector4 GetPosVector( const MFnTransform& fnTransform );
    vector4 GetScaleVector( const MFnTransform& fnTransform );

    // Names of the UV sets
    MStringArray m_vUVSetNames;
    // Array of the initial (relaxed) translations of the joints
    MVectorArray m_vRelaxedTrans;

    // Write a joint and its children (recursively)
    MStatus WriteJoint(int ourIdx, std::ostream&);

    
    bool m_Skinned;
    int m_VertOffset;

};

#endif
