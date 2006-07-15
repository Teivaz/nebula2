# setup.py

import sys
from distutils.core import setup
import py2exe

# add n2 root dir to modules path
sys.path += ["../../../"]

# Manifest resource to enable XP theme on XP
manifest_template = '''
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<assembly xmlns="urn:schemas-microsoft-com:asm.v1" manifestVersion="1.0">
<assemblyIdentity
    version="5.0.0.0"
    processorArchitecture="x86"
    name="%(prog)s"
    type="win32"
/>
<description>%(prog)s Program</description>
<dependency>
    <dependentAssembly>
        <assemblyIdentity
            type="win32"
            name="Microsoft.Windows.Common-Controls"
            version="6.0.0.0"
            processorArchitecture="X86"
            publicKeyToken="6595b64144ccf1df"
            language="*"
        />
    </dependentAssembly>
</dependency>
</assembly>
'''
RT_MANIFEST = 24
      
setup( 
    windows = [ 
        { 
            "script": "../../../appwizard.py", 
            "other_resources": [(RT_MANIFEST, 1, manifest_template % dict(prog="appwizard"))],
            "icon_resources": [(1, "../../../bin/win32/nebula.ico")],
            "modules": ["../../../appwiz", "../../../buildsys3"],
        },
        { 
            "script": "../../../update.py", 
            "other_resources": [(RT_MANIFEST, 1, manifest_template % dict(prog="appwizard"))],
            "icon_resources": [(1, "../../../bin/win32/nebula.ico")], 
            "modules": ["../../../appwiz", "../../../buildsys3"],
        } 
    ], 
) 
