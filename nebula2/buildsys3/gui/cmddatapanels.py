#--------------------------------------------------------------------------
# cmddatapanels.py
# Panels for editing cmd file elements
# (c) 2005 Oleg Kreptul (Haron)
# Contents are licensed under the Nebula license.
#--------------------------------------------------------------------------

import wx
import wx.lib.dialogs

class GeneralInfoPanel:
    def __init__(self, parent, cfg):
        self.parent = parent
        self.cfg = cfg
        hdr = cfg.header
        cli = cfg.classInfo
        scf = cfg.saveCmdsFunc

        h = 0
        p = 5
        w = parent.GetSize().width - 2 * p

        # Header data
        headerText = wx.StaticText(parent, -1, 'Header data', pos=(p, h))
        h += headerText.GetSize().height + p

        self.hdr_info = wx.TextCtrl(parent, -1, hdr.generalInfo,
                                    style=wx.TE_MULTILINE|wx.TE_READONLY,
                                    size=(w, 50), pos=(p, h))
        h += self.hdr_info.GetSize().height + p

        line1 = wx.StaticLine(parent, -1, size=(w,-1), pos=(p, h))
        h += line1.GetSize().height + p

        # Includes
        includesText = wx.StaticText(parent, -1, 'Includes', pos=(p, h))
        h += includesText.GetSize().height + p

        self.includes = wx.ListBox(parent, -1, choices=hdr.includes,
                                   style=wx.LB_SINGLE,
                                   size=(w, 50), pos=(p, h))
        h += self.includes.GetSize().height + p

        line2 = wx.StaticLine(parent, -1, size=(w,-1), pos=(p, h))
        h += line2.GetSize().height + p

        # Class info
        classInfoText = wx.StaticText(parent, -1, 'Class info', pos=(p, h))
        h += classInfoText.GetSize().height + p

        scriptClassText = wx.StaticText(parent, -1, '@scriptclass ', pos=(p, h))
        sz = scriptClassText.GetSize()
        self.scriptClass = wx.TextCtrl(parent, -1, cli.scriptClass,
                                style=wx.TE_READONLY,
                                size=(w - sz.width, -1), pos=(p + sz.width, h))
        h += max(sz.height, self.scriptClass.GetSize().height) + p

        cppClassText = wx.StaticText(parent, -1, '@cppclass ', pos=(p, h))
        sz = cppClassText.GetSize()
        self.cppClass = wx.TextCtrl(parent, -1, cli.cppClass,
                                style=wx.TE_READONLY,
                                size=(w - sz.width, -1), pos=(p + sz.width, h))
        h += max(sz.height, self.cppClass.GetSize().height) + p

        superClassText = wx.StaticText(parent, -1, '@superclass ', pos=(p, h))
        sz = superClassText.GetSize()
        self.superClass = wx.TextCtrl(parent, -1, cli.superClass,
                                      style=wx.TE_READONLY,
                                      size=(w - sz.width, -1), pos=(p + sz.width, h))
        h += max(sz.height, self.superClass.GetSize().height) + p

        classInfoText = wx.StaticText(parent, -1, '@classinfo ', pos=(p, h))
        sz = classInfoText.GetSize()
        self.classInfo = wx.TextCtrl(parent, -1, cli.info,
                                     style=wx.TE_MULTILINE|wx.TE_READONLY,
                                     size=(w - sz.width, 50), pos=(p + sz.width, h))
        h += max(sz.height, self.classInfo.GetSize().height) + p

        # SaveCmds function
        if scf:
            line3 = wx.StaticLine(parent, -1, size=(w,-1), pos=(p, h))
            h += line3.GetSize().height + p
    
            scView = wx.Button(parent, -1, "SaveCmds preview", pos=(p, h))
            parent.Bind(wx.EVT_BUTTON, self.OnSaveCmdsView, scView)
            h += scView.GetSize().height + p

        # FIXME: sizers doesn't work here. why?
##        sizerA = wx.BoxSizer(wx.VERTICAL)
##        sizerA.Add(headerText, 0, wx.ALIGN_LEFT|wx.ALL, 4)
##        sizerA.Add(self.info, 0, wx.ALL, 4)
##        sizerA.Add(wx.StaticLine(parent, -1, size=(1024,-1)), 0, wx.ALL, 4)
##
##        sizerA.Fit(parent)
##        parent.SetSizer(sizerA)

    def OnSaveCmdsView(self, evt):
        dlg = wx.lib.dialogs.ScrolledMessageDialog(self.parent,
                                repr(self.cfg.saveCmdsFunc), 'SaveCmds function')
        dlg.ShowModal()

##        dlg = wx.MessageDialog(self.parent, repr(self.cfg.saveCmdsFunc),
##                               'SaveCmds function', wx.OK)
##        dlg.ShowModal()
##        dlg.Destroy()

class CommandPanel:
    def __init__(self, parent, cfg, cmd_name):
        self.cmd = cfg.findCmd(cmd_name)
        if not self.cmd: return
        self.parent = parent
        self.cfg = cfg

        h = 0
        p = 5
        w = parent.GetSize().width - 2 * p

        outFormatLabel = wx.StaticText(self.parent, -1, 'Output format ', pos=(p, h))
        sz = outFormatLabel.GetSize()
        self.outFormat = wx.TextCtrl(self.parent, -1, self.cmd.outFormat,
                                   style=wx.TE_READONLY,
                                   size=(w - sz.width, -1), pos=(p + sz.width, h))
        h += max(sz.height, self.outFormat.GetSize().height) + p

        inFormatLabel = wx.StaticText(self.parent, -1, 'Input format ', pos=(p, h))
        sz = inFormatLabel.GetSize()
        self.inFormat = wx.TextCtrl(self.parent, -1, self.cmd.inFormat,
                                   style=wx.TE_READONLY,
                                   size=(w - sz.width, -1), pos=(p + sz.width, h))
        h += max(sz.height, self.inFormat.GetSize().height) + p

        fourCCLabel = wx.StaticText(self.parent, -1, 'FourCC ', pos=(p, h))
        sz = fourCCLabel.GetSize()
        self.fourCC = wx.TextCtrl(self.parent, -1, self.cmd.fourCC,
                                   style=wx.TE_READONLY,
                                   size=(w - sz.width, -1), pos=(p + sz.width, h))
        h += max(sz.height, self.fourCC.GetSize().height) + p

        cmdNameLabel = wx.StaticText(self.parent, -1, '@cmd ', pos=(p, h))
        sz = cmdNameLabel.GetSize()
        self.cmdName = wx.TextCtrl(self.parent, -1, self.cmd.cmdName,
                                   style=wx.TE_READONLY,
                                   size=(w - sz.width, -1), pos=(p + sz.width, h))
        h += max(sz.height, self.cmdName.GetSize().height) + p

        cmdInfoLabel = wx.StaticText(self.parent, -1, '@info ', pos=(p, h))
        sz = cmdInfoLabel.GetSize()
        self.cmdInfo = wx.TextCtrl(self.parent, -1, self.cmd.info,
                                   style=wx.TE_MULTILINE|wx.TE_READONLY,
                                   size=(w - sz.width, 50), pos=(p + sz.width, h))
        h += max(sz.height, self.cmdInfo.GetSize().height) + p

        cmdView = wx.Button(parent, -1, "Cmd preview", pos=(p + sz.width, h))
        self.parent.Bind(wx.EVT_BUTTON, self.OnCmdView, cmdView)

    def OnCmdView(self, evt):
        dlg = wx.lib.dialogs.ScrolledMessageDialog(self.parent,
                                repr(self.cmd), 'Command function')
        dlg.ShowModal()