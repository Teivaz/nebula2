#include "export/nmaxexport.h"
#include "../res/resource.h"

#include "tools/napplauncher.h"

//------------------------------------------------------------------------------
/**
    3ds max to Nebula2 exporter using the IGame lib.

    This file is licensed under the terms of the Nebula License.
    (C) 2004 Johannes Kellner
    Sep 2004 Oleg Kreptul (Haron)
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
    Dialog box creator
*/
BOOL N_THREADPROC Nebula2ExporterOptionsDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	nMaxExportTask *exp = (nMaxExportTask*)GetWindowLongPtr(hWnd,GWLP_USERDATA); 
    nFileServer2 *fileServer = nFileServer2::Instance();
	switch(message)
    {
		case WM_INITDIALOG:
			exp = (nMaxExportTask *)lParam;
			SetWindowLongPtr(hWnd,GWLP_USERDATA,lParam); 
			CenterWindow(hWnd,GetParent(hWnd));

            SetDlgItemText(hWnd, IDC_EXT_DIRNAME,     exp->GetHomeDir().Get());
            SetDlgItemText(hWnd, IDC_EXT_BINARY_PATH, exp->binaryPath.Get());

            CheckDlgButton(hWnd, IDC_EXT_EXPORT_ANIM, exp->exportAnimations);

            SetDlgItemText(hWnd, IDC_EXT_ASSIGN_ANIM,     exp->animsAssign.Get());
            SetDlgItemText(hWnd, IDC_EXT_ASSIGN_GFXLIB,   exp->gfxlibAssign.Get());
            SetDlgItemText(hWnd, IDC_EXT_ASSIGN_GUI,      exp->guiAssign.Get());
            SetDlgItemText(hWnd, IDC_EXT_ASSIGN_LIGHTS,   exp->lightsAssign.Get());
            SetDlgItemText(hWnd, IDC_EXT_ASSIGN_MESHES,   exp->meshesAssign.Get());
            SetDlgItemText(hWnd, IDC_EXT_ASSIGN_SHADERS,  exp->shadersAssign.Get());
            SetDlgItemText(hWnd, IDC_EXT_ASSIGN_TEXTURES, exp->texturesAssign.Get());

            SetDlgItemText(hWnd, IDC_EXT_PATH_ANIM,     exp->animsPath.Get());
            SetDlgItemText(hWnd, IDC_EXT_PATH_GFXLIB,   exp->gfxlibPath.Get());
            SetDlgItemText(hWnd, IDC_EXT_PATH_GUI,      exp->guiPath.Get());
            SetDlgItemText(hWnd, IDC_EXT_PATH_LIGHTS,   exp->lightsPath.Get());
            SetDlgItemText(hWnd, IDC_EXT_PATH_MESHES,   exp->meshesPath.Get());
            SetDlgItemText(hWnd, IDC_EXT_PATH_SHADERS,  exp->shadersPath.Get());
            SetDlgItemText(hWnd, IDC_EXT_PATH_TEXTURES, exp->texturesPath.Get());

			return TRUE;
		case WM_COMMAND:
			switch (LOWORD(wParam))
            {
            case IDC_SET_DEFAULT_ASSIGNS:
                SetDlgItemText(hWnd, IDC_EXT_ASSIGN_ANIM,     N_MAXEXPORT_ANIMS_ASSIGN);
                SetDlgItemText(hWnd, IDC_EXT_ASSIGN_GFXLIB,   N_MAXEXPORT_GFXLIB_ASSIGN);
                SetDlgItemText(hWnd, IDC_EXT_ASSIGN_GUI,      N_MAXEXPORT_GUI_ASSIGN);
                SetDlgItemText(hWnd, IDC_EXT_ASSIGN_LIGHTS,   N_MAXEXPORT_LIGHTS_ASSIGN);
                SetDlgItemText(hWnd, IDC_EXT_ASSIGN_MESHES,   N_MAXEXPORT_MESHES_ASSIGN);
                SetDlgItemText(hWnd, IDC_EXT_ASSIGN_SHADERS,  N_MAXEXPORT_SHADERS_ASSIGN);
                SetDlgItemText(hWnd, IDC_EXT_ASSIGN_TEXTURES, N_MAXEXPORT_TEXTURES_ASSIGN);
                break;
            case IDC_SET_DEFAULT_PATHS:
                SetDlgItemText(hWnd, IDC_EXT_PATH_ANIM,     N_MAXEXPORT_ANIMS_PATH);
                SetDlgItemText(hWnd, IDC_EXT_PATH_GFXLIB,   N_MAXEXPORT_GFXLIB_PATH);
                SetDlgItemText(hWnd, IDC_EXT_PATH_GUI,      N_MAXEXPORT_GUI_PATH);
                SetDlgItemText(hWnd, IDC_EXT_PATH_LIGHTS,   N_MAXEXPORT_LIGHTS_PATH);
                SetDlgItemText(hWnd, IDC_EXT_PATH_MESHES,   N_MAXEXPORT_MESHES_PATH);
                SetDlgItemText(hWnd, IDC_EXT_PATH_SHADERS,  N_MAXEXPORT_SHADERS_PATH);
                SetDlgItemText(hWnd, IDC_EXT_PATH_TEXTURES, N_MAXEXPORT_TEXTURES_PATH);
                break;
			case IDOK:
                char str[512];
                GetDlgItemText(hWnd, IDC_EXT_DIRNAME,         str, 512); exp->SetHomeDir(str);
                GetDlgItemText(hWnd, IDC_EXT_BINARY_PATH,     str, 512); exp->binaryPath = str;

				exp->exportAnimations = (BST_CHECKED == IsDlgButtonChecked(hWnd, IDC_EXT_EXPORT_ANIM) ? true : false);

                GetDlgItemText(hWnd, IDC_EXT_ASSIGN_ANIM,     str, 512); exp->animsAssign = str;
                GetDlgItemText(hWnd, IDC_EXT_ASSIGN_GFXLIB,   str, 512); exp->gfxlibAssign = str;
                GetDlgItemText(hWnd, IDC_EXT_ASSIGN_GUI,      str, 512); exp->guiAssign = str;
                GetDlgItemText(hWnd, IDC_EXT_ASSIGN_LIGHTS,   str, 512); exp->lightsAssign = str;
                GetDlgItemText(hWnd, IDC_EXT_ASSIGN_MESHES,   str, 512); exp->meshesAssign = str;
                GetDlgItemText(hWnd, IDC_EXT_ASSIGN_SHADERS,  str, 512); exp->shadersAssign = str;
                GetDlgItemText(hWnd, IDC_EXT_ASSIGN_TEXTURES, str, 512); exp->texturesAssign = str;

                GetDlgItemText(hWnd, IDC_EXT_PATH_ANIM,     str, 512); exp->animsPath = str;
                GetDlgItemText(hWnd, IDC_EXT_PATH_GFXLIB,   str, 512); exp->gfxlibPath = str;
                GetDlgItemText(hWnd, IDC_EXT_PATH_GUI,      str, 512); exp->guiPath = str;
                GetDlgItemText(hWnd, IDC_EXT_PATH_LIGHTS,   str, 512); exp->lightsPath = str;
                GetDlgItemText(hWnd, IDC_EXT_PATH_MESHES,   str, 512); exp->meshesPath = str;
                GetDlgItemText(hWnd, IDC_EXT_PATH_SHADERS,  str, 512); exp->shadersPath = str;
                GetDlgItemText(hWnd, IDC_EXT_PATH_TEXTURES, str, 512); exp->texturesPath = str;

                if (!fileServer->DirectoryExists(exp->GetHomeDir().Get()))
                {
                    n_message("ALERT: home path '%s' doesn't exists!", exp->GetHomeDir().Get());
                }
                else
                {
                    nString tmp;

                    fileServer->SetAssign("home", exp->GetHomeDir().Get());

                    if (!fileServer->DirectoryExists(exp->binaryPath.Get()))
                    {
                        tmp += "\n  binary path: ";
                        tmp += exp->binaryPath;
                    }

                    if (!fileServer->DirectoryExists(exp->animsAssign.Get()))
                    {
                        tmp += "\n  'anims' assign: ";
                        tmp += exp->animsAssign;
                    }
                    else
                    {
                        fileServer->SetAssign("anims", exp->animsAssign.Get());
                        if (!fileServer->DirectoryExists(exp->animsPath.Get()))
                        {
                            tmp += "\n  'Animation' path: ";
                            tmp += exp->animsPath;
                        }
                    }

                    if (!fileServer->DirectoryExists(exp->gfxlibAssign.Get()))
                    {
                        tmp += "\n  'gfxlib' assign: ";
                        tmp += exp->gfxlibAssign;
                    }
                    else
                    {
                        fileServer->SetAssign("gfxlib", exp->gfxlibAssign.Get());
                        if (!fileServer->DirectoryExists(exp->gfxlibPath.Get()))
                        {
                            tmp += "\n  'Gfx' path: ";
                            tmp += exp->gfxlibPath;
                        }
                    }

                    if (!fileServer->DirectoryExists(exp->guiAssign.Get()))
                    {
                        tmp += "\n  'gui' assign: ";
                        tmp += exp->guiAssign;
                    }
                    else
                    {
                        fileServer->SetAssign("gui", exp->guiAssign.Get());
                        if (!fileServer->DirectoryExists(exp->guiPath.Get()))
                        {
                            tmp += "\n  'Gui' path: ";
                            tmp += exp->guiPath;
                        }
                    }

                    if (!fileServer->DirectoryExists(exp->lightsAssign.Get()))
                    {
                        tmp += "\n  'lights' assign: ";
                        tmp += exp->lightsAssign;
                    }
                    else
                    {
                        fileServer->SetAssign("lights", exp->lightsAssign.Get());
                        if (!fileServer->DirectoryExists(exp->lightsPath.Get()))
                        {
                            tmp += "\n  'Lights' path: ";
                            tmp += exp->lightsPath;
                        }
                    }

                    if (!fileServer->DirectoryExists(exp->meshesAssign.Get()))
                    {
                        tmp += "\n  'meshes' assign: ";
                        tmp += exp->meshesAssign;
                    }
                    else
                    {
                        fileServer->SetAssign("meshes", exp->meshesAssign.Get());
                        if (!fileServer->DirectoryExists(exp->meshesPath.Get()))
                        {
                            tmp += "\n  'Meshes' path: ";
                            tmp += exp->meshesPath;
                        }
                    }

                    if (!fileServer->DirectoryExists(exp->shadersAssign.Get()))
                    {
                        tmp += "\n  'shaders' assign: ";
                        tmp += exp->shadersAssign;
                    }
                    else
                    {
                        fileServer->SetAssign("shaders", exp->shadersAssign.Get());
                        if (!fileServer->DirectoryExists(exp->shadersPath.Get()))
                        {
                            tmp += "\n  'Shaders' path: ";
                            tmp += exp->shadersPath;
                        }
                    }

                    if (!fileServer->DirectoryExists(exp->texturesAssign.Get()))
                    {
                        tmp += "\n  'textures' assign: ";
                        tmp += exp->texturesAssign;
                    }
                    else
                    {
                        fileServer->SetAssign("textures", exp->texturesAssign.Get());
                        if (!fileServer->DirectoryExists(exp->texturesPath.Get()))
                        {
                            tmp += "\n  'Textures' path: ";
                            tmp += exp->texturesPath;
                        }
                    }

                    if (tmp.Length() > 0)
                    {
                        nString alerts("ALERT: some pathes are wrong:");
                        alerts += tmp;
                        n_message("%s", alerts.Get());
                    }
                    else
                    {
                        EndDialog(hWnd, 1);
                    }
                }
				break;
			case IDCANCEL:
				EndDialog(hWnd,0);
				break;
			}
		default:
			return FALSE;
    
	}
	return TRUE;
    
}

//------------------------------------------------------------------------------
/**
*/
nMaxExport::nMaxExport()
:	maxInterface(0),
	logHandler(0),
    scriptServer(0),
    varServer(0),
    task(0),
	suppressPrompts(false),
    nohBase("/export")
{
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
    {
        this->scriptServer->Release();
        this->scriptServer = 0;
    }

    if (this->varServer)
    {
        this->varServer->Release();
        this->varServer = 0;
    }

    if (this->task)
    {
        n_delete(this->task);
        this->task = 0;
    }
}

//------------------------------------------------------------------------------
/**
    Get path to config file path placed in plugcfg directory of 3DS Max
*/
void
nMaxExport::GetCfgFilename(nPathString& fileName)
{
	fileName += GetCOREInterface()->GetDir(APP_PLUGCFG_DIR);
	fileName += "\\Nebula2Export.cfg";
}

//------------------------------------------------------------------------------
/**
    Init base assigns
*/
void
nMaxExport::InitAssigns()
{
    nFileServer2 *fileServer = nFileServer2::Instance();

    fileServer->SetAssign("home",     this->task->GetHomeDir().Get());
    fileServer->SetAssign("anims",    this->task->animsAssign.Get());
    fileServer->SetAssign("gfxlib",   this->task->gfxlibAssign.Get());
    fileServer->SetAssign("gui",      this->task->guiAssign.Get());
    fileServer->SetAssign("lights",   this->task->lightsAssign.Get());
    fileServer->SetAssign("meshes",   this->task->meshesAssign.Get());
    fileServer->SetAssign("shaders",  this->task->shadersAssign.Get());
    fileServer->SetAssign("textures", this->task->texturesAssign.Get());
}

//------------------------------------------------------------------------------
/**
*/
int
nMaxExport::DoExport(const TCHAR *ExportFileName, ExpInterface *ei, Interface *i, BOOL suppressPrompts, DWORD options)
{
    n_assert(i);	    

    if (!this->task)
    {
        this->task = n_new(nMaxExportTask);
    }

    nPathString cfn;
    this->GetCfgFilename(cfn);
    this->task->SetCfgFileName(cfn);
    this->task->ReadConfig();

	// setup interfaces
    this->maxInterface = i;
	this->expInterface = ei;
    
    //init the maxLogHandler 
    static_cast<nMaxLogHandler*>(nKernelServer::Instance()->GetLogHandler())->SetLogSys(i->Log());

   	// Prompt the user with our dialogbox, and get all the options.
	if (!DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_PANEL), i->GetMAXHWnd(),
                        Nebula2ExporterOptionsDlgProc, (LPARAM)this->task))
    {
		return 1;
	}

    this->task->exportHiddenNodes = false;
    this->task->groupMeshBySourceObject = true;
    this->task->sampleRate = 1; 
    this->task->useWeightedNormals = true;

    //the selected file name from the export dialog
    nPathString fileName(ExportFileName);
    fileName = fileName.ExtractFileName();
    fileName.StripExtension();
        
    //the meshfile
    this->task->meshFileName = fileName;
    this->task->meshFileExtension = ".n3d2";

	//the scenefile
    this->task->sceneFileName = fileName;
    this->task->sceneFileName += ".n2";

    //not needed any more
    //this->InitAssigns();
    
	this->suppressPrompts = (suppressPrompts == 0) ? false : true;
	this->exportSelected = (options & SCENE_EXPORT_SELECTED) ? true : false;

    //create a var server, because the setting of a animators channel requires a varserver for the handle lookup
    if (! this->varServer )
    {
        this->varServer = static_cast<nVariableServer*>(nKernelServer::Instance()->New("nvariableserver", "/sys/servers/variable"));
        this->varServer->DeclareVariable(N_MAXEXPORT_TIMECHANNELNAME, this->varServer->StringToFourCC(N_MAXEXPORT_TIMECHANNELNAME));
    }

    //setup time values
    this->tickTime = (1.0f / (float)GetFrameRate()) / (float)GetTicksPerFrame(); //WARNING: maybe a float don't has enought resolution!

    //init IGame
    this->iGameScene = GetIGameInterface();
	if (0 == this->iGameScene)
	{
		n_message("ERROR: nMaxExport: Could not load IGame!\nMake shure you have the right IGame.dll in you 3dsmax path!\n");
		return 0;
	}
	const float igVersion = GetIGameVersion();
	if (1.122f > igVersion)
	{
		n_message("ERROR: nMaxExport: The loaded IGame.dll has the version '%f' but required is at least v1.122!"
			    "\nMake shure you have the right IGame.dll in you 3dsmax path!", igVersion);
        return 0;
    }
    this->iGameScene->SetPropertyFile(this->propertyFile);
    
    //setup the coord system for export
    IGameConversionManager* igConManager = GetConversionManager();
    if (0 == igConManager)
    {
        n_message("ERROR: nMaxExport: Can't init the IGameConversionManager!\nMake shure you have the right IGame.dll in you 3dsmax path!\n");
        return 0;
    }
    igConManager->SetCoordSystem(N_MAXEXPORT_IGAMECOORDSYS);    

    //create a scene node for this path, so the exported scene file is a scenenode
    if (!nKernelServer::Instance()->Lookup(this->nohBase.Get()))
    {
        nKernelServer::Instance()->New("nscenenode", this->nohBase.Get());
    }
    
    bool success = true;  
    
    //do the hierarchie walk
    this->iGameScene->InitialiseIGame(/*this->exportSelected*/); //init IGame for this task
    
    this->iGameScene->SetStaticFrame(0);
    
    this->ticksPerFrame = this->iGameScene->GetSceneTicks();
    this->startFrame = this->iGameScene->GetSceneStartTime() / this->ticksPerFrame;
    this->endFrame = this->iGameScene->GetSceneEndTime() / this->ticksPerFrame;

    //initiate progress bar
    this->progressNumTotal = this->iGameScene->GetTotalNodeCount();
    this->progressOnePercent = 100.0f / this->progressNumTotal;
    this->progressCount = 0;
    
    this->maxInterface->ProgressStart("Nebula2 scene export:", true, dummy, 0);
    this->maxInterface->ProgressUpdate(0);
    
    //export the nodes
    const int numTopNodes = this->iGameScene->GetTopLevelNodeCount();
    for (int n = 0; success && n < numTopNodes; n++)
    {
        IGameNode* igNode = this->iGameScene->GetTopLevelNode(n);
        success &= this->exportNode(igNode, this->nohBase);
        if (!success)
        {
            n_message("ERROR: nMaxExport: Failed to export '%s'!\nStoping export now!\n", igNode->GetName());
            // do not return here, because a the exporter must run down to cleanup data.
        }
    }

    if (success)
    {
        //store the mesh data
        this->storeDataPools();
        
        //store the scene description
        if (!this->scriptServer)
            this->scriptServer = static_cast<nScriptServer*>(nKernelServer::Instance()->New(N_MAXEXPORT_SCRIPTSERVER, "/sys/servers/script"));
        
        nSceneNode* exportNode = static_cast<nSceneNode*>(nKernelServer::Instance()->Lookup(this->nohBase.Get()));
        n_assert(exportNode);
        
        nString sceneFilePath(this->task->gfxlibPath);
        sceneFilePath += this->task->sceneFileName;
        success &= exportNode->SaveAs(sceneFilePath.Get());
        if (!success)
        {
            n_message("ERROR: nMaxExport: Failed to save scene to file '%s'!\n", sceneFilePath.Get());
        }
        else
        {
            nAppLauncher appLauncher(nKernelServer::Instance());
            nString appPath = this->task->binaryPath.Get();
            appPath.Append("nviewer.exe");
            appLauncher.SetExecutable(appPath.Get());
            appLauncher.SetWorkingDirectory("home:");
            nString args = "-view ";
            args.Append(sceneFilePath.Get());
            appLauncher.SetArguments(args.Get());
            success &= appLauncher.Launch();

            if (!success)
            {
                n_message("ERROR: nMaxExport: Failed to start nviewer '%s'!\n", nFileServer2::Instance()->ManglePath(appPath.Get()));
            }
        }
    }
    
    // DATA CLEANUP
    this->CleanupData();
        
    //relase IGame
    this->iGameScene->ReleaseIGame();
    this->iGameScene = 0;

    //stop the progress bar
    this->maxInterface->ProgressEnd();

    this->task->WriteConfig();

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
            n_delete(this->meshPool[i].meshBuilder);
            this->meshPool[i].meshBuilder = 0;
        }
        if (this->meshPool[i].animBuilder)
        {
            n_delete(this->meshPool[i].animBuilder);
            this->meshPool[i].animBuilder = 0;
        }
    }
    this->meshPool.Clear();

    //clean scene
    nSceneNode* exportNode = static_cast<nSceneNode*>(nKernelServer::Instance()->Lookup(this->nohBase.Get()));
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
        int nodeID = igNode->GetNodeID();
        IGameNode *nodeParent = igNode->GetNodeParent();

        nodeName += this->checkChars(igNode->GetName());
        //is this a group owner node? (this is a dummy node in max, add a transform node,
        //so we can handle the group complete later)
        if (igNode->IsGroupOwner())
        {
            nTransformNode* nNode = static_cast<nTransformNode*>(nKernelServer::Instance()->New("ntransformnode", nodeName.Get()));

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
                        //export the mesh data                       
                        this->exportMesh(igNode, nodeName);
                    }
                    else
                    {
                        //FIXME: export the mesh in a flat hierarchy
                        this->exportMesh(igNode);
                    }
                }
                break;
/*
                case IGameObject::IGAME_LIGHT:
                {
                    this->exportLight(nodeName, igNode);
                }
                break;
                case IGameObject::IGAME_CAMERA:
                {
                    nodeName += "FIXME_CAMERA_";
                    this->kernelServer->New("ntransformnode", nodeName.Get());
                }
                break;
                case IGameObject::IGAME_BONE:
                {
                    nodeName += "FIXME_IKCHAIN_";
                    this->kernelServer->New("ntransformnode", nodeName.Get());
                }
                break;
                case IGameObject::IGAME_IKCHAIN:
                {
                    nodeName += "FIXME_IKCHAIN_";
                    this->kernelServer->New("ntransformnode", nodeName.Get());
                    //TODO:
                    //create a skeleton with all acociated bones
                    //remove the already handled child nodes (bones from this skeleton)
                }
                break;
                case IGameObject::IGAME_SPLINE:
                {
                    nodeName += "FIXME_SPLINE_";
                    this->kernelServer->New("ntransformnode", nodeName.Get());
                }
                break;
                case IGameObject::IGAME_HELPER:
                {
                    nodeName += "FIXME_HELPER_";
                    this->kernelServer->New("ntransformnode", nodeName.Get());                    
                }
                break;
                case IGameObject::IGAME_UNKNOWN:
                {
                    nodeName += "FIXME_UNKNOWN_";
                    this->kernelServer->New("ntransformnode", nodeName.Get());
                }
                break;
*/
                default:
                {
                    nodeName += "_FIXME_NOT_SUPPORTED_";
					nodeName.AppendInt(igObject->GetIGameType());
                    n_printf("nMaxExporter: %s!", nodeName.Get());
                }
                break;
            }
        }
    }

    //create progess message
    this->progressCount++;
    nString msg;
    msg = "node ";
	msg.AppendInt(this->progressCount);
    msg += "/";
	msg.AppendInt(this->progressNumTotal);
	msg += " <";
	msg += igNode->GetName();
	msg += ">";

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

    // free memory
    igNode->ReleaseIGameObject();

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
    char* temp = n_new_array(char, string.Length() + 1);
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
    char* temp = n_new(char, strlen(string) + 1);
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
