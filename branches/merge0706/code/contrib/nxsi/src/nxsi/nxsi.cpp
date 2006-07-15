//-----------------------------------------------------------------------------
// Copyright (c) Ville Ruusutie, 2004. All Rights Reserved.
//-----------------------------------------------------------------------------
// See the file "nxsi_license.txt" for information on usage and redistribution
// of this file, and for a DISCLAIMER OF ALL WARRANTIES.
//-----------------------------------------------------------------------------
#include "nxsi/nxsi.h"
#include <kernel/nfileserver2.h>

//-----------------------------------------------------------------------------
// used packages

nNebulaUsePackage(nnebula);

//-----------------------------------------------------------------------------

nXSI::nXSI(int argc, char *argv[])
{
    this->options.Parse(argc, argv);
}

nXSI::~nXSI()
{
}

bool nXSI::Export()
{
    // set packages
    this->kernelServer.AddPackage(nnebula);

    // init all necessary for script export
    this->scriptServer = (nScriptServer*)this->kernelServer.New(this->options.GetScriptServerName().Get(), "/sys/servers/script");
    this->variableServer = (nVariableServer*)this->kernelServer.New("nvariableserver", "/sys/servers/variable");
    nRoot* sceneRoot = this->kernelServer.New("ntransformnode", "/usr/scene");
    this->kernelServer.SetCwd(sceneRoot);

    // init xsi parser
    if (!this->InitXSIParser()) return false;

    // init group ids
    this->meshGroupId = 0;
    this->animGroupId = 0;

    // process scene
    this->HandleSIModel(this->xsiScene.Root());

    // save merged meshes
    if ((this->options.GetOutputFlags() & nXSIOptions::OUTPUT_MESH) &&
        (this->options.GetOutputFlags() & nXSIOptions::OUTPUT_MERGEALL) &&
        (this->meshGroupId > 0))
    {
        this->meshBuilder.BuildTriangleTangents();
        this->meshBuilder.BuildVertexTangents();
        this->meshBuilder.Cleanup(0);
        this->meshBuilder.Optimize();
        this->meshBuilder.Save(nFileServer2::Instance(), this->options.GetMeshFilename().Get());
        n_printf("merged meshes saved: %s\n", this->options.GetMeshFilename().Get());
    }

    // save merged anims
    if ((this->options.GetOutputFlags() & nXSIOptions::OUTPUT_ANIM) &&
        (this->animGroupId > 0))
    {
        this->animBuilder.Optimize();
        this->animBuilder.FixKeyOffsets();
        this->animBuilder.Save(nFileServer2::Instance(), this->options.GetAnimFilename().Get());
        n_printf("merged anims saved: %s\n", this->options.GetAnimFilename().Get());
    }

    // save script
    if (this->options.GetOutputFlags() & nXSIOptions::OUTPUT_SCRIPT)
    {
        sceneRoot->SaveAs(this->options.GetScriptFilename().Get());
        n_printf("creation script saved: %s\n", this->options.GetScriptFilename().Get());
    }

    // print empty line
    n_printf("\n");

    // uninit xsi parser
    this->xsiScene.Close();

    return true;
}

//-----------------------------------------------------------------------------

bool nXSI::InitXSIParser()
{
    if (this->xsiScene.Open((char*)this->options.GetXSIFilename().Get()) == SI_SUCCESS)
    {
        this->xsiScene.Read();
        
        int minor = this->xsiScene.Parser()->GetdotXSIFileVersionMinor();
        int major = this->xsiScene.Parser()->GetdotXSIFileVersionMajor();

        int version = (major * 100) + minor;
        if (version >= 300)
        {
            return true;
        }
        else
        {
            this->xsiScene.Close();
            n_error("ERROR: legacy xsi fileformat (%i.%i). Currently only supports 3.00+ files.\n", major, minor);
            return false;
        }
    }
    n_error("ERROR: Failed to open xsi file.\n");
    return false;
}

//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    nXSI m(argc, argv);
    return m.Export();
}

//-----------------------------------------------------------------------------
// Eof
