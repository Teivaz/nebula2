#include "export/nmaxexport.h"
//------------------------------------------------------------------------------
/**
    3ds max to Nebula2 exporter using the IGame lib.

    This file is licensed under the terms of the Nebula License.
    (C) 2004 Johannes Kellner
*/

//------------------------------------------------------------------------------
/**
    dummy function needed for the progressbar startup of max
*/
DWORD WINAPI dummy(LPVOID arg)
{
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
nMaxExport::nMaxExport()
 :	maxInterface(0),
	logHandler(0),
    scriptServer(0),
	kernelServer(0),
    varServer(0),
    task(0),
	suppressPrompts(false),
    nohBase("/export")
{
    //access the DLL-global kernelServer
    n_assert(nKernelServer::ks);
    this->kernelServer = nKernelServer::ks;
	
    this->fileServer = this->kernelServer->GetFileServer();

	//the IGame property file
	this->propertyFile = N_MAXEXPORT_IGAMEPROPERYIFILE; 
}

//------------------------------------------------------------------------------
/**
*/
nMaxExport::~nMaxExport()
{
    this->CleanupData();
    
    if (this->scriptServer)
        this->scriptServer->Release();

    if(this->varServer)
        this->varServer->Release();

    if (this->task)
        n_delete this->task;
}

//------------------------------------------------------------------------------
/**
*/
int
nMaxExport::DoExport(const TCHAR *ExportFileName, ExpInterface *ei, Interface *i, BOOL suppressPrompts, DWORD options)
{
	    
    //FIXME: this should be suplied by the UI
    //create a fake ExportTaskSetup
        if (! this->task)
        {
            this->task = n_new nMaxExportTask;
        }

        this->task->exportHiddenNodes = false;
        this->task->exportAnimations = true;
        this->task->groupMeshBySourceObject = true;
        this->task->sampleRate = 1; 
        this->task->useWeightedNormals = true;

        //DISABLE the skinned export until the code is finished.
        this->task->exportStatic = true;

        //the selected file name from the export dialog
        nPathString fileName(ExportFileName);
        fileName = fileName.ExtractFileName();
        fileName.StripExtension();
            
        //the meshfile
        this->task->meshFileName = fileName;
        this->task->meshFileName = "meshes:" + this->task->meshFileName;
        this->task->meshFileExtension = ".n3d2";

        //the scenefile
        this->task->sceneFileName = fileName;
        this->task->sceneFileName = "gfxlib:" + this->task->sceneFileName;
    //END
    
    n_assert(i);
	this->maxInterface = i;
	this->expInterface = ei;
	
    //init the maxLogHandler 
    static_cast<nMaxLogHandler*>(this->kernelServer->GetLogHandler())->SetLogSys(i->Log());
	
	this->suppressPrompts = (suppressPrompts == 0) ? false : true;
	this->exportSelected = (options & SCENE_EXPORT_SELECTED) ? true : false;

    this->fileServer->SetAssign("meshes",   N_MAXEXPORT_ASSIGN_MESHES);
    this->fileServer->SetAssign("gfxlib",   N_MAXEXPORT_ASSIGN_GFXLIB);

    //create a var server, because the setting of a animators channel requires a varserver for the handle lookup
    if (! this->varServer )
    {
        this->varServer = static_cast<nVariableServer*>(this->kernelServer->New("nvariableserver", "/sys/servers/variable"));
        this->varServer->DeclareVariable(N_MAXEXPORT_TIMECHANNELNAME, this->varServer->StringToFourCC(N_MAXEXPORT_TIMECHANNELNAME));
    }

    //setup time values
    this->tickTime = (1.0f / (float)GetFrameRate()) / (float)GetTicksPerFrame(); //WARNING: maybe a float don't has enought resolution!

    //init IGame
    this->iGameScene = GetIGameInterface();
    this->iGameScene->SetPropertyFile(this->propertyFile);
    
    //setup the coord system for export
    IGameConversionManager* igConManager = GetConversionManager();
    if (igConManager == 0)
    {
        n_printf("Can't init the IGameConversionManager!\n");
        return 0;
    }
    igConManager->SetCoordSystem(N_MAXEXPORT_IGAMECOORDSYS);    

    //create a scene node for this path, so the exported scene file is a scenenode
    if (! this->kernelServer->Lookup(this->nohBase.Get()))
    {
        this->kernelServer->New("nscenenode", this->nohBase.Get());
    }
    
    bool success = true;  
    
    //do the hierarchie walk
    this->iGameScene->InitialiseIGame(this->exportSelected); //init IGame for this task
    
    //initiate progress bar
    this->progressNumTotal = this->iGameScene->GetTotalNodeCount();
    this->progressOnePercent = 100.0f / this->progressNumTotal;
    this->progressCount = 0;
    
    this->maxInterface->ProgressStart("Nebula2 scene export:", true, dummy, 0);
    this->maxInterface->ProgressUpdate(0);
    
    //export the nodes
    const int numTopNodes = this->iGameScene->GetTopLevelNodeCount();
    for (int n = 0; n < numTopNodes && success; n++)
    {
        IGameNode* igNode = this->iGameScene->GetTopLevelNode(n);
        success = this->exportNode(igNode, this->nohBase);
    }

    if (success)
    {
        //store the mesh data
        this->storeMeshPools();
        
        //store the scene description
        if (!this->scriptServer)
            this->scriptServer = static_cast<nScriptServer*>(this->kernelServer->New(N_MAXEXPORT_SCRIPTSERVER, "/sys/servers/script"));
        
        nSceneNode* exportNode = static_cast<nSceneNode*>(this->kernelServer->Lookup(this->nohBase.Get()));
        if (exportNode)
        {
            success = exportNode->SaveAs(this->task->sceneFileName.Get());
        }
    }
    
    this->CleanupData();
        
    //relase IGame
    this->iGameScene->ReleaseIGame();
    this->iGameScene = 0;

    //stop the progress bar
    this->maxInterface->ProgressEnd();

	return success ? 1 : 0;
}


//------------------------------------------------------------------------------
/**
*/
void
nMaxExport::CleanupData()
{
    //cleanup mesh data
    for (int i = 0; i < this->meshPool.Size(); i++)
    {
        if (this->meshPool[i].meshBuilder)
        {
            n_delete this->meshPool[i].meshBuilder;
            this->meshPool[i].meshBuilder = 0;
        }
    }
    this->meshPool.Clear();

    //clean scene
    nSceneNode* exportNode = static_cast<nSceneNode*>(this->kernelServer->Lookup(this->nohBase.Get()));
    if (exportNode)
    {
        exportNode->Release();
    }
}

//------------------------------------------------------------------------------
/**
    The main loop for the export:

    - get child nodes from the current node and store in a array
    - determine type of current node and handle it
        - handle skelton nodes
    - recurse into the childnodes array
    
    @todo if the node is head of a skeleton remove all child nodes that are also part of this skeleton from the child nodes array
    @todo handle the complete skeleton when found the head 
*/
bool
nMaxExport::exportNode(IGameNode* igNode, nString node)
{
    n_assert(igNode);
    
    //create the path of the current node
    const nString path = node + "/";
    nString nodeName(path);

    bool success = true;
    
    nArray<IGameNode*> childNodes(igNode->GetChildCount(),0);

    int i; // to resolve redefinition error in vc6.

    //collect childnodes
    for (i = 0; i < igNode->GetChildCount(); i++)
    {
        childNodes.Append(igNode->GetNodeChild(i));
    }

    if (!igNode->IsNodeHidden() || this->task->exportHiddenNodes)
    {
        //is this a group owner node? (this is a dummy node in max, add a transform node,
        //so we can handle the group complete later)
        if (igNode->IsGroupOwner())
        {
            nodeName += this->checkChars(igNode->GetName());
            nTransformNode* nNode = static_cast<nTransformNode*>(this->kernelServer->New("ntransformnode", nodeName.Get()));

            this->exportPosition(nNode, nodeName, igNode);
            this->exportRotation(nNode, nodeName, igNode);
            this->exportScale(nNode, nodeName, igNode);
        }
        else
        {
            IGameObject* igObject = igNode->GetIGameObject();            
            switch (igObject->GetIGameType())
            {
                case IGameObject::IGAME_MESH:
                {
                    if (this->task->groupMeshBySourceObject)
                    {
                        //create a transformnode as root node for for the material shapennodes the mesh will use
                        nodeName += this->checkChars(igNode->GetName());
                        nTransformNode* nNode = static_cast<nTransformNode*>(this->kernelServer->New("ntransformnode", nodeName.Get()));
                    
                        this->exportPosition(nNode, nodeName, igNode);
                        this->exportRotation(nNode, nodeName, igNode);
                        this->exportScale(nNode, nodeName, igNode);
                        
                        //create a matrix to transform from worldspace to object/model space
                        matrix44* transform = this->buildInverseModelWorldMatrix(igNode);
                        
                        //export the mesh data                       
                        this->exportMesh(igNode, static_cast<nSceneNode*>(nNode), nodeName, transform);
                        
                        //mem cleanup
                        n_delete transform;
                    }
                    else
                    {
                        //FIXME: export the mesh in a flat hierarchy
                        this->exportMesh(igNode);
                    }
                }
                break;
                case IGameObject::IGAME_LIGHT:
                {
                    this->exportLight(nodeName, igNode);
                }
                break;
                case IGameObject::IGAME_CAMERA:
                {
                    nodeName += "FIXME_CAMERA";
                    nodeName += this->checkChars(igNode->GetName());
                    this->kernelServer->New("ntransformnode", nodeName.Get());
                }
                break;
                case IGameObject::IGAME_BONE:
                {
                    nodeName += "FIXME_IKCHAIN";
                    nodeName += this->checkChars(igNode->GetName());
                    this->kernelServer->New("ntransformnode", nodeName.Get());
                }
                break;
                case IGameObject::IGAME_IKCHAIN:
                {
                    nodeName += "FIXME_IKCHAIN";
                    nodeName += this->checkChars(igNode->GetName());
                    this->kernelServer->New("ntransformnode", nodeName.Get());
                    //TODO:
                    //create a skeleton with all acociated bones
                    //remove the already handled child nodes (bones from this skeleton)
                }
                break;
                case IGameObject::IGAME_SPLINE:
                {
                    nodeName += "FIXME_SPLINE";
                    nodeName += this->checkChars(igNode->GetName());
                    this->kernelServer->New("ntransformnode", nodeName.Get());
                }
                break;
                case IGameObject::IGAME_HELPER:
                {
                    nodeName += "FIXME_HELPER";
                    nodeName += this->checkChars(igNode->GetName());
                    this->kernelServer->New("ntransformnode", nodeName.Get());                    
                }
                break;
                case IGameObject::IGAME_UNKNOWN:
                {
                    nodeName += "FIXME_UNKNOWN";
                    nodeName += this->checkChars(igNode->GetName());
                    this->kernelServer->New("ntransformnode", nodeName.Get());
                }
                break;
                default:
                {
                    nodeName += "FIXME_NOT_ENUM_";
                    nodeName += igObject->GetIGameType();
                    nodeName += this->checkChars(igNode->GetName());
                    this->kernelServer->New("ntransformnode", nodeName.Get());
                }
                break;
            }
        }
    }

    //create progess message
    this->progressCount++;
    nString msg;
    msg = "node ";
    msg += this->progressCount;
    msg += "/";
    msg += this->progressNumTotal;

    //update the progressbar
    const int percent = (int) ((float) this->progressCount * this->progressOnePercent);
    this->progressUpdate(percent, msg);
    
    //check if the export was cancled
    if (this->maxInterface->GetCancel())
    {
        int retval = MessageBox(this->maxInterface->GetMAXHWnd(), _T("Stop Export?"), _T("Question"), MB_ICONQUESTION | MB_YESNO);
        if (retval == IDYES)
        {
            success = false;
        }
        else
        if (retval == IDNO)
        {
            this->maxInterface->SetCancel(false);
        }
    }

    //recurse to sub nodes
    for (i = 0; success && i < childNodes.Size(); i++)
    {
        IGameNode* childNode = childNodes[i];
        success = this->exportNode(childNode, nodeName);
    }

    return success;
}

//------------------------------------------------------------------------------
/**
*/
void
nMaxExport::progressUpdate(int percent, nString msg)
{
    TCHAR* tempMsg = strdup(msg.Get()); //DON'T USE n_strdup (uses max incompatible n_malloc)
    this->maxInterface->ProgressUpdate(percent, false, tempMsg);
    delete tempMsg;
}

//------------------------------------------------------------------------------
/**
*/
nString
nMaxExport::checkChars(nString &string)
{
    char* temp = n_new char[string.Length()+1];
    strcpy(temp, string.Get());
    string.Set(this->checkChars(temp));
    return string;
}


//------------------------------------------------------------------------------
/**
*/
char*
nMaxExport::checkChars(const char* string)
{
    char* temp = n_new char[strlen(string)+1];
    strcpy(temp, string);
    temp = this->checkChars(temp);
    return temp;
}

//------------------------------------------------------------------------------
/**
*/
char*
nMaxExport::checkChars(char* string)
{
	char *t = string;
	char *s = t;
	while(*t)
	{
		switch(*t)
		{
		case '>':	//bad character
		case '<':	//bad character
		case '\\':	//bad character
		case '/':	//bad character
		case ':':	//bad character
        case '[':   //bad character
        case ']':   //bad character
        case '(':   //bad character
        case ')':   //bad character
		case ' ':	//space
		case '\n':	//newline
		case '\r':	//carriage return
		case '\t':	//tab
			if(*(t+1) == 0 ) *s = 0;
			else *s = '_';
			s++;
			break;
		default:
			*s = *t;
			s++;
			break;
		}
		t++;
	}
	return string;
}
