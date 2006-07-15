import time
import wx
import pynebula as nebula

class NOHTree(wx.TreeCtrl):
    """
    This wxTreeCtrl derivative displays a tree view of nebula's namespace.
    """
    def __init__(self, parent, id, root):
        wx.TreeCtrl.__init__(self, parent, id)
        self.root = self.AddRoot(root.getname(), -1, -1, wx.TreeItemData(root))
 
        if root.gethead() != None:
            self.SetItemHasChildren(self.root, True)
        self.Bind(wx.EVT_TREE_ITEM_EXPANDING, self.OnItemExpanding, self)
        self.Bind(wx.EVT_TREE_ITEM_COLLAPSED, self.OnItemCollapsed, self)
        self.Bind(wx.EVT_TREE_SEL_CHANGED, self.OnSelChanged, self)
        self.output = None
        self.SelectionChangedCallback = None

    def SetSelectionChangedCallback(self, callback):
        self.SelectionChangedCallback = callback

    def OnItemExpanding(self, event):
        item = event.GetItem()
        if self.IsExpanded(item):
            # This event can happen twice in the self.Expand call
            return
        obj = self.GetPyData(item)
        node = obj.gethead()
        while node != None:
            obj = nebula.sel(node.getfullname())
            new_item = self.AppendItem(item, obj.getname(), -1, -1,
                                      wx.TreeItemData(obj))
            if obj.gethead() != None:
                self.SetItemHasChildren(new_item, True)
            node = obj.getsucc()

    def OnItemCollapsed(self, event):
        item = event.GetItem()
        self.DeleteChildren(item)

    def OnSelChanged(self, event):
        obj = self.GetPyData(event.GetItem())
        if self.SelectionChangedCallback is not None:
            self.SelectionChangedCallback(obj)


class ParamEditCtrl:
    def __init__(self, parent, dataSource, paramName, paramType):
        # find position to place ourselves
        xPos = 0
        yPos = 0
        for c in parent.paramEditCtrls:
            yPos += c.GetHeight()
        # create label
        self.label = wx.StaticText(parent, -1, paramName, wx.Point(xPos, yPos))
        #create edit
        xPos += 100
        if paramType == "f":
            self.edit = wx.TextCtrl(parent, -1, str(dataSource.getfloat(paramName)), wx.Point(xPos, yPos))
            self.edit.Bind(wx.EVT_TEXT, self.OnFloatChanged, self.edit)
        elif paramType == "i":
            self.edit = wx.TextCtrl(parent, -1, str(dataSource.getint(paramName)), wx.Point(xPos, yPos))
            self.edit.Bind(wx.EVT_TEXT, self.OnIntChanged, self.edit)
        elif paramType == "f4": 
            # create 4 float edits, and a button with a colour that launches a wx.ColourDialog
            c = dataSource.getvector(paramName)
            self.edit = wx.Button(parent, -1, "Edit RGB", wx.Point(xPos, yPos))
            self.edit.Bind(wx.EVT_BUTTON, self.OnF4Clicked, self.edit)
            self.F4SetButtonColour(c)
            xPos += self.edit.GetSize().width + 3
            self.edit0 = wx.TextCtrl(parent, -1, str(c[0]), wx.Point(xPos, yPos))
            self.edit0.Bind(wx.EVT_TEXT, self.OnF4Changed, self.edit0)            
            xPos += self.edit0.GetSize().width + 3
            self.edit1 = wx.TextCtrl(parent, -1, str(c[1]), wx.Point(xPos, yPos))
            self.edit1.Bind(wx.EVT_TEXT, self.OnF4Changed, self.edit1)
            xPos += self.edit1.GetSize().width + 3            
            self.edit2 = wx.TextCtrl(parent, -1, str(c[2]), wx.Point(xPos, yPos))
            self.edit2.Bind(wx.EVT_TEXT, self.OnF4Changed, self.edit2)
            xPos += self.edit2.GetSize().width + 3
            self.edit3 = wx.TextCtrl(parent, -1, str(c[3]), wx.Point(xPos, yPos))
            self.edit3.Bind(wx.EVT_TEXT, self.OnF4Changed, self.edit3)
        self.dataSource = dataSource       
        self.paramName = paramName
        self.paramType = paramType

    def GetHeight(self):
        if self.paramType == "f" or self.paramType == "i" or self.paramType == "f4":
            return self.edit.GetSize().height
        else:
            return self.label.GetSize().height
            
    def OnFloatChanged(self, event):
        try:
            self.dataSource.setfloat(self.paramName, float(self.edit.GetValue()))
        except ValueError:
            self.dataSource.setfloat(self.paramName, 0.0)
    
    def OnIntChanged(self, event):
        try:
            self.dataSource.setint(self.paramName, int(self.edit.GetValue()))
        except ValueError:
            self.dataSource.setint(self.paramName, 0)

    def OnF4Clicked(self, event):
        colourDialog = wx.ColourDialog(self.edit.GetParent())
        cdata = colourDialog.GetColourData()
        cdata.SetChooseFull(1)
        cdata.SetColour(self.edit.GetBackgroundColour())
        if colourDialog.ShowModal() == wx.ID_OK:
            colour = colourDialog.GetColourData().GetColour()
            self.edit.SetBackgroundColour(colour)
            self.edit0.SetValue(str(colour.Red() / 255.0))
            self.edit1.SetValue(str(colour.Green() / 255.0))
            self.edit2.SetValue(str(colour.Blue() / 255.0))

    def OnF4Changed(self, event):
        c = [0.0, 0.0, 0.0, 0.0]
        try:
            c[0] = float(self.edit0.GetValue())
        except ValueError:
            pass    
        try:
            c[1] = float(self.edit1.GetValue())
        except ValueError:
            pass    
        try:
            c[2] = float(self.edit2.GetValue())
        except ValueError:
            pass    
        try:
            c[3] = float(self.edit3.GetValue())
        except ValueError:
            pass    
        self.dataSource.setvector(self.paramName, c[0], c[1], c[2], c[3])        
        self.F4SetButtonColour(c)

    def F4SetButtonColour(self, c):
        colour = wx.Colour(max(min(c[0], 1.0), 0.0) * 255, max(min(c[1], 1.0), 0.0) * 255, max(min(c[2], 1.0), 0.0) * 255)
        self.edit.SetBackgroundColour(colour)

class MainWindowFrame(wx.Frame):
    def __init__(self):
        wx.Frame.__init__(self, None, -1, "Nebula WX Viewer",
                            wx.DefaultPosition, wx.Size(1024, 768))
        self.myApplication = None
        self.tree = NOHTree(self, -1, nebula.sel('/'))
        self.tree.SetSelectionChangedCallback(self.OnNOHSelectionChanged)
        self.Bind(wx.EVT_CLOSE, self.OnCloseWindow)

        self.nebulaRenderPanel = wx.Panel(self, -1, wx.Point(0, 0),
                                          wx.Size(640, 480), wx.SUNKEN_BORDER,
                                          "NebulaRender")
        self.nebulaRenderPanel.SetMinSize(wx.Size(640, 480))
        parentHwndEnv = nebula.new('nenv', '/sys/env/parent_hwnd')
        parentHwndEnv.seti(self.nebulaRenderPanel.GetHandle())
        self.propertyPanel = wx.Notebook(self, -1, wx.DefaultPosition,
                                        wx.Size(640, 288), wx.SUNKEN_BORDER,
                                        "Properties")
        # Set up layout
        hSizer = wx.BoxSizer(wx.HORIZONTAL)
        hSizer.Add(self.tree, 1, wx.EXPAND)
        rSizer = wx.BoxSizer(wx.VERTICAL)
        rSizer.Add(self.nebulaRenderPanel, 0, wx.EXPAND)
        rSizer.Add(self.propertyPanel, 1, wx.EXPAND)
        hSizer.Add(rSizer, 0, wx.EXPAND)
        self.SetSizer(hSizer)
        # Select the root node by default
        self.tree.SelectItem(self.tree.root)

    def SetApplication(self, application):
        self.myApplication = application

    def OnNOHSelectionChanged(self, object):
        self.propertyPanel.DeleteAllPages()
        for className in object.getclasses():
            classPanel = wx.Panel(self.propertyPanel, -1, name=className)
            self.propertyPanel.AddPage(classPanel, className)
            if className == "nabstractshadernode":
                classPanel.paramEditCtrls = []
                for p in object.getparams():
                    classPanel.paramEditCtrls.append(ParamEditCtrl(classPanel, object, p[0], p[1]))

    def OnCloseWindow(self, event):
        self.myApplication.keepGoing = False
        self.Destroy()

