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
    texturesPath(N_MAXEXPORT_TEXTURES_PATH)
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

    file->GetS(hd, 512);
    this->homeDir = hd;

    file->GetS(hd, 512); this->animsAssign = hd;
    file->GetS(hd, 512); this->animsPath = hd;

    file->GetS(hd, 512); this->gfxlibAssign = hd;
    file->GetS(hd, 512); this->gfxlibPath = hd;

    file->GetS(hd, 512); this->guiAssign = hd;
    file->GetS(hd, 512); this->guiPath = hd;

    file->GetS(hd, 512); this->lightsAssign = hd;
    file->GetS(hd, 512); this->lightsPath = hd;

    file->GetS(hd, 512); this->meshesAssign = hd;
    file->GetS(hd, 512); this->meshesPath = hd;

    file->GetS(hd, 512); this->shadersAssign = hd;
    file->GetS(hd, 512); this->shadersPath = hd;

    file->GetS(hd, 512); this->texturesAssign = hd;
    file->GetS(hd, 512); this->texturesPath = hd;

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

