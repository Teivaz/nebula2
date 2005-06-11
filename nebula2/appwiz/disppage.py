#----------------------------------------------------------------------
#  disppage.py
#
#  (C)2005 Kim, Hyoun Woo
#----------------------------------------------------------------------
import wx
import wx.wizard as wiz
import glob, os, time
from common import *

#----------------------------------------------------------------------
#  Display setting page
#----------------------------------------------------------------------
class DispPage(wiz.WizardPageSimple):
    def __init__(self, parent, title):
        wiz.WizardPageSimple.__init__(self, parent)

        # create a sizer of this page.
        self.sizerA = makePageTitle(self, title)

        # window title
        self.windowTitleLabel = wx.StaticText(self, -1, 'Window Title')
        self.windowTitleTextBox = wx.TextCtrl(self, -1, '', (0, 0), (250, 21))

        # window type
        windowType = ['Windowed', 'Fullscreen', 'AlwaysonTop', 'ChildWindow']
        self.windowTypeLabel = wx.StaticText(self, -1, 'Window Type')
        self.windowTypeComboBox = wx.ComboBox(self, 500, 'Windowed', (0, 0),
            (200, 20), windowType, wx.CB_DROPDOWN #|wxTE_PROCESS_ENTER
            )

        # resolution
        self.width = 640
        self.height = 480
        self.resolutionLabel = wx.StaticText(self, -1, 'Resolution')
        resolutions = ['640x480', '800x600', '1024x768', '1280x1024']
        self.resolRadioBox = wx.RadioBox(self, -1, '', 
                wx.DefaultPosition, wx.DefaultSize,
                resolutions, 2, wx.RA_SPECIFY_COLS
                )
        self.Bind(wx.EVT_RADIOBOX, self.OnResolution, self.resolRadioBox)

        # vsync
        self.vsyncCheckBox = wx.CheckBox(self, -1, 'VSync', 
                                         style=wx.ALIGN_RIGHT)

        # Bpp(bits per pixel)
        bpp = ['Bpp16', 'Bpp32']
        self.windowBppLabel = wx.StaticText(self, -1, 'Bpp')
        self.windowBppComboBox = wx.ComboBox(self, 500, 'Bpp32', (0, 0),
            (200, 20), bpp, wx.CB_DROPDOWN #|wxTE_PROCESS_ENTER
            )

        # Layout
        sizerB = wx.GridBagSizer(4, 5)
        #
        sizerB.Add(self.windowTitleLabel, (0, 0), 
                   flag = wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL)
        sizerB.Add(self.windowTitleTextBox, (0, 1), flag = wx.EXPAND)
        #
        sizerB.Add(self.resolutionLabel, (1, 0), 
                   flag = wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL)
        sizerB.Add(self.resolRadioBox, (1, 1), flag = wx.EXPAND)
        #
        sizerB.Add(self.windowTypeLabel, (2, 0), 
                   flag = wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL)
        sizerB.Add(self.windowTypeComboBox, (2, 1), flag = wx.EXPAND)
        #
        sizerB.Add(self.vsyncCheckBox, (3, 0),
                   flag = wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL)
        #
        sizerB.Add(self.windowBppLabel, (4, 0), 
                   flag = wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL)
        sizerB.Add(self.windowBppComboBox, (4, 1), flag = wx.EXPAND)

        self.sizerA.Add(sizerB, 0, wx.ALL, 5)
        self.sizerA.Fit(self)
        self.SetSizer(self.sizerA)

    #----------------------------------------------------------------------
    #  Called on user chanes the item in the resolution combobox.
    #----------------------------------------------------------------------
    def OnResolution(self, evt):
        index = evt.GetInt()

        resList = [[640, 480], [800, 600], [1024, 768], [1280, 1024]]
        res = resList[index]
        self.width = res[0]
        self.height = res[1]

    #----------------------------------------------------------------------
    #  Check the validation of the page.
    #----------------------------------------------------------------------
    def validate(self):
        valid = True
        return valid

#----------------------------------------------------------------------
# EOF
#----------------------------------------------------------------------
