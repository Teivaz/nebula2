//-----------------------------------------------------------------------------
//  nmaxmaterial_particle2.cc
//
//  Retrieves particle2 custom attributes from 3dsmax material editor.
//
//  (c)2006 Kim, Hyoun Woo
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
#include "export2/nmaxoptions.h"

#include "kernel/nfileserver2.h"
#include "scene/nshapenode.h"
#include "mathlib/envelopecurve.h"
#include "particle/nparticle2emitter.h"
#include "scene/nparticleshapenode2.h"

nParticle2Emitter::CurveType GetCurveTpe(TCHAR* name);

//-----------------------------------------------------------------------------
/**
*/
void nMaxMaterial::GetParticle2Material(Mtl* mtl, nShapeNode* shapeNode)
{
    if (NULL == mtl || NULL == shapeNode)
        return;

    nParticleShapeNode2* particleNode = static_cast<nParticleShapeNode2*>(shapeNode);

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

                int numParamBlocks = custAttr->NumParamBlocks();

                for (int i=0; i<numParamBlocks; i++)
                {
                    // NOTE: the only valid parameter block which can be exported is 
                    //       the custom attribute's one but we process it with general way.
                    IParamBlock2* pblock2 = custAttr->GetParamBlock(i);

                    // get the number of the params.
                    int numParams = pblock2->NumParams();

                    int paramType;

                    for (int j=0; j<numParams; j++)
                    {
                        ParamBlockDesc2* pblockDesc = pblock2->GetDesc();
                        if (NULL == pblockDesc)
                            return;

                        ParamID paramID = pblockDesc->IndextoID(j);
                        ParamDef& paramDef = pblockDesc->GetParamDef(paramID);
                        TCHAR* name = paramDef.int_name;
                        
                        paramType = pblock2->GetParameterType(j);

                        switch(paramType)
                        {
                        case TYPE_FLOAT:
                            {
                                BOOL result;
                                float value;
                                Interval interval;
                                result = pblock2->GetValue(j, 0, value, interval);
                                if (result)
                                {
		                            if (strcmp(name, "EmissionDuration") == 0)
                                    {
                                        particleNode->SetEmissionDuration(value);
                                    }
                                    else
                                    if (strcmp(name, "ActivityDistance") == 0)
                                    {
                                        particleNode->SetActivityDistance(value);
                                    }
                                    else
                                    if (strcmp(name, "ParticleStartRotation") == 0)
                                    {
                                        //particleNode->SetStartRotationMin(value);
                                        //particleNode->SetStartRotationMax(value);
                                    }
                                    else
		                            if (strcmp(name, "ParticleSpreadAngle") == 0)
                                    {
                                        ;//particleNode->SetParticleSpre
                                    }
                                    else
		                            if (strcmp(name, "ParticleBirthDelay") == 0)
                                    {
                                        ;
                                    }
                                    else
                                    {
                                        n_maxlog(Error, "Failed to retrieve the value of the parameter %s.", name);
                                    }
                                }
                            }
                            break;
                        case TYPE_BOOL: //4
                            {
                                BOOL result;
                                int value;
                                Interval interval;
                                result = pblock2->GetValue(j, 0, value, interval);
                                if (result)
                                {
                                    if (strcmp(name, "Loop") == 0)
                                    {
                                        particleNode->SetLoop(((value > 0) ? true : false));
                                    }
                                }
                            }
                            break;
                        case TYPE_INT:
                            {
                                BOOL result;
                                int value;
                                Interval interval;
                                result = pblock2->GetValue(j, 0, value, interval);
                            }
                            break;
                        case TYPE_FLOAT_TAB:
                            {
                                BOOL result;
                                float values[14] = {0};
                                Interval interval;

                                short tabSize = paramDef.tab_size;

                                // get values of parameters block.
                                for (int p=0; p<tabSize; ++p)
                                {
                                    result &= pblock2->GetValue(j, 0, values[p],interval, p);
                                }

                                //FIXME: onlue nParticle2ShapeNode::SetRGBCurve() has 14 in-args.
                                if (tabSize > 9)
                                {
                                    n_assert2(tabSize == 14, "It is not particle rgb.\n");

                                    particleNode->SetRGBCurve(nVector3EnvelopeCurve (
                                        vector3(values[0], values[1], values[2]),
                                        vector3(values[3], values[4], values[5]),
                                        vector3(values[6], values[7], values[8]),
                                        vector3(values[9], values[10], values[11]),
                                        values[12], values[13]));
                                }
                                else
                                {
                                    nParticle2Emitter::CurveType curveType;
                                    curveType = GetCurveTpe(name);

                                    nEnvelopeCurve envelopeCurve 
                                    (
                                        values[0], values[1], values[2], values[3],
                                        values[4], values[5], values[6], values[7],
                                        (int)(values[9])
                                    );

                                    particleNode->SetCurve(curveType, envelopeCurve);
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
*/
static
nParticle2Emitter::CurveType GetCurveTpe(TCHAR* name)
{
    if (strcmp(name, "EmissionFrequency") == 0)
    {
        return nParticle2Emitter::EmissionFrequency;
    }
    else
    if (strcmp(name, "ParticleLifeTime") == 0)
    {
        return nParticle2Emitter::ParticleLifeTime;
    }
    else
    if (strcmp(name, "ParticleStartVelocity") == 0)
    {
        return nParticle2Emitter::ParticleStartVelocity;
    }
    else
    if (strcmp(name, "ParticleRotationVelocity") == 0)
    {
        return nParticle2Emitter::ParticleRotationVelocity;
    }
    else
    if (strcmp(name, "ParticleScale") == 0)
    {
        return nParticle2Emitter::ParticleScale;
    }
    //else
    //if (strcmp(name, "ParticleWeight") == 0)
    //{
    //    ;
    //}
    //else
    //if (strcmp(name, "ParticleRGB") == 0)
    //{
    //    ;
    //}
    else
    if (strcmp(name, "ParticleAlpha") == 0)
    {
        return nParticle2Emitter::ParticleAlpha;
    }
    //else
    //if (strcmp(name, "ParticleSideVelocity1") == 0)
    //{
    //    ;
    //}
    //else
    //if (strcmp(name, "ParticleSideVelocity2") == 0)
    //{
    //    ;
    //}
    else
    if (strcmp(name, "ParticleMass") == 0)
    {
        return nParticle2Emitter::ParticleMass;
    }
    else
    if (strcmp(name, "TimeManipulator") == 0)
    {
        return nParticle2Emitter::TimeManipulator;
    }
    else
    if (strcmp(name, "ParticleAirResistance") == 0)
    {
        return nParticle2Emitter::ParticleAirResistance;
    }
    else
    if (strcmp(name, "ParticleVelocityFactor") == 0)
    {
        return nParticle2Emitter::ParticleVelocityFactor;
    }
    else
    {
        return nParticle2Emitter::CurveTypeCount; // invalid param
    }        
}
