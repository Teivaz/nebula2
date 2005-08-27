#------------------------------------------------------------------------------
# renderpath.py
# Viewing RenderPath XML files
# (c) 2005 Oleg Kreptul (Haron) okreptul@yahoo.com
# Contents are licensed under the Nebula license.
#------------------------------------------------------------------------------

import wx, os.path
from xml.dom import minidom, Node
from buildsys3.gui.xrcguiutils import *

#--------------------------------------------------------------------------
class MainFrame(wx.Frame):
    def __init__(self):
        self.xrcRootItem = None
        xrcSetFile("buildsys3/gui/xrc/renderpath.xrc")
        xrcLoadFrame(self, None, 'RenderPathFrame')
    
        # extracting controls

        # renderpath controls
        self.rpFileList   = xrcCTRLUnpack(self, "RPFile", {EVT_COMBOBOX:self.OnSelRPFile})
        self.rpName       = xrcCTRLUnpack(self, "RPname")
        self.rpShaderPath = xrcCTRLUnpack(self, "RPshaderpath")

        self.rpVarList    = xrcCTRLUnpack(self, "RPVarList", {EVT_LISTBOX:self.OnSelRPVar})
        self.rpVarName    = xrcCTRLUnpack(self, "RPVar_name")
        self.rpVarValue   = xrcCTRLUnpack(self, "RPVar_value")

        self.rtList       = xrcCTRLUnpack(self, "RTList", {EVT_LISTBOX:self.OnSelRT})
        self.rtName       = xrcCTRLUnpack(self, "RTname")
        self.rtFormat     = xrcCTRLUnpack(self, "RTformat")
        self.rtRelSize    = xrcCTRLUnpack(self, "RTrelSize")
        self.rtWidth      = xrcCTRLUnpack(self, "RTwidth")
        self.rtHeight     = xrcCTRLUnpack(self, "RTheight")

        # pass controls
        self.psList                   = xrcCTRLUnpack(self, "PSList", {EVT_LISTBOX:self.OnSelPS})
        self.psName                   = xrcCTRLUnpack(self, "PSname")
        self.psShader                 = xrcCTRLUnpack(self, "PSshader")
        self.psRenderTarget           = xrcCTRLUnpack(self, "PSrenderTarget")
        self.psClearColor             = xrcCTRLUnpack(self, "PSclearColor")
        self.psClearDepth             = xrcCTRLUnpack(self, "PSclearDepth")
        self.psClearStencil           = xrcCTRLUnpack(self, "PSclearStencil")
        self.psDrawQuad               = xrcCTRLUnpack(self, "PSdrawQuad")
        self.psDrawShadowVolumes      = xrcCTRLUnpack(self, "PSdrawShadowVolumes")
        self.psDrawGui                = xrcCTRLUnpack(self, "PSdrawGui")
        self.psTechnique              = xrcCTRLUnpack(self, "PStechnique")
        self.psShadowEnabledCondition = xrcCTRLUnpack(self, "PSshadowEnabledCondition ")

        self.psVarList                = xrcCTRLUnpack(self, "PSVarList", {EVT_LISTBOX:self.OnSelPSVar})
        self.psVarName                = xrcCTRLUnpack(self, "PSVar_name")
        self.psVarValue               = xrcCTRLUnpack(self, "PSVar_value")
        self.psVarVariable            = xrcCTRLUnpack(self, "PSVar_variable")

        # phase controls
        self.phList       = xrcCTRLUnpack(self, "PHList", {EVT_LISTBOX:self.OnSelPH})
        self.phName       = xrcCTRLUnpack(self, "PHname")
        self.phShader     = xrcCTRLUnpack(self, "PHshader")
        self.phFourcc     = xrcCTRLUnpack(self, "PHfourcc")
        self.phSort       = xrcCTRLUnpack(self, "PHsort")
        self.phLights     = xrcCTRLUnpack(self, "PHlights")
        self.phTechnique  = xrcCTRLUnpack(self, "PHtechnique")

        # sequence controls
        self.sList        = xrcCTRLUnpack(self, "SList")

        # configuring controls
        self.rpEl = None
        files_list = os.listdir("data/shaders/")
        files_list = [d for d in files_list\
                      if os.path.isfile("data/shaders/" + d) and d.endswith('_renderpath.xml')]
        files_list.sort()

        self.rpFileList.AppendItems(files_list)
    
    def OnSelRPFile(self, evt):
        cb = evt.GetEventObject()
        self.rpFile = cb.GetString(cb.GetSelection())
        self.rpEl = minidom.parse("data/shaders/" + self.rpFile).documentElement

        self.UpdateRP()

    def OnSelRT(self, evt):
        self.UpdateRT()

    def OnSelRPVar(self, evt):
        self.UpdateRPVar()

    def OnSelPS(self, evt):
        self.UpdatePS()

    def OnSelPSVar(self, evt):
        self.UpdatePSVar()

    def OnSelPH(self, evt):
        self.UpdatePH()

    # -------------------------------------------------------------------------
    def UpdateRP(self):
        self.rpName.SetValue(self.rpEl.attributes.get('name').value)
        self.rpShaderPath.SetValue(self.rpEl.attributes.get('shaderpath').value)

        self.rtList.Clear()
        self.rpVarList.Clear()
        self.psList.Clear()

        for child in self.rpEl.childNodes:
            if child.nodeType == Node.ELEMENT_NODE:
                if child.nodeName == "RenderTarget":
                    self.rtList.Append(child.attributes.get('name').value, child)
                elif child.nodeName == "Float" or child.nodeName == "Float4" or\
                     child.nodeName == "Int" or child.nodeName == "Texture":
                    self.rpVarList.Append(child.nodeName + ' <' + child.attributes.get('name').value + '>', child)
                elif child.nodeName == "Pass":
                    self.psList.Append(child.attributes.get('name').value, child)

        if self.rtList.GetCount() > 0:
            self.rtList.SetSelection(0)
        self.UpdateRT()

        if self.rpVarList.GetCount() > 0:
            self.rpVarList.SetSelection(0)
        self.UpdateRPVar()

        if self.psList.GetCount() > 0:
            self.psList.SetSelection(0)
        self.UpdatePS()

    def UpdateRT(self):
        if self.rtList.GetCount() > 0:
            n = self.rtList.GetSelection()
            el = self.rtList.GetClientData(n)
    
            self.rtName.SetValue(el.attributes.get('name').value)
            self.rtFormat.SetValue(el.attributes.get('format').value)
            self.rtRelSize.SetValue(el.attributes.get('relSize').value)
            if el.attributes.get('width') is not None:
                self.rtWidth.SetValue(el.attributes.get('width').value)
            else: self.rtWidth.SetValue('')
            if el.attributes.get('height') is not None:
                self.rtHeight.SetValue(el.attributes.get('height').value)
            else: self.rtHeight.SetValue('')
        else:
            self.rtName.SetValue('')
            #self.rtFormat.SetValue('')
            self.rtFormat.SetSelection(-1)
            self.rtRelSize.SetValue('')
            self.rtWidth.SetValue('')
            self.rtHeight.SetValue('')

    def UpdateRPVar(self):
        if self.rpVarList.GetCount() > 0:
            n = self.rpVarList.GetSelection()
            el = self.rpVarList.GetClientData(n)
    
            self.rpVarName.SetValue(el.attributes.get('name').value)
            self.rpVarValue.SetValue(el.attributes.get('value').value)
        else:
            self.rpVarName.SetValue('')
            self.rpVarValue.SetValue('')

    def UpdatePS(self):
        self.psVarList.Clear()
        self.phList.Clear()

        if self.psList.GetCount() > 0:
            n = self.psList.GetSelection()
            el = self.psList.GetClientData(n)
    
            self.psName.SetValue(el.attributes.get('name').value)
            if el.attributes.get('shader') is not None:
                self.psShader.SetValue(el.attributes.get('shader').value)
            else: self.psShader.SetValue('?????????')
            if el.attributes.get('renderTarget') is not None:
                self.psRenderTarget.SetValue(el.attributes.get('renderTarget').value)
            else: self.psRenderTarget.SetValue('?????????')
    
            if el.attributes.get('clearColor') is not None:
                self.psClearColor.SetValue(el.attributes.get('clearColor').value)
            else: self.psClearColor.SetValue('')
            if el.attributes.get('clearDepth') is not None:
                self.psClearDepth.SetValue(el.attributes.get('clearDepth').value)
            else: self.psClearDepth.SetValue('')
            if el.attributes.get('clearStencil') is not None:
                self.psClearStencil.SetValue(el.attributes.get('clearStencil').value)
            else: self.psClearStencil.SetValue('')
    
            if el.attributes.get('drawQuad') is not None:
                self.psDrawQuad.SetValue(el.attributes.get('drawQuad').value)
            else: self.psDrawQuad.SetValue('No')
            if el.attributes.get('drawShadowVolumes') is not None:
                self.psDrawShadowVolumes.SetValue(el.attributes.get('drawShadowVolumes').value)
            else: self.psDrawShadowVolumes.SetValue('No')
            if el.attributes.get('drawGui') is not None:
                self.psDrawGui.SetValue(el.attributes.get('drawGui').value)
            else: self.psDrawGui.SetValue('No')
    
            if el.attributes.get('technique') is not None:
                self.psTechnique.SetValue(el.attributes.get('technique').value)
            else: self.psTechnique.SetValue('')
            if el.attributes.get('shadowEnabledCondition') is not None:
                self.psShadowEnabledCondition.SetValue(el.attributes.get('shadowEnabledCondition').value)
            else: self.psShadowEnabledCondition.SetValue('No')
    
            for child in el.childNodes:
                if child.nodeType == Node.ELEMENT_NODE:
                    if child.nodeName == "Float" or child.nodeName == "Float4" or\
                       child.nodeName == "Int" or child.nodeName == "Texture":
                        self.psVarList.Append(child.nodeName + ' <' + child.attributes.get('name').value + '>', child)
                    elif child.nodeName == "Phase":
                        self.phList.Append(child.attributes.get('name').value, child)
        else:
            self.psName.SetValue('')
            self.psShader.SetValue('')
            self.psRenderTarget.SetValue('')
            self.psClearColor.SetValue('')
            self.psClearDepth.SetValue('')
            self.psClearStencil.SetValue('')
            #self.psDrawQuad.SetValue('No')
            #self.psDrawShadowVolumes.SetValue('No')
            #self.psDrawGui.SetValue('No')
            self.psDrawQuad.SetSelection(-1)
            self.psDrawShadowVolumes.SetSelection(-1)
            self.psDrawGui.SetSelection(-1)
            self.psTechnique.SetValue('')
            #self.psShadowEnabledCondition.SetValue('No')
            self.psShadowEnabledCondition.SetSelection(-1)

        if self.psVarList.GetCount() > 0:
            self.psVarList.SetSelection(0)
        self.UpdatePSVar()

        if self.phList.GetCount() > 0:
            self.phList.SetSelection(0)
        self.UpdatePH()

    def UpdatePSVar(self):
        if self.psVarList.GetCount() > 0:
            n = self.psVarList.GetSelection()
            el = self.psVarList.GetClientData(n)
    
            self.psVarName.SetValue(el.attributes.get('name').value)
            if el.attributes.get('value') is not None:
                #self.psVarVariable.Enable(False)
                self.psVarVariable.SetValue('')
                #self.psVarValue.Enable(True)
                self.psVarValue.SetValue(el.attributes.get('value').value)
            else:
                #self.psVarValue.Enable(False)
                self.psVarValue.SetValue('')
                #self.psVarVariable.Enable(True)
                self.psVarVariable.SetValue(el.attributes.get('variable').value)
        else:
            self.psVarName.SetValue('')
            #self.psVarValue.Enable(True)
            self.psVarValue.SetValue('')
            #self.psVarVariable.Enable(True)
            self.psVarVariable.SetValue('')

    def UpdatePH(self):
        self.sList.Clear()

        if self.phList.GetCount() > 0:
            n = self.phList.GetSelection()
            el = self.phList.GetClientData(n)
    
            self.phName.SetValue(el.attributes.get('name').value)
            self.phShader.SetValue(el.attributes.get('shader').value)
            self.phFourcc.SetValue(el.attributes.get('fourcc').value)
            self.phSort.SetValue(el.attributes.get('sort').value)
            if el.attributes.get('lights') is not None:
                self.phLights.SetValue(el.attributes.get('lights').value)
            else: self.phLights.SetValue('Off')
            if el.attributes.get('technique') is not None:
                self.phTechnique.SetValue(el.attributes.get('technique').value)
            else: self.phTechnique.SetValue('')
    
            for child in el.childNodes:
                if child.nodeType == Node.ELEMENT_NODE:
                    if child.nodeName == "Sequence":
                        self.sList.Append(child.attributes.get('shader').value, child)
        else:
            self.phName.SetValue('')
            self.phShader.SetValue('')
            self.phFourcc.SetValue('')
            #self.phSort.SetValue('FrontToBack')
            #self.phLights.SetValue('Off')
            self.phSort.SetSelection(-1)
            self.phLights.SetSelection(-1)
            self.phTechnique.SetValue('')

#--------------------------------------------------------------------------
class MainApp(wx.App):
    def __init__(self):
        wx.App.__init__(self, filename = "renderpath.log")

    def OnInit(self):
        mainFrame = MainFrame()
        self.SetTopWindow(mainFrame)
        mainFrame.Show(True)
        return True
        
#--------------------------------------------------------------------------
app = MainApp()
app.MainLoop()