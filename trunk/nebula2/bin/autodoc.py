"""
    Autodoc.py
    
    generates html docs from nebula cmd source files.

    Run from within nebula/bin,
    html files are created in nebula/doc/autodoc.

    TODO:
       - Add grouping to create some form of hierarchy.

    30-Aug-01   leaf    created.
    12-Jun-02   leaf    new style and other improvements
    16-Aug-03   bruce   added doxygen output
    17-Aug-03   leaf    added C++ class name. Currently it
                        searches for @cppclass or 
                        N_IMPLEMENTS nClassName
"""
import sys, os, fnmatch, re, time

# global dict of all classes
classes = {}

# regular expressions to extract class/cmd info
rClass = re.compile(r"@scriptclass\s*(?P<classname>\S*)(?:(?:\s*)|(?:\s*@cppclass\s*(?P<cppname>\S*?))\s*)@superclass\s*(?P<superclass>\S*)\s*@classinfo\s*(?P<info>.*?)\s*\*/", re.S)
rCmd = re.compile(r"@cmd\s*(?P<cmd>\S*)\s*@input\s*(?P<input>.*?)\s*@output\s*(?P<output>.*?)\s*@info\s*(?P<info>.*?)\s*\*/", re.S)

# set up paths in hopefully OS independant manner
dir_autodoc = os.path.join(sys.path[0], os.pardir, 'doc', 'autodoc')
dir_classes = os.path.join(dir_autodoc, 'classes')
dir_src = os.path.join(sys.path[0], os.pardir, 'code', 'nebula2', 'src')

class aCmd:
    "simple structure for storing cmd details"
    name = ""
    input = ""
    output = ""
    info = ""
    def __repr__(self):
        return self.name

class aClass:
    "simple structure for storing class details"
    name = ""
    info = ""
    def findSubclasses(self):
        """recursively finds subclasses, adds them as
        subclasses list (very brute force)."""
        self.subclasses = []
        for k in classes.keys():
            if classes[k].superclass == self.name:
                classes[k].findSubclasses()
                self.subclasses.append(classes[k])
        self.subclasses.sort()
    def __repr__(self):
        return self.name
    def __cmp__(self, other):
        if self.name < other.name: return -1
        if self.name > other.name: return 1
        return 0

def escapeHtml(string):
    return string.replace("&", "&amp;").replace(">", "&gt;").replace("<", "&lt;")

def scanFiles(pathname):
    """Recursively scans a directory tree,
    looking for *_cmds.cc files and extracting
    class/cmd information from them."""
    dirlist = os.listdir(pathname)
    for filename in dirlist:
        path = os.path.join(pathname, filename)
        if os.path.isdir(path):
            scanFiles(path)
        if fnmatch.fnmatch(filename, "*_cmds.cc"):
            fp = open(path, "r")
            file = fp.read()
            mClass = rClass.search(file)
            if mClass:
                clas = aClass()
                clas.name = mClass.group('classname')
                clas.superclass = mClass.group('superclass')
                clas.info = escapeHtml(mClass.group('info'))
                clas.cppname = mClass.group('cppname') or clas.name
                cmds = []
                mCmd = rCmd.search(file)
                while mCmd:
                    cmd = aCmd()
                    cmd.name = mCmd.group('cmd')
                    cmd.input = mCmd.group('input')
                    cmd.output = mCmd.group('output')
                    cmd.info = escapeHtml(mCmd.group('info'))
                    cmds.append(cmd)
                    mCmd = rCmd.search(file, mCmd.end())
                clas.cmds = cmds
                classes[clas.name] = clas
            else:
                print "No class information found in " + path
            fp.close()



# functions for creating doxygen output
def classTreeDoxygen(clas, indent = 0):
    out = '%s - @ref N2ScriptInterface_%s "%s"\n' % (' ' * indent, clas.name,
                                                  clas.name)
    if clas.subclasses:
        for sc in clas.subclasses:
            out += classTreeDoxygen(sc, indent + 4)
    return out

def classInfoDoxygen(clas):
    out = """/**
@page N2ScriptInterface_%s %s Script Interface

@section N2ScriptInterfaceClass_%s %s:
""" % (clas.name, clas.name, clas.name, clas.name)

    if clas.superclass != "---":
        out += """
@subsection N2ScriptInterfaceSuperClass_%s Super Class:
 
   - @ref N2ScriptInterface_%s "%s"\n""" % (clas.name, clas.superclass, clas.superclass)

    if clas.subclasses:
        out += "\n@subsection N2ScriptInterfaceSubClasses_%s Subclasses:\n" % (clas.name)
        for subclas in clas.subclasses:
            out += """    - @ref N2ScriptInterface_%s "%s"\n""" % (subclas.name,
                                                                   subclas.name)

    out += """
@subsection N2ScriptInterface_%sCPPClass C++ Class:
   - @ref %s
   """ % (clas.name, clas.cppname)

    out += """

@subsection N2ScriptInterface_%sInfo Info

%s
""" % (clas.name, clas.info.replace("\n    ","\n"))
    out += "\n@subsection N2ScriptInterfaceCommands_%s Commands:\n" % (clas.name)
    # do command block
    numcmds = len(clas.cmds)
    for i in range(numcmds):
        cmd = clas.cmds[i]
        out += "    - @ref N2ScriptInterface_%s_%s\n" % (clas.name, cmd.name)
    out += "\n\n<hr>\n\n"
    return out

def cmdBlockDoxygen(cmd, clas):
    out = """@subsubsection N2ScriptInterface_%s_%s %s

@par Command Input:
<tt>%s</tt>

@par Command Output:
<tt>%s</tt>

@par Command Description:
%s

<hr>
""" % (
        clas.name,
        cmd.name,
        cmd.name,
        cmd.input.replace("\n    ","\n"),
        cmd.output.replace("\n    ","\n"),
        cmd.info.replace("\n    ","\n"))
    return out

def classPageDoxygen(clas):
    out = classInfoDoxygen(clas)
    for cmd in clas.cmds:
        out += cmdBlockDoxygen(cmd, clas)
    out += "*/"
    fo = open(os.path.join(dir_classes, '%s.dox' % clas), 'w')
    fo.write(out)
    fo.close()

def cmdsPageDoxygen(root):
    classPageDoxygen(root)
    for clas in root.subclasses:
        cmdsPageDoxygen(clas)
    return out

# parse all the cmd files
scanFiles(dir_src)
root = classes['nroot']
root.findSubclasses()

# create class tree
out = """/**
@page NebulaTwoScriptInterface Nebula 2 Script Interface

@section NebulaTwoScriptInterfaceForUsers Notes for Users

Select the class you're interested in from the tree below. Each class documents
the commands that that class defines, plus a class also inherits commands from
its parent. For example @c nanimator also accepts @c nscenenode
commands, and every class inherits @c nroot commands.

You can use the command @c getcmds on any class to see a full
list of commands for that class.

The @b input / @b output parameter information for each command uses a single letter code to
represent the type of the parameter.
   - f - float value
   - i - integer value
   - o - object reference
   - s - string value
   - l - list value
   - b - true|false
   - v - no parameters

@section NebulaTwoScriptInterfaceTree Scriptable Classes
"""
out += classTreeDoxygen(root) 
out += """
@section NebulaTwoScriptInterfaceForDevelopers Notes for Developers

<p>In order for your commands to be documented here you must put the functions
  in a file called <code>*_cmds.cc</code>. To describe the class use a comment
  block like this, usually put in front of the <code>n_initcmds</code> function:</p>

@verbatim
/**
    @ scriptclass
    ntimeserver

    @ superclass
    nroot

    @ classinfo
    The ntimeserver object lives under the name /sys/servers/time
    and provides a central time source for Nebula.
*/
@endverbatim

Then put a comment block in front of
each script function like this:

@verbatim
/**
    @ cmd
    gettime

    @ input
    v

    @ output
    f (Time)

    @ info
    Return current global time in seconds since the time server
    was created.
*/
@endverbatim

	This will be output <i>as is</i> so make sure to wrap
your text at a reasonable point, and try to use spaces instead of tabs.
See any @c *_cmds.cc file for more detail.
"""
out += "*/"
fp = open(os.path.join(dir_autodoc, 'tree.dox'), 'w')
fp.write(out)
fp.close()

# create class pages
if not os.path.exists(dir_classes):
    os.mkdir(dir_classes)
cmdsPageDoxygen(root)

