#ifndef C_CCOLLISION_H
#define C_CCOLLISION_H
//------------------------------------------------------------------------------
/**
    @class BBCollision
    @ingroup BombsquadBruceContribModule
    @brief Brutally simple collision detection (static cylinders)

    (C)	2004 Rafael Van Daele-Hunt
*/

#include "util/narray.h"
#include "BombsquadBruce/bbref.h"
#include "mathlib/vector.h"
#include <vector>
class BBRoot;

struct BBCylinder
{
    BBCylinder( const vector3& pos, float diameter, float height ) :
        m_Pos( pos ),
        m_Diameter( diameter ),
        m_Height( height )
        {}
    bool intersects( const BBCylinder& rhs ) const
    {
        return     rhs.m_Pos.y < m_Pos.y + m_Height && rhs.m_Pos.y + rhs.m_Height > m_Pos.y
                && vector2( m_Pos.x - rhs.m_Pos.x, m_Pos.z - rhs.m_Pos.z ).len() < rhs.m_Diameter + m_Diameter;
    }
    //bool contains( const vector2& pos )
    //{
    //    return ( m_Pos - pos ).len() < m_Diameter;
    //}

    vector3 m_Pos;
    float m_Diameter;
    float m_Height;
};

class BBCollision
{
public:
    typedef BBCylinder CollVolT;
    virtual ~BBCollision();
    const CollVolT* GetCollision( const BBRoot& ) const;
    //const CollVolT* GetCollision(  const vector2& ) const;
    bool AddStaticObject( /*const*/ BBRoot& ); // returns false if it collides with an object already added, in which case it also isn't added
    bool AddDynamicObject( /*const*/ BBRoot& ); // as above.
    void CullShortObjects( float minHeight ); // removes all collVols shorter than minHeight
    void ClearDynamicObjects(); // removes all dynamic objects
private:
    const CollVolT* DoGetCollision( const BBRoot&, const std::vector<BBRef<CollVolT> >& ) const;

    std::vector< BBRef<CollVolT> > m_BoundingVolumes;
    std::vector< BBRef<CollVolT> > m_DynamicVolumes;
};

//---------------------------------------------------------------------------

#endif
