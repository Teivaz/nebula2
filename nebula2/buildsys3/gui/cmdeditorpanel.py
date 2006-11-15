#--------------------------------------------------------------------------
# cmdeditorpanel.py
# Cmd file editor panel for Nebula 2 buildsys
# (c) 2005 Oleg Kreptul (Haron) okreptul@yahoo.com
# Contents are licensed under the Nebula license.
#--------------------------------------------------------------------------

import wx, string, os.path
import wx.lib.rcsizer as rcs
import wx.gizmos as gizmos

from cmdparser import *
from cmddatapanels import *

sel_type_commoncmd = 0
sel_type_cmd = 1
sel_type_property = 2
sel_type_signal = 3

#----------------------------------------------------------------------
class CmdEditorPanel(wx.Panel):
    def __init__(self, parent, buildSys):
        self.buildSys = buildSys
        self.codeDir = buildSys.GetAbsPathFromRel('code')

        self.xrcRootItem = None
        xrcLoadPanel(self, parent, 'CmdEditorPanel')

        # extracting controls
        treePanel       = xrcCTRLUnpack(self, "TreePanel")
        self.fileName   = xrcCTRLUnpack(self, "FileName")
        self.previewBtn = xrcCTRLUnpack(self, "Preview", {wx.EVT_BUTTON:self.OnPreview})
        self.convertBtn = xrcCTRLUnpack(self, "Convert", {wx.EVT_BUTTON:self.OnConvert})
        self.applyBtn   = xrcCTRLUnpack(self, "Apply", {wx.EVT_BUTTON:self.OnApply})
        self.workPanel  = xrcCTRLUnpack(self, "WorkPanel")
        self.statusLine = xrcCTRLUnpack(self, "StatusLine")

        self.tree = gizmos.TreeListCtrl(treePanel, -1,
                    style = wx.TR_HIDE_ROOT|wx.TR_FULL_ROW_HIGHLIGHT|wx.TR_NO_BUTTONS)
        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(self.tree, 1, wx.EXPAND|wx.ALL)
        sizer.Fit(treePanel)
        treePanel.SetSizer(sizer)

        # configuring controls
        self.InitTree()
        self.tree.GetMainWindow().Bind(wx.EVT_RIGHT_UP, self.OnRightUp)
        self.tree.GetMainWindow().Bind(wx.EVT_TREE_SEL_CHANGED, self.OnSelChanged)
        self.tree.GetMainWindow().Bind(wx.EVT_TREE_ITEM_EXPANDED, self.OnItemExpanded)
        #self.tree.GetMainWindow().Bind(wx.EVT_LEFT_DCLICK, self.OnLeftDClick)

        self.workSubPanel = None

        # popup stuff
        self.popupParseID = wx.NewId()
        self.Bind(wx.EVT_MENU, self.OnPopupParse, id=self.popupParseID)

        # cmd parser & configs
        self.parser = CmdFileProcessor(self.codeDir)
        self.cmd_configs = []
        self.active_cmdfile = None

        self.parsing_dlg = None

    def InitTree(self):
        isz = (16,16)
        il = wx.ImageList(isz[0], isz[1])
        fldridx     = il.Add(wx.ArtProvider_GetBitmap(wx.ART_FOLDER,      wx.ART_OTHER, isz))
        fldropenidx = il.Add(wx.ArtProvider_GetBitmap(wx.ART_FILE_OPEN,   wx.ART_OTHER, isz))
        fileidx     = il.Add(wx.ArtProvider_GetBitmap(wx.ART_REPORT_VIEW, wx.ART_OTHER, isz))
        #smileidx    = il.Add(images.getSmilesBitmap())

        self.tree.SetImageList(il)
        self.il = il

        # create some columns
        self.tree.AddColumn("")
        self.tree.AddColumn("")
        self.tree.SetMainColumn(0) # the one with the tree in it...
        self.tree.SetColumnWidth(0, 200)
        self.tree.SetColumnWidth(1, 100)

        self.root = self.tree.AddRoot("Nebula 2 cmds")
        self.tree.SetItemImage(self.root, fldridx, which = wx.TreeItemIcon_Normal)
        self.tree.SetItemImage(self.root, fldropenidx, which = wx.TreeItemIcon_Expanded)

        # main block
        main_modules = self.tree.AppendItem(self.root, "<main>")
        self.tree.SetItemImage(main_modules, fldridx, which = wx.TreeItemIcon_Normal)
        self.tree.SetItemImage(main_modules, fldropenidx, which = wx.TreeItemIcon_Expanded)

        main_modules_list = os.listdir(self.codeDir)
        main_modules_list = [d for d in main_modules_list\
                             if d.upper() != "CVS" and d.lower() != ".svn" and\
                                d.lower() != "contrib" and\
                                d.lower() != "doxycfg" and\
                                os.path.isdir(os.path.join(self.codeDir, d))]
        main_modules_list.sort()

        self.tree.SetItemText(main_modules, repr(len(main_modules_list)) + " modules", 1)

        # contrib block
        contrib_modules = self.tree.AppendItem(self.root, "<contrib>")
        self.tree.SetItemImage(contrib_modules, fldridx, which = wx.TreeItemIcon_Normal)
        self.tree.SetItemImage(contrib_modules, fldropenidx, which = wx.TreeItemIcon_Expanded)

        contrib_path = os.path.join(self.codeDir, 'contrib')
        contrib_modules_list = os.listdir(contrib_path)
        contrib_modules_list = [d for d in contrib_modules_list\
                                if d.upper() != "CVS" and d.lower() != ".svn" and\
                                   os.path.isdir(os.path.join(contrib_path, d))]
        contrib_modules_list.sort()

        self.tree.SetItemText(contrib_modules, repr(len(contrib_modules_list)) + " modules", 1)

        # fill the tree
        block = [[self.codeDir, main_modules, main_modules_list],
                 [contrib_path, contrib_modules, contrib_modules_list]]
        for bl in block:
            for mod_name in bl[2]:
                packages_path = os.path.join(bl[0], mod_name, 'src')
                if not os.path.exists(packages_path): continue
                mod = self.tree.AppendItem(bl[1], mod_name)
                self.tree.SetItemHasChildren(mod, True)

                packages_list = os.listdir(packages_path)

                packages_list = [d for d in packages_list\
                                 if d.upper() != "CVS" and d.lower() != ".svn" and\
                                    os.path.isdir(os.path.join(packages_path, d))]
                packages_list.sort()

                self.tree.SetItemText(mod, repr(len(packages_list)) + " packages", 1)
                self.tree.SetItemImage(mod, fldridx, which = wx.TreeItemIcon_Normal)
                self.tree.SetItemImage(mod, fldropenidx, which = wx.TreeItemIcon_Expanded)

                #self.tree.Expand(mod)

                for pkg_name in packages_list:
                    pkg = self.tree.AppendItem(mod, pkg_name)
                    self.tree.SetItemHasChildren(pkg, True)
                    files_path = os.path.join(packages_path, pkg_name + os.sep)
                    files_list = os.listdir(files_path)

                    files_list = [d for d in files_list\
                                  if os.path.isfile(os.path.join(files_path, d)) and d.endswith('_cmds.cc')]
                    files_list.sort()

                    self.tree.SetItemText(pkg, repr(len(files_list)) + " files", 1)
                    self.tree.SetItemImage(pkg, fldridx, which = wx.TreeItemIcon_Normal)
                    self.tree.SetItemImage(pkg, fldropenidx, which = wx.TreeItemIcon_Expanded)

                    for file_name in files_list:
                        file = self.tree.AppendItem(pkg, file_name)
                        self.tree.SetItemHasChildren(file, True)
                        self.tree.SetItemImage(file, fileidx, which = wx.TreeItemIcon_Normal)
                        #self.tree.SetItemImage(file, smileidx, which = wx.TreeItemIcon_Selected)

        self.tree.Expand(self.root)
        self.tree.Expand(main_modules)
        self.tree.Expand(contrib_modules)

    def OnPreview(self, evt):
        txt = None
        title = None
        if not self.subelement:
            txt = repr(self.GetConfig())
            title = 'Cmd file preview'
        elif self.subelement[0] == 0:
            cmd = self.GetConfig().findCmd(self.subelement[1])
            if cmd:
                txt = repr(cmd)
                title = 'Cmd preview'
        if txt:
            dlg = wx.lib.dialogs.ScrolledMessageDialog(self, txt, title)
            dlg.ShowModal()

    def OnConvert(self, evt):
        if self.subelement:
            if self.subelement[0] == sel_type_commoncmd:
                if self.workSubPanel.changed:
                    dlg = wx.MessageDialog(self, 'Command was changed. Save changes?',
                                           'Save changes', wx.OK | wx.CANCEL | wx.ICON_QUESTION)
                    val = dlg.ShowModal()

                    if val == wx.ID_OK:
                        self.log.WriteText("You pressed OK\n")
                    else:
                        self.log.WriteText("You pressed Cancel\n")

                    dlg.Destroy()
                cfg = self.GetConfig()

    def OnApply(self, evt):
        None

    def GetConfig(self, item = None):
        if not item:
            item = self.active_cmdfile
        if not item: return None
        n = self.tree.GetItemPyData(item)
        if isinstance(n, IntType) and 0 <= n < len(self.cmd_configs):
            return self.cmd_configs[n]
        return None

    def SetConfig(self, cfg, item = None):
        if not item:
            item = self.active_cmdfile
        if not item: return
        n = self.tree.GetItemPyData(item)
        if n and 0 <= n < len(self.cmd_configs):
            self.cmd_configs[n] = cfg
        else:
            n = len(self.cmd_configs)
            self.cmd_configs.append(cfg)
        self.tree.SetItemPyData(item, n)

    def ShowGeneralInfoPanel(self):
        cfg = self.GetConfig()
        if cfg:
            if not isinstance(self.workSubPanel, GeneralInfoPanel):
                self.ClearWorkPanel()
                self.workSubPanel = GeneralInfoPanel(self.workPanel)

                sizer = self.workPanel.GetSizer() #wx.BoxSizer(wx.VERTICAL)
                sizer.Add(self.workSubPanel, 0, wx.EXPAND|wx.ALL)
                w = self.workPanel.GetSize().width
                self.workSubPanel.SetSizeWH(w - 2, -1)

                self.convertBtn.Enable(False)
                self.applyBtn.Enable(False)

            self.workSubPanel.SetData(cfg)
        else:
            self.ClearWorkPanel()
            wx.StaticText(self.workPanel, -1,
                          'Right click on file name to parse it.')

    def ShowCommonCmdPanel(self):
        cfg = self.GetConfig()
        if cfg and self.subelement and self.subelement[0] == sel_type_commoncmd:
            if not isinstance(self.workSubPanel, CommonCmdPanel):
                self.ClearWorkPanel()
                self.workSubPanel = CommonCmdPanel(self.workPanel)

                sizer = self.workPanel.GetSizer() #wx.BoxSizer(wx.VERTICAL)
                sizer.Add(self.workSubPanel, 0, wx.EXPAND|wx.ALL)
                w = self.workPanel.GetSize().width
                self.workSubPanel.SetSizeWH(w - 2, -1)

                self.convertBtn.Enable(True)
                self.applyBtn.Enable(False)

            self.workSubPanel.SetData(cfg, self.subelement[1])
        else:
            self.ClearWorkPanel()
            wx.StaticText(self.workPanel, -1,
                          'ERROR: there is no cfg data.')

    def ClearWorkPanel(self):
        self.workPanel.DestroyChildren()
        self.workSubPanel = None

    def OnSelChanged(self, evt):
        item = evt.GetItem()

        items = []
        tree_path = []
        while item:
            tree_path.insert(0, self.tree.GetItemText(item, 0))
            items.insert(0, item)
            item = self.tree.GetItemParent(item)

        del tree_path[0] # delete root node
        del items[0]

        l = len(tree_path)

        if l > 3:
            self.active_cmdfile = items[3]
            self.fileName.SetLabel(tree_path[3])
        else:
            self.fileName.SetLabel('')
            self.ClearWorkPanel()

        self.subelement = None
        if l == 4: # cmd file choosen
            s = 'path: nebula2/code/'
            if tree_path[0] == '<contrib>': s += 'contrib/'
            s += tree_path[1] + '/src/' + tree_path[2] + '/' + tree_path[3]
            self.statusLine.SetLabel(s)
            self.ShowGeneralInfoPanel()
        elif l == 5: # one of cmds or properties choosen
            self.statusLine.SetLabel('Command or property choosen: ' + tree_path[-1])
            modif, cmd_name = tree_path[-1].split()
            if modif[0] == 'c':
                self.subelement = [sel_type_commoncmd, cmd_name]
                self.ShowCommonCmdPanel()
        elif l == 6: # one of setters/getters
            self.statusLine.SetLabel('Setter or getter choosen: ' + tree_path[-1])
            self.ClearWorkPanel()

    def OnItemExpanded(self, event):
        item = event.GetItem()

        tree_path = []
        i = item
        while i:
            tree_path.insert(0, self.tree.GetItemText(i, 0))
            i = self.tree.GetItemParent(i)

        del tree_path[0] # delete root node

        if len(tree_path) == 3:
            if self.tree.GetItemPyData(item) == None:
                cnum = self.tree.GetChildrenCount(item, False)
                if cnum > 0:
                    #print "Parsing begin(" + str(cnum) + ")..."
                    progressVal = 0
                    self.buildSys.CreateProgressDialog('Parsing cmds', ' ' * 130, cnum)
                    child, cookie = self.tree.GetFirstChild(item)
                    while child:
                        #print " - " + self.tree.GetItemText(child, 0)
                        self.active_cmdfile = child
                        fname = self.tree.GetItemText(child, 0)

                        is_contrib = False
                        if tree_path[0] == "<contrib>": is_contrib = True

                        #self.statusLine.SetLabel('Parsing ' + tree_path[3] + '...')
                        #print str(tree_path) + fname
                        self.tree.DeleteChildren(self.active_cmdfile)
                        config = self.parser.process(is_contrib, tree_path[1], tree_path[2], fname)
                        if self.parser.processed:
                            #n = len(self.cmd_configs)
                            #self.cmd_configs.append(config)
                            self.SetConfig(config)
                            #self.tree.SetItemPyData(self.active_cmdfile, n)
                            self.OnCmdFileParsed(False)
                            #self.ShowGeneralInfoPanel()
                        progressVal += 1
                        self.buildSys.UpdateProgressDialog(progressVal, 'Processing ' + fname)
                        if self.buildSys.ProgressDialogCancelled():
                            break
                        child, cookie = self.tree.GetNextChild(item, cookie)
                    self.buildSys.DestroyProgressDialog()
                    self.ShowGeneralInfoPanel()
                    #print "Parsing end..."
                self.active_cmdfile = None
                self.tree.SelectItem(item)
                self.tree.SetItemPyData(item, True)

    def OnRightUp(self, evt):
        pos = evt.GetPosition()
        item, flags, col = self.tree.HitTest(pos)

        items = []
        while item:
            items.insert(0, item)
            item = self.tree.GetItemParent(item)

        del items[0] # delete root node
##        print 'OnRightUp: ' + self.tree.GetItemText(items[-1], 0)

        # make a menu
        menu = wx.Menu()
        n = 0

        if len(items) == 4: # and not self.GetConfig(items[3]):
            self.active_cmdfile = items[3]
            n += 1
            menu.Append(self.popupParseID, "Reparse")

        # Popup the menu.  If an item is selected then its handler
        # will be called before PopupMenu returns.
        if n > 0:
            self.PopupMenu(menu, evt.GetPosition())

        menu.Destroy()

    def OnPopupParse(self, evt):
##        print 'OnPopupParse'

        item = self.active_cmdfile
        tree_path = []
        while item:
            tree_path.insert(0, self.tree.GetItemText(item, 0))
            item = self.tree.GetItemParent(item)

        del tree_path[0] # delete root node

        self.fileName.SetLabel(tree_path[3])

        is_contrib = False
        if tree_path[0] == "<contrib>": is_contrib = True

        self.statusLine.SetLabel('Parsing ' + tree_path[3] + '...')
        self.tree.DeleteChildren(self.active_cmdfile)
        config = self.parser.process(is_contrib, tree_path[1], tree_path[2], tree_path[3])
        if self.parser.processed:
            n = len(self.cmd_configs)
            #self.cmd_configs.append(config)
            self.SetConfig(config) #, self.active_cmdfile)
            #self.tree.SetItemPyData(self.active_cmdfile, n)
            self.OnCmdFileParsed()
            self.ShowGeneralInfoPanel()
        self.statusLine.SetLabel('Parsed.')

    def OnSize(self, evt):
        self.tree.SetSize(self.GetSize())

    def OnCmdFileParsed(self, expand = True):
        # general info
##        gi = self.tree.AppendItem(self.active_cmdfile, "<general info>")

        usedProperty = {}
        config = self.GetConfig()
        self.tree.SetItemText(self.active_cmdfile, str(len(config.cmds)) + " cmds", 1)
        for cmd in config.cmds:
            if isinstance(cmd, Cmd):
                propName = cmd.propertyName
                prop_item = None
                if not usedProperty.has_key(propName):
                    # add property
                    prop_item = self.tree.AppendItem(self.active_cmdfile, "p " + propName)
                    usedProperty[propName] = prop_item
                else: prop_item = usedProperty[propName]

                d = "g"
                if cmd.type == Cmd.T_GETTER: d = "s"
                if cmd.subType != Cmd.M_NONE:
                    d += ["", "c", "b", "a", "e"][c.subType]
                else:
                    d += " "

                self.tree.AppendItem(prop_item, d + " " + cmd.cmdName)
            else:
                self.tree.AppendItem(self.active_cmdfile, "c " + cmd.cmdName)

        # SaveCmds function
##        if config.saveCmdsFunc:
##            self.tree.AppendItem(self.active_cmdfile, "[SaveCmds]")

        if expand:
            self.tree.Expand(self.active_cmdfile)
##            self.tree.SelectItem(gi, True)
            self.tree.SelectItem(self.active_cmdfile, True)

#--------------------------------------------------------------------------
# EOF
#--------------------------------------------------------------------------
