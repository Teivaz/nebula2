========================================================================
                   nmaxenvelopecurve activex control
========================================================================

1. Overview

The nmaxenvelopecurve is activex control for 3dsmax custom ui control.

You might ask why the custom control is needed not just use 
already exsting control what 3dsmax natively provides.
The purpose of using custom control is to provide more convenient way 
to the artists.

And this package does not use Nebula build system because it is redundant 
task to change build system again to be compatible VC++ ocx project.
Also this is not such a package which is frequently needed to change.


2. Installation

The activex control which is built can be found under 
'$nebula2/bin/win32' or /$nebula2/bin/win32d' directory.

When you build the solution, the ActiveX control is automatically 
registered to the system. But if you want to register(or unregister) it 
by manual, do the followings.

To use the activex control within 3dsmax, it should be registered.

Please, open command-line console and type the following and return:

d:\dev\nebula2\bin\win32\regsvr32 nmaxenvelopecurve.ocx

(it assumes that nebula2 is already installed under 'd:\dev\nebula2')


To unregister the activex cottrol, type it with '/u' option like the following:

d:\dev\nebula2\bin\win32\regsvr32 /u nmaxenvelopecurve.ocx

It unregisteres the activex control.