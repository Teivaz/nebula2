//-----------------------------------------------------------------------------
// Copyright (c) Ville Ruusutie, 2004. All Rights Reserved.
//-----------------------------------------------------------------------------
// See the file "nxsi_license.txt" for information on usage and redistribution
// of this file, and for a DISCLAIMER OF ALL WARRANTIES.
//-----------------------------------------------------------------------------
#include "nxsi/nxsi.h"
#include <kernel/nfileserver2.h>
#include <iostream>

using std::cerr;

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

    // save merged mesh
    if ((this->options.GetOutputFlags() & nXSIOptions::OUTPUT_MESH) &&
        (this->options.GetOutputFlags() & nXSIOptions::OUTPUT_MERGEALL) &&
        (this->meshGroupId > 0))
    {
        this->meshBuilder.BuildTriangleTangents();
        this->meshBuilder.BuildVertexTangents();
        this->meshBuilder.Cleanup(0);
        this->meshBuilder.Optimize();
        this->meshBuilder.Save(nFileServer2::Instance(), this->options.GetMeshFilename().Get());
        cerr << "merged mesh saved: " << this->options.GetMeshFilename().Get() << "\n";
    }

    // save merged anim
    if ((this->options.GetOutputFlags() & nXSIOptions::OUTPUT_ANIM) &&
        (this->options.GetOutputFlags() & nXSIOptions::OUTPUT_MERGEALL) &&
        (this->animGroupId > 0))
    {
        this->animBuilder.Optimize();
        this->animBuilder.Save(nFileServer2::Instance(), this->options.GetAnimFilename().Get());
        cerr << "merged animation saved: " << this->options.GetAnimFilename().Get() << "\n";
    }

    // save script
    if (this->options.GetOutputFlags() & nXSIOptions::OUTPUT_SCRIPT)
    {
        sceneRoot->SaveAs(this->options.GetScriptFilename().Get());
        cerr << "creation script saved: " << this->options.GetScriptFilename().Get() << "\n";
    }

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
            cerr << "ERROR: legacy xsi fileformat (" << major << "." << minor << "). Currently only supports 3.00+ files.\n";
            this->xsiScene.Close();
            return false;
        }
    }

    cerr << "ERROR: Failed to open xsi file.\n";
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