#----------------------------------------------------------------------
#  common.py
#
#  (C)2005 Kim, Hyoun Woo
#----------------------------------------------------------------------
import  wx
import  wx.wizard as wiz

#----------------------------------------------------------------------
#  Create title and sizer of a title page.
#----------------------------------------------------------------------
def makePageTitle(wizPg, title):
    sizer = wx.BoxSizer(wx.VERTICAL)
    wizPg.SetSizer(sizer)
    title = wx.StaticText(wizPg, -1, title)
    title.SetFont(wx.Font(14, wx.SWISS, wx.NORMAL, wx.BOLD))
    sizer.Add(title, 0, wx.ALIGN_CENTRE|wx.ALL, 5)
    sizer.Add(wx.StaticLine(wizPg, -1), 0, wx.EXPAND|wx.ALL, 5)
    return sizer

