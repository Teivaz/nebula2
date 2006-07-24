//-----------------------------------------------------------------------------
/* Copyright (c) 2002 Ling Lo, adapted to N2 by Rafael Van Daele-Hunt (c) 2004
 *
 * See the file "nmap_license.txt" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */
//-----------------------------------------------------------------------------

#include "scene/nsceneserver.h"
#include "map/mapquadtree.h"
#include "map/mapblock.h"
#include "map/nmapnode.h"
#include "map/nmap.h"


bool
nMapNode::HasGeometry() const
{
    return true;
}


/**
    Compute
*/
bool
nMapNode::RenderGeometry(nSceneServer * scene_graph, nRenderContext * renderContext)
{
    int i;
    n_assert(scene_graph);

    // Process map if dirty
    if (true == isDirty)
    {
        ProcessMap();
    }

    nGfxServer2* gfx_server = refGfxServer;

    // Set visible
    for (int j = 0; j < numBlocks; ++j)
        for (i = 0; i < numBlocks; ++i)
            blockArray[j][i]->SetVisible(false);

    // Frustum culling
    MapQuadElement** collect_array = NULL;
    int num_blocks = mapQuadtree->CollectByFrustum(gfx_server, collect_array);

    // Calculate mipmaps
    matrix44 view;
    view = gfx_server->GetTransform(nGfxServer2::View);
    vector3 camera_pos = view.pos_component();

    for (i = 0; i < num_blocks; ++i)
    {
        MapBlock* block = (MapBlock*)collect_array[i]->GetPtr();
        block->SelectMipMapLevel(camera_pos);
    }

    // Level mipmapping with neighbours
    bool repeat = false;
    do
    {
        repeat = false;

        for (i = 0; i < num_blocks; ++i)
        {
            MapBlock* block = (MapBlock*)collect_array[i]->GetPtr();
            if (true == block->AlignMipMapLevel())
                repeat = true;
        }
    }
    while (true == repeat);

    // Render it all
    for (i = 0; i < num_blocks; ++i)
    {
        MapBlock* block = (MapBlock*)collect_array[i]->GetPtr();
        block->Render(scene_graph);
    }
    return true;
}

/**
    Visualises terrain occlusion details
*/
int
nMapNode::Visualize()
{
    nGfxServer2* gfx_server = refGfxServer;
    if (NULL != mapQuadtree)
    {
        mapQuadtree->Visualize(gfx_server);
        MapQuadElement** collect_array = NULL;
        return mapQuadtree->GetCollect(collect_array);
    }

    return 0;
}
