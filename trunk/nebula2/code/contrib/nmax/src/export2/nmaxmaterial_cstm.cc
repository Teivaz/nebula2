//-----------------------------------------------------------------------------
//  nmaxmaterial_cstm.cc
//
//  Handles Nebula2 custom material of 3dsmax.
//
//  (c)2004 Kim, Hyoun Woo
//-----------------------------------------------------------------------------
#include "export2/nmax.h"

#include "export2/nmaxmaterial.h"
#include "export2/nmaxinterface.h"
#include "export2/nmaxtexture.h"
#include "pluginlibs/nmaxdlg.h"
#include "pluginlibs/nmaxlogdlg.h"
#include "export2/nmaxcontrol.h"
#include "export2/nmaxintanimator.h"
#include "export2/nmaxfloatanimator.h"
#include "export2/nmaxvectoranimator.h"

#include "scene/nshapenode.h"
#include "scene/nintanimator.h"
#include "scene/nvectoranimator.h"
#include "scene/nfloatanimator.h"

// the followings are 3dmax predefined custom attribute.
#define NUM_PREDEFINED_CA  3
const char* predefinedCA[NUM_PREDEFINED_CA] = {
    "Viewport Manager",
    "DirectX Manager",
    "mental ray: material custom attribute"
};

//-----------------------------------------------------------------------------
/**
    Check the given name of the custom attribute is predefined standard 
    custom attribute of 3dsmax or not.
    
    @param caName custom attribute name.
*/
static
bool IsPredefinedCA(TCHAR* caName)
{
    for (uint i=0; i<NUM_PREDEFINED_CA; i++)
    {
        if (n_stricmp (caName, predefinedCA[i]) == 0)
            return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
/**
    Retrieves material values from Mtl's parameter block's parameters then
    specifies it to the given shape node.

    @param mtl pointer to the Mtl which contains material of the node.
    @param shapeNode pointer to the nebula shape node to be specified its material.
*/
void nMaxMaterial::GetNebulaMaterial(Mtl* mtl, nShapeNode* shapeNode)
{
    if (NULL == mtl || NULL == shapeNode)
        return;

    ICustAttribContainer* caContainer = mtl->GetCustAttribContainer();
    if (caContainer)
    {
        // we assume that there's only one custom attribute.
        int numAttribs = caContainer->GetNumCustAttribs();

        CustAttrib* custAttr = 0;

        for (int ca=0; ca<numAttribs; ca++)
        {
            custAttr = (CustAttrib*)caContainer->GetCustAttrib(ca);

            if (custAttr)
            {
                // we only deal with nebula2 custom material's CA. skip others.
                TCHAR* caName = custAttr->GetName();
                if (IsPredefinedCA(caName))
                    continue;

                shapeNode->SetMayaShaderName(caName);

                int numParamBlocks = custAttr->NumParamBlocks();

                for (int i=0; i<numParamBlocks; i++)
                {
                    // NOTE: the only valid parameter block which can be exported is 
                    //       the custom attribute's one but we process it with general way.
                    IParamBlock2* pblock2 = custAttr->GetParamBlock(i);

                    // get the number of the params.
                    int numParams = pblock2->NumParams();

                    nShaderState::Param shaderParam;
                    int paramType;

                    for (int j=0; j<numParams; j++)
                    {
                        ParamBlockDesc2* pblockDesc = pblock2->GetDesc();
                        if (NULL == pblockDesc)
                            return;

                        ParamID paramID = pblockDesc->IndextoID(j);
                        ParamDef& paramDef = pblockDesc->GetParamDef(paramID);

                        TCHAR* name = paramDef.int_name;
                        
                        // find nShaderState::Param which match to parameter name.
                        shaderParam = this->GetShaderParam(name);

                        // filter it if the given parameter name does not match
                        // to nebula's shader state parameter.
                        //if (shaderParam == nShaderState::InvalidParameter)
                        //    continue;

                        paramType = pblock2->GetParameterType(j);

                        switch(paramType)
                        {
                        case TYPE_STRING:
                            {
                                //HACK: string type used only for effect filename at the present.

                                BOOL result;

                                // Shader name
                                TCHAR* value; 
                                Interval interval;
                                result = pblock2->GetValue(j, 0, value, interval);

                                if (result)
                                {
                                    // is this parameter for 'shader'?
                                    if (strcmp(name, "Shader") == 0)
                                    {
                                        //FIXME: other type of FourCC should be available.
                                        shapeNode->SetShader(value);
                                    }
                                    else
                                    {
                                        // HACK: we just pass it if the parameter block contains 'dirSetting' for its name.
                                        if (strstr(name, "dirSetting") == 0 )
                                        {
                                            n_maxlog(Error, "The parameter block has string type parameter but it is not effect filename.");
                                        }
                                    }
                                }
                                else
                                {
                                    n_maxlog(Error, "Failed to retrieve the value of the parameter %s.", name);
                                }
                            }
                            break;

                        case TYPE_FLOAT:
                            {
                                BOOL result;
                                float value;
                                Interval interval;
                                result = pblock2->GetValue(j, 0, value, interval);

                                if (result)
                                    shapeNode->SetFloat(shaderParam, value);
                                else
                                    n_maxlog(Error, "Failed to retrieves the value of the parameter %s.", name);

                                Control* control = pblock2->GetController(paramID);
                                if (control)
                                {
                                    // the parameter was animated.
                                    nFloatAnimator* animator = 0;

                                    nMaxFloatAnimator floatAnimator;
                                    animator = static_cast<nFloatAnimator*>(floatAnimator.Export(name, control));
                                    if (animator)
                                    {
                                        // add the animator to the shapenode.
                                        shapeNode->AddAnimator(animator->GetName());

                                        nKernelServer::Instance()->PopCwd();
                                    }
                                    else
                                    {
                                        n_maxlog(Error, "Failed to create nFloatAnimator for the parameter %s", name);
                                    }
                                }

                            }
                            break;

                        case TYPE_BOOL:
                            {
                                BOOL result;
                                int value;
                                Interval interval;
                                result = pblock2->GetValue(j, 0, value, interval);

                                if (result)
                                    shapeNode->SetBool(shaderParam, ((value > 0) ? true : false));
                                else
                                    n_maxlog(Error, "Failed to retrieves the value of the parameter %s.", name);
                            }
                            break;

                        case TYPE_INT:
                            {
                                BOOL result;
                                int value;
                                Interval interval;
                                result = pblock2->GetValue(j, 0, value, interval);

                                if (result)
                                    shapeNode->SetInt(shaderParam, value);
                                else
                                    n_maxlog(Error, "Failed to retrieves the value of the parameter %s.", name);

                                Control* control = pblock2->GetController(paramID);
                                if (control)
                                {
                                    // the parameter was animated.
                                    nIntAnimator* animator = 0;

                                    nMaxIntAnimator intAnimator;
                                    animator = static_cast<nIntAnimator*>(intAnimator.Export(name, control));
                                    if (animator)
                                    {
                                        // add the animator to the shapenode.
                                        shapeNode->AddAnimator(animator->GetName());

                                        nKernelServer::Instance()->PopCwd();
                                    }
                                    else
                                    {
                                        n_maxlog(Error, "Failed to create nIntAnimator for the parameter %s", name);
                                    }
                                }
                            }
                            break;

                        case TYPE_RGBA:
                            {
                                BOOL result0, result1;
                                Color value;
                                Interval interval;
                                result0 = pblock2->GetValue(j, 0, value, interval);

                                // Get alpha value of the Color.
                                // The next parameter of the Color type always should be alpha which type is float.
                                float alpha;
                                int alphaIndex = j + 1; // next param index of the color.
                                if (alphaIndex < numParams)
                                {
                                    int type = pblock2->GetParameterType(alphaIndex);
                                    if (type == TYPE_FLOAT)
                                    {
                                        result1 = pblock2->GetValue(alphaIndex, 0, alpha, interval);
                                    }
                                }
                                
                                vector4 color;
                                color.set(value.r, value.g, value.b, alpha);
                                ClampColor(color);

                                if (result0 && result1)
                                    shapeNode->SetVector(shaderParam, color);
                                else
                                    n_maxlog(Error, "Failed to retrieves the value of the parameter %s.", name);

                                Control* control = pblock2->GetController(paramID);
                                if (control)
                                {
                                    n_maxlog(Warning, "The animation of rgba type of parameter %s is not supported.", name);
                                }
                            }
                            break;

                    // The following types are Available in 3dsmax6 or higher. 
                    // See '$3dsmax/maxsdk/include/paramtype.h' for more details.
                    #if MAX_RELEASE >= 6000
                        case TYPE_FRGBA:
                            {
                                BOOL result;
                                AColor value;
                                Interval interval;
                                result = pblock2->GetValue(j, 0, value, interval);

                                vector4 color;
                                color.set(value.r, value.g, value.b, value.a);
                                ClampColor(color);

                                if (result)
                                    shapeNode->SetVector(shaderParam, color);
                                else
                                    n_maxlog(Error, "Failed to retrieves the value of the parameter %s.", name);

                                Control* control = pblock2->GetController(paramID);
                                if (control)
                                {
                                    // the parameter was animated.
                                    nVectorAnimator* animator = 0;

                                    nMaxVectorAnimator vectorAnimator;
                                    animator = static_cast<nVectorAnimator*>(vectorAnimator.Export(name, control));
                                    if (animator)
                                    {
                                        // add the animator to the shapenode.
                                        shapeNode->AddAnimator(animator->GetName());

                                        nKernelServer::Instance()->PopCwd();
                                    }
                                    else
                                    {
                                        n_maxlog(Error, "Failed to create nVectorAnimator %s", animator->GetName());
                                    }
                                }
                            }
                            break;

                        case TYPE_POINT4:
                            {
                                BOOL result;
                                Point4 pt4;
                                Interval interval;
                                result = pblock2->GetValue(j, 0, pt4, interval);

                                vector4 value;
                                value.set(pt4.x, pt4.y, pt4.z, pt4.w);
                                if (result)
                                    shapeNode->SetVector(shaderParam, value);
                                else
                                    n_maxlog(Error, "Failed to retrieves the value of the parameter %s.", name);

                                Control* control = pblock2->GetController(paramID);
                                if (control)
                                {
                                    // the parameter was animated.
                                    nVectorAnimator* animator = 0;

                                    nMaxVectorAnimator vectorAnimator;
                                    animator = static_cast<nVectorAnimator*>(vectorAnimator.Export(name, control));
                                    if (animator)
                                    {
                                        // add the animator to the shapenode.
                                        shapeNode->AddAnimator(animator->GetName());

                                        nKernelServer::Instance()->PopCwd();
                                    }
                                    else
                                    {
                                        n_maxlog(Error, "Failed to create nVectorAnimator %s", animator->GetName());
                                    }
                                }
                            }
                            break;
                    #endif // MAX_RELEASE >= 6000

                        case TYPE_TEXMAP:
                            {
                                BOOL result0, result1;
                                Texmap* value;
                                TCHAR* mapDir;
                                Interval interval0, interval1;
                                
                                // access to texture directory setting parameter block.
                                // we always assume the next parameter block of the texture is 
                                // texture directory setting dialog button.
                                int pbType = pblock2->GetParameterType(j+1);
                                if (pbType == TYPE_STRING)
                                {
                                    result0 = pblock2->GetValue(j+1, 0, mapDir, interval0);                                    
                                    if (!result0)
                                    {
                                        n_maxlog(Error, "The next parameter block of textuermap should be texture directory setting");
                                        continue;
                                    }
                                }
                                else
                                {
                                    n_maxlog(Error, "The next parameter block of textuermap should be texture directory setting");
                                }

                                result1 = pblock2->GetValue(j, 0, value, interval1);
                                if (result1)
                                {
                                    nMaxTexture texture;
                                    texture.Export(value, shaderParam, shapeNode, mapDir);
                                }
                                else
                                {
                                    n_maxlog(Error, "Failed to retrieve the value of the parameter %s.", name);
                                }
                            }
                            break;

                        default:
                            n_maxlog(Error, "Unsupported type of the parameter %s.", name);
                            break;
                        }// end of switch.
                    }//end of for each number of parameters.
                }// end of for each param blocks
            }
        }
    }
}

//-----------------------------------------------------------------------------
/**
    Retrieves matched shader param from the given script parameter name.

    @note
    parameter's internal name should be one of the member of type nShaderState::Param.
    (it's case senstive)

    @return return 'nShaderState::InvalidParameter' if there is no matched param.
*/
nShaderState::Param 
nMaxMaterial::GetShaderParam(const char* param)
{
    return nShaderState::StringToParam(param);
}
