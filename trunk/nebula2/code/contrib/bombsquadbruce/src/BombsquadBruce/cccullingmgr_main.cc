//------------------------------------------------------------------------------
//  (c) 2004 Rafael Van Daele-Hunt
//------------------------------------------------------------------------------
#include "BombsquadBruce/cccullingmgr.h"
#include "scene/nshapenode.h"
#include "spatialdb/nspatialsector.h"
#include "spatialdb/nspatialelements.h"
#include "BombsquadBruce/cccamera.h"
#include "BombsquadBruce/ccroot.h"
nNebulaScriptClass(CCCullingMgr, "nroot");


//------------------------------------------------------------------------------
// Helper function
//------------------------------------------------------------------------------

void CCCullingMgr::DestroySpatialDB(nSpatialSector *cleanme)
{
    int i;
    for (i=0; i < m_currentelements.Size(); ++i)
    {
        m_rRootSector->RemoveElement(m_currentelementhandles[i]);
        n_delete(m_currentelements[i]);
    }
    m_currentelementhandles.Clear();
    m_currentelements.Clear();
}

///////////////////////////////////////////
// Basic Methods
///////////////////////////////////////////

CCCullingMgr::CCCullingMgr() :
    m_ObjectsToRender(100,100),
    m_pCuller(0)
{
}

CCCullingMgr::~CCCullingMgr()
{
    n_delete(m_pCuller);
    if (m_rRootSector.isvalid())
        DestroySpatialDB(m_rRootSector.get());
}

///////////////////////////////////////////
// Nonscript public Methods
///////////////////////////////////////////

void CCCullingMgr::StartElements()
{
    if( m_rRootSector.isvalid() )
    {  // destroy and recreate to reinitialize the octree memory pool
        DestroySpatialDB(m_rRootSector.get());
        m_rRootSector->Release();
    }
    n_assert(m_currentelements.Size() == 0);
    n_assert(m_currentelementhandles.Size() == 0);
    nString rootSectorPath = this->GetFullName() + "/rootsector";
    m_rRootSector = (nSpatialSector *)kernelServer->New("nspatialsector", rootSectorPath.Get());
    m_rRootSector->Configure(matrix44());
 }

bool CCCullingMgr::AddElement(CCRoot& root)
{
    n_assert( vector3( 1.0f, 1.0f, 1.0f ).isequal( root.GetShapeNode()->GetScale(), 0.0001f ) ); // otherwise scale the bounding box as well
    bbox3 worldSpaceBBox = root.GetShapeNode()->GetLocalBox();
    worldSpaceBBox.vmin += root.GetPosition();
    worldSpaceBBox.vmax += root.GetPosition();

    nSpatialElement* pElement = n_new(nBBoxSpatialElement(worldSpaceBBox));
    pElement->SetPtr( &root );
    nSpatialSector::ElementHandle elementhandle = m_rRootSector->AddElement( pElement );    
    m_currentelements.PushBack(pElement);
    m_currentelementhandles.PushBack(elementhandle);
//    m_rRootSector->UpdateElement( pElement, root.GetPosition(), worldSpaceBBox );
    return true;
}

void CCCullingMgr::EndElements()
{
    //m_rRootSector->BalanceTree();
}

const nVisibilityVisitor::VisibleElements& CCCullingMgr::GetVisibleElements(const CCCamera& cullCamera)
{
    n_delete( m_pCuller );
    m_ObjectsToRender.Clear();
    m_pCuller = cullCamera.GetVisibilityVisitor(m_ObjectsToRender);
//    m_pCuller->Visit(m_rRootSector.get(),3);
    m_rRootSector->Accept(*m_pCuller, 3, VisitorFlags());
    return m_ObjectsToRender;
}

/**
    Show the culling frustum based on the last GetVisibleGameObjs call
    and the bounding boxes of unculled objects (for debugging purposes).
*/
void CCCullingMgr::Visualize()
{ 
    nGfxServer2::Instance()->SetTransform(nGfxServer2::Model, matrix44());

    m_pCuller->StartVisualizeDebug(nGfxServer2::Instance());
    m_rRootSector->Accept(*m_pCuller, 3, VisitorFlags());
    m_pCuller->EndVisualizeDebug();
    //    m_pCuller->Visit(m_rRootSector.get(),3);

    nGfxServer2::Instance()->BeginShapes();
    int i; // VC6 compatibility
    for( i = m_ObjectsToRender.Size(); i > 0; --i )
    {
        bbox3 nodebbox ( m_ObjectsToRender[i-1]->GetAABB() );
        matrix44 m;
        m.scale( nodebbox.extents() * 2.0f );
        m.set_translation( nodebbox.center() );
        nGfxServer2::Instance()->DrawShape( nGfxServer2::Box,m, vector4( 0, 0, 1.0f, 0.5f ) );
    }
    nGfxServer2::Instance()->EndShapes();
    //m_rRootSector->SetVisualize(true);
    //m_rRootSector->Visualize();
}


