#include "export/nmaxexporttask.h"
#include "kernel/nfileserver2.h"
#include "kernel/nfile.h"

//------------------------------------------------------------------------------
/**
*/
nMaxExportTask::nMaxExportTask() :
	maxJointPaletteSize(72),
	forcedVextexComponents(0),
    cfgFileName(""),
    homeDir("."),
    binaryPath(N_MAXEXPORT_BINARY_PATH),
    animsAssign(N_MAXEXPORT_ANIMS_ASSIGN),
    gfxlibAssign(N_MAXEXPORT_GFXLIB_ASSIGN),
    guiAssign(N_MAXEXPORT_GUI_ASSIGN),
    lightsAssign(N_MAXEXPORT_LIGHTS_ASSIGN),
    meshesAssign(N_MAXEXPORT_MESHES_ASSIGN),
    shadersAssign(N_MAXEXPORT_SHADERS_ASSIGN),
    texturesAssign(N_MAXEXPORT_TEXTURES_ASSIGN),
    animsPath(N_MAXEXPORT_ANIMS_PATH),
    gfxlibPath(N_MAXEXPORT_GFXLIB_PATH),
    guiPath(N_MAXEXPORT_GUI_PATH),
    lightsPath(N_MAXEXPORT_LIGHTS_PATH),
    meshesPath(N_MAXEXPORT_MESHES_PATH),
    shadersPath(N_MAXEXPORT_SHADERS_PATH),
    texturesPath(N_MAXEXPORT_TEXTURES_PATH),
    exportAnimations(true),
    normalizeMeshScale(false)
{
    //empty
}

//------------------------------------------------------------------------------
/**
*/
nMaxExportTask::~nMaxExportTask()
{
    //empty
}

//------------------------------------------------------------------------------
/**
*/
bool
nMaxExportTask::ReadConfig()
{
    nFile *file = nFileServer2::Instance()->NewFileObject();
    if (!file->Open(this->cfgFileName.Get(), "r"))
    {
        return false;
    }

    char hd[512];
    nString s;

    file->GetS(hd, 512);
    s = hd; s.Strip("\r\n"); 
    this->homeDir = s.Get();
    
    file->GetS(hd, 512);
    s = hd; s.Strip("\r\n"); 
    this->binaryPath = s.Get();
    
    file->GetS(hd, 512);
    s = hd; s.Strip("\r\n"); 
    if (s == nString("1"))
        this->exportAnimations = true;
    else
        this->exportAnimations = false;
    
    file->GetS(hd, 512);
    s = hd; s.Strip("\r\n"); 
    if (s == nString("1"))
        this->normalizeMeshScale = true;
    else
        this->normalizeMeshScale = false;

    file->GetS(hd, 512); 
    s = hd; s.Strip("\r\n");
    this->animsAssign = s.Get();

    file->GetS(hd, 512);
    s = hd; s.Strip("\r\n");
    this->animsPath = s.Get();

    file->GetS(hd, 512); 
    s = hd; s.Strip("\r\n");
    this->gfxlibAssign = s.Get();

    file->GetS(hd, 512); 
    s = hd; s.Strip("\r\n");
    this->gfxlibPath = s.Get();

    file->GetS(hd, 512); 
    s = hd; s.Strip("\r\n");
    this->guiAssign = s.Get();
    
    file->GetS(hd, 512); 
    s = hd; s.Strip("\r\n");
    this->guiPath = s.Get();

    file->GetS(hd, 512); 
    s = hd; s.Strip("\r\n");
    this->lightsAssign = s.Get();

    file->GetS(hd, 512); 
    s = hd; s.Strip("\r\n");
    this->lightsPath = s.Get();

    file->GetS(hd, 512); 
    s = hd; s.Strip("\r\n");
    this->meshesAssign = s.Get();

    file->GetS(hd, 512); 
    s = hd; s.Strip("\r\n");
    this->meshesPath = s.Get();

    file->GetS(hd, 512); 
    s = hd; s.Strip("\r\n");
    this->shadersAssign = s.Get();

    file->GetS(hd, 512); 
    s = hd; s.Strip("\r\n");
    this->shadersPath = s.Get();

    file->GetS(hd, 512); 
    s = hd; s.Strip("\r\n");
    this->texturesAssign = s.Get();

    file->GetS(hd, 512); 
    s = hd; s.Strip("\r\n");
    this->texturesPath = s.Get();

    file->Close();
    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
nMaxExportTask::WriteConfig()
{
    nFile *file;

    file = nFileServer2::Instance()->NewFileObject();
    if (!file->Open(this->cfgFileName.Get(), "w"))
    {
        return false;
    }

    file->PutS(this->homeDir.Get()); file->PutS("\n");
    file->PutS(this->binaryPath.Get()); file->PutS("\n");

    if (this->exportAnimations) file->PutS("1");
    else file->PutS("0");
    file->PutS("\n");
    
    if (this->normalizeMeshScale) file->PutS("1");
    else file->PutS("0");
    file->PutS("\n");

    file->PutS(this->animsAssign.Get()); file->PutS("\n");
    file->PutS(this->animsPath.Get()); file->PutS("\n");

    file->PutS(this->gfxlibAssign.Get()); file->PutS("\n");
    file->PutS(this->gfxlibPath.Get()); file->PutS("\n");

    file->PutS(this->guiAssign.Get()); file->PutS("\n");
    file->PutS(this->guiPath.Get()); file->PutS("\n");

    file->PutS(this->lightsAssign.Get()); file->PutS("\n");
    file->PutS(this->lightsPath.Get()); file->PutS("\n");

    file->PutS(this->meshesAssign.Get()); file->PutS("\n");
    file->PutS(this->meshesPath.Get()); file->PutS("\n");

    file->PutS(this->shadersAssign.Get()); file->PutS("\n");
    file->PutS(this->shadersPath.Get()); file->PutS("\n");

    file->PutS(this->texturesAssign.Get()); file->PutS("\n");
    file->PutS(this->texturesPath.Get()); file->PutS("\n");

    file->Close();
    return true;
}

