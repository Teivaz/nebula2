#--------------------------------------------------------------------------
# 3rd Generation Nebula 2 Build System GUI
#
# (c) 2005 Vadim Macagon
#
# Contents are licensed under the Nebula license.
#--------------------------------------------------------------------------

import wx
import wx.lib.newevent
import glob, os.path, time

STR_MISSING_PARAMS = """\
Please ensure you've selected the output directory
and entered a class name and a superclass name.
"""

STR_INVALID_CLASS_PREFIX = """\
The class prefix must be shorter than the class name.
"""

STR_HEADER_S1 = '''\
#ifndef %(classPrefixU)s_%(classNameNoPrefixU)s_H
#define %(classPrefixU)s_%(classNameNoPrefixU)s_H
//----------------------------------------------------------------------------
/**
    @class %(className)s
    @ingroup %(docGroup)s
    @brief %(briefDoc)s

    (c) %(curYear)s    %(author)s
*/
//----------------------------------------------------------------------------

'''

STR_HEADER_S2 = '''\
#include "%(superHeaderPath)s"
'''

STR_HEADER_S3 = '''\

//----------------------------------------------------------------------------
class %(className)s : public %(superClassName)s
{
    public:
        /// constructor
        %(className)s();
        /// destructor
        virtual ~%(className)s();
'''

STR_HEADER_S4 = '''\
        /// persistency
        virtual bool SaveCmds(nPersistServer*);
'''

STR_HEADER_S5 = '''\
};

//----------------------------------------------------------------------------
#endif // %(classPrefixU)s_%(classNameNoPrefixU)s_H

'''

STR_MAIN_S1 = '''\
//----------------------------------------------------------------------------
//  (c) %(curYear)s    %(author)s
//----------------------------------------------------------------------------
#include "%(headerPath)s"

'''

STR_MAIN_S2 = '''\
nNebulaScriptClass(%(className)s, "%(superClassNameL)s")

'''

STR_MAIN_S3 = '''\
nNebulaClass(%(className)s, "%(superClassNameL)s")

'''

STR_MAIN_S4 = '''\
//----------------------------------------------------------------------------
/**
*/
%(className)s::%(className)s()
{
}

//----------------------------------------------------------------------------
/**
*/
%(className)s::~%(className)s()
{
}

//----------------------------------------------------------------------------
// EOF
//----------------------------------------------------------------------------
'''

STR_CMDS_S1 = '''\
//----------------------------------------------------------------------------
//  (c) %(curYear)s    %(author)s
//----------------------------------------------------------------------------
#include "%(headerPath)s"

//----------------------------------------------------------------------------
/**
    @scriptclass
    %(classNameL)s

    @cppclass
    %(className)s

    @superclass
    %(superClassNameL)s

    @classinfo
    %(briefDoc)s
*/
void
n_initcmds(nClass* clazz)
{
    clazz->BeginCmds();
    // clazz->AddCmd("v_Xxx_v", 'XXXX', n_Xxx);
    // etc.
    clazz->EndCmds();
}

'''

STR_CMDS_S2 = '''\
//----------------------------------------------------------------------------
/**
    @param  ps    Writes the nCmd object contents out to a file.
    @return       Success or failure.
*/
bool
%(className)s::SaveCmds(nPersistServer* ps)
{
    if (%(superClassName)s::SaveCmds(ps))
    {
        //nCmd* cmd = ps->GetCmd(this, 'XXXX');
        //ps->PutCmd(cmd);

        return true;
    }
    return false;
}

'''

STR_CMDS_S3 = '''\
//----------------------------------------------------------------------------
// EOF
//----------------------------------------------------------------------------
'''

#--------------------------------------------------------------------------
class ClassBuilderPanel(wx.Panel):

    def __init__(self, parentWindow, buildSys):
        wx.Panel.__init__(self, parentWindow)
        self.buildSys = buildSys
        defaultOutputDir = buildSys.GetAbsPathFromRel('code')
        self.outputDirLabel = wx.StaticText(self, -1, 'Output Dir')
        self.outputDirTextBox = wx.TextCtrl(self, -1, defaultOutputDir,
                                            (0, 0), (310, 21))
        self.outputDirBtn = wx.Button(self, -1, 'Browse...')
        self.Bind(wx.EVT_BUTTON, self.OnOutputDirBtn, self.outputDirBtn)
        self.moduleSubdirLabel = wx.StaticText(self, -1, 'Module Subdir')
        self.moduleSubdirTextBox = wx.TextCtrl(self, -1, '',
                                               (0, 0), (200, 21))
        self.classPrefixLabel = wx.StaticText(self, -1, 'Class Prefix')
        self.classPrefixTextBox = wx.TextCtrl(self, -1, 'n', (0, 0), (200, 21))
        self.classNameLabel = wx.StaticText(self, -1, 'Class Name')
        self.classTextBox = wx.TextCtrl(self, -1, '', (0, 0), (200, 21))
        self.scriptableCheckBox = wx.CheckBox(self, -1, 'Scriptable')
        self.Bind(wx.EVT_CHECKBOX, self.OnTickScriptable, self.scriptableCheckBox)
        self.persistentCheckBox = wx.CheckBox(self, -1, 'Persistent')
        self.persistentCheckBox.Enable(False)
        self.superClassLabel = wx.StaticText(self, -1, 'Superclass')
        self.superClassTextBox = wx.TextCtrl(self, -1, '', (0, 0), (200, 21))
        self.authorLabel = wx.StaticText(self, -1, 'Author')
        self.authorTextBox = wx.TextCtrl(self, -1, '', (0, 0), (200, 21))
        self.docGroupLabel = wx.StaticText(self, -1, 'Doxygen Group')
        self.docGroupTextBox = wx.TextCtrl(self, -1, '', (0, 0), (200, 21))
        self.briefDocLabel = wx.StaticText(self, -1, 'Brief Description')
        self.briefDocTextBox = wx.TextCtrl(self, -1,
                                           'a brief description of the class',
                                           (0, 0), (200, 80),
                                           wx.TE_MULTILINE|wx.TE_WORDWRAP)
        self.runBtn = wx.Button(self, -1, 'Run')
        self.Bind(wx.EVT_BUTTON, self.OnRunBtn, self.runBtn)

        # Layout
        sizerB = wx.GridBagSizer(4, 10)
        # row 0
        sizerB.Add(self.outputDirLabel, (0, 0),
                   flag = wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL)
        sizerB.Add(self.outputDirTextBox, (0, 1),
                   flag = wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL)
        sizerB.Add(self.outputDirBtn, (0, 2),
                   flag = wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL)
        # row 1
        sizerB.Add(self.moduleSubdirLabel, (1, 0),
                   flag = wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL)
        sizerB.Add(self.moduleSubdirTextBox, (1, 1), flag = wx.EXPAND)
        # row 2
        sizerB.Add(self.classPrefixLabel, (2, 0),
                   flag = wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL)
        sizerB.Add(self.classPrefixTextBox, (2, 1), flag = wx.EXPAND)
        # row 3
        sizerB.Add(self.classNameLabel, (3, 0),
                   flag = wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL)
        sizerB.Add(self.classTextBox, (3, 1), flag = wx.EXPAND)
        # row 4
        sizerB.Add(self.scriptableCheckBox, (4, 1), flag = wx.TOP, border = 5)
        # row 5
        sizerB.Add(self.persistentCheckBox, (5, 1))
        # row 6
        sizerB.Add(self.superClassLabel, (6, 0),
                   flag = wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL)
        sizerB.Add(self.superClassTextBox, (6, 1), flag = wx.EXPAND)
        # row 7
        sizerB.Add(self.authorLabel, (7, 0),
                   flag = wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL)
        sizerB.Add(self.authorTextBox, (7, 1), flag = wx.EXPAND)
        # row 8
        sizerB.Add(self.docGroupLabel, (8, 0),
                   flag = wx.ALIGN_RIGHT|wx.ALIGN_CENTER_VERTICAL)
        sizerB.Add(self.docGroupTextBox, (8, 1), flag = wx.EXPAND)
        # row 9
        sizerB.Add(self.briefDocLabel, (9, 0),
                   flag = wx.ALIGN_RIGHT|wx.ALIGN_TOP)
        sizerB.Add(self.briefDocTextBox, (9, 1), flag = wx.EXPAND)
        # row 10
        sizerB.Add(self.runBtn, (10, 1),
                   flag = wx.ALIGN_CENTER|wx.TOP, border = 10)
        # top-level sizer
        sizerA = wx.BoxSizer(wx.VERTICAL)
        sizerA.Add(sizerB, 0, wx.ALL, 10)
        sizerA.Fit(self)
        self.SetSizer(sizerA)

    #--------------------------------------------------------------------------
    # Called when the Browse... button is clicked.
    def OnOutputDirBtn(self, evt):
        dlg = wx.DirDialog(self, "Choose output directory:",
                           self.outputDirTextBox.GetValue().strip(),
                           style=wx.DD_DEFAULT_STYLE|wx.DD_NEW_DIR_BUTTON)
        if dlg.ShowModal() == wx.ID_OK:
            self.outputDirTextBox.SetValue(dlg.GetPath())
        dlg.Destroy()

    #--------------------------------------------------------------------------
    # Called when the Run button is clicked.
    def OnRunBtn(self, evt):
        try:
            if self.validateInput():
                self.writeFiles()
        except:
            self.buildSys.logger.exception('ClassBuilderPanel.OnRunBtn()')

    #--------------------------------------------------------------------------
    # Called when the Scriptable checkbox is ticked/unticked.
    def OnTickScriptable(self, evt):
        if self.scriptableCheckBox.IsChecked():
            self.persistentCheckBox.Enable(True)
        else:
            self.persistentCheckBox.SetValue(False)
            self.persistentCheckBox.Enable(False)

    #--------------------------------------------------------------------------
    # Validate user input.
    def validateInput(self):
        valid = True
        if self.outputDirTextBox.GetValue().strip() == '':
            valid = False
        className = self.classTextBox.GetValue().strip()
        if '' == className:
            valid = False
        if self.superClassTextBox.GetValue().strip() == '':
            valid = False
        if not valid:
            dlg = wx.MessageDialog(self, STR_MISSING_PARAMS,
                                   'Class Builder',
                                   wx.ICON_EXCLAMATION|wx.OK )
            dlg.ShowModal()
            dlg.Destroy()
        classPrefix = self.classPrefixTextBox.GetValue().strip()
        if len(classPrefix) >= len(className):
            valid = False
            dlg = wx.MessageDialog(self, STR_INVALID_CLASS_PREFIX,
                                   'Class Builder',
                                   wx.ICON_EXCLAMATION|wx.OK )
            dlg.ShowModal()
            dlg.Destroy()
        return valid

    #--------------------------------------------------------------------------
    def writeFiles(self):
        outputDir = self.outputDirTextBox.GetValue().strip()
        subdir = self.moduleSubdirTextBox.GetValue().strip()
        className = self.classTextBox.GetValue().strip()
        success = True
        filePaths = []
        incDirPath = os.path.join(outputDir, 'inc', subdir)
        filePaths.append(os.path.join(incDirPath, className.lower() + '.h'))
        srcDirPath = os.path.join(outputDir, 'src', subdir)
        filePaths.append(os.path.join(srcDirPath,
                                      className.lower() + '_main.cc'))
        if self.scriptableCheckBox.IsChecked():
            filePaths.append(os.path.join(srcDirPath,
                                          className.lower() + '_cmds.cc'))
        confirmDlg = ConfirmationDialog(self, filePaths)
        if wx.ID_OK == confirmDlg.ShowModal():
            try:
                if not self.writeHeader():
                    success = False
                if not self.writeMain():
                    success = False
                if self.scriptableCheckBox.IsChecked():
                    if not self.writeCmds():
                        success = False
            except:
                self.buildSys.logger.exception('ClassBuilderPanel.writeFiles()')

            if success:
                dlg = wx.MessageDialog(self,
                                       'All files were generated successfuly.',
                                       'Class Builder',
                                       wx.ICON_INFORMATION|wx.OK)
                dlg.ShowModal()
                dlg.Destroy()
            else:
                dlg = wx.MessageDialog(self,
                                       'Some errors occured, please check the log.',
                                       'Class Builder',
                                       wx.ICON_EXCLAMATION|wx.OK)
                dlg.ShowModal()
                dlg.Destroy()
        confirmDlg.Destroy()

    #--------------------------------------------------------------------------
    # Write out the xxx.h
    def writeHeader(self):
        outputDir = self.outputDirTextBox.GetValue().strip()
        subdir = self.moduleSubdirTextBox.GetValue().strip()
        classPrefix = self.classPrefixTextBox.GetValue().strip()
        className = self.classTextBox.GetValue().strip()
        superClassName = self.superClassTextBox.GetValue().strip()
        authorName = self.authorTextBox.GetValue().strip()
        docGroup = self.docGroupTextBox.GetValue()
        briefDoc = self.briefDocTextBox.GetValue()

        # try to find the superclass header
        superPath = ''
        # first look in the core and user projects
        basePath = os.path.join(self.buildSys.homeDir, 'code', '*', 'inc')
        paths = glob.glob(os.path.join(basePath, '*',
                                       superClassName.lower() + '.h'))
        if len(paths) < 1:
            # still haven't found it, so look in contrib
            basePath = os.path.join(self.buildSys.homeDir, 'code', 'contrib',
                                    '*', 'inc')
            paths = glob.glob(os.path.join(basePath, '*',
                                           superClassName.lower() + '.h'))
        if len(paths) > 0:
            # extract the last 2 path components
            incDir, incName = os.path.split(paths[0])
            incDir, superPath = os.path.split(incDir)
            superPath += '/' + incName

        # write the class header
        incDirPath = os.path.join(outputDir, 'inc', subdir)
        if not os.path.exists(incDirPath):
            os.makedirs(incDirPath)
        headerPath = os.path.join(incDirPath, className.lower() + '.h')
        try:
            f = file(headerPath, 'w')
        except IOError:
            self.buildSys.logger.exception("Couldn't open %s for writing.",
                                           headerPath)
        else:
            args = { 'classPrefixU' : classPrefix.upper(),
                     'classNameNoPrefixU' : className[len(classPrefix):].upper(),
                     'className' : className,
                     'docGroup' : docGroup,
                     'briefDoc' : briefDoc,
                     'curYear' : time.strftime('%Y', time.localtime()),
                     'author' : authorName,
                     'superHeaderPath' : superPath,
                     'superClassName' : superClassName }
            f.write(STR_HEADER_S1 % args)
            if superPath != '':
                f.write(STR_HEADER_S2 % args)
            f.write(STR_HEADER_S3 % args)
            if self.persistentCheckBox.IsChecked():
                f.write(STR_HEADER_S4 % args)
            f.write(STR_HEADER_S5 % args)
            f.close()
            return True

        return False

    #--------------------------------------------------------------------------
    # Write out the xxx_main.cc
    def writeMain(self):
        outputDir = self.outputDirTextBox.GetValue().strip()
        subdir = self.moduleSubdirTextBox.GetValue().strip()
        classPrefix = self.classPrefixTextBox.GetValue().strip()
        className = self.classTextBox.GetValue().strip()
        superClassName = self.superClassTextBox.GetValue().strip()
        authorName = self.authorTextBox.GetValue().strip()

        srcDirPath = os.path.join(outputDir, 'src', subdir)
        if not os.path.exists(srcDirPath):
            os.makedirs(srcDirPath)
        mainPath = os.path.join(srcDirPath, className.lower() + '_main.cc')

        try:
            f = file(mainPath, 'w')
        except IOError:
            self.buildSys.logger.exception("Couldn't open %s for writing.",
                                           mainPath)
        else:
            args = { 'curYear' : time.strftime('%Y', time.localtime()),
                     'author' : authorName,
                     'headerPath' : subdir + '/' + className.lower() + '.h',
                     'className' : className,
                     'superClassNameL' : superClassName.lower() }
            f.write(STR_MAIN_S1 % args)
            if self.scriptableCheckBox.IsChecked():
                f.write(STR_MAIN_S2 % args)
            else:
                f.write(STR_MAIN_S3 % args)
            f.write(STR_MAIN_S4 % args)
            f.close()
            return True

        return False

    #--------------------------------------------------------------------------
    # Write out the xxx_cmds.cc
    def writeCmds(self):
        outputDir = self.outputDirTextBox.GetValue().strip()
        subdir = self.moduleSubdirTextBox.GetValue().strip()
        classPrefix = self.classPrefixTextBox.GetValue().strip()
        className = self.classTextBox.GetValue().strip()
        superClassName = self.superClassTextBox.GetValue().strip()
        authorName = self.authorTextBox.GetValue().strip()
        briefDoc = self.briefDocTextBox.GetValue()

        srcDirPath = os.path.join(outputDir, 'src', subdir)
        if not os.path.exists(srcDirPath):
            os.makedirs(srcDirPath)
        cmdsPath = os.path.join(srcDirPath, className.lower() + '_cmds.cc')

        try:
            f = file(cmdsPath, 'w')
        except IOError:
            self.buildSys.logger.exception("Couldn't open %s for writing.",
                                           cmdsPath)
        else:
            args = { 'curYear' : time.strftime('%Y', time.localtime()),
                     'author' : authorName,
                     'headerPath' : subdir + '/' + className.lower() + '.h',
                     'classNameL' : className.lower(),
                     'superClassNameL' : superClassName.lower(),
                     'briefDoc' : briefDoc,
                     'className' : className,
                     'superClassName' : superClassName }
            f.write(STR_CMDS_S1 % args)
            if self.persistentCheckBox.IsChecked():
                f.write(STR_CMDS_S2 % args)
            f.write(STR_CMDS_S3 % args)
            f.close()
            return True

        return False

#--------------------------------------------------------------------------
class ConfirmationDialog(wx.Dialog):
    def __init__(self, parentWindow, filePaths):
        wx.Dialog.__init__(self, parentWindow, -1, 'Confirmation',
                           style = wx.DEFAULT_DIALOG_STYLE)
        self.textLabel = wx.StaticText(self, -1,
                                       'The following files will be generated:')
        text = ''
        for filePath in filePaths:
            text += filePath + '\n'
        self.textBox = wx.TextCtrl(self, -1, text, size = (400, 60),
                                   style = wx.TE_MULTILINE|wx.TE_READONLY|
                                           wx.TE_LINEWRAP)
        self.okBtn = wx.Button(self, wx.ID_OK, 'OK')
        self.okBtn.SetDefault()
        self.cancelBtn = wx.Button(self, wx.ID_CANCEL, 'Cancel')

        # Layout
        sizerA = wx.BoxSizer(wx.VERTICAL)
        sizerA.Add(self.textLabel, 0, wx.ALIGN_LEFT|wx.ALL, 5)
        sizerA.Add(self.textBox, 0, wx.EXPAND|wx.ALL, 5)
        sizerB = wx.BoxSizer(wx.HORIZONTAL)
        sizerB.Add(self.okBtn, 0, wx.CENTER|wx.ALL, 5)
        sizerB.Add(self.cancelBtn, 0, wx.CENTER|wx.ALL, 5)
        sizerA.Add(sizerB, 0, wx.CENTER)
        self.SetSizer(sizerA)
        self.SetAutoLayout(True)
        self.Fit()

#--------------------------------------------------------------------------
# EOF
#--------------------------------------------------------------------------
