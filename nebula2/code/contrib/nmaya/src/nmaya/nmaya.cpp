// Original maya->Nebula 1 exporter copyright 2003 Sane Asylum Studios, Inc.  All Rights Reserved.
// Nebula2 port copyright 2004 Rafael Van Daele-Hunt, but Johannes Kellner deserves credit
// for his 3DSMax->Nebula 2 exporter as well.
//--------------------------------------------------------------------------------
// You may choose to accept and redistribute this program under any of the following licenses:
//   * Nebula License (http://nebuladevice.sourceforge.net/doc/source/license.txt)
//   * MIT License (http://www.opensource.org/licenses/mit-license.php)
//   * GPL License (http://www.gnu.org/copyleft/gpl.html)
//   * LGPL License (http://www.gnu.org/copyleft/lgpl.html)

//--------------------------------------------------------------------
// notes: add the following to include path
//  INCDIR += $(IPATH_OPT)"[Maya Root]/include"
//  LIBDIR += $(LPATH_OPT)"[Maya Root]/lib"
//--------------------------------------------------------------------
// TODO:
// joint hierarchies must have unique node names (although they shouldn't have to...?)
// |root node hard-coded
//--------------------------------------------------------------------

#include "nmaya/nmaya.h"

#define _BOOL
#define REQUIRE_IOSTREAM
#include <maya/MFnSkinCluster.h>
#include <maya/MFnMesh.h>
#include <maya/MFnIkJoint.h>
#include <maya/MItDag.h>
#include <maya/MItDependencyNodes.h>
#include <maya/MItGeometry.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MDagPath.h>
#include <maya/MDagPathArray.h>
#include <maya/MFloatArray.h>
#include <maya/MMatrix.h>
#include <maya/MFloatPointArray.h>
#include <maya/MPointArray.h>
#include <maya/MPlug.h>
#include <maya/MString.h>
#include <maya/MFnSet.h>
#include <maya/MSelectionList.h>
#include <maya/MItDependencyGraph.h>

#include <fstream>

#pragma comment( lib, "Foundation" )
#pragma comment( lib, "OpenMaya" )
#pragma comment( lib, "OpenMayaAnim" )

#include "mathlib/quaternion.h"
#include "kernel/nfileserver2.h"
#include "kernel/npersistserver.h"
#include "tools/nskinpartitioner.h"
#include "scene/nscenenode.h"
#include "scene/nshapenode.h"
#include "scene/nmaterialnode.h"
#include "gfx2/nshader2.h"

using std::cerr;
using std::endl;

#define STATUS(stat, str) if (!stat) { stat.perror(str); continue; }

// DAG paths to our models' joints
MDagPathArray m_vJointPaths;


/********************************************************\
 * Find the last '|' symbol in the string and return a pointer to it
\********************************************************/
const char* FindLastLine( const char* szPString, int& nPIndex )
{
  nPIndex = strlen(szPString);
  const char* szPointer = szPString + nPIndex;
  while (szPointer != szPString)
  {
    if (*szPointer == '|') break;
    szPointer--;
    nPIndex--;
  }

  return (szPointer == szPString && *szPointer != '|') ? NULL : szPointer;
}


// a FindShortestUniquePath would be useful (also would help check for duplicates)


MStatus nMaya::openMayaFile()
{
  const char* fileName =  options.mayaInputFilename().asChar();
  if ( options.verboseP() ) cerr << fileName << endl;

  // prepare Maya to read a new scene file
  MFileIO::newFile( true );

  // read the scene file
  MStatus stat = MFileIO::open( fileName );
  assert( stat );

  // remove any temporary data created by Maya while loading (things like the undo list which we won't be using)
  stat = MGlobal::executeCommand( "delete -ch" );
  assert( stat );

  return stat;
}

//--------------------------------------------------------------------
int 
main(int argc, char *argv[])
{
    nMaya m( argc, argv );
    return m.Export().statusCode();
}  
  

nMaya::nMaya(int argc, char *argv[]) :
    m_Skinned( false ),
    m_iVertProcessed(-1),
    m_iGroupProcessed(-1),
    m_iMaterialsProcessed(-1)
{
    // TODO: have parse not do the whole command line, but stop (and restart) at the input
    options.parse( argc, argv );
    if( 0 == options.mayaInputFilename().length() )
    {
        cerr << "No input file given!" << endl;
        options.usage();  // usage() exits with error code
    }
}

MStatus nMaya::Export()
{
    // Init all necessary for script export
    scriptServer = (nScriptServer*)kernelServer.New("ntclserver", "/sys/servers/script");
    persistServer = (nPersistServer*)kernelServer.New("npersistserver","/sys/servers/persist");
    fileServer = (nFileServer2*) kernelServer.New( "nfileserver2", "/sys/servers/file" );

    MStatus vResult = MLibrary::initialize( "maya" );  CHECK_MSTATUS( vResult );

    // TODO: iterate over mayaInputFile's (or something like that)
    vResult = openMayaFile(); CHECK_MSTATUS( vResult );
    vResult = parse();  CHECK_MSTATUS( vResult );
    
    if ( !options.analyzeP() ) {
        if( m_Skinned )
        {
            if ( options.exportAnimP() ) 
            {
                vResult = WriteAnimKeys();  CHECK_MSTATUS( vResult );
            }
        
            if ( options.exportMeshP() ) 
            { 
                vResult = ExportRiggedMesh(); CHECK_MSTATUS( vResult ); 
            }

            if ( options.exportScriptP() ) 
            {
                WriteCreationScript();
            }
        }
        else
        {
            if ( options.exportMeshP() ) 
            { 
                vResult = ExportSimpleMesh(); CHECK_MSTATUS( vResult ); 
            }
        }
    } else { 
        Analyze(); // we analyze the scene
    }
    return vResult;
}

MStatus nMaya::Analyze() {
    
    MStatus vResult;

    // create iterator
    MItDag dagIterator( MItDag::kBreadthFirst, MFn::kInvalid, &vResult);

    // Look for meshes
    cerr << "Meshes:" << endl;
    for ( dagIterator.reset(); !dagIterator.isDone(); dagIterator.next() )
    {
        MDagPath dagPath;
        
        vResult = dagIterator.getPath(dagPath);

        MFnDagNode dagNode( dagPath, &vResult );

        
        if (dagPath.hasFn(MFn::kMesh))
        {
            // we want the real thing !
            if ( dagNode.isIntermediateObject()) continue;
            if ( dagPath.hasFn( MFn::kTransform )) continue;
            cerr << dagNode.fullPathName().asChar()<< endl;
            
            // access the mesh
            MFnMesh fnMesh( dagPath, &vResult );
            CHECK_MSTATUS_AND_RETURN_IT(vResult);
            MString setName;
            fnMesh.getCurrentUVSetName(setName);
            cerr << "UvSets: " << fnMesh.numUVSets() << " -> " << setName.asChar() << endl;
        }
    }

    // Look for Joints
    cerr << endl << "Joints:" << endl;
    for ( dagIterator.reset(); !dagIterator.isDone(); dagIterator.next() )
    {
        MDagPath dagPath;
        
        vResult = dagIterator.getPath(dagPath);

        MFnDagNode dagNode( dagPath, &vResult );

        
        if (dagPath.hasFn(MFn::kJoint))
        {
            cerr << '[' << dagPath.fullPathName().asChar() << ']' << endl;
        }
    }
    
    return vResult;
};

nMaya::~nMaya()
{
  MLibrary::cleanup();
}
  
/*
  Fill in the m_vJointPaths array with paths to all the joints.
*/
MStatus nMaya::parse()
{
    // Remove / Improve this check

    MStatus vResult = MS::kSuccess;
    MItDag dagIter( MItDag::kDepthFirst, MFn::kJoint, &vResult );
    if ( dagIter.isDone() || MS::kSuccess != vResult )
    {
        cerr << "This model has no joints." << endl;
    }
    else
    {
        
        for ( ; !dagIter.isDone(); dagIter.next() )
        {
            MDagPath dagPath;
            vResult = dagIter.getPath( dagPath );
            n_assert( (MS::kSuccess == vResult) && "Oh oh, traversal got screwed!" );
            m_vJointPaths.append( dagPath );
            cerr << '[' << dagPath.fullPathName().asChar() << ']' << endl;
        }

        m_Skinned = true;
    }
    

    return vResult;
}

/********************************************************\
 * Write the mesh file                                  *
\********************************************************/
MStatus nMaya::WriteMeshFile() 
{
    MStatus vResult = MS::kSuccess;


    cerr << "Last vertex processed :        " << m_iVertProcessed << endl;
    cerr << "Last group processed :     " << m_iGroupProcessed << endl;
    cerr << "Number of created vertices :   " << m_MeshBuilder.GetNumVertices() << endl;
    cerr << "Number of created triangles :  " << m_MeshBuilder.GetNumTriangles() << endl;

    // Clean up and Rebuild Normals and Tangents
    m_MeshBuilder.Cleanup(0);
    m_MeshBuilder.BuildTriangleNormals();
    m_MeshBuilder.BuildVertexTangents();
    
    if( !m_MeshBuilder.Save( (nFileServer2*)fileServer.get(), options.meshFilename().Get() ) )
    {
        cerr << "WriteMeshFile: error in m_MeshBuilder.Save: " << options.meshFilename().Get() << endl;
    }

   return vResult;
}

MStatus nMaya::WriteAnimKeys()
{
 //   return WriteSimpleAnimKeys();
    return WriteRiggedAnimKeys();
}


bool nonZero( float val )
{
    return abs(val) > 0.0001;
}

MStatus nMaya::SetWeights( nMeshBuilder::Vertex& vertex, const MDagPath& vDagPath, const MObject& vGeoObject, const MFnSkinCluster& vSkinClustFuncSet )
{   // Influence objects (joints in most cases) weights
    MStatus vResult = MS::kSuccess;
    MFloatArray vWeights;
    unsigned int nInfluenceCount;
    vResult = vSkinClustFuncSet.getWeights(vDagPath, vGeoObject, vWeights, nInfluenceCount);    CHECK_MSTATUS_AND_RETURN_IT(vResult);

    unsigned int numJointsUsed = 0;
    float weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    int jointIndices[4] = { -1, -1, -1, -1 };
    for( unsigned int i = 0; i < vWeights.length() && numJointsUsed < 4; ++i )
    {
        if( nonZero( vWeights[i] ) )
        {
            weights[numJointsUsed] = vWeights[i];
            jointIndices[numJointsUsed] = i;
            ++numJointsUsed;
        }
        
    }
    vertex.SetJointIndices( vector4( (float)jointIndices[0], (float)jointIndices[1], (float)jointIndices[2], (float)jointIndices[3] ) );
    vertex.SetWeights( vector4( weights[0], weights[1], weights[2], weights[3] ) );
    return vResult;
}

MStatus nMaya::ExportRiggedMesh( )
{
    MStatus vResult = MS::kSuccess;
    
    // Disable IK Solving
    MGlobal::executeCommand("ikSystem -e -sol 0;");
    MGlobal::selectByName( "|root" );
    MGlobal::executeCommand("dagPose -r -g -bp");

    MItDependencyNodes vDepIterator(MFn::kSkinClusterFilter, &vResult);  CHECK_MSTATUS_AND_RETURN_IT( vResult );

    
    // Now traverse all those skin clusters
    for (; !vDepIterator.isDone(); vDepIterator.next())
    {
        MObject vCurObject = vDepIterator.item(&vResult);  STATUS(vResult, "ExportRiggedMesh: MItDependencyNodes::item failed");
        MFnSkinCluster vSkinClustFuncSet(vCurObject, &vResult);  STATUS(vResult, "ExportRiggedMesh: MFnSkinCluster constructor failed");
        
        // Get the output geometries connected to us (those are, in fact, meshes)
        unsigned int nNumConnect = vSkinClustFuncSet.numOutputConnections(&vResult);  STATUS(vResult, "ExportRiggedMesh: MFnSkinCluster::numOutputConnections failed");
        // Loop through all the meshes - begin
        for (unsigned int numMeshes = 0; numMeshes < nNumConnect; ++numMeshes )
        {
            unsigned int nPlugIdx = vSkinClustFuncSet.indexForOutputConnection(numMeshes, &vResult);   STATUS(vResult, "ExportRiggedMesh: MFnSkinCluster::indexForOutputConnection failed");
            MDagPath vDagPath;
            
            vResult = vSkinClustFuncSet.getPathAtIndex(nPlugIdx, vDagPath);  STATUS(vResult, "ExportRiggedMesh: MFnSkinCluster::getPathAtIndex failed");
            
            vResult = InitVerticesAndFaces( vDagPath);  STATUS(vResult, "ExportRiggedMesh: InitFaces failed");

            MItGeometry vGeoIterator(vDagPath, &vResult);
            CHECK_MSTATUS_AND_RETURN_IT(vResult);
            int numVerts = 0;
            for (; !vGeoIterator.isDone(); vGeoIterator.next())
            {
                nMeshBuilder::Vertex& vertex = m_MeshBuilder.GetVertexAt(numVerts);
                
                MObject vGeoObject = vGeoIterator.component(&vResult); // This is actually the vertex (hopefully)
                STATUS(vResult, "SaveVertexAttributes: MItGeometry::component failed ");
                
                if( MFn::kMeshVertComponent != vGeoObject.apiType() )
                {
                    continue;
                }

                vResult = SetWeights( vertex, vDagPath, vGeoObject, vSkinClustFuncSet );
                STATUS( vResult, "SetWeights failed" );
                n_assert( vGeoIterator.index( &vResult ) == numVerts ); // in InitFaces we rely on the indices of the vertex list in m_MeshBuilder corresponding to Maya's
                n_assert( !MFAIL( vResult ) );
                ++numVerts;
            }
/**/
        }
    } 
/*
    MTime timeFirstFrame = MAnimControl::minTime();
    
    // reset time
    // Move Maya to first frame
    MGlobal::viewFrame(timeFirstFrame);

    int iJointCount = 0;

    MItDag dagIterator( MItDag::kBreadthFirst, MFn::kInvalid, &vResult);

    for ( dagIterator.reset(); !dagIterator.isDone(); dagIterator.next() )
    {
        MDagPath dagPath;
        
        dagIterator.getPath(dagPath);

        MFnDagNode dagNode( dagPath, &vResult );

        if (dagPath.hasFn(MFn::kJoint))
        {
            MObject obJoint = dagNode.object();

            MItDependencyGraph mGraph(obJoint, MFn::kGeometryFilt ,  MItDependencyGraph::kDownstream, MItDependencyGraph::kDepthFirst, MItDependencyGraph::kNodeLevel);
            
            for (;  !mGraph.isDone(); mGraph.next())
            {
                //cout << "Entering Loop for Weightmaps..."<< endl;

                MObject mObject;

                if ( mGraph.thisNodeHasUnknownType() == MS::kSuccess ) 
                {
                    // strange, but hey this works !
                    //cout << "mGraph.thisNodeHasUnknownType() !"<< endl;
                }
                
                mObject = mGraph.thisNode();// we get a null pointer here - fixed !
                // changed this !!
                MFnSkinCluster  mGFilt(mObject);
                
                MSelectionList obMembers;
                GetClusterMembership(mObject, obMembers);

                unsigned nGeometryInCluster;
                nGeometryInCluster = mGFilt.numOutputConnections();
                //cout << "GeomInCluster :"<< nGeometryInCluster << endl;

                for (size_t ii = 0; ii < nGeometryInCluster; ii++)
                {
                    //cout << "Entering 2nd Loop for Weightmaps..."<< endl;
                    MDagPath skinPath;
                    unsigned int index;

                    index = mGFilt.indexForOutputConnection(ii , &vResult);
                    if (vResult != MS::kSuccess)
                        continue;

                    vResult = mGFilt.getPathAtIndex(index, skinPath);
                    STATUS(vResult, "Error retrieving with getPathAtIndex()");

                    MItGeometry gIter(skinPath);
                    
                    for (; !gIter.isDone(); gIter.next())
                    {
                        //cout << "Entering 3rd Loop for Weightmaps..."<< endl;

                        MObject comp = gIter.component(&vResult);
                        if (vResult != MS::kSuccess)
                            continue;

                        // get weights

                        MDagPath joint_path;
                        dagNode.getPath ( joint_path );

                        unsigned inf_index = mGFilt.indexForInfluenceObject (joint_path, &vResult);

                        if (vResult != MS::kSuccess)
                        {
                            cerr << "Error getting index for Influence Object" << endl;
                            continue;
                        }

                        MFloatArray wts;
                        
                        vResult = mGFilt.getWeights(skinPath, comp, inf_index, wts); // the index must be the joint-index !!!
                        
                        
                        if (vResult != MS::kSuccess)
                        {
                            cerr << "Error getting weights from component\n";
                            continue;
                        }

                        //cout << "Influence Index : " << inf_index << endl;
                        //cout << "Length : " << wts.length() << endl;
                        //cout << skinPath.fullPathName() << endl;

                        
                

                        if (!obMembers.hasItem(skinPath,comp))
                        {
                            // weight reported for this component is not in this Cluster
                            continue;

                        }
                        
                        if ( wts[0] == 0.0f ) {
                            //cout << "Value skipped" << endl;
                            continue;
                        }

                        // check for vertices with the same position and add them !

                        for ( int si = 0; si < m_iVertProcessed; si++)
                        {
                            MPoint p = gIter.position(MSpace::kWorld);

                            nMeshBuilder::Vertex& curVert = m_MeshBuilder.GetVertexAt(si);

                            if ( (curVert.coord.x == p.x) && (curVert.coord.y == p.y) && (curVert.coord.z == p.z) )
                            {
                                curVert.SetWeights(vector4(wts[0],wts[1],wts[2],wts[3]));
                                curVert.SetJointIndices(vector4(inf_index,-1,-1,-1));
                                //szTemp.Format("    { %d; %f; }\n", skaPool[si].Id, wts[0]);
                                //szWeightsList += szTemp;
                                //iWeightSetCount++;
                            }
                        }
                        

                        
                        //cout << "Weightmapcount :" << iWeightSetCount << endl;
                    }
                }

            }

        }
    }
    */

    // Re-enable IK Solving
    MGlobal::executeCommand("ikSystem -e -sol 1;");
    MGlobal::selectByName( "|root" );
    MGlobal::executeCommand("dagPose -r -g -bp");

    return WriteMeshFile();
}

MStatus nMaya::GetClusterMembership( MObject &obCluster, MSelectionList &obMembers)
{
    MStatus status = MS::kSuccess;
    obMembers.clear();

    MItDependencyGraph itGraph( obCluster, MFn::kSet);

    if (itGraph.isDone())
    {
        cerr << "No Set Found for GetClusterMembership!\n";
        return MS::kFailure;
    }

    MFnSet fnSet(itGraph.thisNode(), &status);

    if (status == MS::kSuccess)
    {
        status = fnSet.getMembers(obMembers, true);
    }

    return status;
}


MStatus nMaya::InitVerticesAndFaces( const MDagPath& vDagPath )
{
    MStatus vResult  = MS::kSuccess;

    // access the mesh
    MFnMesh fnMesh( vDagPath, &vResult );
    CHECK_MSTATUS_AND_RETURN_IT(vResult);

    // we have new group
    m_iGroupProcessed++;
    m_iMaterialsProcessed++;
    
    // save the name
    MFnDagNode dagNode( vDagPath, &vResult );

    if ( options.exportScriptP() )
    {
        // collect the shaderinfo for the new shapenodes
        MObjectArray shaders;
        MIntArray indices;
        
        MPlug curPlug;
        MString strShaderRlName;
        MString strShaderFile;
        MString strDiffMap0;
        MString strBumpMap0;
        vector4 ambColor(0,0,0,1);
        vector4 diffColor(0,0,0,1);
        vector4 specColor(0,0,0,1);
        float specPower = 32.0f;
        int iAlphaRef;
        int iCullMode;

        fnMesh.getConnectedShaders(vDagPath.instanceNumber(),shaders,indices);
              
        if (shaders[0].hasFn(MFn::kDependencyNode)) 
        {
            
            MFnDependencyNode temp(shaders[0]);
            
            MObject shaderNode;
            MPlugArray connections;
            temp.getConnections(connections);

            for(unsigned i = 0; i < connections.length(); i++)
            {
                MPlugArray connectedPlugs;
                connections[i].connectedTo(connectedPlugs, true, false);

                for(unsigned j = 0; j < connectedPlugs.length(); j++)
                {
                    //if(MFnDependencyNode(connectedPlugs[j].node()).typeName() == "lambert")
                    MObject plugNode( connectedPlugs[j].node() );
                    if ( plugNode.hasFn( MFn::kLambert ) ||
                         plugNode.hasFn( MFn::kAnisotropy ) ||
                         plugNode.hasFn( MFn::kPhong ) ||
                         plugNode.hasFn( MFn::kPhongExplorer ) ||
                         plugNode.hasFn( MFn::kBlinn ) ||
                         plugNode.hasFn( MFn::kLayeredShader ) ||
                         plugNode.hasFn( MFn::kRampShader ))
                    {
                        shaderNode = connectedPlugs[j].node();
                        MFnDependencyNode temp1(shaderNode);
                        //cerr << "Collecting Shaderinformation of '" << temp1.name().asChar() << "'" << endl;
                        break;
                    }
                }
                if(!shaderNode.isNull())
                {
                    break;
                }
            }

            //
            // TODO: Find an Efficient way to pull all shaderinfo out of Maya and fill them in.
            //       The RL Toolkit uses costum attrbutes that get attached to the Maya-Shaders
            //       via mel-scripts to store neb-specific stuff. Maya be we should do the same?
            //
        }
        
        cerr << "Creating Shapenode '" << dagNode.name().asChar() << "'" << endl;
        // create the shapenode
        nShapeNode* newShape = (nShapeNode*)kernelServer.New("nshapenode", (nString("/" + options._szScriptFilename.ExtractFileName() + "/" + dagNode.name().asChar())).Get());
        newShape->SetShader(nMaterialNode::StringToFourCC("colr"),strShaderFile.asChar()); 
        newShape->SetTexture( nShader2::DiffMap0, strDiffMap0.asChar());
        newShape->SetTexture( nShader2::BumpMap0, strBumpMap0.asChar());
        newShape->SetVector( nShader2::MatAmbient, ambColor);
        newShape->SetVector( nShader2::MatDiffuse, diffColor);
        newShape->SetVector( nShader2::MatSpecular, specColor);
        newShape->SetFloat( nShader2::MatSpecularPower, specPower);
        newShape->SetInt( nShader2::AlphaRef, iAlphaRef);
        newShape->SetInt( nShader2::CullMode, iCullMode);
        newShape->SetGroupIndex(m_iGroupProcessed);
        //newShape->SetMesh( (nString("levels:" + options.meshFilename().ExtractLastDirName() + "/" + options.meshFilename().ExtractFileName())).Get() ); // change the fixed path
        newShape->SetMesh( (nString("levels:" + options.meshFilename().ExtractFileName())).Get() ); // change the fixed path
    }
    
    // Loop thru the polygons and write them to the nMeshBuilder
    MItMeshPolygon vPolyIterator(vDagPath, MObject::kNullObj, &vResult);
    
    CHECK_MSTATUS_AND_RETURN_IT(vResult);

    vResult = ExtractVerticesAndTriangles( vPolyIterator , fnMesh);
    CHECK_MSTATUS_AND_RETURN_IT( vResult );

    return vResult;
}

MStatus nMaya::ExtractVerticesAndTriangles( MItMeshPolygon& vPolyIterator , MFnMesh& fnMesh) {

    /* 
    OK what's so different in this code here ? Well, we implent our own vertex indices instead
    of passing a copy of the Maya indices to Nebula. This way we can extract anything in our way
    and dont rely on Maya to give us the right indices for extraction. This pretty much
    fixed the last uvmap bug.
    */
    
    MStatus vResult;

    for (; !vPolyIterator.isDone(); vPolyIterator.next()) { // Loop through all triangles

        MPointArray tri_PointList;
        MIntArray vertexList;

        // fill both arrays
        vResult = vPolyIterator.getTriangles(tri_PointList,vertexList, MSpace::kWorld);
        CHECK_MSTATUS_AND_RETURN_IT(vResult);

        // create the needed vertices and their very own indices
        nMeshBuilder::Vertex vertex;
        for( int i = 0; i < 3; i++ ) {
            m_MeshBuilder.AddVertex( vertex );
            m_iVertProcessed++;
        }

        // create the new triangle and add it to the meshbuilder
        nMeshBuilder::Triangle tri;
        tri.SetVertexIndices(m_iVertProcessed-2, m_iVertProcessed-1, m_iVertProcessed);
        tri.SetNormal(vector3(0,1,0)); // we need to init this, otherwise lighting issues appear !
        tri.SetTangent(vector3(0,1,0)); // we need to init this, otherwise lighting issues appear ! 
        tri.SetGroupId(m_iGroupProcessed);
        tri.SetMaterialId(m_iMaterialsProcessed);
        m_MeshBuilder.AddTriangle( tri );

        // fill in the normals | TODO : Find out what happens with the normals, we shouldn't need to rebuild them when the file is saved !
        MFloatVectorArray normalArray;
        fnMesh.getFaceVertexNormals( vPolyIterator.index(), normalArray, MSpace::kWorld );

        // uvmap extraction
        float u = 0, v = 0;

        // apply some size multiplier
        float fSize = 1.0f;
        if ( options.hasSizeMult() )
            fSize = options.getSizeMult().asFloat();

        // vertex colors
        MColor curColor;

        /* TODO: Ok this is some very ugly code right now, put this into a loop */
        nMeshBuilder::Vertex& vert1 = m_MeshBuilder.GetVertexAt( m_iVertProcessed-2 );
        vert1.SetCoord( vector3( (float)tri_PointList[0].x, (float)tri_PointList[0].y, (float)tri_PointList[0].z ) );
        vert1.SetNormal( vector3( (float)normalArray[0].x, (float)normalArray[0].y, (float)normalArray[0].z ) );
        vPolyIterator.getColor(curColor, 0);
        vert1.SetColor( vector4(curColor.r, curColor.g, curColor.b, curColor.a));
        fnMesh.getPolygonUV( vPolyIterator.index(), 0, u, v );
        vert1.SetUv( 0, vector2( u, -v) ); // flip v !!! Strange and dodgy !!!
        vert1.coord *= fSize;

        nMeshBuilder::Vertex& vert2 = m_MeshBuilder.GetVertexAt( m_iVertProcessed-1 );
        vert2.SetCoord( vector3( (float)tri_PointList[1].x, (float)tri_PointList[1].y, (float)tri_PointList[1].z ) );
        vert2.SetNormal( vector3( (float)normalArray[1].x, (float)normalArray[1].y, (float)normalArray[1].z ) );
        vPolyIterator.getColor(curColor, 1);
        vert2.SetColor( vector4(curColor.r, curColor.g, curColor.b, curColor.a));
        fnMesh.getPolygonUV( vPolyIterator.index(), 1, u, v );
        vert2.SetUv( 0, vector2( u, -v) ); // flip v !!! Strange and dodgy !!!
        vert2.coord *= fSize;

        nMeshBuilder::Vertex& vert3 = m_MeshBuilder.GetVertexAt( m_iVertProcessed );
        vert3.SetCoord( vector3( (float)tri_PointList[2].x, (float)tri_PointList[2].y, (float)tri_PointList[2].z ) );
        vert3.SetNormal( vector3( (float)normalArray[2].x, (float)normalArray[2].y, (float)normalArray[2].z ) );
        vPolyIterator.getColor(curColor, 2);
        vert3.SetColor( vector4(curColor.r, curColor.g, curColor.b, curColor.a));
        fnMesh.getPolygonUV( vPolyIterator.index(), 2, u, v );
        vert3.SetUv( 0, vector2( u, -v) ); // flip v !!! Strange and dodgy !!!  
        vert3.coord *= fSize;
    }
    
    return vResult; 
};

MStatus nMaya::ExportSimpleMesh( )
{
    MStatus vResult = MS::kSuccess;

    //create an iterator for only the mesh components of the DAG
    MItDag itDag(MItDag::kBreadthFirst, MFn::kInvalid, &vResult);
    CHECK_MSTATUS_AND_RETURN_IT(vResult);

    for(itDag.reset();!itDag.isDone();itDag.next()) 
    {
        //get the current DAG path
        MDagPath dagPath;
        vResult = itDag.getPath(dagPath);
        STATUS(vResult, "getPath" );

        MFnDagNode dagNode( dagPath, &vResult );
        STATUS(vResult, "DagNode Creation" );

        if (dagPath.hasFn(MFn::kMesh))
        {
            // we want the real thing !
            if ( dagNode.isIntermediateObject()) continue;
            // we only want meshdata linked to transforms
            // this takes instanced shapenodes in maya into account and converts
            // them into independent shapenodes in Nebula
            if ( !dagPath.hasFn( MFn::kTransform ) ) continue;

            MFnDagNode visTester(dagPath);

            //if this node is visible, then process the poly mesh it represents
            if(isVisible(visTester, vResult))
            {
                STATUS( vResult, "isVisible" );
            
                vResult = InitVerticesAndFaces(  dagPath );
                CHECK_MSTATUS_AND_RETURN_IT( vResult );
            }
        }
    }
    return WriteMeshFile();
}

bool nMaya::isVisible(MFnDagNode& fnDag, MStatus& status) 
//Summary:  determines if the given DAG node is currently visible
//Args   :  fnDag - the DAG node to check
//Returns:  true if the node is visible;        
//          false otherwise
{
    if(fnDag.isIntermediateObject())
        return false;

    MPlug visPlug = fnDag.findPlug("visibility", &status);
    if( MFAIL( status ) )
    {
        MGlobal::displayError("MPlug::findPlug");
        return false;
    } 
    else 
    {
        bool visible;
        status = visPlug.getValue(visible);
        if( MFAIL( status ) )
        {
            MGlobal::displayError("MPlug::getValue");
        }
        return visible;
    }
}

void nMaya::WriteCreationScript()
{
    n_assert( 0 < m_MeshBuilder.GetNumVertices() );
    const char* fileName = options.scriptFilename().Get();
    if( options.verboseP() ) cerr << "Script filename: '" << fileName << "'\n";
    nSceneNode* Root = (nSceneNode*)kernelServer.Lookup(options._szScriptFilename.ExtractFileName().Get());
    Root->SaveAs(options.scriptFilename().Get());
}

void nMaya::WriteSkinAnimator_TCL(  std::ofstream& fpScript ) {
    MAnimControl vAnimCtrl;
    nAnimBuilder::Group::LoopType loopType = CalcPlaybackMode( vAnimCtrl );
    nString strLoopType = ( nAnimBuilder::Group::LoopType::CLAMP == loopType ) ? "oneshot" : "loop";

    fpScript << "new nskinanimator skinanimator\n"
             << "sel skinanimator\n"
             << "    .setchannel \"time\"\n"
             << "    .setlooptype \"" << strLoopType.Get() << "\"\n"
             << "      # Override !!!" << endl
             << "    .setanim \"objects:enterpath.nanim2\"\n"; //<< options.animKeysFilename().asChar() << "' )\n"; // TODO parameterize
    WriteJointTree_TCL( fpScript );
    fpScript << "    .setstatechannel \"charState\"\n" // TODO parameterize
             << "    .beginstates 1 \n"
             << "        .setstate 0 0 0.3 \n"
             << "        .beginclips 0 1 \n"
             << "            .setclip 0 0 \"one\" \n"
             << "        .endclips 0 \n"
             << "    .endstates \n";
    fpScript << "sel ..\n";
}

void nMaya::WriteSkinShapeNode_TCL( std::ofstream& fpScript )
{
    nMeshBuilder newMeshBuilder;
    nSkinPartitioner skinPartitioner;
    skinPartitioner.PartitionMesh( m_MeshBuilder, newMeshBuilder, 72 ); // 72 is a magic number built into Nebula, but not one I can access from here
    
    int numParts = skinPartitioner.GetNumPartitions();
    cerr << "Skinpartitions :" << skinPartitioner.GetNumPartitions() << endl;


    for ( int j = 0; j < numParts; j++ ) {

        fpScript << "new nskinshapenode " << m_szGroupName[j].asChar() << "\n"// TODO parameterize
                 << "sel " << m_szGroupName[j].asChar() << "\n"// TODO parameterize
                 << "      # Override !!!" << endl
                 << "    .settexture \"DiffMap0\" \"textures:system/white.dds\"\n" // TODO parameterize
                 << "    .settexture \"BumpMap0\" \"textures:system/nobump.tga\"\n"
                 << "    .setvector \"MatAmbient\" 0.300000 0.300000 0.300000 1.000000 \n"
                 << "    .setvector \"MatDiffuse\" 1.000000 1.000000 1.000000 1.000000 \n"
                 << "    .setvector \"MatSpecular\" 0.200000 0.200000 0.200000 1.000000 \n"
                 << "    .setfloat \"MatSpecularPower\" 32.000000\n"
                 << "    .setshader \"colr\" \"shaders:default.fx\"\n"
                 << "      # Override !!!" << endl
                 << "    .setmesh \"meshes:examples/nano.n3d2\"\n" //<< options.meshFilename().asChar() <<  "' )\n"// TODO parameterize
     
                 << "    .setgroupindex "<< j <<"\n"
             
                 //<< "    .setmeshusage 65 \n"
                 << "    .beginfragments 1 \n"
                 << "        .setfraggroupindex 0 0 \n" 
                 << "        .beginjointpalette 0 " << m_vJointPaths.length() << endl;
        for( unsigned int i = 0; i < m_vJointPaths.length(); )
        {
            fpScript << "            .setjointindices 0 " << i;
            for( int j = 0; j < 8; ++j, ++i )
            {
                if( i < m_vJointPaths.length() )
                    fpScript << " " << i ;
                else
                    fpScript << " 0";
            }
            fpScript << "\n";
        }
        fpScript   << "        .endjointpalette 0\n"
                   << "    .endfragments\n"
                   << "    # Override !!!" << endl
                   << "    .setskinanimator \"../skinanimator\"\n"
                   << "sel .." << endl;
    }
}

MStatus nMaya::WriteJointTree_TCL( std::ofstream& fpScript )
{
  MStatus vResult = MS::kSuccess;

  uint nJoints = m_vJointPaths.length();
  n_assert( nJoints > 0 );
  
  fpScript << "    .beginjoints " << nJoints  << endl;
  for( uint idxChild = 0; idxChild < nJoints; ++idxChild )
  {
      WriteJoint_TCL( idxChild, fpScript );
  }
  fpScript << "    .endjoints" << endl;

  return vResult;
}

MStatus nMaya::WriteJoint_TCL( int idxJoint, std::ostream& fpScript )
{
    MStatus vResult = MS::kSuccess;

    MString sOurPath = m_vJointPaths[idxJoint].fullPathName( &vResult ); n_assert( MS::kSuccess == vResult );
    MFnIkJoint vJointFuncSet( m_vJointPaths[idxJoint], &vResult );  n_assert( MS::kSuccess == vResult );

    fpScript << "        .setjoint " << idxJoint;

    int idxBarChar;
    const char* szpLastBar = FindLastLine( sOurPath.asChar(), idxBarChar );
    assert( szpLastBar );
    char szParentName[256];
    strncpy( szParentName, sOurPath.asChar(), idxBarChar );
    *(szParentName + idxBarChar) = '\0';

    int idxParent = -1;
    MString sParent;
    for( uint idxChild = 0; idxChild < m_vJointPaths.length(); ++idxChild )
    {
        MString sCurrentPath = m_vJointPaths[idxChild].fullPathName( &vResult ); CHECK_MSTATUS_AND_RETURN_IT( vResult );
        if( 0 == strcmp( szParentName, sCurrentPath.asChar() ) )
        {
            idxParent = idxChild;
            break;
        }
    }
    fpScript << "  " << idxParent;

    MVector vTrans = vJointFuncSet.translation( MSpace::kTransform, &vResult );  CHECK_MSTATUS_AND_RETURN_IT( vResult );
    fpScript << "  " << vTrans.x << " " << vTrans.y << " " << vTrans.z;
    MTransformationMatrix transform = vJointFuncSet.transformation( &vResult );
    MMatrix mat = transform.asMatrix();
    //fprintf( stderr, "%s pos: %g %g %g\n", szpLastBar, 
    //          mat(3, 0), mat(3, 1), mat(3, 2) );
    quaternion q;
    matrix33 rotmat( (float)mat(0, 0), (float)mat(0, 1), (float)mat(0, 2),
                     (float)mat(1, 0), (float)mat(1, 1), (float)mat(1, 2),
                     (float)mat(2, 0), (float)mat(2, 1), (float)mat(2, 2) );
    q = rotmat.get_quaternion();
    fpScript << "  " << q.x << " " << q.y << " " << q.z << " " << q.w;
    double3 scale;
    vJointFuncSet.getScale( scale ); CHECK_MSTATUS_AND_RETURN_IT( vResult );
    fpScript << "  " << scale[0] << " " << scale[1] << " " << scale[2];
    fpScript << endl;

    return vResult; 
}

void nMaya::WriteSkinAnimator( std::ofstream& fpScript )
{
    MAnimControl vAnimCtrl;
    nAnimBuilder::Group::LoopType loopType = CalcPlaybackMode( vAnimCtrl );
    nString strLoopType = ( nAnimBuilder::Group::LoopType::CLAMP == loopType ) ? "oneshot" : "loop";

    fpScript << "new( 'nskinanimator', 'skinanimator' )\n"
             << "sel( 'skinanimator' )\n"
             << "    call( 'setchannel', 'time' )\n"
             << "    call( 'setlooptype', '" << strLoopType.Get() << "' )\n"
             << "    call( 'setanim', 'objects:'..bobName..'/'..bobName..'.nanim2' )\n"; //<< options.animKeysFilename().asChar() << "' )\n"; // TODO parameterize
    WriteJointTree( fpScript );
    fpScript << "    call( 'setstatechannel', 'charState' )\n" // TODO parameterize
             << "    call( 'beginstates', 1 )\n"
             << "        call( 'setstate', 0, 0, 0.3 )\n"
             << "        call( 'beginclips', 0, 1 )\n"
             << "            call( 'setclip', 0, 0, 'one' )\n"
             << "        call( 'endclips', 0 )\n"
             << "    call( 'endstates' )\n";
    fpScript << "sel('..')\n";
}    

void nMaya::WriteSkinShapeNode( std::ofstream& fpScript )
{
    nMeshBuilder newMeshBuilder;
    nSkinPartitioner skinPartitioner;
    skinPartitioner.PartitionMesh( m_MeshBuilder, newMeshBuilder, 72 ); // 72 is a magic number built into Nebula, but not one I can access from here
    cerr << "Skinpartitions :" << skinPartitioner.GetNumPartitions() << endl;

    fpScript << "new('nskinshapenode', bobName )\n"// TODO parameterize
             << "sel( bobName )\n"// TODO parameterize
             << "    call('settexture', 'DiffMap0', 'objects:'..bobName..'/'..bobName..'.dds' )\n" // TODO parameterize
             << "    call('settexture', 'BumpMap0', 'textures:system/nobump.tga')\n"
             << "    call('setvector', 'MatAmbient', 0.300000, 0.300000, 0.300000, 1.000000 )\n"
             << "    call('setvector', 'MatDiffuse', 1.000000 ,1.000000 ,1.000000 ,1.000000 )\n"
             << "    call('setvector', 'MatSpecular', 0.200000, 0.200000, 0.200000 ,1.000000 )\n"
             << "    call('setfloat', 'MatSpecularPower', 32.000000 )\n"
             << "    call('setshader', 'colr', 'shaders:default_skinned.fx' )\n"
             << "    call('setmesh', 'objects:'..bobName..'/'..bobName..'.n3d2' )\n" //<< options.meshFilename().asChar() <<  "' )\n"// TODO parameterize
             << "    call('setgroupindex', 0 )\n"
             << "    call('setmeshusage', 65 )\n"
             << "    call('beginfragments', 1 )\n"
             << "        call('setfraggroupindex', 0, 0 )\n" 
             << "        call('beginjointpalette', 0, " << m_vJointPaths.length() << " )\n";
    for( unsigned int i = 0; i < m_vJointPaths.length(); )
    {
        fpScript << "            call('setjointindices', 0, " << i;
        for( int j = 0; j < 8; ++j, ++i )
        {
            if( i < m_vJointPaths.length() )
                fpScript << ", " << i ;
            else
                fpScript << ", 0";
        }
        fpScript << ")\n";
    }
    fpScript   << "        call('endjointpalette', 0 )\n"
               << "    call('endfragments')\n"
               << "    call('setskinanimator', modelsPath..'/'..'skinanimator' )\n";
}

MStatus nMaya::WriteJointTree( std::ofstream& fpScript )
{
  MStatus vResult = MS::kSuccess;

  uint nJoints = m_vJointPaths.length();
  n_assert( nJoints > 0 );
  
  fpScript << "    call( 'beginjoints', " << nJoints << " )" << endl;
  for( uint idxChild = 0; idxChild < nJoints; ++idxChild )
  {
      WriteJoint( idxChild, fpScript );
  }
  fpScript << "    call( 'endjoints' )" << endl;

  return vResult;
}


MStatus nMaya::WriteJoint( int idxJoint, std::ostream& fpScript )
{
    MStatus vResult = MS::kSuccess;

    MString sOurPath = m_vJointPaths[idxJoint].fullPathName( &vResult ); n_assert( MS::kSuccess == vResult );
    MFnIkJoint vJointFuncSet( m_vJointPaths[idxJoint], &vResult );  n_assert( MS::kSuccess == vResult );

    fpScript << "        call( 'setjoint', " << idxJoint;

    int idxBarChar;
    const char* szpLastBar = FindLastLine( sOurPath.asChar(), idxBarChar );
    assert( szpLastBar );
    char szParentName[256];
    strncpy( szParentName, sOurPath.asChar(), idxBarChar );
    *(szParentName + idxBarChar) = '\0';

    int idxParent = -1;
    MString sParent;
    for( uint idxChild = 0; idxChild < m_vJointPaths.length(); ++idxChild )
    {
        MString sCurrentPath = m_vJointPaths[idxChild].fullPathName( &vResult ); CHECK_MSTATUS_AND_RETURN_IT( vResult );
        if( 0 == strcmp( szParentName, sCurrentPath.asChar() ) )
        {
            idxParent = idxChild;
            break;
        }
    }
    fpScript << ",  " << idxParent;

    MVector vTrans = vJointFuncSet.translation( MSpace::kTransform, &vResult );  CHECK_MSTATUS_AND_RETURN_IT( vResult );
    fpScript << ",  " << vTrans.x << ", " << vTrans.y << ", " << vTrans.z;
    MTransformationMatrix transform = vJointFuncSet.transformation( &vResult );
    MMatrix mat = transform.asMatrix();
    //fprintf( stderr, "%s pos: %g %g %g\n", szpLastBar, 
    //          mat(3, 0), mat(3, 1), mat(3, 2) );
    quaternion q;
    matrix33 rotmat( (float)mat(0, 0), (float)mat(0, 1), (float)mat(0, 2),
                     (float)mat(1, 0), (float)mat(1, 1), (float)mat(1, 2),
                     (float)mat(2, 0), (float)mat(2, 1), (float)mat(2, 2) );
    q = rotmat.get_quaternion();
    fpScript << ",  " << q.x << ", " << q.y << ", " << q.z << ", " << q.w;
    double3 scale;
    vJointFuncSet.getScale( scale ); CHECK_MSTATUS_AND_RETURN_IT( vResult );
    fpScript << ",  " << scale[0] << ", " << scale[1] << ", " << scale[2];
    fpScript << " )" << endl;

    return vResult; 
}

//MStatus
//nMaya::WriteSimpleAnimKeys()
//{
//    MStatus vResult = MS::kSuccess;
//
//    nAnimBuilder animBuilder;
//    MItDependencyNodes vDepIterator(MFn::kAnimCurve, &vResult);
//    CHECK_MSTATUS_AND_RETURN_IT( vResult );
//
//    for (; !vDepIterator.isDone(); vDepIterator.next())
//    {
//        MObject vCurObject = vDepIterator.item(&vResult);
//        CHECK_MSTATUS_AND_RETURN_IT( vResult );
//        MFnAnimCurve fnCurve( vCurObject, &vResult );
//        CHECK_MSTATUS_AND_RETURN_IT( vResult );
//        //const char* name = fnCurve.typeName().asChar();
//        //MPlugArray myArray;
//        //fnCurve.getConnections( myArray );
//        //unsigned int len = myArray.length();
//        //MPlug plug = myArray[0];
//        //const char* plugName = plug.name().asChar();
//        const unsigned int numKeys = fnCurve.numKeys( &vResult );
//        CHECK_MSTATUS_AND_RETURN_IT( vResult );
//        if( numKeys > 1 )
//        {
//            nAnimBuilder::Group animGroup;
//            nAnimBuilder::Curve curve( numKeys );
//            animGroup.SetNumKeys( numKeys );
//            for( int i = 0; i < numKeys; ++i )
//            {
//                nAnimBuilder::Key key = curve.GetKeyAt( i );
//                double val = fnCurve.value( i, &vResult );
//                CHECK_MSTATUS_AND_RETURN_IT( vResult );
//                key.SetW( val );
//            }
//            animGroup.AddCurve( curve );
//            animBuilder.AddGroup( animGroup );
//        }
//    }
//
//    animBuilder.Optimize();
//    nKernelServer kernelServer;
//    nFileServer2* fileServer = static_cast<nFileServer2*>( kernelServer.New( "nfileserver2", "fileserver" ) );
//    if( !animBuilder.Save( fileServer, options.animKeysFilename().asChar() ) )
//    {
//        MString errStr( "WriteSimpleAnimKeys: error in animBuilder.Save: " );
//        errStr += options.animKeysFilename();
//        vResult.perror(errStr);
//    }
//    return vResult;
//}

MStatus nMaya::WriteRiggedAnimKeys()
{
    MStatus vResult = MS::kSuccess;
    MAnimControl vAnimCtrl;
    vAnimCtrl.setAutoKeyMode( true ); // No idea what this does
    const double fMaxTime = vAnimCtrl.maxTime().value();
    const double fMinTime = vAnimCtrl.minTime().value();
    n_assert( options.startFrame() <= fMaxTime );
    n_assert( options.endFrame() <= fMaxTime );

    const int startFrame = CalcStartFrame( int( fMinTime ) );
    const int endFrame = CalcEndFrame( int( fMaxTime ), int( fMaxTime - fMinTime + 1 ) );
    const int outLen = endFrame - startFrame + 1; // shouldn't this be divided by options.stride()?
    if( options.verboseP() ) cerr << "start=" << startFrame << "  end=" << endFrame << endl;

    nAnimBuilder::Group& animGroup = CreateAnimGroup( outLen, CalcPlaybackMode( vAnimCtrl ) );
    int numUncompressedCurves = 0; // in this group
    for( uint idxJoint = 0; idxJoint < m_vJointPaths.length(); ++idxJoint )
    {
        MFnIkJoint vJointFuncSet( m_vJointPaths[ idxJoint ], &vResult );  CHECK_MSTATUS_AND_RETURN_IT( vResult );
        vResult = WriteCurves( animGroup, numUncompressedCurves, vAnimCtrl, vJointFuncSet, startFrame, endFrame );  CHECK_MSTATUS_AND_RETURN_IT( vResult );
    }
    n_assert( animGroup.Validate() );
    animGroup.SetKeyStride( numUncompressedCurves );

    if( options.verboseP() ) cerr << endl;

    SaveAnim( animGroup );

    return vResult;
}

nAnimBuilder::Group nMaya::CreateAnimGroup( int numFrames, nAnimBuilder::Group::LoopType loopType )
{ // when we build the curves, we renormalize them to go from 0 to numFrames
    nAnimBuilder::Group animGroup;
    animGroup.SetStartKey( 0 );
    animGroup.SetNumKeys( numFrames ); // outLen
    animGroup.SetLoopType( loopType );
    animGroup.SetKeyTime( 1.0f ); // there's probably some way of reading that out of maya
    return animGroup;
}

int nMaya::CalcStartFrame( int minFrame ) 
{
    int startFrame = options.startFrame();   
    if( 0 == startFrame )
    {
        startFrame = minFrame;
    }
    else if( 0 > startFrame )
    {
        startFrame += options.endFrame();
    }
    return startFrame;
}

int nMaya::CalcEndFrame( int maxFrame, int totalLen )
{
    int endFrame = options.endFrame();
    if( 0 == endFrame )
    {
        endFrame = maxFrame;
    }
    else if( 0 > options.endFrame() )
    {
        endFrame += totalLen;
    }
    return endFrame;
}

nAnimBuilder::Group::LoopType nMaya::CalcPlaybackMode( const MAnimControl& vAnimCtrl )
{
    nAnimBuilder::Group::LoopType loopType = nAnimBuilder::Group::LoopType::REPEAT;
    const MAnimControl::PlaybackMode playbackMode = vAnimCtrl.playbackMode();
    if( MAnimControl::PlaybackMode::kPlaybackOnce == playbackMode ) 
    {
        loopType = nAnimBuilder::Group::LoopType::CLAMP;
    }
    else if( MAnimControl::PlaybackMode::kPlaybackLoop != playbackMode )
    {
        n_assert( 0 && "Invalid Playback mode!" );
    }
    return loopType;
}

MStatus nMaya::WriteCurves( nAnimBuilder::Group& OUT_animGroup, int& INOUT_numUncompressedCurves, const MAnimControl& vAnimCtrl, const MFnTransform& vJointFuncSet, int startFrame, int endFrame )
{
    int numUncompressedCurves = 0;
    MStatus vResult = MS::kSuccess;
    nAnimBuilder::Key key;
    nAnimBuilder::Curve rotCurve, posCurve, scaleCurve;
    rotCurve.SetIpolType( nAnimBuilder::Curve::QUAT ); 
    posCurve.SetIpolType( nAnimBuilder::Curve::LINEAR );  
    scaleCurve.SetIpolType( nAnimBuilder::Curve::LINEAR );  
    for( int nKeyFrame = startFrame, i = 0; nKeyFrame <= endFrame; nKeyFrame += options.animStep(), ++i )
    {
        MTime vTime( nKeyFrame, MTime::uiUnit() );
        vResult = vAnimCtrl.setCurrentTime( vTime ); CHECK_MSTATUS_AND_RETURN_IT( vResult );

        key.Set( GetPosVector( vJointFuncSet ) );
        posCurve.SetKey( i, key );

        key.Set( GetRotVector( vJointFuncSet ) );
        rotCurve.SetKey( i, key );

        key.Set( GetScaleVector( vJointFuncSet ) );
        scaleCurve.SetKey( i, key );
    }
    if( !rotCurve.Optimize() )
    {
        rotCurve.SetFirstKeyIndex( INOUT_numUncompressedCurves++ );
    }
    if( !posCurve.Optimize() )
    {
        posCurve.SetFirstKeyIndex( INOUT_numUncompressedCurves++ );
    }
    if( !scaleCurve.Optimize() )
    {
        scaleCurve.SetFirstKeyIndex( INOUT_numUncompressedCurves++ );
    }
    OUT_animGroup.AddCurve( posCurve );
    OUT_animGroup.AddCurve( rotCurve );
    OUT_animGroup.AddCurve( scaleCurve );
    return vResult;
}

vector4 nMaya::GetRotVector( const MFnTransform& fnTransform )
{
    MStatus vResult;
    MMatrix jointMatrix = fnTransform.transformation( &vResult ).asMatrix(); CHECK_MSTATUS( vResult );
    matrix33 rotmat( (float)jointMatrix(0, 0), (float)jointMatrix(0, 1), (float)jointMatrix(0, 2),
                (float)jointMatrix(1, 0), (float)jointMatrix(1, 1), (float)jointMatrix(1, 2),
                (float)jointMatrix(2, 0), (float)jointMatrix(2, 1), (float)jointMatrix(2, 2) );
    quaternion q = rotmat.get_quaternion();
    return vector4( q.x, q.y, q.z, q.w );
}

vector4 nMaya::GetPosVector(  const MFnTransform& fnTransform ) 
{
    MStatus vResult;
    MVector vTrans = fnTransform.translation( MSpace::kTransform, &vResult );  CHECK_MSTATUS( vResult );
    return vector4( float(vTrans.x), float(vTrans.y), float(vTrans.z), 1.0f );
}

vector4 nMaya::GetScaleVector( const MFnTransform& fnTransform ) 
{
    double3 scale;
    const MStatus& vResult = fnTransform.getScale( scale );  CHECK_MSTATUS( vResult );
    return vector4( float(scale[0]), float(scale[1]), float(scale[2]), 1.0f );
}

MStatus nMaya::SaveAnim( nAnimBuilder::Group& animGroup )
{
    MStatus vResult = MS::kSuccess;
    nAnimBuilder animBuilder;
    animBuilder.AddGroup( animGroup );
    animBuilder.Optimize();
    nKernelServer kernelServer;
    nFileServer2* fileServer = static_cast<nFileServer2*>( kernelServer.New( "nfileserver2", "fileserver" ) );
    if( !animBuilder.Save( fileServer, options.animKeysFilename().Get() ) )
    {
        MString errStr( "WriteSimpleAnimKeys: error in animBuilder.Save: " );
        errStr += options.animKeysFilename().Get();
        vResult.perror(errStr);
    }
    return vResult;
}
