#------------------------------------------------------------------------------
# cmddatapanels.py
# Panels for editing cmd file elements
# (c) 2005 Oleg Kreptul (Haron) okreptul@yahoo.com
# Contents are licensed under the Nebula license.
#------------------------------------------------------------------------------

from wxPython.wx import *
from wxPython.xrc import *
import wx.lib.dialogs
import types

from cmdparser import *
from xrcguiutils import *

#------------------------------------------------------------------------------
class GeneralInfoPanel(wx.Panel):
    def __init__(self, parent):
        self.parent = parent
        self.cfg = None

        self.xrcRootItem = None
        xrcLoadPanel(self, parent, 'GeneralInfoPanel')
        #self.SetSizeWH(parent.GetSize().x - 6, -1)

        # extracting controls
        self.hdr_info    = xrcCTRLUnpack(self, "HeaderInfo") #, {EVT_CHECKBOX:self.OnUseVoid})
        self.includes    = xrcCTRLUnpack(self, "Includes") #, {EVT_CHECKBOX:self.OnUseVoid})
        self.scriptClass = xrcCTRLUnpack(self, "ScriptClass") #, {EVT_CHECKBOX:self.OnUseVoid})
        self.cppClass    = xrcCTRLUnpack(self, "CppClass") #, {EVT_CHECKBOX:self.OnUseVoid})
        self.superClass  = xrcCTRLUnpack(self, "SuperClass") #, {EVT_CHECKBOX:self.OnUseVoid})
        self.classInfo   = xrcCTRLUnpack(self, "ClassInfo") #, {EVT_CHECKBOX:self.OnUseVoid})
        self.scPreview   = xrcCTRLUnpack(self, "SaveCmds", {EVT_BUTTON:self.OnSaveCmdsView})

    def SetData(self, cfg):
        self.cfg = cfg
        hdr = cfg.header
        cli = cfg.classInfo
        scf = cfg.saveCmdsFunc
        self.changed = False

        # configuring controls
        self.hdr_info.SetValue(hdr.generalInfo)
        self.includes.AppendItems(hdr.includes)
        self.scriptClass.SetValue(cli.scriptClass)
        self.cppClass.SetValue(cli.cppClass)
        self.superClass.SetValue(cli.superClass)
        self.classInfo.SetValue(cli.info)
        if not scf: self.scPreview.Enable(False)

    def OnSaveCmdsView(self, evt):
        dlg = wx.lib.dialogs.ScrolledMessageDialog(self.parent,
                                repr(self.cfg.saveCmdsFunc), 'SaveCmds function')
        dlg.ShowModal()

#------------------------------------------------------------------------------
class CommonCmdPanel(wx.Panel):
    def __init__(self, parent):
        self.parent = parent
        self.cfg = None
        self.cmd = None

        self.xrcRootItem = None
        xrcLoadPanel(self, parent, 'CommonCmdPanel')
        #self.SetSizeWH(parent.GetSize().x - 6, -1)
    
        # extracting controls
        self.cmdDecl      = xrcCTRLUnpack(self, "CmdDecl") #, {EVT_CHECKBOX:self.OnUseVoid})
        self.fourCC       = xrcCTRLUnpack(self, "FourCC") #, {EVT_BUTTON:self.OnFormatClick})
        self.cmdName      = xrcCTRLUnpack(self, "Cmd") #, {EVT_BUTTON:self.OnFormatClick})
        self.outFormatBtn = xrcCTRLUnpack(self, "OutputParams", {EVT_BUTTON:self.OnOutFormatClick})
        self.inFormatBtn  = xrcCTRLUnpack(self, "InputParams", {EVT_BUTTON:self.OnInFormatClick})
        self.cmdInfo      = xrcCTRLUnpack(self, "Info") #, {EVT_BUTTON:self.OnFormatClick})
        #self.cmdPreview   = xrcCTRLUnpack(self, "CmdPreview", {EVT_BUTTON:self.OnCmdView})
        #self.convertTo    = xrcCTRLUnpack(self, "ConvertTo") #, {EVT_BUTTON:self.OnConvertTo})

    def SetData(self, cfg, cmd_name):
        self.cmd = cfg.findCmd(cmd_name)
        if not self.cmd: return
        self.cfg = cfg
        self.changed = False

        # configuring controls
        self.cmdDecl.SetLabel(self.cmd.outFormat + '_' + self.cmd.cmdName + '_' + self.cmd.inFormat)
        self.fourCC.SetLabel('[' + self.cmd.fourCC + '] Edit')
        self.cmdName.SetValue(self.cmd.cmdName)
        self.cmdInfo.SetValue(self.cmd.info)

    def OnOutFormatClick(self, evt):
        dlg = ParamsEditDlg(self.parent, self.cmd.outParams, "This is a Dialog")
        dlg.CenterOnScreen()

        # this does not return until the dialog is closed.
        val = dlg.ShowModal()
    
##        if val == wx.ID_OK:
##            self.log.WriteText("You pressed OK\n")
##        else:
##            self.log.WriteText("You pressed Cancel\n")

        dlg.Destroy()

    def OnInFormatClick(self, evt):
        dlg = ParamsEditDlg(self.parent, self.cmd.inParams, "This is a Dialog")
        dlg.CenterOnScreen()

        # this does not return until the dialog is closed.
        val = dlg.ShowModal()
    
##        if val == wx.ID_OK:
##            self.log.WriteText("You pressed OK\n")
##        else:
##            self.log.WriteText("You pressed Cancel\n")

        dlg.Destroy()

    def OnConvertTo(self, evt): None

#------------------------------------------------------------------------------
class CmdPanel(wx.Panel):
    def __init__(self, parent):
        self.parent = parent
        self.cfg = None
        self.cmd = None

        self.xrcRootItem = None
        xrcLoadPanel(self, parent, 'CmdPanel')
        #self.SetSizeWH(parent.GetSize().x - 6, -1)
    
        # extracting controls
        self.cmdDecl      = xrcCTRLUnpack(self, "CmdDecl") #, {EVT_CHECKBOX:self.OnUseVoid})
        self.cmdName      = xrcCTRLUnpack(self, "Cmd") #, {EVT_BUTTON:self.OnFormatClick})
        self.type         = xrcCTRLUnpack(self, "Type") #, {EVT_BUTTON:self.OnOutFormatClick})
        self.subType      = xrcCTRLUnpack(self, "Subtype") #, {EVT_BUTTON:self.OnInFormatClick})
        self.property     = xrcCTRLUnpack(self, "Property") #, {EVT_BUTTON:self.OnInFormatClick})
        self.cmdInfo      = xrcCTRLUnpack(self, "Info") #, {EVT_BUTTON:self.OnFormatClick})
        #self.cmdPreview   = xrcCTRLUnpack(self, "CmdPreview", {EVT_BUTTON:self.OnCmdView})
        #self.convertTo    = xrcCTRLUnpack(self, "ConvertTo") #, {EVT_BUTTON:self.OnConvertTo})

    def SetData(self, cfg, cmd_name):
        self.cmd = cfg.findCmd(cmd_name)
        if not self.cmd: return
        self.cfg = cfg
        self.changed = False

        # configuring controls
        self.cmdDecl.SetLabel(self.cmd.outFormat + '_' + self.cmd.cmdName + '_' + self.cmd.inFormat)
        self.cmdName.SetValue(self.cmd.cmdName)
        self.property.SetLabel('[' + self.cmd.propertyName + '] Edit')
        self.cmdInfo.SetValue(self.cmd.info)

#------------------------------------------------------------------------------
class ParamsEditDlg(wx.Dialog):
    def __init__(self, parent, params, title):
        self.xrcRootItem = None
        xrcLoadDialog(self, parent, 'ParamsDlg')
    
        # extracting controls
        self.voidCheckBox = xrcCTRLUnpack(self, "VoidCheckBox", {EVT_CHECKBOX:self.OnUseVoid})
        self.paramList    = xrcCTRLUnpack(self, "ParamList", {EVT_LISTBOX:self.OnSelParam})
        self.btnParamUp   = xrcCTRLUnpack(self, "ParamUp") #, {EVT_BUTTON:self.OnParamUp})
        self.btnParamDown = xrcCTRLUnpack(self, "ParamDown") #, {EVT_BUTTON:self.OnParamDown})
        self.btnAddParam  = xrcCTRLUnpack(self, "AddParam", {EVT_BUTTON:self.OnAddParam})
        self.btnDelParam  = xrcCTRLUnpack(self, "DelParam", {EVT_BUTTON:self.OnDelParam})

        # format panel
        self.paramFormat     = xrcCTRLUnpack(self, "ParamFormat")
        self.paramName       = xrcCTRLUnpack(self, "ParamName")
        self.attrList        = xrcCTRLUnpack(self, "AttrList", {EVT_LIST_ITEM_SELECTED:self.OnSelAttr})
        self.btnAttrUp       = xrcCTRLUnpack(self, "AttrUp") #, {EVT_BUTTON:self.OnAttrUp})
        self.btnAttrDown     = xrcCTRLUnpack(self, "AttrDown") #, {EVT_BUTTON:self.OnAttrDown})
        self.btnAddAttr      = xrcCTRLUnpack(self, "AddAttr") #, {EVT_BUTTON:self.OnAddAttr})
        self.btnEditAttr     = xrcCTRLUnpack(self, "EditAttr") #, {EVT_BUTTON:self.OnEditAttr})
        self.btnDelAttr      = xrcCTRLUnpack(self, "DelAttr") #, {EVT_BUTTON:self.OnDelAttr})
        self.btnUpdateFormat = xrcCTRLUnpack(self, "UpdateFormat") #, {EVT_BUTTON:self.OnUpdateFormat})

        self.btnOk     = xrcCTRLUnpack(self, "ID_OK") #, {EVT_BUTTON:self.OnOk})
        self.btnCancel = xrcCTRLUnpack(self, "ID_CANCEL") #, {EVT_BUTTON:self.OnCancel})

        self.vList = [self.paramList,
                      self.btnParamUp, self.btnParamDown,
                      self.btnAddParam, self.btnDelParam,
                      self.paramFormat, self.paramName, self.attrList,
                      self.btnAttrUp, self.btnAttrDown,
                      self.btnAddAttr, self.btnEditAttr, self.btnDelAttr,
                      self.btnUpdateFormat]

        # configuring controls
        self.attrList.InsertColumn(0, "Attr")
        self.attrList.InsertColumn(1, "Value")
        self.attrList.SetColumnWidth(0, 80)
        self.attrList.SetColumnWidth(1, wx.LIST_AUTOSIZE_USEHEADER)
        self.paramFormat.AppendItems(TYPES_LIST)

        # data managing
        self.changed = False
        self.isVoid = False

        self.p = []
        if isinstance(params, types.ListType):
            for p in params:
                if isinstance(p, CmdParam):
                    self.p.append(CmdParam(p))
            if len(self.p) > 0:
                if len(self.p) == 1 and self.p[0].type == 'v':
                    self.isVoid = True
                    self.voidCheckBox.SetValue(True)
                else:
                    self.selIdx = 0
                    #self.updateParamList()
            else:
                self.p.append(CmdParam())
                self.isVoid = True
                self.voidCheckBox.SetValue(True)
        else:
            self.p.append(CmdParam())
            self.isVoid = True
            self.voidCheckBox.SetValue(True)
        self.updateByVoid()

    def updateByVoid(self):
        for w in self.vList: w.Enable(not self.isVoid)
        if not self.isVoid:
            if len(self.p) == 0 or len(self.p) == 1 and self.p[0].type == 'v':
                self.p = [CmdParam('i', 'name')]
                self.selIdx = 0
            self.updateParamList()

    def OnUseVoid(self, evt):
        if evt.IsChecked():
            self.isVoid = True
        else:
            self.isVoid = False
        self.updateByVoid()

    def updateParamList(self):
        self.paramList.Clear()
        for i in range(len(self.p)):
            self.paramList.Append(repr(self.p[i]), i)
        self.paramList.SetSelection(self.selIdx)
        self.selIdx = -1
        self.updateFormatPanel()

    def updateFormatPanel(self):
        idx = self.paramList.GetClientData(self.paramList.GetSelection())
        if not idx: idx = 0
        if self.selIdx != idx:
            if len(self.p) > 0:
                self.selIdx = idx
                param = self.p[idx]

                # type
                t = param.type
                if t in TYPES_LIST:
                    self.paramFormat.SetSelection(TYPES_LIST.index(t))
    
                # name
                self.paramName.SetValue(self.p[idx].name)
    
                # attributes
                self.attrList.DeleteAllItems()
                for a, v in param.attribute:
                    index = self.attrList.InsertStringItem(sys.maxint, a)
                    self.attrList.SetStringItem(index, 0, a)
                    self.attrList.SetStringItem(index, 1, v)
                    #self.attrList.SetItemData(index, a)
            else:
                self.paramFormat.SetSelection(None)
                self.paramName.SetValue("")
                self.attrList.DeleteAllItems()
                return

        # update param arrows
        if idx == 0:
            self.btnParamUp.Enable(False)
        else:
            self.btnParamUp.Enable(True)

        if idx == len(self.p) - 1:
            self.btnParamDown.Enable(False)
        else:
            self.btnParamDown.Enable(True)

    def OnSelParam(self, evt):
        self.updateFormatPanel()

    def OnSelAttr(self, evt):
        item = evt.m_itemIndex

    def OnAddParam(self, evt):
        idx = len(self.p)
        self.p.append(CmdParam('i', 'name'))
        self.paramList.Append(repr(self.p[idx]), idx)
        self.paramList.SetSelection(idx)
        #self.selIdx = -1
        self.updateFormatPanel()

    def OnDelParam(self, evt):
        if 0 <= self.selIdx < len(self.p):
            del self.p[self.selIdx]
            self.paramList.Delete(self.selIdx)
            if len(self.p) > 0:
                if self.selIdx == len(self.p):
                    self.paramList.SetSelection(len(self.p) - 1)
                else:
                    self.paramList.SetSelection(self.selIdx)
                self.selIdx = -1
                self.updateFormatPanel()
            else:
                self.isVoid = True
                self.voidCheckBox.SetValue(True)
                self.updateByVoid()
