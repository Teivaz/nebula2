#ifndef N_MAXEXPORTTASK_H
#define N_MAXEXPORTTASK_H
//------------------------------------------------------------------------------
/**
    3ds max to Nebula2 exporter using the IGame lib.

    This file is licensed under the terms of the Nebula License.
    (C) 2004 Johannes Kellner
*/

#include "util/npathstring.h"

class nMaxExportTask
{
public:
    nMaxExportTask();
    ~nMaxExportTask();
    bool UseWeightedNormals() { return this->useWeightedNormals; }
    bool ExportStatic() { return this->exportStatic; }

    ///export nodes that are hidden?
    bool exportHiddenNodes;
    ///export tranformation or vector animations?
    bool exportAnimations;
    ///group the meshes by source object (increases the meshsize, but makes it possible to work with the original node, not only the materials used in the scene)
    bool groupMeshBySourceObject;

    ///the sample rate used for the transform and vector animation samplings (only used when no keyframe export is possible)
    int sampleRate;

    ///use the IGame option to create wighted normals? (requires that every face has min 1 smoth group assigned)
    bool useWeightedNormals;
    
    ///baseName of the meshfile
    nPathString meshFileName;

    ///extension of the mesh file (.n3d2|.nvx2)
    nString meshFileExtension;

    ///basename of the scenefile
    nPathString sceneFileName;

    ///export the mesh data as static (don't export skinning vertex data)
    bool exportStatic;

    ///the maxium joint palette size (see nSkinPartioner for more)
    int maxJointPaletteSize;
    
    ///when this is not 0, than only these vertexcomponents will be exported.
    int forcedVextexComponents;
};

#endif