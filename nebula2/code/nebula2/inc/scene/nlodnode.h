#ifndef N_LODNODE_H
#define N_LODNODE_H
//------------------------------------------------------------------------------
/**
    @class nLodNode
    @ingroup SceneNodes
    
    @brief A lod node switches its child nodes on and off according to its
    current camera distance to accomplish different representations with
    different levels of detail.

    (C) 2002 RadonLabs GmbH
*/
#include "scene/ntransformnode.h"

//------------------------------------------------------------------------------
class nLodNode : public nTransformNode
{
public:
    /// constructor
    nLodNode();
    /// destructor
    virtual ~nLodNode();
    /// object persistency
    virtual bool SaveCmds(nPersistServer *ps);
    /// called by nSceneServer when object is attached to scene
    virtual void Attach(nSceneServer* sceneServer, nRenderContext* renderContext);
    /// append camera distance threshold when the child node at index is switched on and its predecessor switched off
    void AppendThreshold(float distance);
    /// get camera distance threshold when the child node at index is switched on and its predecessor switched off
    float GetThreshold(int index) const;
    /// set the minimum camera distance where the node is shown
    void SetMinDistance(float distance);
    /// get the minimum camera distance where the node is shown
    float GetMinDistance() const;
    /// set the maximum camera distance where the node is shown
    void SetMaxDistance(float distance);
    /// get the maximum camera distance where the node is shown
    float GetMaxDistance() const;

protected:
    nArray< nRef<nSceneNode> > lods;
    nArray<float> thresholds;
    float maxDistance;
    float minDistance;
};


//------------------------------------------------------------------------------
/**
*/
inline
void
nLodNode::AppendThreshold(float distance)
{
    this->thresholds.Append(distance);
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nLodNode::GetThreshold(int index) const
{
    return this->thresholds[index];
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nLodNode::SetMaxDistance(float distance)
{
    this->maxDistance = distance;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nLodNode::GetMaxDistance() const
{
    return this->maxDistance;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nLodNode::SetMinDistance(float distance)
{
    this->minDistance = distance;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nLodNode::GetMinDistance() const
{
    return this->minDistance;
}

//------------------------------------------------------------------------------
#endif
