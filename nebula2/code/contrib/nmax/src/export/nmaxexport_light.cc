#include "export/nmaxexport.h"
//------------------------------------------------------------------------------
/**
    3ds max to Nebula2 exporter using the IGame lib.

    This file is licensed under the terms of the Nebula License.
    (C) 2004 Johannes Kellner
*/

//------------------------------------------------------------------------------
/**
*/
bool
nMaxExport::exportLight(nString nodeName, IGameNode* igNode)
{
    /*
    n_assert(igNode);

    nTransformNode* nNode = 0;
                    
    //get the IGameLight
    IGameObject* igObject = igNode->GetIGameObject();
    n_assert(igObject && igObject->GetIGameType() == IGameObject::IGAME_LIGHT);
    IGameLight* igLight = static_cast<IGameLight*>(igObject);

    switch(igLight->GetLightType())
    {
        
        case IGameLight::IGAME_FSPOT:
        {
            nSpotLightNode* spotNode = this->kernelServer->New("nspotlightnode", nodeName.Get());
            
            this->exportParam(spotNode->SetVector(), igLight->GetLightColor());
            
            spotNode->SetVector(nShader2::LightDiffuse)

        }
        break;
        default:
            n_printf("FIXME: Light: '%s' Type: %i Not supported!", igNode->GetName(), igLight->GetLightType());
    }

    
    
    this->exportPosition(nNode, nodeName, igNode);
    this->exportRotation(nNode, nodeName, igNode);
    this->exportScale(nNode, nodeName, igNode);

    nodeName += "FIXME_LIGHT";
    nodeName += this->checkChars(igNode->GetName());
    this->kernelServer->New("ntransformnode", nodeName.Get());   
    */
    return true;
}