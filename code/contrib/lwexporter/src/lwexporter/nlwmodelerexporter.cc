//----------------------------------------------------------------------------
// (c) 2005    Vadim Macagon
//----------------------------------------------------------------------------
#include <lwserver.h>
#include <lwhandler.h>
#include <lwxpanel.h>
#include "kernel/ntypes.h"
#include "lwexporter/nlwmodelerexporter.h"
#include "kernel/nfileserver2.h"

//----------------------------------------------------------------------------
class nLWModelerExportOptions
{
	public:
		/// Set whether to save the mesh as a binary .nvx2 or a ascii .n3d2
		void SetBinaryFormat(bool binary) {}
		bool GetBinaryFormat() { return true; }
		void SetMeshOutputPath(const char* path) {}
		const char* GetMeshOutputPath() { return 0; }

	private:
		//nString meshOutputPath;
		//bool saveAsBinary;
		//nString textureOutputPath;
};

//----------------------------------------------------------------------------
class nSurfaceNode : public nHashNode
{
	public:
		/// surface ID
		LWSurfaceID id;
		/// number of polygons that use this surface
		int polyCount;

		nSurfaceNode(const char* surfaceName)
		: nHashNode(surfaceName)
		{
		}
};

//----------------------------------------------------------------------------
class nLWPolygon
{
	public:
		LWPolID id;
		LWPntID points[3];
		nSurfaceNode* surface;
};

//----------------------------------------------------------------------------
/**
*/
static
EDError PolyScanCallback(void* userData, const EDPolygonInfo* polyInfo)
{
	n_assert(userData);
	if (!userData)
		return EDERR_BADARGS;

	nLWModelerExporter* exporter = (nLWModelerExporter*)userData;
	return exporter->PolyScan(polyInfo);
}

//----------------------------------------------------------------------------
/**
*/
nLWModelerExporter::nLWModelerExporter(MeshEditOp* meshEditOp,
									   GlobalFunc* globals)
: numPolys(0), polyArray(0), curPolyIdx(0)
{
	this->meshEditOp = meshEditOp;
	this->globals = globals;
	this->surff = (LWSurfaceFuncs*)globals(LWSURFACEFUNCS_GLOBAL,
										   GFUSE_TRANSIENT);
	this->queryf = (LWStateQueryFuncs*)globals(LWSTATEQUERYFUNCS_GLOBAL,
											   GFUSE_TRANSIENT);
	this->msgf = (LWMessageFuncs*)globals(LWMESSAGEFUNCS_GLOBAL,
										  GFUSE_TRANSIENT);
	this->txtrf = (LWTextureFuncs*)globals(LWTEXTUREFUNCS_GLOBAL,
										   GFUSE_TRANSIENT);
}

//----------------------------------------------------------------------------
/**
*/
nLWModelerExporter::~nLWModelerExporter()
{
	this->ClearSurfaceList();
	if (this->polyArray)
		n_delete_array(this->polyArray);
}

//----------------------------------------------------------------------------
/**
*/
EDError nLWModelerExporter::PolyScan(const EDPolygonInfo* polyInfo)
{
	if (polyInfo->type != LWPOLTYPE_FACE)
	{
		this->msgf->error("You can only export faces!", 0);
		return EDERR_BADARGS;
	}

	if (polyInfo->numPnts != 3)
	{
		this->msgf->error("All faces must be triangles!", 0);
		return EDERR_BADARGS;
	}

	this->polyArray[this->curPolyIdx].id = polyInfo->pol;

	// keep count of how many polys use any particular surface
	const char* surfName = polyInfo->surface;
	if (surfName)
	{
		nSurfaceNode* node = (nSurfaceNode*)this->surfaceList.Find(surfName);
		if (node)
		{
			node->polyCount++;
			this->polyArray[this->curPolyIdx].surface = node;
		}
		else
			this->polyArray[this->curPolyIdx].surface = 0;
	}

	// extract vertices
	nMeshBuilder::Vertex vertex;
	for (int i = 0; i < 3; i++)
		this->polyArray[this->curPolyIdx].points[i] = polyInfo->points[i];

	this->curPolyIdx++;
	return EDERR_NONE;
}

//----------------------------------------------------------------------------
/**
*/
void nLWModelerExporter::ClearSurfaceList()
{
	nSurfaceNode* node = 0;
	while (node = (nSurfaceNode*)this->surfaceList.RemTail())
		n_delete(node);
}

//----------------------------------------------------------------------------
/**
*/
void nLWModelerExporter::ExportMesh()
{
	if (!this->surff)
		return;

	if (!this->queryf)
		return;

	if (!this->txtrf)
		return;

	if (!this->msgf)
		return;

	const char* objName = this->queryf->object();
	if (!objName)
	{
		this->msgf->error("No object loaded!", 0);
		return;
	}

	nKernelServer kernelServer;

	// get a list of surfaces for the object
	LWSurfaceID* objSurfaces = this->surff->byObject(objName);
	if (objSurfaces)
	{
		int i = 0;
		while (objSurfaces[i])
		{
			const char* surfName = this->surff->name(objSurfaces[i]);
			nSurfaceNode* node = n_new(nSurfaceNode(surfName));
			node->id = objSurfaces[i];
			node->polyCount = 0;
			this->surfaceList.AddTail((nHashNode*)node);
			++i;
		}
	}

	// extract polygons from modeler
	this->curPolyIdx = 0;
	this->numPolys = meshEditOp->polyCount(meshEditOp->state, OPLYR_FG,
										   EDCOUNT_ALL);
	this->polyArray = n_new_array(nLWPolygon, this->numPolys);
	this->meshEditOp->polyScan(meshEditOp->state, PolyScanCallback, this,
							   OPLYR_FG);

	n_assert(this->curPolyIdx == this->numPolys);

	// go through and add all vertices and polygons to the mesh builder
	int groupId = 0;
	nMeshBuilder::Vertex vertex;
	nMeshBuilder::Triangle tri;

	// first handle the polys with surfaces
	nSurfaceNode* surfNode = (nSurfaceNode*)this->surfaceList.GetHead();
	for (; surfNode; surfNode = (nSurfaceNode*)surfNode->GetSucc())
		if (surfNode->polyCount > 0)
		{
			// TODO: optimize!
			LWTextureID textureId = this->surff->getTex(surfNode->id, SURF_COLR);
			LWTLayerID textureLayerId = 0;
			if (textureId)
			{
				// only handle 1 layer for now (will add support for another 3 later)
				textureLayerId = txtrf->firstLayer(textureId);
				if (textureLayerId)
				{
					int layerType = txtrf->layerType(textureLayerId);
					if (TLT_IMAGE != layerType)
						textureLayerId = 0;
				}
			}
			for (int polyIdx = 0; polyIdx < this->numPolys; polyIdx++)
			{
				if (this->polyArray[polyIdx].surface == surfNode)
				{
					nLWPolygon& poly = this->polyArray[polyIdx];
					for (int pointIdx = 0; pointIdx < 3; pointIdx++)
					{
						EDPointInfo* point = this->meshEditOp->pointInfo(
											     this->meshEditOp->state,
												 poly.points[pointIdx]);
						vertex.SetCoord(vector3((float)point->position[0],
							                    (float)point->position[1],
												(float)point->position[2]));
						// extract UVs
						if (textureLayerId)
						{
							int proj;
							float uv[2];
							this->txtrf->getParam(textureLayerId, TXTAG_PROJ, &proj);
							if (TXPRJ_UVMAP == proj)
							{
								void* vmap;
								this->txtrf->getParam(textureLayerId, TXTAG_VMAP, &vmap);
								this->meshEditOp->pointVSet(this->meshEditOp->state,
									                        vmap, 0, 0);
								this->meshEditOp->pointVEval(this->meshEditOp->state,
									                         poly.points[pointIdx],
															 poly.id, uv);
								vertex.SetUv(0, vector2(uv));
							}
							//else
							//{
							//	this->txtrf->evaluateUV(textureLayerId, wAxis, oAxis, oPos, wPos, uv);
							//}
						}
						this->meshBuilder.AddVertex(vertex);
					}
					tri.SetGroupId(groupId);
					tri.SetVertexIndices(polyIdx * 3,
						                 polyIdx * 3 + 1,
										 polyIdx * 3 + 2);
					this->meshBuilder.AddTriangle(tri);
				}
			}
			++groupId;
		}

	// now handle the polys without any surface
	// only export coordinates for these
	for (int polyIdx = 0; polyIdx < this->numPolys; polyIdx++)
	{
		if (!this->polyArray[polyIdx].surface)
		{
			nLWPolygon& poly = this->polyArray[polyIdx];
			for (int pointIdx = 0; pointIdx < 3; pointIdx++)
			{
				EDPointInfo* point = this->meshEditOp->pointInfo(
									     this->meshEditOp->state,
										 poly.points[pointIdx]);
				vertex.SetCoord(vector3((float)point->position[0],
					                    (float)point->position[1],
										(float)point->position[2]));
				this->meshBuilder.AddVertex(vertex);
			}
			tri.SetGroupId(groupId);
			tri.SetVertexIndices(polyIdx * 3,
				                 polyIdx * 3 + 1,
								 polyIdx * 3 + 2);
			this->meshBuilder.AddTriangle(tri);
		}
	}

	this->meshBuilder.Cleanup(0);
	this->meshBuilder.SaveN3d2(nFileServer2::Instance(), "D:\\testmesh.n3d2");
}

//----------------------------------------------------------------------------
/**
*/
/*
extern "C"
void UIChangeNotify(LWXPanelID panel, unsigned long cid, unsigned long vid,
					int eventType)
{
	if (LWXPEVENT_VALUE == eventType)
		switch (cid)
		{
			case ID_BINARY:

		}
}
*/

//----------------------------------------------------------------------------
/**
*/
static
bool ShowUI(LWXPanelFuncs* xpanf, nLWModelerExportOptions* options)
{
	LWXPanelID panel;
	bool ok = false;
	enum
	{
		ID_OUTPUT_FILENAME = 0x8001,
		ID_BINARY
	};

	LWXPanelControl controls[] =
	{
        { ID_OUTPUT_FILENAME, "Output File", "sFileName" },
		{ ID_BINARY, "Save As Binary", "iBoolean" },
        { 0 }
    };

    LWXPanelDataDesc controlDesc[] =
	{
        { ID_OUTPUT_FILENAME, "OutputFile", "string" },
		{ ID_BINARY, "SaveAsBinary", "integer" },
        { 0 }
    };

    LWXPanelHint controlHints[] =
	{
        XpLABEL(0, "Nebula 2 Mesh Exporter"),
		XpXREQCFG(ID_OUTPUT_FILENAME, LWXPREQ_SAVE, "Save Mesh As...", "*.n3d2;*.nvx2"),
		XpEND
    };

	panel = xpanf->create(LWXP_FORM, controls);
	if (!panel)
		return false;
	xpanf->describe(panel, controlDesc, 0, 0);
	xpanf->hint(panel, 0, controlHints);
	// set default values
	xpanf->formSet(panel, ID_OUTPUT_FILENAME, "");
	xpanf->formSet(panel, ID_BINARY, 0);
	// display
	ok = (xpanf->post(panel) != 0);
	if (ok)
	{
		// get control values
		// ...
	}
	xpanf->destroy(panel);
	return ok;
}

//----------------------------------------------------------------------------
/**
	@brief Modeler Mesh Exporter plugin activation function.
*/
XCALL_(int) Activate_ModelerMeshExporter(long version, GlobalFunc* global,
										 void* local, void* serverData)
{
	if (version != LWINTERFACE_VERSION)
		return AFUNC_BADVERSION;

	MeshEditBegin* meshEditBegin = (MeshEditBegin*)local;
	MeshEditOp* meshEditOp = meshEditBegin(1, 1, OPSEL_GLOBAL);

	LWXPanelFuncs* xpanf = (LWXPanelFuncs*)global(LWXPANELFUNCS_GLOBAL, GFUSE_TRANSIENT);
    if (!xpanf)
        return AFUNC_BADGLOBAL;

	nLWModelerExporter* exporter;
	nLWModelerExportOptions options;
	if (ShowUI(xpanf, &options))
	{
		exporter = n_new(nLWModelerExporter(meshEditOp, global));
		if (exporter)
			exporter->ExportMesh();
	}

	if (exporter)
		n_delete(exporter);

	meshEditOp->done(meshEditOp->state, EDERR_NONE, 0);

	return AFUNC_OK;
}

//----------------------------------------------------------------------------
// EOF
//----------------------------------------------------------------------------
