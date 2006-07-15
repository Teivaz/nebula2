#ifndef N_LW_MODELER_EXPORTER_H
#define N_LW_MODELER_EXPORTER_H
//----------------------------------------------------------------------------
#include <lwmeshedt.h>
#include <lwsurf.h>
#include <lwhost.h>
#include <lwtxtr.h>
#include "util/nhashlist.h"
#include "tools/nmeshbuilder.h"

//----------------------------------------------------------------------------
XCALL_(int) Activate_ModelerMeshExporter(long version, 
										 GlobalFunc* global, 
										 void* local, 
										 void* serverData);

//----------------------------------------------------------------------------
class nLWModelerExportOptions;
class nLWPolygon;

//----------------------------------------------------------------------------
class nLWModelerExporter
{
	public:
		nLWModelerExporter(MeshEditOp* meshEditOp, GlobalFunc* globals);
		~nLWModelerExporter();

		void ExportMesh();

		EDError PolyScan(const EDPolygonInfo* polyInfo);
		void ModelerMessageBox(const char* text, const char* title);

	private:
		void ClearSurfaceList();

		MeshEditOp* meshEditOp;
		GlobalFunc* globals;
		LWSurfaceFuncs* surff;
		LWTextureFuncs* txtrf;
		LWStateQueryFuncs *queryf;
		LWMessageFuncs* msgf;
		//nLWModelerExportOptions options;
		
		nHashList surfaceList;
		nMeshBuilder meshBuilder;

		int curPolyIdx;
		int numPolys;
		nLWPolygon* polyArray;
};

#endif // N_LW_MODELER_EXPORTER_H
