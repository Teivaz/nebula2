//------------------------------------------------------------------------------
//  (c) 2004 Rafael Van Daele-Hunt
//------------------------------------------------------------------------------
#include "BombsquadBruce/cccollision.h"
#include "BombsquadBruce/ccroot.h"
#include "scene/nshapenode.h"
#include <algorithm>
#include "mathlib/bbox.h"

////////////////////////////////////
// Helper functions
////////////////////////////////////

CCCylinder GenCollVol( const CCRoot& gameObj ) // gets gameObj's bounding box in worldspace
{  
    bbox3 box( gameObj.GetShapeNode()->GetLocalBox() );
    const vector3& pos = gameObj.GetPosition();
    float collRad = gameObj.GetCollisionRadius();
    return CCCylinder( pos, collRad, box.vmax.y - box.vmin.y );
}

struct CollidesWith
{
    CollidesWith( const CCCollision::CollVolT& collVol ) : m_CollVol( collVol )  { }
    bool operator() ( const CCRef<CCCollision::CollVolT>& collVol ) const
    {
        return collVol->intersects( m_CollVol );    
    }
    const CCCollision::CollVolT& m_CollVol;
};

//struct CollidesWithPos
//{
//    CollidesWithPos( const vector2& pos ) : m_Pos( pos )  { }
//    bool operator() ( const CCRef<CCCollision::CollVolT>& collVol ) const
//    {
//        return collVol->contains( m_Pos );    
//    }
//    const vector2& m_Pos;
//};

////////////////////////////////////
// Public functions
////////////////////////////////////

CCCollision::~CCCollision()
{
    for( std::vector<CCRef<CollVolT> >::iterator i = m_BoundingVolumes.begin(); i != m_BoundingVolumes.end(); ++i )
    {
        n_delete( *i );
    }
    ClearDynamicObjects();
}

const CCCollision::CollVolT* CCCollision::GetCollision( const CCRoot& gameObj ) const
{ // not especially efficient, but my number of objects is small so far, so I don't care
    const CollVolT* retVal = DoGetCollision( gameObj, m_BoundingVolumes );
    if( !retVal )
    {
        retVal = DoGetCollision( gameObj, m_DynamicVolumes );
    }
    return retVal;
}

//const CCCollision::CollVolT* CCCollision::GetCollision( const vector2& pos ) const
//{ 
//    const CollVolT* retVal = DoGetCollision( const vector2& pos, m_BoundingVolumes );
//    if( !retVal )
//    {
//        retVal = DoGetCollision( const vector2& pos, m_DynamicVolumes );
//    }
//    return retVal;
//}

const CCCollision::CollVolT* CCCollision::DoGetCollision( const CCRoot& gameObj, const std::vector<CCRef<CollVolT> >& vec ) const
{
    const CollVolT* retVal = 0;
    std::vector<CCRef<CollVolT> >::const_iterator firstColliderIt = std::find_if( vec.begin(), vec.end(), CollidesWith( GenCollVol( gameObj ) ) );
    if( vec.end() != firstColliderIt )
    {
        retVal = (*firstColliderIt);
    }
    return retVal;
}

bool CCCollision::AddStaticObject( /*const*/ CCRoot& gameObj )
{
    bool retVal = ( 0 == GetCollision( gameObj ) );
    if( retVal ) 
    {
        CollVolT* collVol = n_new( CollVolT( GenCollVol( gameObj ) ) );
        if( collVol->m_Diameter > 0.0f )
        {
            m_BoundingVolumes.push_back( CCRef<CollVolT>( collVol ) );
        } // otherwise is can't collide, so don't pollute the list with it
    }
    return retVal;
}

bool CCCollision::AddDynamicObject( /*const*/ CCRoot& gameObj )
{
    bool retVal = ( 0 == GetCollision( gameObj ) );
    if( retVal ) 
    {
        CollVolT* collVol = n_new( CollVolT( GenCollVol( gameObj ) ) );
        if( collVol->m_Diameter > 0.0f )
        {
            m_DynamicVolumes.push_back( CCRef<CollVolT>( collVol ) );
        } // otherwise is can't collide, so don't pollute the list with it
    }
    return retVal;
}

struct TooShort
{
    TooShort( float minHeight ) : m_MinHeight( minHeight ) {}
    bool operator() ( const CCRef<CCCollision::CollVolT>& rCollVol )
    {
        return rCollVol->m_Height < m_MinHeight;
    }

    float m_MinHeight;
};

void CCCollision::CullShortObjects( float minHeight )
{
    m_BoundingVolumes.erase( std::remove_if( m_BoundingVolumes.begin(), m_BoundingVolumes.end(), TooShort( minHeight ) ), m_BoundingVolumes.end() );
    
}

void CCCollision::ClearDynamicObjects() 
{
    for( std::vector<CCRef<CollVolT> >::iterator i = m_DynamicVolumes.begin(); i != m_DynamicVolumes.end(); ++i )
    {
        n_delete( *i );
    }
    m_DynamicVolumes.clear();
}