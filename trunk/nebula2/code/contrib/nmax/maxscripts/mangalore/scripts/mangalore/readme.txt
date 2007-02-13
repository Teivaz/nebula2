3ds Max Managalore Tool Readme
==============================

It's a prerelease of mangalore tool for 3ds Max.

3ds Max mangalore tool provides a tool written in Python which creates automatically 
generates modifier plugin which is used for each of game entities.
For that reason, you might need to write your own .xml file to create your own entity 
modifier plugin.

Example:
>build.py -xml mangalore.xml -entity ../startup/entity.ms

It generates enity.ms MAXScript file in the '$3dsmax/scripts/startup' directory.

Python reads XML file by its reverse order. So when you write mangalore.xml file, 
keep the '_Entities' element first then follow '_Categories' element next.

Correct database file path and filename should be described in the 
$3dsmax/plugcfg/mangalore.ini file. Otherwise it might crash the plugin.


*TODO*

- need more smart way to specify gfx and physics.
- physics exporter will be placed.


Have fun! :-)

- Kim Hyoun Woo

