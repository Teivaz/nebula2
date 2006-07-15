#----------------------------------------------------------------------
#  appframe.py
#
#  (C)2005 Kim, Hyoun Woo
#----------------------------------------------------------------------
import wx
import  wx.wizard as wiz
import  glob, os, time

from  appwiz.wizimage1 import *
from  appwiz.common import *
from  appwiz.titlepage import *
from  appwiz.appgenpage import *
from  appwiz.appmodpage import *
from  appwiz.disppage import *
from  appwiz.appentrypage import *
from  appwiz.bldpage import *
from  appwiz.scripthelper import *
from  appwiz.finpage import *

cwd = os.getcwd()

#----------------------------------------------------------------------
#  Frame window.
#----------------------------------------------------------------------
class AppWizFrame(wx.Frame):
    def __init__(self, parent, ID, title, 
                 pos, size, style):
        wx.Frame.__init__(self, parent, ID, title, pos, size, style)

        # bind event with its handlers.
        self.Bind(wiz.EVT_WIZARD_PAGE_CHANGING, self.OnWizPageChanging)
        self.Bind(wiz.EVT_WIZARD_PAGE_CHANGED, self.OnWizPageChanged) 
        self.Bind(wiz.EVT_WIZARD_FINISHED, self.OnWizPageFinished)
        self.Bind(wiz.EVT_WIZARD_CANCEL, self.OnWizPageCancel)

        # Create the wizard and the its pages.
        self.wizard = wiz.Wizard(self, -1, "Nebula2 Application Wizard", 
                            getwizimage1Bitmap())

        self.page1 = TitledPage (self.wizard,   "Nebula2 AppWizard")
        self.page2 = AppGenPage (self.wizard,   "General Setting", cwd)
        self.page3 = AppModPage (self.wizard,   "Module Setting")
        self.page4 = DispPage (self.wizard,     "Display Setting")
        self.page5 = AppEntryPage (self.wizard, "App Entry Setting")
        self.page6 = BldPage (self.wizard,      "Bld File Setting")
        self.page7 = FinPage (self.wizard,      "Finished", self)

        # Set the initial order of the pages.
        self.page1.SetNext(self.page2)
        self.page2.SetPrev(self.page1)
        self.page2.SetNext(self.page3)
        self.page3.SetPrev(self.page2)
        self.page3.SetNext(self.page4)
        self.page4.SetPrev(self.page3)
        self.page4.SetNext(self.page5)
        self.page5.SetPrev(self.page4)
        self.page5.SetNext(self.page6)
        self.page6.SetPrev(self.page5)
        self.page6.SetNext(self.page7)
        self.page7.SetPrev(self.page6)

        # specifies the size of the wizard. (set the wizard size fit to the page1)
        self.wizard.FitToPage(self.page1)

        # run self.wizard
        msgBoxTitle = "Nebula2 Application Wizard"
        if self.wizard.RunWizard(self.page1):
            wx.MessageBox("AppWizard completed successfully", msgBoxTitle)
        else:
            wx.MessageBox("AppWizard was cancelled", msgBoxTitle)

    #----------------------------------------------------------------------
    #  Called whenever the page is changing.
    #----------------------------------------------------------------------
    def OnWizPageChanging(self, evt):
        # check the validation of the current page.
        if evt.GetDirection():
            # the page which to be changed by forward direction.
            page = evt.GetPage()
            valid = page.validate()
            if valid == False:
                evt.Veto()

    #----------------------------------------------------------------------
    # Called whenever the page is changed.
    #----------------------------------------------------------------------
    def OnWizPageChanged(self, evt):
        # check the validation of the current page.
        if evt.GetDirection():
            # the changed page
            page = evt.GetPage()
            if False == self.wizard.HasNextPage(page):
                self.page7.UpdatePage(self)

    #----------------------------------------------------------------------
    # Read application skeleton header file.
    #----------------------------------------------------------------------
    def readTemplate(self, templateFile):
        fname = self.appwizDir + "\\template\\" + templateFile
        f = open(fname)
        strTemplate = f.read()
        f.close()
        return strTemplate

    #----------------------------------------------------------------------
    # type      : 'int' or 'src'
    # classname : 
    # ext       : '.h', '_main.cc' or '_cmds.cc'
    # str       : string which to put the output directory
    # args      : words which to replace with keyword in str
    # e.g) writeTemplate ('src', self.className, '_main.cc', strMain, self.args)
    #----------------------------------------------------------------------
    def writeTemplate(self, type, classname, ext, str, args):
        dirPath = os.path.join(self.outputDir, type, self.subdir)
        if not os.path.exists(dirPath):
            os.makedirs(dirPath)
        filePath = os.path.join(dirPath, classname.lower() + ext)
        try:
            f = file(filePath, 'w')
        except IOError:
            #errMsg = "Couldn't open %s for writing." + filePath
            errMsg = "IO error"
            dlg = wx.MessageDialog(self, errMsg,
                                   'AppWizard', 
                                   wx.ICON_EXCLAMATION|wx.OK )
            dlg.ShowModal()
            dlg.Destroy()

        else:

            f.write(str % args)
            f.close()

    #----------------------------------------------------------------------
    # Write bld file.
    #----------------------------------------------------------------------
    def writeBld(self, strBld):
        # write out the bld file
        bldDirPath = os.path.join(self.outputDir, 'bldfiles')
        if not os.path.exists(bldDirPath):
            os.makedirs(bldDirPath)
        bldPath = os.path.join(bldDirPath, self.className.lower() + '.bld')
        try:
            f = file(bldPath, 'w')
        except IOError:
            #errMsg = "Couldn't open %s for writing." + headerPath
            errMsg = "IO error"
            dlg = wx.MessageDialog(self, errMsg,
                                   'AppWizard', 
                                   wx.ICON_EXCLAMATION|wx.OK )
            dlg.ShowModal()
            dlg.Destroy()

        else:
            f.write(strBld % self.args)
            f.close()      

    #----------------------------------------------------------------------
    # Write application header file.
    #----------------------------------------------------------------------
    def OnWizPageFinished(self, evt):
        self.appwizDir = os.path.join(cwd, 'appwiz')

        self.outputDir = self.page2.outputDirTextBox.GetValue().strip()
        self.subdir    = self.page2.moduleSubdirTextBox.GetValue().strip()
        companyName    = self.page2.companyNameTextBox.GetValue()
        authorName     = self.page2.authorTextBox.GetValue().strip()
        docGroup       = self.page2.docGroupTextBox.GetValue()

        appBriefDoc       = self.page3.appBriefDocTextBox.GetValue()
        appStateBriefDoc = self.page3.appStateBriefDocTextBox.GetValue()

        classPrefix    = self.page3.classPrefixTextBox.GetValue().strip()
        self.className = self.page3.classTextBox.GetValue().strip()
        self.appObjName = self.page3.appObjNameTextBox.GetValue()
        self.appStateName   = self.page3.appStateNameTextBox.GetValue()
        self.appStateObjName = self.page3.appStateObjNameTextBox.GetValue()

        windowTitle = self.page4.windowTitleTextBox.GetValue()
        windowType  = self.page4.windowTypeComboBox.GetValue()      
        windowType = "nDisplayMode2::" + windowType
        windowWidth = self.page4.width
        windowHeight = self.page4.height
        vsync = 'false'
        if self.page4.vsyncCheckBox.GetValue():
            vsync = 'true'
        else:
            vsync = 'false'
        
        bpp = self.page4.windowBppComboBox.GetValue()
        bpp = "nDisplayMode2::" + bpp

        appName        = self.page5.appNameTextBox.GetValue()
        scriptServerName = self.page5.scriptComboBox.GetValue()

        targetName     = self.page6.targetNameTextBox.GetValue()
        targetAnnotation = self.page6.targetAnnotationTextBox.GetValue()

        scriptHelper = ScriptHelper(self.className, targetName, scriptServerName)

        # dictionary which to be replaced keywords in skeleton files.
        self.args = { 'classPrefixU' : classPrefix.upper(),
                      'classNameNoPrefixU' : self.className[len(classPrefix):].upper(),
                      'className' : self.className,
                      'classNameL' : self.className.lower(),
                      'appObjName' : self.appObjName.lower(),
                      'companyName' : companyName,
                      'docGroup' : docGroup,
                      'appBriefDoc' : appBriefDoc,
                      'appStateBriefDoc' : appStateBriefDoc,
                      'curYear' : time.strftime('%Y', time.localtime()),
                      'author' : authorName, 
                      'appName' : appName,
                      'appNameL' : appName.lower(),
                      'appStateNameNoPrefixU' : self.appStateName[len(classPrefix):].upper(),
                      'appStateName' : self.appStateName,
                      'appStateObjName' : self.appStateObjName.lower(),
                      'appStateNameL' : self.appStateName.lower(),
                      'scriptServerName' : scriptServerName.lower(),
                      'subDirL'  : self.subdir.lower(),
                      'targetName' : targetName.lower(),
                      'targetAnnotation' : targetAnnotation,
                      'scriptServerPackage' : scriptHelper.GetScriptServerPackage(),
                      'pythonRegPackage' : scriptHelper.GetPythonRegisterPackage(),
                      'addPackageDef' : scriptHelper.GetAddPackageDef(),
                      'bldScriptServerName' : scriptHelper.GetBldScriptServerName(),
                      'windowTitle' : windowTitle,
                      'windowType' : windowType,
                      'windowWidth' : windowWidth,
                      'windowHeight' : windowHeight,
                      'vsync' : vsync,
                      'bpp' : bpp
                    }

        # napplication derived module
        strAppHeader = self.readTemplate('napp.sh')
        self.writeTemplate('inc', self.className, '.h', strAppHeader, self.args)
        strAppMain = self.readTemplate('napp_main.sc')
        self.writeTemplate('src', self.className, '_main.cc', strAppMain, self.args)
        strAppCmds = self.readTemplate('napp_cmds.sc')
        self.writeTemplate('src', self.className, '_cmds.cc', strAppCmds, self.args)

        # nappstate derived module
        strAppStateHeader = self.readTemplate('nappstate.sh')
        self.writeTemplate('inc', self.appStateName, '.h', strAppStateHeader, self.args)
        strAppStateMain = self.readTemplate('nappstate_main.sc')
        self.writeTemplate('src', self.appStateName, '_main.cc', strAppStateMain, self.args)
        strAppStateCmds = self.readTemplate('nappstate_cmds.sc')
        self.writeTemplate('src', self.appStateName, '_cmds.cc', strAppStateCmds, self.args)

        # main
        strMain = self.readTemplate('app.sc')
        self.writeTemplate('src', appName, '.cc', strMain, self.args)

        # bld file
        strBld = self.readTemplate('app.sbld')
        self.writeBld(strBld)

        # all things done.
        self.Destroy()

    #----------------------------------------------------------------------
    #
    #----------------------------------------------------------------------
    def OnWizPageCancel(self, evt):
        self.Destroy()

#----------------------------------------------------------------------
# EOF
#----------------------------------------------------------------------
