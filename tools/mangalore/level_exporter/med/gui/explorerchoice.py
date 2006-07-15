#-------------------------------------------------------------------------------
# explorerchoice.py
#-------------------------------------------------------------------------------

import wx

class ExplorerChoice(wx.Choice):
    def __init__(self, parent):
        wx.Choice.__init__(self, parent, choices = medChoices.values())
        self.SetSelection(0)
        self.Bind(wx.EVT_CHOICE, self.onChoice)

# private
    def onChoice(self, event):
        self.GetParent().explorerTree.update(medChoices.keys()[self.GetSelection()])

medChoices = {'*.*' : 'All files (*.*)', '*.db3' : 'SQLite files (*.db3)', '*.xml' : 'XML files (*.xml)'}

#-------------------------------------------------------------------------------
# Eof
#-------------------------------------------------------------------------------