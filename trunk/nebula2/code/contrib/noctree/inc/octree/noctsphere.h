#ifndef N_OCTSPHERE_H
#define N_OCTSPHERE_H

#include "mathlib/sphere.h"
#include "octree/noctvisitor.h"

//-----------------------------------------------------------------------------
/**
    @class nOctSphere
    @ingroup NOctreeContribModule
    @brief culling with sphere
*/
class nOctSphere : public nOctVisitor
{
public:
    virtual void DoCulling(nOctree* oct);
    void SetSphere(const sphere& sphere);
    const sphere& GetSphere();
private:
    void recurse_collect_by_sphere(nOctNode* on,
                                   const sphere& clip,
                                   bool full_intersect);
    void collect_nodes_in_sphere(nOctNode* on,
                                 const sphere& clip,
                                 bool full_intersect);

private:
    sphere    clip;

};

//-----------------------------------------------------------------------------
/**
*/
inline
void nOctSphere::SetSphere(const sphere& sphere)
{
    this->clip = sphere;
}

//-----------------------------------------------------------------------------
/**
*/
inline
const sphere& nOctSphere::GetSphere()
{
    return this->clip;
}

#endif /*N_OCTVISITOR_SPHERE_H*/
