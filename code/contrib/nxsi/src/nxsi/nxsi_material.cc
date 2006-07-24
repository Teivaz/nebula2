//-----------------------------------------------------------------------------
// Copyright (c) Ville Ruusutie, 2004. All Rights Reserved.
//-----------------------------------------------------------------------------
// See the file "nxsi_license.txt" for information on usage and redistribution
// of this file, and for a DISCLAIMER OF ALL WARRANTIES.
//-----------------------------------------------------------------------------
// nXSI handle functions
//-----------------------------------------------------------------------------
#include "nxsi/nxsi.h"
#include <scene/nshapenode.h>
#include <gfx2/nshader2.h>
#include <iostream>

using std::cerr;

//-----------------------------------------------------------------------------

void nXSI::HandleXSIMaterialVariables(CSLXSIMaterial* material, nShapeNode* node, bool isSkinned)
{
//  CSLShaderConnectionPoint** shaderConnections;
    CSLVariantParameter** paramList;
//  int shaderConnectionCount;
    int paramCount;
    CSIBCString variableName;
    vector4 color;

    CSIBCString surfaceStr("surface");
    CSIBCString realtimeStr("RealTime");
    CSIBCString previousStr("previous");
    CSIBCString textureStr("Texture");
    CSIBCString diffuseStr("diffuse");
    CSIBCString directxStr("Softimage.DXDraw.1");

    // try to find realtime shader
    CSLConnectionPoint** connectionList = material->GetConnectionPointList();
    CSLConnectionPoint* connection = 0;
    for (int i = 0; i < material->GetConnectionPointCount(); i++)
    {
        if (realtimeStr == connectionList[i]->GetName())
        {
            connection = connectionList[i];
            break;
        }
    }
    // check if realtime shader found.
    if (false && connection && (connection->GetShader()) && (directxStr == connection->GetShader()->GetProgID()))
    {
/*      CSLXSIShader* shader = connection->GetShader();
        CSIBCString shaderFilename = shader->Name();
        char* extension = 0;

        // check shader filename
        if ((extension = strstr(shaderFilename.GetText(), "_fx"))) {
            *extension = '.';
        } else {
            shaderFilename.Concat(".fx");
        }

        // find variables
        while (shader && (shader->GetConnectionPointCount() > 0))
        {
            shaderConnections = shader->GetConnectionPointList();
            shaderConnectionCount = shader->GetConnectionPointCount();
            CSIBCString shaderType(shader->GetProgID());

            if (shaderType == "Softimage.DXHLSLColor.1")
            {
                // handle color variable
                paramCount = shader->GetParameterCount();
                paramList = shader->GetParameterList();

                for (int p = 0; p < paramCount; p++)
                {
                    CSIBCString param_name(paramList[p]->GetName());
                    if (param_name == "Constant_Register")  variableName = paramList[p]->GetValue()->p_cVal;
                    else if (param_name == "Constant_Value.red")   color.x = paramList[p]->GetValue()->fVal;
                    else if (param_name == "Constant_Value.green") color.y = paramList[p]->GetValue()->fVal;
                    else if (param_name == "Constant_Value.blue")  color.z = paramList[p]->GetValue()->fVal;
                    else if (param_name == "Constant_Value.alpha") color.w = paramList[p]->GetValue()->fVal;
                }
                node->SetVector(variableName.GetText(), color);
            }
            else if (shaderType == "Softimage.DXTexture.1")
            {
                // handle texture variable
                variableName = shader->Name();
                for (int c = 0; c < shaderConnectionCount; c++)
                {
                    if (textureStr == shaderConnections[c]->GetName())
                    {
                        CSLImage* image = this->xsiScene.GetImageLibrary()->FindImage(shaderConnections[c]->GetImage());
                        if (image)
                        {
                            node->SetTexture(variableName.GetText(), image->GetSourceFile());
                        }
                        break;
                    }
                }
            }

            // find previous shader
            shader = 0;
            for (int c = 0; c < shaderConnectionCount; c++)
            {
                if (previousStr == shaderConnections[c]->GetName())
                {
                    shader = shaderConnections[c]->GetShader();
                    break;
                }
            }
        }

        // add shader filename
        node->SetShader(variableName.GetText(), image->GetSourceFile());
        this->scriptFile.InsertLine(".setshader \"colr\" \"shaders:%s\"", shaderFilename.GetText());
*/  }
    else
    {
        // realtime shader not found. try to find surface shader
        for (int i = 0; i < material->GetConnectionPointCount(); i++)
        {
            if (surfaceStr == connectionList[i]->GetName())
            {
                connection = connectionList[i];
                break;
            }
        }

        // default material colors
        vector4 ambient(0.2f, 0.2f, 0.2f, 1.0f);
        vector4 diffuse(0.6f, 0.6f, 0.6f, 1.0f);
        vector4 specular(0.8f, 0.8f, 0.8f, 1.0f);

        // check if surface shader is found
        if (connection && (connection->GetShader()))
        {
            // surface shader found
            CSLXSIShader* shader = connection->GetShader();
            CSIBCString shaderType(shader->GetProgID());

            if (shaderType == "Softimage.material-phong.1")
            {
                paramCount = shader->GetParameterCount();
                paramList = shader->GetParameterList();

                // scan parameters
                for (int p = 0; p < paramCount; p++)
                {
                    CSIBCString paramName(paramList[p]->GetName());

                    if (paramName == "ambient.red")         ambient.x = paramList[p]->GetValue()->fVal;
                    else if (paramName == "ambient.green")  ambient.y = paramList[p]->GetValue()->fVal;
                    else if (paramName == "ambient.blue")   ambient.z = paramList[p]->GetValue()->fVal;
                    else if (paramName == "ambient.alpha")  ambient.w = paramList[p]->GetValue()->fVal;
                    else if (paramName == "diffuse.red")    diffuse.x = paramList[p]->GetValue()->fVal;
                    else if (paramName == "diffuse.green")  diffuse.y = paramList[p]->GetValue()->fVal;
                    else if (paramName == "diffuse.blue")   diffuse.z = paramList[p]->GetValue()->fVal;
                    else if (paramName == "diffuse.alpha")  diffuse.w = paramList[p]->GetValue()->fVal;
                    else if (paramName == "specular.red")   specular.x = paramList[p]->GetValue()->fVal;
                    else if (paramName == "specular.green") specular.y = paramList[p]->GetValue()->fVal;
                    else if (paramName == "specular.blue")  specular.z = paramList[p]->GetValue()->fVal;
                    else if (paramName == "specular.alpha") specular.w = paramList[p]->GetValue()->fVal;
                }
                // TODO: check if material has diffuse texture
            }
        }

        node->SetVector(nShaderState::MatAmbient, ambient);
        node->SetVector(nShaderState::MatDiffuse, diffuse);
        node->SetVector(nShaderState::MatSpecular, specular);
        node->SetFloat(nShaderState::MatSpecularPower, 32.0f);
        node->SetTexture(nShaderState::DiffMap0, "textures:system/white.dds");
        node->SetTexture(nShaderState::BumpMap0, "textures:system/nobump.dds");

        // add material commands
        if (isSkinned) {
            node->SetShader(MAKE_FOURCC('c','o','l','r'), "shaders:default_skinned.fx");
        } else {
            node->SetShader(MAKE_FOURCC('c','o','l','r'), "shaders:default.fx");
        }
    }
}

void nXSI::HandleSIMaterialVariables(CSLMaterial* material, nShapeNode* node, bool isSkinned)
{
    vector4 ambient       = (vector4&)material->GetAmbientColor();
    vector4 diffuse       = (vector4&)material->GetDiffuseColor();
    vector4 specular      = (vector4&)material->GetSpecularColor();
    CSLTexture2D* texture = material->Texture2D();

    // get texture filename
    CSIBCString textureFilename;
    if (texture)
    {
        // texture found, get filename
        textureFilename = texture->GetImageFileName();
        if (textureFilename == "noIcon.pic")
        {
            // if xsi default texture, use nebula default texture
            textureFilename = "textures:system/white.dds";
        }
    }
    else
    {
        // texture not found, use default texture
        textureFilename = "textures:system/white.dds";
    }

    // set material commands
    node->SetVector(nShaderState::MatAmbient, ambient);
    node->SetVector(nShaderState::MatDiffuse, diffuse);
    node->SetVector(nShaderState::MatSpecular, specular);
    node->SetFloat(nShaderState::MatSpecularPower, 32.0f);
    node->SetTexture(nShaderState::DiffMap0, textureFilename.GetText());
    node->SetTexture(nShaderState::BumpMap0, "textures:system/nobump.dds");
//  node->SetTexture(nShaderState::CubeMap0, "textures:system/normalize.dds");

    if (isSkinned) {
        node->SetShader(MAKE_FOURCC('c','o','l','r'), "shaders:default_skinned.fx");
    } else {
        node->SetShader(MAKE_FOURCC('c','o','l','r'), "shaders:default.fx");
    }
}

//-----------------------------------------------------------------------------
// Eof