//----------------------------------------------------------------------------
// (c) 2006    Vadim Macagon
//----------------------------------------------------------------------------
#include "lwexporter/nlwshaderpanelfactory.h"
#include "lwwrapper/nlwglobals.h"
#include "lwexporter/nlwshaderpaneltemplate.h"
#include "lwexporter/nlwshaderpanel.h"
#include "lwexporter/nlwsettingsregistry.h"
#include "tinyxml/tinyxml.h"
#include "lwexporter/nlwshaderexportsettings.h"
#include "lwexporter/nlwviewerremote.h"

nLWShaderPanelFactory* nLWShaderPanelFactory::singleton = 0;

//----------------------------------------------------------------------------
/**
*/
nLWShaderPanelFactory::nLWShaderPanelFactory()
{
    nLWShaderPanelFactory::singleton = this;
}

//----------------------------------------------------------------------------
/**
*/
nLWShaderPanelFactory::~nLWShaderPanelFactory()
{
    nHashNode* node = 0;
    while ((node = this->panelTemplateList.RemTail()))
    {
        nLWShaderPanelTemplate* panelTemplate = (nLWShaderPanelTemplate*)node->GetPtr();
        if (panelTemplate)
        {
            n_delete(panelTemplate);
        }
        n_delete(node);
    }
}

//----------------------------------------------------------------------------
/**
*/
nLWShaderPanelFactory*
nLWShaderPanelFactory::Instance()
{
    if (!nLWShaderPanelFactory::singleton)
    {
        n_new(nLWShaderPanelFactory);
    }
    n_assert(nLWShaderPanelFactory::singleton);
    return nLWShaderPanelFactory::singleton;
}

//----------------------------------------------------------------------------
/**
*/
void
nLWShaderPanelFactory::FreeInstance()
{
    if (nLWShaderPanelFactory::singleton)
    {
        n_delete(nLWShaderPanelFactory::singleton);
        nLWShaderPanelFactory::singleton = 0;
    }
}

//----------------------------------------------------------------------------
/**
*/
void
nLWShaderPanelFactory::SetShadersFile(const nString& shadersFileName)
{
    this->shadersFileName = shadersFileName;
}

//----------------------------------------------------------------------------
/**
*/
void
nLWShaderPanelFactory::Load()
{
    n_assert(!this->shadersFileName.IsEmpty());
    if (this->shadersFileName.IsEmpty())
        return;

    TiXmlDocument xmlDoc;
    if (!xmlDoc.LoadFile(this->shadersFileName.Get()))
    {
        n_printf("Failed to load %s.", this->shadersFileName.Get());
        return;
    }

    // create a template for the default (blank) shader panel
    nLWShaderPanelTemplate* panelTemplate = n_new(nLWShaderPanelTemplate("None", "None"));
    if (panelTemplate)
    {
        nHashNode* panelTemplateNode = n_new(nHashNode("None"));
        n_assert(panelTemplateNode);
        if (panelTemplateNode)
        {
            panelTemplateNode->SetPtr(panelTemplate);
            this->panelTemplateList.AddTail(panelTemplateNode);
        }
        else
        {
            n_delete(panelTemplate);
            panelTemplate = 0;
        }

        this->shaderNames.Append(panelTemplate->GetShaderName());
    }

    // create templates for the rest of the shader panels
    TiXmlHandle xmlhDoc(&xmlDoc);
    TiXmlElement* shaderElem = xmlhDoc.FirstChild("NebulaShaderDatabase").FirstChild("shader").Element();
    for ( ; shaderElem; shaderElem = shaderElem->NextSiblingElement())
    {
        // get shader name
        nString shaderName(shaderElem->Attribute("name"));
        if (shaderName.IsEmpty())
            continue;

        nString shaderFile(shaderElem->Attribute("file"));
        if (shaderFile.IsEmpty())
            continue;

        // prepare corresponding panel template
        panelTemplate = n_new(nLWShaderPanelTemplate(shaderName, shaderFile));
        if (panelTemplate)
        {
            if (panelTemplate->LoadLayout(shaderElem))
            {
                nHashNode* panelTemplateNode = n_new(nHashNode(shaderName.Get()));
                n_assert(panelTemplateNode);
                if (panelTemplateNode)
                {
                    panelTemplateNode->SetPtr(panelTemplate);
                    this->panelTemplateList.AddTail(panelTemplateNode);
                }
                else
                {
                    n_delete(panelTemplate);
                    panelTemplate = 0;
                }
            }
            else
            {
                n_delete(panelTemplate);
                panelTemplate = 0;
            }

            this->shaderNames.Append(panelTemplate->GetShaderName());
        }
    }
}

//----------------------------------------------------------------------------
/**
*/
void
nLWShaderPanelFactory::SetNextShaderPanelToDisplay(const nString& shaderName)
{
    this->nextShaderPanelName = shaderName;
}

//----------------------------------------------------------------------------
/**
*/
nLWShaderPanelTemplate*
nLWShaderPanelFactory::GetNextShaderPanelToDisplay() const
{
    if (this->nextShaderPanelName.IsEmpty())
        return 0;

    nHashNode* panelNode = this->panelTemplateList.Find(this->nextShaderPanelName.Get());
    if (panelNode)
        return (nLWShaderPanelTemplate*)panelNode->GetPtr();

    return 0;
}

//----------------------------------------------------------------------------
/**
*/
void
nLWShaderPanelFactory::DisplayNextPanel(nLWShaderExportSettings* settings)
{
    nLWShaderPanelTemplate* panelTemplate = 0;
    while ((panelTemplate = this->GetNextShaderPanelToDisplay()))
    {
        this->SetNextShaderPanelToDisplay(0);

        HostDisplayInfo* hdi = nLWGlobals::GetHostDisplayInfo();
        if (!hdi)
            return;

        wxWindow parent;
        parent.SetHWND(hdi->window);
        parent.Enable(false);
        if (!nLWViewerRemote::Instance()->IsOpen())
        {
            nLWViewerRemote::Instance()->Open();
        }
        nLWShaderPanel shaderPanel(&parent, panelTemplate, this->shaderNames);
        shaderPanel.SetShaderSettings(settings);
        shaderPanel.ResetShaderPopup();
        shaderPanel.ShowModal();
        if (nLWViewerRemote::Instance()->IsOpen())
        {
            nLWViewerRemote::Instance()->Close();
        }
        parent.Enable(true);
        parent.SetHWND(0);
    }
}

//----------------------------------------------------------------------------
/**
*/
void
nLWShaderPanelFactory::DisplayPanel(LWSurfaceID surfId)
{
    nLWGlobals::SurfaceFuncs surfFuncs;
    n_assert(surfFuncs.IsValid());
    if (!surfFuncs.IsValid())
        return;

    nLWSettingsRegistry* registry = nLWSettingsRegistry::Instance();
    n_assert(registry);
    if (registry)
    {
        nLWShaderExportSettings* settings = registry->Get(surfId);
        n_assert(settings);
        if (settings)
        {
            this->SetNextShaderPanelToDisplay(settings->GetShaderName());
            this->DisplayNextPanel(settings);
        }
    }
}

//----------------------------------------------------------------------------
// EOF
//----------------------------------------------------------------------------
