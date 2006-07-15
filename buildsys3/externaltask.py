#--------------------------------------------------------------------------
# 3rd Generation Nebula 2 Build System
# External Task Launcher
#
# (c) 2005 Vadim Macagon
#
# Contents are licensed under the Nebula license.
#--------------------------------------------------------------------------

import os, subprocess, signal, threading, platform

#--------------------------------------------------------------------------
# Terminate the given process and it's immediate children.
def TerminateChildProcess(proc):
    if proc != None:
        if platform.system() == 'Windows':
            try:
                from buildsys3 import pykillwinproc as prockiller
            except ImportError, err:
                #print str(err) + '\n'
                pass
            else:
                #print 'terminating process with pid = ' + str(proc.pid)
                prockiller.kill_process_tree(proc.pid)
        else:
            os.kill(proc.pid, signal.SIGTERM)

#--------------------------------------------------------------------------
# Use an instance of this class to launch an external program and wait for
# it's completion. When the build system is run in GUI mode a dialog will
# pop up to display standard output/errors from the external program.
class ExternalTask:
    
    #--------------------------------------------------------------------------
    def __init__(self, taskName, cmdLine, workingDir = None, mainFrame = None):
        self.taskName = taskName
        self.mainFrame = mainFrame
        self.cmdLine = cmdLine
        self.workingDir = workingDir
        self.dialogProxy = None
        self.evtDialogCreated = threading.Event()
        self.evtDialogClosed = threading.Event()
        self.proc = None
        self.userCancelled = False
        self.firstRun = True
        
    #--------------------------------------------------------------------------
    # Run the task (blocks until task terminates).
    # Return True if the task completed successfuly, or False if it didn't run
    # to completion (usually due to the user aborting it).
    def Run(self):
        assert self.firstRun # for now only allow the task to be run once
        if self.firstRun:
            self.firstRun = False
            if self.mainFrame != None:
                return self.runWithGUI()
            else:
                return self.runWithoutGUI()
        return False
    
    #--------------------------------------------------------------------------
    # Private Stuff
    #--------------------------------------------------------------------------
    
    #--------------------------------------------------------------------------
    # Will be called by self.mainFrame when the dialog proxy associated with
    # this task is created.
    def OnCreateDialogProxy(self, dlgProxy):
        assert dlgProxy != None
        self.dialogProxy = dlgProxy
        # wake up any threads waiting for the dialog proxy to be set
        self.evtDialogCreated.set()
    
    #--------------------------------------------------------------------------
    def createDialogProxy(self):
        self.mainFrame.CreateExternalTaskDialogProxy(self.taskName + ' Output',
                                                     self.OnCreateDialogProxy,
                                                     self.OnOkBtn, 
                                                     self.OnCancelBtn)
        # block until the dialog is created
        self.evtDialogCreated.wait()
        
    #--------------------------------------------------------------------------
    def destroyDialogProxy(self):
        # block until the user closes the dialog
        self.evtDialogClosed.wait()
        self.mainFrame.DestroyExternalTaskDialogProxy(self.dialogProxy)
        self.dialogProxy = None
    
    #--------------------------------------------------------------------------
    # This will be called after the user presses the OK button to close the
    # dialog associated with this task.
    def OnOkBtn(self):
        # wake up any threads waiting for the dialog to close
        self.evtDialogClosed.set()
        
    #--------------------------------------------------------------------------
    # This will be called after the user presses the Cancel button to close the
    # dialog associated with this task.
    def OnCancelBtn(self):
        if not self.userCancelled:
            self.userCancelled = True
            TerminateChildProcess(self.proc)
        # wake up any threads waiting for the dialog to close
        self.evtDialogClosed.set()
    
    #--------------------------------------------------------------------------
    def runWithGUI(self):
        oldPath = os.getcwd()
        if self.workingDir != None:
            os.chdir(self.workingDir)
            
        self.createDialogProxy()
        self.dialogProxy.Display()
        
        try:
            self.proc = subprocess.Popen(self.cmdLine,
                                         shell = True, 
                                         stdin = None, 
                                         stdout = subprocess.PIPE, 
                                         stderr = subprocess.STDOUT)
        except:
            os.chdir(oldPath)
            raise
        else:
            outLine = self.proc.stdout.readline()
            while outLine != '':
                self.dialogProxy.AppendText(outLine)
                outLine = self.proc.stdout.readline()
            
            if not self.userCancelled:
                # enable OK button and disable Cancel button
                self.dialogProxy.EnableButtons(True, False)
        
            self.proc.stdout.close()
            
        self.destroyDialogProxy()
        os.chdir(oldPath)
        return not self.userCancelled
        
    #--------------------------------------------------------------------------
    def runWithoutGUI(self):
        oldPath = os.getcwd()
        if self.workingDir != None:
            os.chdir(self.workingDir)
        os.system(self.cmdLine)
        os.chdir(oldPath)
        return True

#--------------------------------------------------------------------------
# EOF
#--------------------------------------------------------------------------

