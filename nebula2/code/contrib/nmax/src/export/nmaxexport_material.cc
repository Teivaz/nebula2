#include "export/nmaxexport.h"
//------------------------------------------------------------------------------
/**
    3ds max to Nebula2 exporter using the IGame lib.

    This file is licensed under the terms of the Nebula License.
    (C) 2004 Johannes Kellner
*/


//------------------------------------------------------------------------------
/**
    Setup the provided nMaterialNode as defined in the IGameMaterial

    @param  materialNode    the nebula material node to setup
    @param  nodeName        the nohPath of the material node, to be able to create child nodes
    @param  igMaterial      the IGame material node which should be exported

    @todo only default max materials are exported
    @todo only the uv channel 0 is used (because no information from shader is available what texture should use what uv channel)
    @todo find a calculation that includes the specular power value
    @todo find the right value to scale the specular glossiness from 0-1 to nebula values
    @todo transfer the texture to nebula, maybe convert them on runtime.
*/
void
nMaxExport::exportMaterial(nMaterialNode* materialNode, nString nodeName, IGameMaterial* igMaterial, bool skinned)
{
    bool hasDiffuseMap = false;
    bool hasBumpMap = false;

    for (int texIdx = 0; texIdx < igMaterial->GetNumberOfTextureMaps(); texIdx++)
    {
        IGameTextureMap* igTexMap = igMaterial->GetIGameTextureMap(texIdx);
        n_assert(igTexMap);
        
        if (igTexMap->IsEntitySupported())
        {
            nShaderState::Param shaderParam = nShaderState::InvalidParameter;
        
            //FIXME: only the map channel 0 will be used!
            switch (igTexMap->GetStdMapSlot())
            {
                case ID_AM: //Ambient
                {
                    shaderParam = nShaderState::AmbientMap0;
                }
                break;
                case ID_DI: //Diffuse
                {
                    shaderParam = nShaderState::DiffMap0;
                    hasDiffuseMap = true;
                }
                break;
                case ID_SP: //Specular
                {
                    shaderParam = nShaderState::SpecMap0;
                }
                break;
                case ID_BU: //Bump
                {
                    shaderParam = nShaderState::BumpMap0;
                    hasBumpMap = true;
                }
                break;
                case ID_OP: //Opacity
                case ID_FI: //Filter color
                case ID_SH: //Shininess. In R3 and later this is called Glossiness.
                case ID_SS: //Shininess strength. In R3 and later this is called Specular Level.
                case ID_SI: //Self-illumination
                case ID_RL: //Reflection
                case ID_RR: //Refraction
                case ID_DP: //Displacement
                break;
            }

            if (shaderParam != nShaderState::InvalidParameter) //does we have found the slot type?
            {
                TCHAR* bitmapFileName = igTexMap->GetBitmapFileName();
                if (bitmapFileName)
                {
                    nPathString texFile(bitmapFileName);
                    texFile = texFile.ExtractFileName().Get();
                    texFile = this->task->texturesPath.Get() + texFile;
                    materialNode->SetTexture(shaderParam, texFile.Get());

                    nFileServer2 *fileServer = nFileServer2::Instance();
                    nString pathTo(fileServer->ManglePath(texFile.Get()));
                    fileServer->CopyFile(bitmapFileName, pathTo.Get());
                }
                else
                {
                    n_printf("ERROR: '%s': Param: '%i' - Could not get the bitmap filename!\n", igTexMap->GetTextureName(), shaderParam);
                }
            }
            else
            {
                n_printf("ERROR: Texture: %s, Slot Type (%i) not supported.\n", igTexMap->GetTextureName(), igTexMap->GetStdMapSlot());
            }
        }
        else
        {
            n_printf("FIXME: Texture: '%s' can not be handled direct by IGame.\n", igTexMap->GetTextureName());
        }
    }    
    
    //alpha value for all 3 material colors
    if (igMaterial->GetOpacityData() && igMaterial->GetOpacityData()->GetType() == IGAME_FLOAT_PROP)
    {
        float opacity;
        if (igMaterial->GetOpacityData()->GetPropertyValue(opacity))
        {
            //ambient
            if (igMaterial->GetAmbientData() && igMaterial->GetAmbientData()->GetType() == IGAME_POINT3_PROP)
            {
                Point3 ambientColor;
                if (igMaterial->GetAmbientData()->GetPropertyValue(ambientColor))
                {
                    vector4 vector(ambientColor.x, ambientColor.y, ambientColor.z, opacity);
                    materialNode->SetVector(nShaderState::MatAmbient, vector);
                }
            }
            
            //diffuse
            if (igMaterial->GetDiffuseData() && igMaterial->GetDiffuseData()->GetType() == IGAME_POINT3_PROP)
            {
                Point3 diffuseColor;
                if (igMaterial->GetDiffuseData()->GetPropertyValue(diffuseColor))
                {
                    vector4 vector(diffuseColor.x, diffuseColor.y, diffuseColor.z, opacity);
                    materialNode->SetVector(nShaderState::MatDiffuse, vector);
                }
            }

            //specular
            if (igMaterial->GetSpecularData() && igMaterial->GetSpecularData()->GetType() == IGAME_POINT3_PROP)
            {
                Point3 specularColor;
                if(igMaterial->GetSpecularData()->GetPropertyValue(specularColor))
                {
                    vector4 vector(specularColor.x, specularColor.y, specularColor.z, opacity);
                    materialNode->SetVector(nShaderState::MatSpecular, vector);
                    
                    //specular power
                    //FIXME: find a calculation that includes the specular power
                    /*if (igMaterial->GetSpecularLevelData() && igMaterial->GetSpecularLevelData()->GetType() == IGAME_FLOAT_PROP)
                    {
                        float specularPower = 32.0f;
                        if (igMaterial->GetSpecularLevelData()->GetPropertyValue(specularPower))
                        {
                            this->writeToFile(sceneFile, tab, ".setfloat \"MatSpecularPower\" %f\n", specularPower);
                        }
                    }
                    */
                    //specular glossiness
                    if (igMaterial->GetGlossinessData() && igMaterial->GetGlossinessData()->GetType() == IGAME_FLOAT_PROP)
                    {
                        float glossiness;
                        if (igMaterial->GetGlossinessData()->GetPropertyValue(glossiness))
                        {
                            //the value from max is 0..1 so we must scale this up to the nebula range
                            materialNode->SetFloat(nShaderState::MatSpecularPower, 255.0f * glossiness);
                        }
                    }
                }
            }
        }
    }

    //fillup missing map with system defaults
    if (!hasDiffuseMap)
    {
        materialNode->SetTexture(nShaderState::DiffMap0, "textures:system/white.dds");
    }

    if (!hasBumpMap)
    {
        materialNode->SetTexture(nShaderState::BumpMap0, "textures:system/nobump.tga");
    }
    
    nString shdFile(this->task->shadersPath);
    //set the default shader
    if (skinned)
    {
        shdFile += "default_skinned.fx";
        materialNode->SetShader(materialNode->StringToFourCC("colr"), shdFile.Get());
    }
    else
    {
        shdFile += "default.fx";
        materialNode->SetShader(materialNode->StringToFourCC("colr"), shdFile.Get());
    }
}
