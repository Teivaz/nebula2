//-----------------------------------------------------------------------------
//  nrmmeshbuilder.h
//
//  Mesh builder for geometry loader plug-in.
//
//  (C) 2005 Kim, Hyoun Woo.
//-----------------------------------------------------------------------------
#ifndef N_RMMESHBUILDER_H
#define N_RMMESHBUILDER_H

#include "rmgeomloader/nrmgeomloader_pch.h"

#include "kernel/nkernelserver.h"
#include "kernel/nfileserver2.h"
#include "tools/nmeshbuilder.h"

bool ConstructScene(const RM_TCHAR *strFileName, RmMeshModelContainer *pModelContainer);
bool BuildMesh(int groupIdx, nArray<nMeshBuilder::Group> &group, RmMeshModel* mesh, 
               RmMeshModelContainer *pModelContainer, const nMeshBuilder& meshbuilder);

#endif
