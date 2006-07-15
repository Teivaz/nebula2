#-------------------------------------------------------------------------------
# aboutdialog.py
#-------------------------------------------------------------------------------

import wx
import wx.lib.hyperlink as hyperlink

class AboutDialog(wx.Dialog):
    def __init__(self, parent):
        pre = wx.PreDialog()
        pre.Create(parent, title = 'About mangalore level editor...')
        self.PostCreate(pre)
        text = wx.StaticText(self, label = 'Mangalore Level Editor 1.0')
        text.SetFont(wx.Font(14, wx.DEFAULT, wx.NORMAL, wx.NORMAL))
        mailto = hyperlink.HyperLinkCtrl(self, wx.ID_ANY, 'Email: johndotsmith@hotmail.com',
                                         URL = 'mailto://johndotsmith@hotmail.com')
        line = wx.StaticLine(self, size = (300, -1))
        okBtn = wx.Button(self, wx.ID_OK)
        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(text, 1, wx.ALIGN_CENTRE | wx.ALL, 10)
        sizer.Add(mailto, 1, wx.ALIGN_CENTRE | wx.LEFT | wx.RIGHT | wx.UP, 10)
        sizer.Add(line, 0, wx.LEFT | wx.RIGHT, 20)
        bsizer = wx.StdDialogButtonSizer()
        bsizer.AddButton(okBtn)
        bsizer.Realize()
        sizer.Add(bsizer, 0, wx.ALIGN_CENTER | wx.ALL, 15)
        self.SetSizer(sizer)
        sizer.Fit(self)

#-------------------------------------------------------------------------------
# Eof
#-------------------------------------------------------------------------------