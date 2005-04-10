#--------------------------------------------------------------------------
# cmdeditorpanel.py
# Cmd file editor panel for Nebula 2 buildsys
# (c) 2005 Oleg Kreptul (Haron)
# Contents are licensed under the Nebula license.
#--------------------------------------------------------------------------

import wx, string, os.path
import wx.lib.rcsizer as rcs
import wx.gizmos as gizmos

from cmdparser import *
from cmddatapanels import *

#----------------------------------------------------------------------
class CmdEditorPanel(wx.Panel):
    
    def __init__(self, parentWindow, buildSys):
        wx.Panel.__init__(self, parentWindow)
        self.buildSys = buildSys
        codeDir = buildSys.GetAbsPathFromRel('code')

        self.tree = gizmos.TreeListCtrl(self, -1, style =
                                        #wx.TR_DEFAULT_STYLE
                                        wx.TR_TWIST_BUTTONS
                                        #| wx.TR_ROW_LINES
                                        #| wx.TR_NO_LINES 
                                        #| wx.TR_AQUA_BUTTONS
                                        | wx.TR_HIDE_ROOT
                                        | wx.TR_FULL_ROW_HIGHLIGHT
                                        | wx.TR_NO_BUTTONS

                                        # By default the style will be adjusted on
                                        # Mac to use twisty buttons and no lines.  If
                                        # you would rather control this yourself then
                                        # add this style.
                                        #| wx.TR_DONT_ADJUST_MAC
                                   )
        self.tree.SetMinSize((320,300))

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

        main_modules_list = os.listdir(codeDir)
        main_modules_list = [d for d in main_modules_list\
                             if d.upper() != "CVS" and\
                                d.lower() != "contrib" and\
                                os.path.isdir(os.path.join(codeDir, d))]
        main_modules_list.sort()

        self.tree.SetItemText(main_modules, repr(len(main_modules_list)) + " modules", 1)

        # contrib block
        contrib_modules = self.tree.AppendItem(self.root, "<contrib>")
        self.tree.SetItemImage(contrib_modules, fldridx, which = wx.TreeItemIcon_Normal)
        self.tree.SetItemImage(contrib_modules, fldropenidx, which = wx.TreeItemIcon_Expanded)

        contrib_path = os.path.join(codeDir, 'contrib')
        contrib_modules_list = os.listdir(contrib_path)
        contrib_modules_list = [d for d in contrib_modules_list\
                                if d.upper() != "CVS" and\
                                   os.path.isdir(os.path.join(contrib_path, d))]
        contrib_modules_list.sort()

        self.tree.SetItemText(contrib_modules, repr(len(contrib_modules_list)) + " modules", 1)

        # fill the tree
        block = [[codeDir, main_modules, main_modules_list],
                 [contrib_path, contrib_modules, contrib_modules_list]]
        for bl in block:
            for mod_name in bl[2]:
                mod = self.tree.AppendItem(bl[1], mod_name)
                packages_path = os.path.join(bl[0], mod_name, 'src')
                if not os.path.exists(packages_path): continue

                packages_list = os.listdir(packages_path)
                
                packages_list = [d for d in packages_list\
                                 if d.upper() != "CVS" and\
                                    os.path.isdir(os.path.join(packages_path, d))]
                packages_list.sort()
    
                self.tree.SetItemText(mod, repr(len(packages_list)) + " packages", 1)
                self.tree.SetItemImage(mod, fldridx, which = wx.TreeItemIcon_Normal)
                self.tree.SetItemImage(mod, fldropenidx, which = wx.TreeItemIcon_Expanded)

                self.tree.Expand(mod)
                
                for pkg_name in packages_list:
                    pkg = self.tree.AppendItem(mod, pkg_name)
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
                        self.tree.SetItemImage(file, fileidx, which = wx.TreeItemIcon_Normal)
                        #self.tree.SetItemImage(file, smileidx, which = wx.TreeItemIcon_Selected)

        self.tree.Expand(self.root)
        self.tree.Expand(main_modules)
        self.tree.Expand(contrib_modules)
        
        # buttons panel
        buttonsPanel = wx.Panel(self, size = (-1, 50))
        self.fileName = wx.StaticText(buttonsPanel, -1, '')
        preView = wx.Button(buttonsPanel, -1, "Preview")
        self.Bind(wx.EVT_BUTTON, self.OnCmdFilePreView, preView)
        
        sizerD = wx.BoxSizer(wx.VERTICAL)
        sizerD.Add(self.fileName, 0, wx.ALL, 4)
        sizerD.Add(preView, 0, wx.ALL, 4)
        sizerD.Fit(buttonsPanel)
        buttonsPanel.SetSizer(sizerD)

        # panel for parameters
        self.workPanel = wx.Panel(self, style = wx.SUNKEN_BORDER, size=(350,200))
        self.workPanel.SetMinSize((350, 350))
        
        # panel for status info
        statusPanel = wx.Panel(self, style = wx.SUNKEN_BORDER)
        statusPanel.SetMaxSize((-1, 5))

        self.statusLine = wx.StaticText(statusPanel, -1, "")

        # layout
        sizerA = wx.BoxSizer(wx.VERTICAL)
        sizerA.Add(buttonsPanel, 0, wx.EXPAND, 4)
        sizerA.Add(self.workPanel, 1, wx.EXPAND, 4)

        sizerB = wx.BoxSizer(wx.HORIZONTAL)
        sizerB.Add(self.tree, 0, wx.EXPAND, 4)
        sizerB.Add(sizerA, 1, wx.EXPAND, 4)

        sizerC = wx.BoxSizer(wx.VERTICAL)
        sizerC.Add(sizerB, 1, wx.EXPAND, 4)
        sizerC.Add(statusPanel, 0, wx.EXPAND, 4)

        sizerC.Fit(self)
        self.SetSizer(sizerC)

        # popup stuff
        self.popupParseID = wx.NewId()
        self.Bind(wx.EVT_MENU, self.OnPopupParse, id=self.popupParseID)

        self.tree.GetMainWindow().Bind(wx.EVT_RIGHT_UP, self.OnRightUp)
        self.tree.GetMainWindow().Bind(wx.EVT_TREE_SEL_CHANGED, self.OnSelChanged)
        #self.tree.GetMainWindow().Bind(wx.EVT_LEFT_DCLICK, self.OnLeftDClick)
        
        # cmd parser & configs
        self.parser = CmdFileProcessor(codeDir)
        self.cmd_configs = []
        self.active_cmdfile = None
        
        self.parsing_dlg = None

    def OnCmdFilePreView(self, evt):
        dlg = wx.lib.dialogs.ScrolledMessageDialog(self, repr(self.GetConfig()),
                                                   'Cmd file preview')
        dlg.ShowModal()

    def GetConfig(self, item = None):
        if not item:
            item = self.active_cmdfile
        if not item: return None
        n = self.tree.GetItemPyData(item)
        if isinstance(n, IntType) and 0 <= n < len(self.cmd_configs):
            return self.cmd_configs[n]
        return None

    def ShowGeneralInfoPanel(self):
        cfg = self.GetConfig()
        self.ClearWorkPanel()
        if cfg:
            GeneralInfoPanel(self.workPanel, cfg)
        else:
            wx.StaticText(self.workPanel, -1,
                          'Right click on file name to parse it.')

    def ShowCommandPanel(self, cmd_name):
        cfg = self.GetConfig()
        self.ClearWorkPanel()
        if cfg:
            CommandPanel(self.workPanel, cfg, cmd_name)

    def ClearWorkPanel(self):
        self.workPanel.DestroyChildren()

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

        if len(tree_path) > 3:
            self.active_cmdfile = items[3]
            self.fileName.SetLabel(tree_path[3])

        if len(tree_path) == 4: # cmd file choosen
            s = 'path: nebula2/code/'
            if tree_path[0] == '<contrib>': s += 'contrib/'
            s += tree_path[1] + '/src/' + tree_path[2] + '/' + tree_path[3]
            self.statusLine.SetLabel(s)
            self.ShowGeneralInfoPanel()
        elif len(tree_path) == 5: # one of cmds or properties choosen
            self.statusLine.SetLabel('Command or property choosen: ' + tree_path[-1])
            modif, cmd_name = tree_path[-1].split()
            if modif[0] == 'c': self.ShowCommandPanel(cmd_name)
        elif len(tree_path) == 6: # one of setters/getters
            self.statusLine.SetLabel('Setter or getter choosen: ' + tree_path[-1])
            self.ClearWorkPanel()

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

        if len(items) == 4 and not self.GetConfig(items[3]):
            self.active_cmdfile = items[3]
            n += 1
            menu.Append(self.popupParseID, "Parse")

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
        config = self.parser.process(is_contrib, tree_path[1], tree_path[2], tree_path[3])
        if self.parser.processed:
            n = len(self.cmd_configs)
            self.cmd_configs.append(config)
            self.tree.SetItemPyData(self.active_cmdfile, n)
            self.OnCmdFileParsed()
            self.ShowGeneralInfoPanel()
        self.statusLine.SetLabel('Parsed.')

    def OnSize(self, evt):
        self.tree.SetSize(self.GetSize())

    def OnCmdFileParsed(self):
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
        
        self.tree.Expand(self.active_cmdfile)
##        self.tree.SelectItem(gi, True)
        self.tree.SelectItem(self.active_cmdfile, True)

#--------------------------------------------------------------------------
# EOF
#--------------------------------------------------------------------------
