"""
    Autodoc.py
    
    generates html docs from nebula cmd source files.

    Run from within nebula/bin,
    html files are created in nebula/doc/autodoc.

    30-Aug-01   leaf    created.
    12-Jun-02   leaf    new style and other improvements
    
"""
import sys, os, fnmatch, re, time

# global dict of all classes
classes = {}

# regular expressions to extract class/cmd info
rClass = re.compile(r"@scriptclass\s*(?P<classname>\S*)\s*@superclass\s*(?P<superclass>\S*)\s*@classinfo\s*(?P<info>.*?)\s*\*/", re.S)
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

# functions for creating the html
def classTree(clas, indent = 0):
    out = '<li><a href="classes/%s.html" target="mainFrame">%s</a>' % (clas.name, clas.name)
    if clas.subclasses:
        out += '\n <ul>\n'
        for sc in clas.subclasses:
            out += classTree(sc, indent)
        out += ' </ul>\n'
    return out+'</li>\n'

def classInfo(clas):
    out = """<html>
<head>
<title>Autodoc: %s</title>
<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
<link rel="stylesheet" href="../../website/main.css" type="text/css">
<style>td.cmd{padding-right:2em;}</style>
</head>
<body>
<h2>class %s</h2>
<ul>
  <h4>super class</h4>
  <ul>
    <a href="%s">%s</a> 
  </ul>
  <h4>info</h4>
  <ul>
    <pre>%s</pre>
  </ul>""" % (clas.name, clas.name, clas.superclass+'.html', clas.superclass, clas.info.replace("\n    ","\n"))
    out += """  <h4>commands:</h4>
  <ul><table border="0">"""
    # do command block
    numcmds = len(clas.cmds)
    rows = (numcmds / 4) + 1
    for i in range(rows):
        out += "<tr>"
        for j in range(4):
            c = i + (j * rows)
            if c < numcmds:
                cmd = clas.cmds[c]
                out += "<td class=\"cmd\"><a href=\"#%s\">%s</a></td>" % (clas.name+cmd.name, cmd.name)
            else:
                out += "<td></td>"
        out += "</tr>"
        
    out += """</table>
  </ul>
</ul><hr noshade size=" 1"color="#CCCCCC"/>"""
    return out

def cmdBlock(cmd, clas):
    out = """<h3><a name="%s">%s</a></h3>
<ul>
  <h4>input:</h4>
  <ul>
    <pre>%s</pre>
  </ul>
  <h4>output:</h4>
  <ul>
    <pre>%s</pre>
  </ul>
  <h4>info:</h4>
  <ul>
    <pre>%s</pre>
  </ul>
</ul><hr noshade size=" 1"color="#CCCCCC"/>""" % (
        clas.name+cmd.name, 
        cmd.name, 
        cmd.input.replace("\n    ","\n"), 
        cmd.output.replace("\n    ","\n"), 
        cmd.info.replace("\n    ","\n"))
    return out

def classPage(clas):
    out = classInfo(clas)
    for cmd in clas.cmds:
        out += cmdBlock(cmd, clas)
    out += "</body></html>"
    fo = open(os.path.join(dir_classes, '%s.html' % clas), 'w')
    fo.write(out)
    fo.close()

def cmdsPage(root):
    classPage(root)
    for clas in root.subclasses:
        cmdsPage(clas)
    return out

# parse all the cmd files
scanFiles(dir_src)
root = classes['nroot']
root.findSubclasses()

# create class tree
out = """<html>
<head>
    <link rel="stylesheet" href="../website/main.css" type="text/css">
    <style>LI {list-style : square; color: #FFCC99; margin-left:-2em; text-indent:0;}</style>
    <title>Nebula 2 Script Commands (generated %s)</title>
</head>
<body>
    <h3>Nebula 2 Class Tree</h3>
    <ul>
""" % (time.strftime("%d/%m/%y %H:%M"))
out += classTree(root)
out += """
    </ul>
</body>
</html>"""
fp = open(os.path.join(dir_autodoc, 'tree.html'), 'w')
fp.write(out)
fp.close()

# create class pages
if not os.path.exists(dir_classes):
    os.mkdir(dir_classes)
cmdsPage(root)









