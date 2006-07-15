#------------------------------------------------------------------------------
# renderpath.py
# Viewing RenderPath XML files
# (c) 2005 Oleg Kreptul (Haron) okreptul@yahoo.com
# Contents are licensed under the Nebula license.
#------------------------------------------------------------------------------

import wx, os.path, sys
from xml.dom import minidom, Node
import wx.gizmos as gizmos

# This is a bit ugly
sys.path.append('..')
from buildsys3.gui.xrcguiutils import *

SHADERS_PATH = "../data/shaders/"

VAR_TYPES = ["Float", "Float4", "Int", "Texture"]

TEXTURE_FORMATS = ["NOFORMAT", "X8R8G8B8", "A8R8G8B8", "R5G6B5", "A1R5G5B5",
                   "A4R4G4B4", "P8", "G16R16", "DXT1", "DXT2", "DXT3", "DXT4",
                   "DXT5", "R16F", "G16R16F", "A16B16G16R16F", "R32F",
                   "G32R32F", "A32B32G32R32F", "A8"]

SHADOW_TECHNIQUES = ["NoShadows", "Simple", "MultiLight"]

YESNO_LIST = ["Yes", "No"]

SORT_ORDERS = ["None", "FrontToBack", "BackToFront"]

LIGHT_MODES = ["Off", "FFP", "Shader"]

#--------------------------------------------------------------------------
class MainFrame(wx.Frame):
    def __init__(self):
        self.xrcRootItem = None
        xrcSetFile("../buildsys3/gui/xrc/renderpath.xrc")
        xrcLoadFrame(self, None, 'RenderPathFrame')
    
        # extracting controls
        self.treePanel  = xrcCTRLUnpack(self, "RPTreePanel")
        self.workPanel  = xrcCTRLUnpack(self, "RPWorkPanel")

        self.tree = gizmos.TreeListCtrl(self.treePanel, -1,
                    style = wx.TR_TWIST_BUTTONS|wx.TR_HIDE_ROOT|wx.TR_FULL_ROW_HIGHLIGHT|wx.TR_NO_BUTTONS)

        # configuring controls
        self.InitTree()
##        self.tree.GetMainWindow().Bind(wx.EVT_RIGHT_UP, self.OnRightUp)
        self.tree.GetMainWindow().Bind(wx.EVT_TREE_SEL_CHANGED, self.OnSelChanged)

        w = self.treePanel.GetMinWidth()
        h = self.treePanel.GetMinHeight()

        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(self.tree, 1, wx.EXPAND|wx.ALL)
        self.treePanel.SetSizer(sizer)
        self.treePanel.SetAutoLayout(True)
        sizer.Fit(self.treePanel)

        self.treePanel.SetSizeWH(w - 2, h - 2)
        self.tree.SetSizeWH(w - 2, h - 2)

        self.workSubPanel = None

        self.rootRPEl = None
        self.globalShaders = []
        self.globalRenderTargets = []
        self.globalVariables = []

    def OnSelChanged(self, evt):
        item = evt.GetItem()
        xmlEl = self.tree.GetItemPyData(item)

        items = []
        while item:
            items.insert(0, item)
            item = self.tree.GetItemParent(item)

        del items[0]
        rootEl = self.tree.GetItemPyData(items[0])
        if self.rootRPEl != rootEl:
            self.rootRPEl = rootEl

            self.globalShaders = []
            self.globalRenderTargets = []
            self.globalVariables = []
            for child in rootEl.childNodes:
                if child.nodeType == Node.ELEMENT_NODE:
                    if child.nodeName == "Shader":
                        self.globalShaders.insert(-1, child.attributes['name'].value)
                    elif child.nodeName == "RenderTarget":
                        self.globalRenderTargets.insert(-1, child.attributes['name'].value)
                    if child.nodeName in VAR_TYPES:
                        self.globalVariables.insert(-1, child.attributes['name'].value)

        recreated = False

        if xmlEl.nodeName == "RenderPath":
            if not isinstance(self.workSubPanel, RenderPathPanel):
                self.workPanel.DestroyChildren()
                self.workSubPanel = RenderPathPanel(self.workPanel)
                recreated = True
        elif xmlEl.nodeName == "Section":
            if not isinstance(self.workSubPanel, RPSectionPanel):
                self.workPanel.DestroyChildren()
                self.workSubPanel = RPSectionPanel(self.workPanel)
                recreated = True
        elif xmlEl.nodeName == "Pass":
            if not isinstance(self.workSubPanel, RPPassPanel):
                self.workPanel.DestroyChildren()
                self.workSubPanel = RPPassPanel(self.workPanel)
                recreated = True
        elif xmlEl.nodeName == "Phase":
            if not isinstance(self.workSubPanel, RPPhasePanel):
                self.workPanel.DestroyChildren()
                self.workSubPanel = RPPhasePanel(self.workPanel)
                recreated = True
        elif xmlEl.nodeName == "Sequence":
            if not isinstance(self.workSubPanel, RPSequencePanel):
                self.workPanel.DestroyChildren()
                self.workSubPanel = RPSequencePanel(self.workPanel)
                recreated = True
        else:
            self.workPanel.DestroyChildren()
            self.workSubPanel = None

        if self.workSubPanel:
            if recreated:
                #print 'Recreated'
                sizer = self.workPanel.GetSizer() #wx.BoxSizer(wx.VERTICAL)
                sizer.Add(self.workSubPanel, 0, wx.EXPAND|wx.ALL)
                w = self.workPanel.GetSize().width
                self.workSubPanel.SetSizeWH(w - 2, -1)
            self.workSubPanel.SetData(xmlEl, self.globalShaders, self.globalRenderTargets, self.globalVariables)

    def InitTree(self):
        isz = (16,16)
        il = wx.ImageList(isz[0], isz[1])
        fldridx     = il.Add(wx.ArtProvider_GetBitmap(wx.ART_FOLDER,      wx.ART_OTHER, isz))
        fldropenidx = il.Add(wx.ArtProvider_GetBitmap(wx.ART_FILE_OPEN,   wx.ART_OTHER, isz))
        fileidx     = il.Add(wx.ArtProvider_GetBitmap(wx.ART_REPORT_VIEW, wx.ART_OTHER, isz))

        self.tree.SetImageList(il)
        self.il = il

        # create some columns
        self.tree.AddColumn("")
        self.tree.AddColumn("")
        self.tree.SetMainColumn(0) # the one with the tree in it...
        self.tree.SetColumnWidth(0, 250)
        self.tree.SetColumnWidth(1, 100)

        self.root = self.tree.AddRoot("Nebula 2 render pathes")
        self.tree.SetItemImage(self.root, fldridx, which = wx.TreeItemIcon_Normal)
        self.tree.SetItemImage(self.root, fldropenidx, which = wx.TreeItemIcon_Expanded)

        rp_files_list = os.listdir(SHADERS_PATH)
        rp_files_list = [d for d in rp_files_list\
                      if os.path.isfile(SHADERS_PATH + d) and d.endswith('_renderpath.xml')]
        rp_files_list.sort()

        # fill the tree
        # render path files
        for rpf in rp_files_list:
            rpXmlEl = minidom.parse(SHADERS_PATH + rpf).documentElement
            rpTreeEl = self.tree.AppendItem(self.root, rpf)
            self.tree.SetItemPyData(rpTreeEl, rpXmlEl)
            self.tree.SetItemText(rpTreeEl, repr(len(rpXmlEl.getElementsByTagName("Section"))) + " sections", 1)
            self.tree.SetItemImage(rpTreeEl, fldridx, which = wx.TreeItemIcon_Normal)
            self.tree.SetItemImage(rpTreeEl, fldropenidx, which = wx.TreeItemIcon_Expanded)
            self.tree.SetItemHasChildren(rpTreeEl, True)

            # sections
            for secXmlEl in rpXmlEl.childNodes:
                if secXmlEl.nodeType == Node.ELEMENT_NODE and secXmlEl.nodeName == "Section":
                    secTreeEl = self.tree.AppendItem(rpTreeEl, secXmlEl.attributes['name'].value)
                    self.tree.SetItemPyData(secTreeEl, secXmlEl)
                    self.tree.SetItemText(secTreeEl, repr(len(secXmlEl.getElementsByTagName("Pass"))) + " passes", 1)
                    self.tree.SetItemImage(secTreeEl, fldridx, which = wx.TreeItemIcon_Normal)
                    self.tree.SetItemImage(secTreeEl, fldropenidx, which = wx.TreeItemIcon_Expanded)
                    self.tree.SetItemHasChildren(secTreeEl, True)

                    # passes
                    for psXmlEl in secXmlEl.childNodes:
                        if psXmlEl.nodeType == Node.ELEMENT_NODE and psXmlEl.nodeName == "Pass":
                            psTreeEl = self.tree.AppendItem(secTreeEl, psXmlEl.attributes['name'].value)
                            self.tree.SetItemPyData(psTreeEl, psXmlEl)
                            self.tree.SetItemText(psTreeEl, repr(len(psXmlEl.getElementsByTagName("Phase"))) + " phases", 1)
                            self.tree.SetItemImage(psTreeEl, fldridx, which = wx.TreeItemIcon_Normal)
                            self.tree.SetItemImage(psTreeEl, fldropenidx, which = wx.TreeItemIcon_Expanded)
                            self.tree.SetItemHasChildren(psTreeEl, True)

                            # phases
                            for phXmlEl in psXmlEl.childNodes:
                                if phXmlEl.nodeType == Node.ELEMENT_NODE and phXmlEl.nodeName == "Phase":
                                    phTreeEl = self.tree.AppendItem(psTreeEl, phXmlEl.attributes['name'].value)
                                    self.tree.SetItemPyData(phTreeEl, phXmlEl)
                                    self.tree.SetItemText(phTreeEl, repr(len(phXmlEl.getElementsByTagName("Sequence"))) + " sequences", 1)
                                    self.tree.SetItemImage(phTreeEl, fldridx, which = wx.TreeItemIcon_Normal)
                                    self.tree.SetItemImage(phTreeEl, fldropenidx, which = wx.TreeItemIcon_Expanded)
                                    self.tree.SetItemHasChildren(phTreeEl, True)

                                    # sequences
                                    for seqXmlEl in phXmlEl.childNodes:
                                        if seqXmlEl.nodeType == Node.ELEMENT_NODE and seqXmlEl.nodeName == "Sequence":
                                            seqTreeEl = self.tree.AppendItem(phTreeEl, seqXmlEl.attributes['shader'].value)
                                            self.tree.SetItemPyData(seqTreeEl, seqXmlEl)
                                            self.tree.SetItemImage(seqTreeEl, fileidx, which = wx.TreeItemIcon_Normal)
                                            self.tree.SetItemHasChildren(seqTreeEl, False)

        self.tree.Expand(self.root)

#--------------------------------------------------------------------------
def SetAttrInGui(el, name, txtctrl, defval = ""):
    attr = el.attributes.get(name)
    if attr:
        txtctrl.SetValue(attr.value)
    else:
        txtctrl.SetValue(defval)

#--------------------------------------------------------------------------
def SetListAttrInGui(el, name, listctrl, list, defsel = 0):
    attr = el.attributes.get(name)
    if attr:
        if attr.value in list:
            listctrl.SetValue(attr.value)
        else:
            listctrl.SetSelection(defsel)
    else:
        listctrl.SetSelection(-1)

#------------------------------------------------------------------------------
class RenderPathPanel(wx.Panel):
    def __init__(self, parent):
        self.parent = parent
        self.xmlEl = None

        self.xrcRootItem = None
        xrcLoadPanel(self, parent, 'RenderPathPanel')

        # extracting controls
        self.rpName = xrcCTRLUnpack(self, "RPname")
        self.rpShaderPath = xrcCTRLUnpack(self, "RPshaderPath")

        self.rpShaderList = xrcCTRLUnpack(self, "RPShaderList", {EVT_LISTBOX:self.OnSelShd})
        self.rpShaderFile = xrcCTRLUnpack(self, "RPShader_file")

        self.rpVarList    = xrcCTRLUnpack(self, "RPVarList", {EVT_LISTBOX:self.OnSelVar})
        self.rpVarType    = xrcCTRLUnpack(self, "RPVar_type")
        self.rpVarValue   = xrcCTRLUnpack(self, "RPVar_value")

        self.rtList       = xrcCTRLUnpack(self, "RTList", {EVT_LISTBOX:self.OnSelRT})
        self.rtFormat     = xrcCTRLUnpack(self, "RTformat")
        self.rtRelSize    = xrcCTRLUnpack(self, "RTrelSize")
        self.rtWidth      = xrcCTRLUnpack(self, "RTwidth")
        self.rtHeight     = xrcCTRLUnpack(self, "RTheight")

        # configuring
        self.rpVarType.AppendItems(VAR_TYPES)
        self.rtFormat.AppendItems(TEXTURE_FORMATS)

    def SetData(self, el, gShd, gRT, gVar):
        self.xmlEl = el
        self.changed = False

        # configuring controls
        self.Update()

    def OnSelShd(self, evt):
        self.DoSelShader()

    def OnSelVar(self, evt):
        self.DoSelVariable()

    def OnSelRT(self, evt):
        self.DoSelRenderTarget()

    def Update(self):
        self.rpName.SetValue(self.xmlEl.attributes['name'].value)
        self.rpShaderPath.SetValue(self.xmlEl.attributes['shaderPath'].value)
        self.UpdateSL()
        self.UpdateVL()
        self.UpdateRTL()

    # shader list
    def UpdateSL(self):
        self.rpShaderList.Clear()
        el = self.xmlEl.getElementsByTagName("Shader")
        if el:
            for s in el:
                self.rpShaderList.Append(s.attributes['name'].value, s)
            self.rpShaderList.SetSelection(0)
            self.DoSelShader()
        else:
            self.rpShaderFile.SetValue("")

    def DoSelShader(self):
        n = self.rpShaderList.GetSelection()
        el = self.rpShaderList.GetClientData(n)

        self.rpShaderFile.SetValue(el.attributes['file'].value)

    # variable list
    def UpdateVL(self):
        self.rpVarList.Clear()

        for child in self.xmlEl.childNodes:
            if child.nodeType == Node.ELEMENT_NODE:
                if child.nodeName in VAR_TYPES:
                    self.rpVarList.Append(child.attributes['name'].value, child)

        if self.rpVarList.GetCount() > 0:
            self.rpVarList.SetSelection(0)
            self.DoSelVariable()
        else:
            self.rpVarType.SetSelection(-1)
            self.rpVarValue.SetValue("")

    def DoSelVariable(self):
        n = self.rpVarList.GetSelection()
        el = self.rpVarList.GetClientData(n)

        self.rpVarType.SetValue(el.nodeName)
        self.rpVarValue.SetValue(el.attributes['value'].value)

    # render target list
    def UpdateRTL(self):
        self.rtList.Clear()
        el = self.xmlEl.getElementsByTagName("RenderTarget")
        if el:
            for s in el:
                self.rtList.Append(s.attributes['name'].value, s)
            self.rtList.SetSelection(0)
            self.DoSelRenderTarget()
        else:
            self.rtFormat.SetSelection(-1)
            self.rtRelSize.SetValue("")
            self.rtWidth.SetValue("")
            self.rtHeight.SetValue("")

    def DoSelRenderTarget(self):
        n = self.rtList.GetSelection()
        el = self.rtList.GetClientData(n)

        SetListAttrInGui(el, 'format', self.rtFormat, TEXTURE_FORMATS)

        SetAttrInGui(el, 'relSize', self.rtRelSize)
        SetAttrInGui(el, 'width', self.rtWidth)
        SetAttrInGui(el, 'height', self.rtHeight)

#------------------------------------------------------------------------------
class RPSectionPanel(wx.Panel):
    def __init__(self, parent):
        self.parent = parent
        self.xmlEl = None

        self.xrcRootItem = None
        xrcLoadPanel(self, parent, 'RPSectionPanel')

        # extracting controls
        self.secName = xrcCTRLUnpack(self, "SECname")

    def SetData(self, el, gShd, gRT, gVar):
        self.xmlEl = el
        self.changed = False

        # configuring controls
        self.Update()

    def Update(self):
        self.secName.SetValue(self.xmlEl.attributes['name'].value)

#------------------------------------------------------------------------------
class RPPassPanel(wx.Panel):
    def __init__(self, parent):
        self.parent = parent
        self.xmlEl = None

        self.xrcRootItem = None
        xrcLoadPanel(self, parent, 'RPPassPanel')

        # extracting controls
        self.psShader                 = xrcCTRLUnpack(self, "PSshader")
        self.psClearColor             = xrcCTRLUnpack(self, "PSclearColor")
        self.psClearDepth             = xrcCTRLUnpack(self, "PSclearDepth")
        self.psClearStencil           = xrcCTRLUnpack(self, "PSclearStencil")
        self.psDrawQuad               = xrcCTRLUnpack(self, "PSdrawQuad")
        self.psDrawShadows            = xrcCTRLUnpack(self, "PSdrawShadows")
        self.psOcclusionQuery         = xrcCTRLUnpack(self, "PSocclusionQuery")
        self.psDrawGui                = xrcCTRLUnpack(self, "PSdrawGui")
        self.psTechnique              = xrcCTRLUnpack(self, "PStechnique")
        self.psShadowEnabledCondition = xrcCTRLUnpack(self, "PSshadowEnabledCondition")

        self.psRTList                 = xrcCTRLUnpack(self, "PSrenderTargets")

        self.psVarList                = xrcCTRLUnpack(self, "PSVarList", {EVT_LISTBOX:self.OnSelVar})
        self.psVarType                = xrcCTRLUnpack(self, "PSVar_type")
        self.psVarValue               = xrcCTRLUnpack(self, "PSVar_value")
        self.psVarVariable            = xrcCTRLUnpack(self, "PSVar_variable")

        # configuring
        self.psDrawQuad.AppendItems(YESNO_LIST)
        self.psDrawShadows.AppendItems(SHADOW_TECHNIQUES)
        self.psOcclusionQuery.AppendItems(YESNO_LIST)
        self.psDrawGui.AppendItems(YESNO_LIST)
        self.psShadowEnabledCondition.AppendItems(YESNO_LIST)
        self.psVarType.AppendItems(VAR_TYPES)

    def SetData(self, el, gShd, gRT, gVar):
        self.xmlEl = el
        self.globalShd = gShd
        self.globalRT = gRT
        self.globalVar = gVar
        self.changed = False

        # configuring controls
        self.psShader.Clear()
        self.psShader.AppendItems(gShd)
        self.psVarVariable.Clear()
        self.psVarVariable.AppendItems(gVar)
        self.Update()

    def OnSelVar(self, evt):
        self.DoSelVariable()

    def Update(self):
        SetListAttrInGui(self.xmlEl, 'shader', self.psShader, self.globalShd, -1)
##        self.psShader.SetValue(self.xmlEl.attributes['shader'].value)

        SetAttrInGui(self.xmlEl, 'clearColor', self.psClearColor)
        SetAttrInGui(self.xmlEl, 'clearDepth', self.psClearDepth)
        SetAttrInGui(self.xmlEl, 'clearStencil', self.psClearStencil)

        SetListAttrInGui(self.xmlEl, 'drawQuad', self.psDrawQuad, YESNO_LIST, 1)
        SetListAttrInGui(self.xmlEl, 'drawShadows', self.psDrawShadows, SHADOW_TECHNIQUES)
        SetListAttrInGui(self.xmlEl, 'occlusionQuery', self.psOcclusionQuery, YESNO_LIST, 1)
        SetListAttrInGui(self.xmlEl, 'drawGui', self.psDrawGui, YESNO_LIST, 1)
        SetAttrInGui(self.xmlEl, 'technique', self.psTechnique)
        SetListAttrInGui(self.xmlEl, 'drawQuad', self.psShadowEnabledCondition, YESNO_LIST, 1)

        self.UpdateRTL()
        self.UpdateVL()

    # render target list
    def UpdateRTL(self):
        self.psRTList.Clear()

        rtAttrName = "renderTarget"
        i = 0
        while self.xmlEl.attributes.get(rtAttrName):
            val = self.xmlEl.attributes[rtAttrName].value
            if val in self.globalRT:
                self.psRTList.Append(val)
            else:
                self.psRTList.Append(val + "*")
            i += 1
            rtAttrName = "renderTarget" + str(i)

    # variable list
    def UpdateVL(self):
        self.psVarList.Clear()

        for child in self.xmlEl.childNodes:
            if child.nodeType == Node.ELEMENT_NODE:
                if child.nodeName in VAR_TYPES:
                    self.psVarList.Append(child.attributes.get('name').value, child)

        if self.psVarList.GetCount() > 0:
            self.psVarList.SetSelection(0)
            self.DoSelVariable()
        else:
            self.psVarType.SetSelection(-1)
            self.psVarValue.SetValue("")
            self.psVarVariable.SetSelection(-1)

    def DoSelVariable(self):
        n = self.psVarList.GetSelection()
        el = self.psVarList.GetClientData(n)

        self.psVarType.SetValue(el.nodeName)
        if el.attributes.get('value'):
            self.psVarValue.SetValue(el.attributes['value'].value)
            self.psVarVariable.SetSelection(-1)
        else:
            SetListAttrInGui(el, 'variable', self.psVarVariable, self.globalVar, -1)
            #self.psVarVariable.SetValue(el.attributes['variable'].value)
            self.psVarValue.SetValue('')

#------------------------------------------------------------------------------
class RPPhasePanel(wx.Panel):
    def __init__(self, parent):
        self.parent = parent
        self.xmlEl = None

        self.xrcRootItem = None
        xrcLoadPanel(self, parent, 'RPPhasePanel')

        # extracting controls
        self.phShader     = xrcCTRLUnpack(self, "PHshader")
        self.phSort       = xrcCTRLUnpack(self, "PHsort")
        self.phLightMode  = xrcCTRLUnpack(self, "PHlightMode")
        self.phTechnique  = xrcCTRLUnpack(self, "PHtechnique")

        # configuring
        self.phSort.AppendItems(SORT_ORDERS)
        self.phLightMode.AppendItems(LIGHT_MODES)

    def SetData(self, el, gShd, gRT, gVar):
        self.xmlEl = el
        self.globalShd = gShd
        self.changed = False

        # configuring controls
        self.phShader.Clear()
        self.phShader.AppendItems(gShd)
        self.Update()

    def Update(self):
        SetListAttrInGui(self.xmlEl, 'shader', self.phShader, self.globalShd, -1)
##        SetAttrInGui(self.xmlEl, 'shader', self.phShader)

        SetListAttrInGui(self.xmlEl, 'sort', self.phSort, SORT_ORDERS)
        SetListAttrInGui(self.xmlEl, 'lightMode', self.phLightMode, LIGHT_MODES)
        SetAttrInGui(self.xmlEl, 'technique', self.phTechnique)

#------------------------------------------------------------------------------
class RPSequencePanel(wx.Panel):
    def __init__(self, parent):
        self.parent = parent
        self.xmlEl = None

        self.xrcRootItem = None
        xrcLoadPanel(self, parent, 'RPSequencePanel')

        # extracting controls
        self.seqTechnique       = xrcCTRLUnpack(self, "SEQtechnique")
        self.seqShaderUpdates   = xrcCTRLUnpack(self, "SEQshaderUpdates")
        self.seqFirstLightAlpha = xrcCTRLUnpack(self, "SEQfirstLightAlpha")
        self.seqMvpOnly         = xrcCTRLUnpack(self, "SEQmvpOnly")

        self.seqVarList         = xrcCTRLUnpack(self, "SEQVarList", {EVT_LISTBOX:self.OnSelVar})
        self.seqVarType         = xrcCTRLUnpack(self, "SEQVar_type")
        self.seqVarValue        = xrcCTRLUnpack(self, "SEQVar_value")
        self.seqVarVariable     = xrcCTRLUnpack(self, "SEQVar_variable")

        # configuring
        self.seqShaderUpdates.AppendItems(YESNO_LIST)
        self.seqFirstLightAlpha.AppendItems(YESNO_LIST)
        self.seqMvpOnly.AppendItems(YESNO_LIST)
        self.seqVarType.AppendItems(VAR_TYPES)

    def SetData(self, el, gShd, gRT, gVar):
        self.xmlEl = el
        self.globalVar = gVar
        self.changed = False

        # configuring controls
        self.seqVarVariable.Clear()
        self.seqVarVariable.AppendItems(gVar)
        self.Update()

    def OnSelVar(self, evt):
        self.DoSelVariable()

    def Update(self):
        SetAttrInGui(self.xmlEl, 'technique', self.seqTechnique)

        SetListAttrInGui(self.xmlEl, 'shaderUpdates', self.seqShaderUpdates, YESNO_LIST)
        SetListAttrInGui(self.xmlEl, 'firstLightAlpha', self.seqFirstLightAlpha, YESNO_LIST, 1)
        SetListAttrInGui(self.xmlEl, 'mvpOnly', self.seqMvpOnly, YESNO_LIST, 1)

        self.UpdateVL()

    # variable list
    def UpdateVL(self):
        self.seqVarList.Clear()

        for child in self.xmlEl.childNodes:
            if child.nodeType == Node.ELEMENT_NODE:
                if child.nodeName in VAR_TYPES:
                    self.seqVarList.Append(child.attributes.get('name').value, child)

        if self.seqVarList.GetCount() > 0:
            self.seqVarList.SetSelection(0)
            self.DoSelVariable()
        else:
            self.seqVarType.SetSelection(-1)
            self.seqVarValue.SetValue("")
            self.seqVarVariable.SetSelection(-1)

    def DoSelVariable(self):
        n = self.seqVarList.GetSelection()
        el = self.seqVarList.GetClientData(n)

        self.seqVarType.SetValue(el.nodeName)
        if el.attributes.get('value'):
            self.seqVarValue.SetValue(el.attributes['value'].value)
            self.seqVarVariable.SetSelection(-1)
        else:
            SetListAttrInGui(el, 'variable', self.seqVarVariable, self.globalVar, -1)
##            self.seqVarVariable.SetValue(el.attributes['variable'].value)
            self.seqVarValue.SetValue('')

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
