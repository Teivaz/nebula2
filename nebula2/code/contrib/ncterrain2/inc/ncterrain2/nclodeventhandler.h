#ifndef N_CLODEVENTHANDLER_H
#define N_CLODEVENTHANDLER_H
//------------------------------------------------------------------------------
/**
    @class nCLODEventHandler
    @ingroup NCTerrain2

    @brief Subclass and attach to nChunkLodTree to get notified about various
    events, like loading, unloading and rendering chunk lod nodes.
    
    (C) 2003 RadonLabs GmbH
*/
#include "kernel/ntypes.h"
#include "kernel/nrefcounted.h"

class nChunkLodNode;

//------------------------------------------------------------------------------
class nCLODEventHandler : public nRefCounted
{
public:
    /// events
    enum Event
    {
        CreateNode,             // a chunk lod node has been created
        DestroyNode,            // a chunk lod node has been destroyed
        RequestLoadData,        // data for chunk lod node has been requesyed
        DataAvailable,          // data for chunk lod node is available
        UnloadData,             // data for chunk lod node unloading
        RenderNode,             // rendering chunk lod node
    };

    /// constructor
    nCLODEventHandler();
    /// destructor
    virtual ~nCLODEventHandler();
    /// called when event occured
    virtual void OnEvent(Event event, nChunkLodNode* node);

protected:
};
//------------------------------------------------------------------------------
#endif

