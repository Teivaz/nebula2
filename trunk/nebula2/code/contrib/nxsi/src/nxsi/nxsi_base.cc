//-----------------------------------------------------------------------------
// Copyright (c) Ville Ruusutie, 2004. All Rights Reserved.
//-----------------------------------------------------------------------------
// See the file "nxsi_license.txt" for information on usage and redistribution
// of this file, and for a DISCLAIMER OF ALL WARRANTIES.
//-----------------------------------------------------------------------------
// nXSI base handling functions
//-----------------------------------------------------------------------------
#include "nxsi/nxsi.h"
#include <scene/nlightnode.h>
#include <scene/ntransformnode.h>

//-----------------------------------------------------------------------------

void nXSI::HandleSIModel(CSLModel* templ)
{
    CSLModel* *childList = templ->GetChildrenList();
    nString animName(templ->Name().GetText());

    if (templ != this->xsiScene.Root())
    {
        if (!(templ->Name() == "MAXSceneRoot"))
        {
            // check if node is visible
            CSLVisibility* visibility = templ->Visibility();
            if (visibility && (visibility->GetVisibility() == false))
            {
                // node is invisible so dont't dump it and it's childrens
                return;
            }
        }

        switch (templ->GetPrimitiveType()) {
            case CSLTemplate::SI_NULL_OBJECT:
            case CSLTemplate::SI_MODEL:
                {
                    // get transform information
                    vector3 position = (vector3&)templ->Transform()->GetTranslation();
                    vector3 rotation = (vector3&)templ->Transform()->GetEulerRotation();
                    vector3 scale    = (vector3&)templ->Transform()->GetScale();
                    VECTOR3_DEG2RAD(rotation);

                    // add script commands
                    nTransformNode* newNode = (nTransformNode*)this->kernelServer.New("ntransformnode", templ->Name().GetText());
                    this->kernelServer.PushCwd(newNode);
                    newNode->SetPosition(position);
                    newNode->SetEuler(rotation);
                    newNode->SetScale(scale);

                    // build transform animation
                    if (this->BuildTransformAnimation(templ->Transform(), animName))
                    {
                        newNode->AddAnimator(animName.Get());
                    }

                    // handle child models
                    for (int i = 0; i < templ->GetChildrenCount(); i++) {
                        this->HandleSIModel(childList[i]);
                    }

                    // add script commands
                    this->kernelServer.PopCwd();
                } break;
            case CSLTemplate::SI_MESH:
                this->HandleSIMesh((CSLMesh*)templ->Primitive());
                break;
            case CSLTemplate::SI_INFINITE_LIGHT:
            case CSLTemplate::SI_DIRECTIONAL_LIGHT:
            case CSLTemplate::SI_POINT_LIGHT:
            case CSLTemplate::SI_SPOT_LIGHT:
                this->HandleSILight((CSLLight*)templ->Primitive());
                break;
            case CSLTemplate::SI_IK_ROOT:
            case CSLTemplate::SI_IK_JOINT:
            case CSLTemplate::SI_IK_EFFECTOR:
                // do nothing
                break;
            case CSLTemplate::SI_CAMERA:
                this->HandleSICamera((CSLCamera*)templ->Primitive());
                break;
            default:
                n_printf("WARNING: found unknown model (%s)\n", templ->Name().GetText());
                return;
        }
    } else {
        // handle child models
        for (int i = 0; i < templ->GetChildrenCount(); i++) {
            HandleSIModel(childList[i]);
        }
    }
}

void nXSI::HandleSICamera(CSLCamera* templ)
{
/*  vector3 position    = GetVector3(templ, SI_CAMERA_POSX);
    vector3 interest    = GetVector3(templ, SI_CAMERA_INTX);
    float roll          = GetFloat(templ, SI_CAMERA_ROLL);
    float fov           = GetFloat(templ, SI_CAMERA_FOV);
    float aspect        = 4.f / 3.f;
    float near_plane    = GetFloat(templ, SI_CAMERA_NEAR_PLANE);
    float far_plane     = GetFloat(templ, SI_CAMERA_FAR_PLANE);

    position += transform.m_position;

    CSIBCString camera_name = templ->InstanceName();
    this->scriptFile.InsertLine("new ncamera %s", camera_name.GetText());
    this->scriptFile.AddSpacing(1);
    this->scriptFile.InsertLine("sel %s", camera_name.GetText());
    this->scriptFile.InsertLine(".setperspective %.6f %.6f %.6f %.6f", fov, aspect, near_plane, far_plane);
    this->scriptFile.InsertLine(".setposition %.6f %.6f %.6f", position.x, position.y, position.z);
    this->scriptFile.InsertLine(".seteuler %.6f %.6f %.6f", transform.m_rotation.x, transform.m_rotation.y, transform.m_rotation.z);
    this->scriptFile.InsertLine(".setscale %.6f %.6f %.6f", transform.m_scale.x, transform.m_scale.y, transform.m_scale.z);
    this->scriptFile.InsertLine(".setinterest %.6f %.6f %.6f", interest.x, interest.y, interest.z);

    this->scriptFile.AddSpacing(-1);
    this->scriptFile.InsertLine("sel ..");
*/
    n_printf("WARNING: currently doesn't handle cameras (%s)\n", templ->Name().GetText());
}

void nXSI::HandleSILight(CSLLight* templ)
{
    vector3 position;
    vector3 euler_rotation;
    vector4 ambient(0.3f, 0.3f, 0.3f, 1.0f);
    vector4 diffuse(0.7f, 0.7f, 0.7f, 1.0f);
    vector4 specular(1.0f, 1.0f, 1.0f, 1.0f);

    switch (templ->Type()) {
        case CSLTemplate::SI_INFINITE_LIGHT:
            n_printf("WARNING: currently doesn't handle infinite lights (%s)\n", templ->Name().GetText());
            break;
        case CSLTemplate::SI_DIRECTIONAL_LIGHT:
            n_printf("WARNING: currently doesn't handle directional lights (%s)\n", templ->Name().GetText());
            break;

        case CSLTemplate::SI_POINT_LIGHT:
        {
            CSLPointLight* pointLight = (CSLPointLight*)templ;
            position = (vector3&)(pointLight->GetPosition());
            diffuse = (vector4&)pointLight->GetColor();

            nLightNode* lightNode = (nLightNode*)this->kernelServer.New("nlightnode", pointLight->Name().GetText());
            lightNode->SetPosition(position);
            lightNode->SetVector(nShaderState::LightAmbient, ambient);
            lightNode->SetVector(nShaderState::LightAmbient1, ambient);
            lightNode->SetVector(nShaderState::LightDiffuse, diffuse);
            lightNode->SetVector(nShaderState::LightDiffuse1, diffuse);
            lightNode->SetVector(nShaderState::LightSpecular, specular);
            lightNode->SetVector(nShaderState::LightSpecular1, specular);
        } break;

        case CSLTemplate::SI_SPOT_LIGHT:
            CSLSpotLight* spotLight = (CSLSpotLight*)templ;
/*
            nLightNode* lightNode = (nLightNode*)this->kernelServer.New("nlightnode", spotLight->Name().GetText());
            this->kernelServer.SetCwd(lightNode);
            lightNode->SetPosition(position);
            lightNode->SetPosition(eulerRotation);
            lightNode->SetOrthogonal(1000, 1000, 0.25, 175);
            lightNode->SetVector(nShaderState::LightAmbient, ambient);
            lightNode->SetVector(nShaderState::LightAmbient1, ambient);
            lightNode->SetVector(nShaderState::LightDiffuse, diffuse);
            lightNode->SetVector(nShaderState::LightDiffuse1, diffuse);
            lightNode->SetVector(nShaderState::LightSpecular, specular);
            lightNode->SetVector(nShaderState::LightSpecular1, specular);
            lightNode->SetTexture(nShaderState::LightModMap, "textures:system/white.dds");
*/
            n_printf("WARNING: currently doesn't handle spot lights (%s)\n", templ->Name().GetText());
            break;
    }
}

//-----------------------------------------------------------------------------
// Eof