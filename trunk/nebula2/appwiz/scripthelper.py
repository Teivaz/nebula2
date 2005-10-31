#----------------------------------------------------------------------
#  scripthelper.py  
#
#  (C)2005 Kim, Hyoun woo
#----------------------------------------------------------------------

STR_PYTHON_REGISTER_PACKAGES = '''
//----------------------------------------------------------------------------
/**     
    Package registration hook.
*/
void
nPythonRegisterPackages(nKernelServer * kernelServer)
{
    kernelServer->AddPackage(nnebula);
    kernelServer->AddPackage(ndirect3d9);
    kernelServer->AddPackage(ndinput8);
    kernelServer->AddPackage(ndsound);
    kernelServer->AddPackage(ndshow);
    kernelServer->AddPackage(ngui);
    kernelServer->AddPackage(napplication);
    kernelServer->AddPackage(npythonserver);
    kernelServer->AddPackage(%(targetName)s);
}
'''

STR_ADD_PACKAGES = '''
    kernelServer.AddPackage(nnebula);
    kernelServer.AddPackage(ndirect3d9);
    kernelServer.AddPackage(ndinput8);
    kernelServer.AddPackage(ndsound);
    kernelServer.AddPackage(ndshow);
    kernelServer.AddPackage(ngui);
    kernelServer.AddPackage(napplication);
    %(scriptServerAddPackage)s
    kernelServer.AddPackage(%(targetName)s);\
'''

#----------------------------------------------------------------------
#  
#----------------------------------------------------------------------
class ScriptHelper:
    def __init__(self, classname, targetname, scriptserver):

        className = classname
        targetName = targetname
        # script server name
        scriptServerName = scriptserver

        packageScriptServerName = ''
        self.scriptServerPackage = ''
        self.bldScriptServerName = ''
        # we dont have to add ntclserver to target (it causes error when execute update.py)
        if 'ntclserver' == scriptServerName:
            packageScriptServerName = ''
            self.scriptServerPackage = ''
        elif 'nluaserver' == scriptServerName:
            self.bldScriptServerName = 'nlua'
            self.scriptServerPackage = 'nNebulaUsePackage(""nlua"");' 
        else:
            self.bldScriptServerName = scriptServerName
            packageScriptServerName = scriptServerName
            self.scriptServerPackage = 'nNebulaUsePackage(' + packageScriptServerName + ');'

        self.pythonRegPackage = ''
        self.addPackageDef = ''

        if 'npythonserver' == scriptServerName:
            substitutions = {}
            substitutions['classNameL'] = className.lower()
            substitutions['targetName'] = targetName
            self.pythonRegPackage = STR_PYTHON_REGISTER_PACKAGES % substitutions
            self.addPackageDef = 'nPythonRegisterPackages (&kernelServer);'
        else:
            self.pythonRegPackage = '' 
            scriptServerAddPackage = ''
            if 'ntclserver' == scriptServerName:
                scriptServerAddPackage = ''
            else:
                tmp = 'kernelServer.AddPackage(%(scriptServerName)s);'
                scriptServerAddPackage = tmp % { 'scriptServerName' : scriptServerName }

            self.addPackageDef = STR_ADD_PACKAGES % { 'classNameL' : className.lower(), 
                                                      'scriptServerAddPackage' : scriptServerAddPackage,
                                                      'targetName' : targetName.lower()
                                                    }

   #----------------------------------------------------------------------
   #  
   #----------------------------------------------------------------------
    def GetScriptServerPackage(self):
        return self.scriptServerPackage

   #----------------------------------------------------------------------
   #  
   #----------------------------------------------------------------------
    def GetBldScriptServerName(self):
        return self.bldScriptServerName 

   #----------------------------------------------------------------------
   #  
   #----------------------------------------------------------------------
    def GetPythonRegisterPackage(self):
        return self.pythonRegPackage

   #----------------------------------------------------------------------
   #  
   #----------------------------------------------------------------------
    def GetAddPackageDef(self):
        return self.addPackageDef

#----------------------------------------------------------------------
# EOF 
#----------------------------------------------------------------------
