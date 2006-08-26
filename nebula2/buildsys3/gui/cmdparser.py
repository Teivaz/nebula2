#--------------------------------------------------------------------------
# cmdparser.py
# *_cmds.cc file parser
# (c) 2005 Oleg Kreptul (Haron)
# Contents are licensed under the Nebula license.
#--------------------------------------------------------------------------

import re
from types import *

COMMENT_MINUS_NUM = 80
COMMENT_LINE = "//" + "-" * COMMENT_MINUS_NUM
PARAM_COMMENT_INDENT = 30
TABSIZE = 4
TABSPACES = " " * TABSIZE

TYPES_LIST = ['i', 's', 'b', 'f', 'ff', 'fff', 'ffff', 'o', 'l']
#TYPES_LIST = ['int', 'string', 'bool', 'float', 'vector2', 'vector3', 'vector4', 'object', 'list', 'quat']
TYPE_CHARS = 'vifsbol'
PARAMDESC_REGEX          = re.compile(r'^([' + TYPE_CHARS + r']+)\s*\(\s*(\w[\w\d]*)\s*(;\s*\w[\w\d]*\s*=\s*[^;]+\s*)*\)\s*(?:--\s*(.+)\s*)?$')
ATTRVAL_REGEX            = re.compile(r'^\s*(\w[\w\d]*)\s*=\s*(.+?\s*(,.+?)?)\s*$')
BASEINFO_REGEX           = re.compile(r'^\s*\/\/(.+)')
INCLUDE_REGEX            = re.compile(r'^\s*#\s*include\s*"(.+)"\s*$')
CMDFUNCTIONDECL_REGEX    = re.compile(r'^.*n_(\w[\w\d]*)\s*\(\s*void\s*\*.*,\s*nCmd\s*\*.*\)\s*;')
INITCMDFUNCTIONDEF_REGEX = re.compile(r'^\s*n_initcmds\s*\(.*\)\s*')
ADDCMDFUNCTION_REGEX     = re.compile(r'^\s*\w[\w\d]*\s*->\s*AddCmd\s*\(\s*"([' + TYPE_CHARS + r']+)_([a-zA-Z][\w\d]+)_([' + TYPE_CHARS + r']+)"\s*,\s*\'(....)\'\s*,\s*n_([a-zA-Z][\w\d]+)\s*\)\s*;')
CMDDOC_REGEX             = re.compile(r'^\s*\/\*\*(.*)$')
CMDFUNCTIONDEF_REGEX     = re.compile(r'^.*n_(\w[\w\d]*)\s*\(\s*void\s*\*.*,\s*nCmd\s*\*.*\)\s*(\{)?')
SAVECMDDEF_REGEX         = re.compile(r'^\s*(bool)?\s*(.+?)::SaveCmds\(.*\)\s*(\{)?')

class CmdFileHeader:
    def __init__(self, gi = "", incl = [], cn = ""):
        self.includes = []
        self.generalInfo = ""
        self.className = ""
        if isinstance(gi, CmdFileHeader):
            self.setFrom(gi)
        else:
            if isinstance(gi, StringType): self.generalInfo = gi
            if isinstance(incl, ListType):
                for e in incl:
                    if isinstance(e, StringType): self.includes.append(e)
                    else: self.includes.append("")
            if isinstance(cn, StringType): self.className = cn

    def clear(self):
        self.generalInfo = ""
        self.includes = []
        self.className = ""

    def __repr__(self):
        res = COMMENT_LINE + "\n"
        res += "//  " + self.className + "_cmds.cc\n"
        for l in self.generalInfo.splitlines():
            res += "//  " + l.strip() + "\n"
        res += COMMENT_LINE + "\n"
        for incl in self.includes:
            res += "#include \"" + incl + "\"\n"
        return res

    def setFrom(self, hdr):
        if isinstance(hdr, CmdFileHeader):
            self.generalInfo = hdr.generalInfo[:]
            self.includes = []
            for e in hdr.includes:
                self.includes.append(e[:])
            self.className = hdr.className[:]

    def __cmp__(self, c):
        if not isinstance(c, CmdFileHeader): return 1
        if not (self.generalInfo == c.generalInfo and \
                self.className == c.className and\
                self.includes == c.includes):
            return 1
        return 0

class CmdClassInfo:
    def __init__(self, cppcl = "", spcl = "", scrcl = "", cli = ""):
        self.cppClass = ""
        self.superClass = ""
        self.info = ""
        self.scriptClass = ""
        if isinstance(cppcl, CmdClassInfo):
            self.setFrom(cppcl)
        else:
            if isinstance(cppcl, StringType): self.cppClass = cppcl
            if isinstance(spcl, StringType): self.superClass = spcl
            if isinstance(cli, StringType): self.info = cli
            if isinstance(scrcl, StringType): self.scriptClass = scrcl

    def clear(self):
        self.cppClass
        self.superClass
        self.info
        self.scriptClass = "", "", "", "", ""

    def __repr__(self):
        if not self.scriptClass:
            self.scriptClass = self.cppClass.lower()
        res = ""
        res += "\n"
        res += COMMENT_LINE + "\n"
        res += "/**\n"
        res += TABSPACES + "@scriptclass " + self.scriptClass + "\n"
        res += TABSPACES + "@cppclass " + self.cppClass + "\n"
        res += TABSPACES + "@superclass " + self.superClass + "\n"
        res += TABSPACES + "@classinfo\n"
        for l in self.info.splitlines():
            res += TABSPACES + "  " + l.strip() + "\n"
        res += "*/\n"
        return res

    def setFrom(self, c):
        if isinstance(c, CmdClassInfo):
            self.cppClass = c.cppClass[:]
            self.superClass = c.superClass[:]
            self.scriptClass = c.scriptClass[:]
            self.info = c.info[:]

    def __cmp__(self, c):
        if not isinstance(c, CmdClassInfo): return 1
        if not (self.cppClass == c.cppClass and\
                self.superClass == c.superClass and\
                self.scriptClass == c.scriptClass and\
                self.info == c.info):
            return 1
        return 0

class CmdParam:
    def __init__(self, t = "v", n = "", a = {}, c = ""):
        self.type = "v"
        self.name = ""
        self.attribute = {}
        self.comment = ""
        if isinstance(t, CmdParam):
            self.setFrom(t)
        else:
            if isinstance(t, StringType) and re.match(r'^[' + TYPE_CHARS + r']+$', t): self.type = t

            # type cleanning
            self.type = self.type.replace('v', '')
            if self.type == '':
                self.type = 'v'
            else:
                if isinstance(n, StringType): self.name = n
                if isinstance(a, DictType):
                    for k, v in map(None, a.keys(), a.values()):
                        if isinstance(v, StringType): self.attribute[k] = v
                        elif isinstance(v, ListType):
                            v_ = []
                            for e in v:
                                if isinstance(v, StringType): v_.append(e)
                                else: v_.append("<unknown>")
                            self.attribute[k] = v_
                        else: self.attribute[k] = "<unknown>"
                if isinstance(c, StringType): self.comment = c

    def __repr__(self):
        res = self.type
        if self.type != 'v':
            res += "(" + self.name
            for k, v in map(None, self.attribute.keys(), self.attribute.values()):
                res += "; " + k + "="
                if isinstance(v, ListType): res += ','.join(v)
                else: res += v
            res += ")"
            ind = PARAM_COMMENT_INDENT - len(res)
            if ind < 1: ind = 1
            res += " " * ind + "-- " + self.comment
        res += "\n"
        return res

    def setFrom(self, c):
        if isinstance(c, CmdParam):
            self.type = c.type[:]
            self.name = c.name[:]
            self.attribute = {}
            for k, v in map(None, self.attribute.keys(), self.attribute.values()):
                if isinstance(v, StringType): self.attribute[k] = v[:]
                elif isinstance(v, ListType):
                    v_ = []
                    for e in v: v_.append(e[:])
                    self.attribute[k] = v_
                else: self.attribute[k] = "<unknown>"
            self.comment = c.comment[:]

    def __cmp__(self, c):
        if not isinstance(c, CmdParam): return 1
        if not (self.type == c.type and\
                self.name == c.name and\
                self.comment == c.comment and\
                self.attribute == c.attribute):
            return 1
        return 0

class CmdProperty:
    def __init__(self, n, p = [], i = ""):
        self.params = []
        self.info = ""
        if isinstance(n, CmdProperty): self.setFrom(n)
        else:
            if isinstance(n, StringType): self.name = n
            else: self.name = "<unknown>"
            if isinstance(p, ListType):
                has_void = False
                has_not_void = False
                for e in p:
                    if isinstance(e, CmdParam):
                        if e.type == 'v':
                            if not has_void and not has_not_void:
                                self.params.append(e)
                                has_void = True
                        else:
                            self.params.append(e)
                            has_not_void = True
                if has_void and has_not_void: #delete first void's
                    for (k, p) in self.params.iteritems():
                        if p.type == 'v': del(self.params[k])
                        else: break
            else: self.params.append(CmdParam())
        if isinstance(i, StringType): self.info = i

    def __repr__(self):
        res = COMMENT_LINE + "\n"
        res += "/**\n"
        res += TABSPACES + "@property " + self.name + "\n"
        res += TABSPACES + "@format\n"
        for p in self.params: res += TABSPACES + "  " + repr(p)
        res += TABSPACES + "@info\n"
        for l in self.info.splitlines(): res += TABSPACES + "  " + l.strip() + "\n"
        res += "*/\n"
        return res

    def setFrom(self, c):
        if isinstance(c, CmdProperty):
            self.name = c.name[:]
            self.params = []
            for e in c.params: self.params.append(CmdParam(e))
            self.info = c.info[:]

    def __cmp__(self, c):
        if not isinstance(c, CmdProperty): return 1
        if not (self.name == c.name and\
                self.info == c.info and\
                self.params == c.params):
            return 1
        return 0

class BaseCmd:
    CMD_DECLARED = 1 << 0
    CMD_ADDED    = 1 << 1
    CMD_DEFINED  = 1 << 2

    def __init__(self, fn, cn = "", out_f = "", in_f = "", fcc = "", i = "", cb = ""):
        self.funcName = "<unknown>"
        self.cmdName = "<unknown>"
        self.outFormat = "v"
        self.inFormat = "v"
        self.fourCC = "---"
        self.info = ""
        self.cmdBody = ""
        self.status = 0
        if isinstance(fn, BaseCmd):
            self.funcName = fn.funcName[:]
            self.cmdName = fn.cmdName[:]
            self.outFormat = fn.outFormat[:]
            self.inFormat = fn.inFormat[:]
            self.fourCC = fn.fourCC[:]
            self.info = fn.info[:]
            self.cmdBody = fn.cmdBody[:]
            self.status = fn.status
        else:
            if isinstance(fn, StringType): self.funcName = fn
            if isinstance(cn, StringType): self.cmdName = cn
            else: self.cmdName = self.funcName[:]
            if isinstance(out_f, StringType) and re.match(r'^[' + TYPE_CHARS + r']+$', out_f):
                self.outFormat = out_f
            if isinstance(in_f, StringType) and re.match(r'^[' + TYPE_CHARS + r']+$', in_f):
                self.inFormat = in_f
            if isinstance(fcc, StringType): self.fourCC = fcc
            if isinstance(i, StringType): self.info = i
            if isinstance(cb, StringType): self.cmdBody = cb

    def setStatus(self, st):
        if st == BaseCmd.CMD_DECLARED or st == BaseCmd.CMD_ADDED or st == BaseCmd.CMD_DEFINED:
            self.status |= st

    def _specific_str(self): return ""

    def __repr__(self):
        res = COMMENT_LINE + "\n"
        res += "/**\n"
        res += TABSPACES + "@cmd " + self.cmdName + "\n"
        res += self._specific_str()
        res += TABSPACES + "@info\n"
        for l in self.info.splitlines(): res += TABSPACES + "  " + l.strip() + "\n"
        res += "*/\n"
        res += "static void\n"
        res += "n_" + self.funcName + "(void* slf, nCmd* cmd)\n"
        res += "{\n"
        for l in self.cmdBody.splitlines(): res += l + "\n"
        res += "}\n"
        return res

    def setFrom(self, c):
        if isinstance(c, BaseCmd):
            self.funcName = c.funcName[:]
            self.cmdName = c.cmdName[:]
            self.outFormat = c.outFormat[:]
            self.inFormat = c.inFormat[:]
            self.fourCC = c.fourCC[:]
            self.info = c.info[:]
            self.cmdBody = c.cmdBody[:]
            self.status = c.status

    def __cmp__(self, c):
        if not issubclass(c.__class__, BaseCmd): return 1
        if not (self.funcName == c.funcName and\
                self.cmdName == c.cmdName and\
                self.info == c.info and\
                self.cmdBody == c.cmdBody and\
                self.outFormat == c.outFormat and\
                self.inFormat == c.inFormat and\
                self.fourCC == c.fourCC): # and self.status == c.status
            return 1
        return 0

class Cmd(BaseCmd):
    T_GETTER, T_SETTER = 0, 1
    M_NONE, M_COUNT, M_BEGIN, M_ADD, M_END = 0, 1, 2, 3, 4

    def __init__(self, common_cmd, t, st = M_NONE, p = ""):
        self.type = Cmd.T_GETTER
        self.subType = Cmd.M_NONE
        self.propertyName = ""
        # default base initialization
        if isinstance(common_cmd, Cmd):
            self.setFrom(common_cmd)
        else:
            BaseCmd.__init__(self, common_cmd)
            if t == Cmd.T_SETTER: self.type = Cmd.T_SETTER
            if (t == Cmd.T_GETTER and st == Cmd.M_COUNT) or\
               (t == Cmd.T_SETTER and (st == Cmd.M_BEGIN or st == Cmd.M_ADD or st == Cmd.M_END)):
                self.subType = st
            if isinstance(p, StringType): self.propertyName = p

    def _specific_str(self):
        res = TABSPACES + "@cmdtype " + ["getter", "setter"][self.type]
        if self.subType != Cmd.M_NONE:
            res += "|" + ["", "count", "begin", "add", "end"][self.subType]
        res += "\n"
        res += TABSPACES + "@property " + self.propertyName + "\n"
        return res

    def setFrom(self, c):
        if isinstance(c, BaseCmd):
            BaseCmd.setFrom(self, c)
            if isinstance(c, Cmd):
                self.type = c.type
                self.subType = c.subType
                self.propertyName = c.propertyName[:]

    def __cmp__(self, c):
        if not isinstance(c, Cmd): return 1
        if BaseCmd.__cmp__(self, c): return 1
        if not (self.type == c.type and\
                self.subType == c.subType and\
                self.propertyName == c.propertyName):
            return 1
        return 0

class CommonCmd(BaseCmd):
    def __init__(self, common_cmd, op = [], ip = []):
        self.outParams = []
        self.inParams = []
        # default base initialization
        if isinstance(common_cmd, CommonCmd):
            self.setFrom(common_cmd)
        else:
            BaseCmd.__init__(self, common_cmd)
            #if isinstance(op, ListType):
            #    for e in op:
            #        if isinstance(e, CmdParam): self.outParams.append(e)
            #        else: self.outParams.append(CmdParam())
            if isinstance(op, ListType):
                has_void = False
                has_not_void = False
                for e in op:
                    if isinstance(e, CmdParam):
                        if e.type == 'v':
                            if not has_void and not has_not_void:
                                self.outParams.append(e)
                                has_void = True
                        else:
                            self.outParams.append(e)
                            has_not_void = True
                if has_void and has_not_void: #delete first void's
                    for (k, op) in self.outParams.iteritems():
                        if op.type == 'v': del(self.outParams[k])
                        else: break
            else: self.outParams.append(CmdParam())

            #if isinstance(ip, ListType):
            #    for e in ip:
            #        if isinstance(e, CmdParam): self.inParams.append(e)
            #        else: self.inParams.append(CmdParam())
            if isinstance(ip, ListType):
                has_void = False
                has_not_void = False
                for e in ip:
                    if isinstance(e, CmdParam):
                        if e.type == 'v':
                            if not has_void and not has_not_void:
                                self.inParams.append(e)
                                has_void = True
                        else:
                            self.inParams.append(e)
                            has_not_void = True
                if has_void and has_not_void: #delete first void's
                    for (k, ip) in self.inParams.iteritems():
                        if ip.type == 'v': del(self.inParams[k])
                        else: break
            else: self.inParams.append(CmdParam())

    def _specific_str(self):
        res = TABSPACES + "@output\n"
        for p in self.outParams:
            res += TABSPACES + "  " + repr(p)

        res += TABSPACES + "@input\n"
        for p in self.inParams:
            res += TABSPACES + "  " + repr(p)

        return res

    def setFrom(self, c):
        if isinstance(c, BaseCmd):
            BaseCmd.setFrom(self, c)
            if isinstance(c, BaseCmd):
                self.outParams = []
                self.inParams = []
                for e in c.outParams: self.outParams.append(CmdParam(e))
                for e in c.inParams: self.inParams.append(CmdParam(e))

    def __cmp__(self, c):
        if not isinstance(c, BaseCmd): return 1
        if BaseCmd.__cmp__(self, c): return 1
        if not (self.outParams == c.outParams and\
                self.inParams == c.inParams):
            return 1
        return 0

class SaveCmdsFunction:
    def __init__(self, cd = "", cb = "", c = ""):
        self.classDef = ""
        self.cmdBody = ""
        self.comment = ""
        if isinstance(cd, SaveCmdsFunction): self.setFrom(cd)
        else:
            if isinstance(cd, StringType): self.classDef = cd
            if isinstance(cb, StringType): self.cmdBody = cb
            if isinstance(c, StringType): self.comment = c

    def clear(self):
        self.classDef, self.cmdBody, self.comment = "", "", ""

    def __repr__(self):
        res = COMMENT_LINE + "\n"
        res += "/**\n"
        for l in self.comment.splitlines(): res += "    " + l.strip() + "\n"
        res += "*/\n"
        res += "bool\n"
        res += self.classDef + "::SaveCmds(nPersistServer *ps)\n"
        res += "{\n"
        for l in self.cmdBody.splitlines(): res += l + "\n"
        res += "}\n"
        return res

    def setFrom(self, c):
        if isinstance(c, SaveCmdsFunction):
            self.classDef = c.classDef[:]
            self.cmdBody = c.cmdBody[:]
            self.comment = c.comment[:]

    def __cmp__(self, c):
        if not isinstance(c, SaveCmdsFunction): return 1
        if not (self.classDef == c.classDef and\
                self.cmdBody == c.cmdBody and\
                self.comment == c.comment):
            return 1
        return 0

#---------------------------------------------------------------------------------------
class CmdFileConfig:
    def __init__(self, hdr = None, cli = None, p = [], c = [], scf = None):
        self.header = CmdFileHeader()
        self.classInfo = CmdClassInfo()
        self.properties = []
        self.cmds = []
        self.saveCmdsFunc = None
        if isinstance(hdr, CmdFileConfig): self.setFrom(hdr)
        else:
            if isinstance(hdr, CmdFileHeader): self.header = hdr
            if isinstance(cli, CmdClassInfo): self.classInfo = cli
            if isinstance(p, ListType):
                for e in p:
                    if isinstance(e, CmdProperty): self.properties.append(e)
                    else: self.properties.append(CmdProperty())
            if isinstance(c, ListType):
                for e in c:
                    if issubclass(e.__class__, BaseCmd): self.cmds.append(e)
                    else: self.cmds.append(BaseCmd(None))
            if isinstance(scf, SaveCmdsFunction): self.saveCmdsFunc = scf

    def findProperty(self, name):
        for p in self.properties:
            if p.name == name: return p
        return None

    def getCmdIdx(self, name):
        n = len(self.cmds)
        for i in range(n):
            if self.cmds[i].funcName == name:
                return i
        return None

    def findCmd(self, name):
        for c in self.cmds:
            if c.funcName == name: return c
        return None

    def setCmdAt(self, idx, cmd):
        if idx < len(self.cmds):
            self.cmds[idx] = cmd
        else:
            self.cmds.append(cmd)

    def clear(self):
        self.header.clear()
        self.classInfo.clear()
        self.properties = []
        self.cmds = []
        self.saveCmdsFunc = None

    def sortCmds(self):
        self.properties.sort(lambda p1, p2: cmp(p1.name, p2.name))
        self.cmds.sort(self._cmdsSortFunc)

    def _cmdsSortFunc(self, c1, c2):
        if isinstance(c1, Cmd):
            if isinstance(c2, Cmd):
                res = cmp(c1.propertyName, c2.propertyName)
                if res == 0:
                    if c1.type == Cmd.T_SETTER: return 1
                    else: return -1
                else: return res
            else: return 1  # c1 is greater then c2 because cmd's with property should be above not propertied
        else:
            if isinstance(c2, Cmd): return -1
            else: return cmp(c1.cmdName, c2.cmdName)

    def __repr__(self):
        size = 0
        res = repr(self.header)
        res += "\n"
        for cmd in self.cmds:
            s = len(cmd.outFormat) + len(cmd.funcName) + len(cmd.inFormat)
            if s > size: size = s
            res += "static void n_" + cmd.funcName + "(void*, nCmd*);\n"
        size += 1
        res += repr(self.classInfo)
        res += "void\n"
        res += "n_initcmds(nClass* cl)\n"
        res += "{\n"
        res += TABSPACES + "cl->BeginCmds();\n"
        for cmd in self.cmds:
            s = len(cmd.outFormat) + len(cmd.funcName) + len(cmd.inFormat)
            res += TABSPACES + "cl->AddCmd(\"" + cmd.outFormat + "_" +\
                cmd.cmdName + "_" + cmd.inFormat + "\"," + " " * (size - s) +\
                "\'" + cmd.fourCC + "\', n_" + cmd.funcName + ");\n"
        res += TABSPACES + "cl->EndCmds();\n"
        res += "}\n"

        printed_prop = {}
        for cmd in self.cmds:
            if isinstance(cmd, Cmd) and not printed_prop.has_key(cmd.propertyName):
                prop = self.findProperty(cmd.propertyName)
                res += '\n' + repr(prop)
                printed_prop[cmd.propertyName] = None
            res += '\n' + repr(cmd)

        if self.saveCmdsFunc: res += '\n' + repr(self.saveCmdsFunc)
        res += "\n"
        return res

    def saveTo(self, fileName):
        f = open(fileName, "wb")
        #for l in self.__repr__().splitlines(): f.write(l)
        f.write(self.__repr__())
        f.close()

    def setFrom(self, c):
        if isinstance(c, CmdFileConfig):
            self.header = CmdFileHeader(c.header)
            self.classInfo = CmdClassInfo(c.classInfo)
            self.properties = []
            self.cmds = []
            for e in c.properties:
                if isinstance(e, CmdProperty): self.properties.append(e)
                else: self.properties.append(CmdProperty())
            for e in c.cmds:
                if isinstance(e, BaseCmd): self.cmds.append(e)
                else: self.cmds.append(BaseCmd(None))
            if self.saveCmdsFunc: self.saveCmdsFunc = SaveCmdsFunction(c.saveCmdsFunc)
            else: self.saveCmdsFunc = None

    def __cmp__(self, c):
        if not isinstance(c, CmdFileConfig): return 1
        if not (self.header == c.header and\
                self.classInfo == c.classInfo and\
                self.properties == c.properties and\
                self.cmds == c.cmds and\
                self.saveCmdsFunc == c.saveCmdsFunc):
            return 1
        return 0

#---------------------------------------------------------------------------------------
def dispatchExtComment(hf, firstStr):
    commentStr = ""
    curStr = firstStr.strip()
    if len(curStr) > 0: curStr += "\n"
    reg = re.compile(r'^(.*?)\*\/')
    g = reg.search(curStr)
    while not g:
        commentStr += curStr
        curStr = hf.readline().strip() + "\n"
        g = reg.search(curStr)
    commentStr += g.group(1)
    return commentStr.strip()

def dispatchDoc(docStr):
    atBlocks = {"?0": ""}
    restBlocks = ""
    atName = ""
    needEnd = ""
    freeBlockIdx = 0

    reg = re.compile(r'^@\s*(\w[\w\d]*)(?:\s+(.*))?$')
    for ln in docStr.splitlines():
        line = ln.strip()
        if len(line) > 0:
            g = reg.search(line)
            if g:
                if needEnd == "":
                    atName = g.group(1)
                    if g.group(2): atBlocks[atName] = g.group(2)
                    else: atBlocks[atName] = ""
                    if line == "verbatim": needEnd = "verbatim"
                    if line == "code": needEnd = "code"
                else:
                    if g.group(1) == "end" + needEnd: needEnd = ""
                    else:
                        if atName: atBlocks[atName] += "\n" + line
                        else: restBlocks += "\n" + line
            elif atName: atBlocks[atName] += "\n" + line
            else:
                restBlocks += "\n" + line
                fbNum = "?" + repr(freeBlockIdx)
                if not atBlocks.has_key(fbNum): atBlocks[fbNum] = ""
                atBlocks[fbNum] += "\n" + line
        else:
            atName = ""
            fbNum = "?" + repr(freeBlockIdx)
            if atBlocks.has_key(fbNum):
                if atBlocks[fbNum]:
                    freeBlockIdx += 1
                    atBlocks[fbNum] = ""
            else:
                atBlocks[fbNum] = ""

##    if atBlocks["?" + freeBlockIdx] == "":
##        atBlocks.delete("?" + freeBlockIdx.to_s)
    for v in atBlocks.values(): v = v.strip()
    return atBlocks

class CmdFileProcessor:
    def __init__(self, code_dir, is_contrib = False, mod_name = "", pkg_name = "", file_name = ""):
        self.codeDir, self.isContrib, self.modName, self.pkgName, self.fileName = code_dir, is_contrib, mod_name, pkg_name, file_name

        self.config = None #CmdFileConfig()
        self.processed = False
        if self.codeDir[-1] != "/": self.codeDir += "/"

    def setCmdFile(self, is_contrib, mod_name, pkg_name, file_name):
        if is_contrib != self.isContrib or mod_name != self.modName or pkg_name != self.pkgName or file_name != self.fileName:
            self.isContrib, self.modName, self.pkgName, self.fileName = is_contrib, mod_name, pkg_name, file_name
            self.config = CmdFileConfig() #.clear
            self.processed = False

    def dispatchParam(self, paramDesc):
        param = None
        if paramDesc == 'v': param = CmdParam('v')
        else:
            g = PARAMDESC_REGEX.search(paramDesc)
            if g:
                param = CmdParam(g.group(1), g.group(2))
                if g.group(4):
                    param.comment = g.group(4)
                if g.group(3):
                    attrList = g.group(3)[1:]
                    for av in attrList.split(';'):
                        g = ATTRVAL_REGEX.search(av)
                        if g:
                            attrName = g.group(1)
                            if g.group(3): param.attribute[attrName] = g.group(2).split(',')
                            else: param.attribute[attrName] = g.group(2)
        return param

    def extractBody(self, file, startBracket):
        res = ""
        endBracket = False
        curFilePos = file.tell()
        line = file.readline()
        while line:
            if startBracket:
                if line[0] == '}':
                    endBracket = True
                    break
                else:
                    if line[0:TABSIZE-1] == TABSPACES:
                        res += line[TABSIZE:-1].rstrip() + "\n"
                    else:
                        res += line.rstrip() + "\n"
            elif line[0] == '{': #/^\{\s*(.+)?/
                startBracket = True
                l = line[1:-1].strip()
                if l: res += TABSPACES + l + "\n"
            line = file.readline()

        if not endBracket:
            res = ""
            file.seek(curFilePos)
            print "Error: can't found bracket }"

        return res.rstrip()

    def _parseBaseInfo(self, g, baseInfo):
        s = g.group(1).strip()
        if not re.search(r'^-+$', s):
            if baseInfo:
                baseInfo += "\n" + s
            elif s and not re.search(r'^\s*\w[\w\d]*_cmds.cc', s):
                baseInfo = s
        return baseInfo

    def _parseInclude(self, g, baseInfo):
        if baseInfo:
            self.config.header.generalInfo = baseInfo
            baseInfo = None
        self.config.header.includes.append(g.group(1).strip())
        return baseInfo

    def _parseCmdFunctionDecl(self, g, baseInfo):
        if baseInfo:
            self.config.header.generalInfo = baseInfo
            baseInfo = None
        cmd = BaseCmd(g.group(1))
        cmd.setStatus(BaseCmd.CMD_DECLARED)
        self.config.cmds.append(cmd)
        return baseInfo

    def _parseAddCmdFunction(self, g):
        cmd = self.config.findCmd(g.group(5))
        if not cmd:
            cmd = BaseCmd.new(g.group(5))
            self.config.cmds.append(cmd)
        cmd.cmdName = g.group(2)
        cmd.outFormat = g.group(1)
        if len(cmd.outFormat) > 1 and 'v' in cmd.outFormat:
            print "Error: wrong output format for the command " + cmd.cmdName + "."
        cmd.inFormat = g.group(3)
        if len(cmd.inFormat) > 1 and 'v' in cmd.inFormat:
            print "Error: wrong input format for the command " + cmd.cmdName + "."
        cmd.fourCC = g.group(4)
        cmd.setStatus(BaseCmd.CMD_ADDED)

    def _parseCmdDoc(self, g, ccFile, commentBlock):
        if commentBlock:
            atBlocks = dispatchDoc(commentBlock)
            if atBlocks.has_key("property") and atBlocks.has_key("format"):
                prop = CmdProperty(atBlocks["property"].strip())
                for l in atBlocks["format"].strip().splitlines():
                    prop.params.append(self.dispatchParam(l.strip()))
                prop.info = atBlocks["info"]
                self.config.properties.append(prop)
        commentBlock = dispatchExtComment(ccFile, g.group(1))
        return commentBlock

    def _parseInitCmdFunctionDef(self, className, commentBlock):
        if not commentBlock:
            print "WARNING: There is no comment block for n_initcmds. Default comment block will be set."
            commentBlock = " @scriptclass " + className + "\n @cppclass " + className + "\n @superclass nroot\n @classinfo <No information>"
        atBlocks = dispatchDoc(commentBlock)
        commentBlock = None

        if atBlocks.has_key("scriptclass"):
            self.config.classInfo.scriptClass = atBlocks["scriptclass"].strip()
        else:
            self.config.classInfo.scriptClass = className

        if atBlocks.has_key("cppclass"):
            self.config.classInfo.cppClass = atBlocks["cppclass"].strip()
        else:
            self.config.classInfo.cppClass = className

        if atBlocks.has_key("superclass"):
            self.config.classInfo.superClass = atBlocks["superclass"].strip()
        else:
            self.config.classInfo.superClass = "nroot"

        if atBlocks.has_key("classinfo"):
            self.config.classInfo.info = atBlocks["classinfo"].strip()
        else:
            self.config.classInfo.info = ""

        return commentBlock

    def _parseCmdFunctionDef(self, g, ccFile, commentBlock):
        cmdName = g.group(1)
        cmd = self.config.findCmd(cmdName)

        if not cmd:
            cmd = BaseCmd(cmdName, cmdName, "v", "v", "----")
            self.config.cmds.append(cmd)
        cmd.setStatus(BaseCmd.CMD_DEFINED)
        cmd_idx = self.config.cmds.index(cmd)

        if not commentBlock:
            print "WARNING: There is no comment block for n_" + cmdName + "."
            commentBlock = " @cmd " + cmdName + "\n @output\n  " + cmd.outFormat + "(outName)\n @input\n  " +cmd.inFormat + "(inName) -- Name of target joint\n @info <No information>"

        atBlocks = dispatchDoc(commentBlock)
        commentBlock = None

        if atBlocks.has_key("info"): cmd.info = atBlocks["info"].strip()
        else: cmd.info = "<no info>"

        if atBlocks["cmd"] != cmdName:
            atBlocks["cmd"] = cmdName
            # send error msg

        if atBlocks.has_key("property"): # getter/setter command
            tt = atBlocks["cmdtype"].split("|")
            type_s = tt[0].strip()
            subType_s = ""
            if len(tt) > 1: subType_s = tt[1].strip()
            type = 0
            subType = Cmd.M_NONE
            if type_s == "getter":
                type = Cmd.T_GETTER
                if subType_s == "count": subType = Cmd.M_COUNT
            elif type_s == "setter":
                type = Cmd.T_SETTER
                if subType_s == "begin": subType = Cmd.M_BEGIN
                elif subType_s == "add": subType = Cmd.M_ADD
                elif subType_s == "end": subType = Cmd.M_END
                else:
                    # try to guess what subType
                    if re.match(r'^begin', cmdName): subType = Cmd.M_BEGIN
                    elif re.match(r'^add', cmdName): subType = Cmd.M_ADD
                    elif re.match(r'^end', cmdName): subType = Cmd.M_END
                    else: subType = Cmd.M_NONE
            else:
                # try to guess type and subType
                if re.match(r'^set', cmdName):
                    type = Cmd.T_SETTER
                    subType = Cmd.M_NONE
                elif re.match(r'^begin', cmdName):
                    type = Cmd.T_SETTER
                    subType = Cmd.M_BEGIN
                elif re.match(r'^add', cmdName):
                    type = Cmd.T_SETTER
                    subType = Cmd.M_ADD
                elif re.match(r'^end', cmdName):
                    type = Cmd.T_SETTER
                    subType = Cmd.M_END
                else:
                    type = Cmd.T_GETTER
                    subType = Cmd.M_NONE

            propName = atBlocks["property"].strip()
            cmd = Cmd(cmd, type, subType, propName)
            prop = self.config.findProperty(propName)
            if not prop:
                prop = CmdProperty(propName)
                self.config.properties.append(prop)
                # try to guess property params
                p_type = cmd.outFormat
                if type == Cmd.T_SETTER: p_type = cmd.inFormat
                prop.params.append(CmdParam(p_type, "name"))
        else: # this is simple command, not getter/setter
            cmd = CommonCmd(cmd)
            if atBlocks.has_key("output"):
                lines = atBlocks["output"].strip().splitlines()
                if len(lines) > 0:
                    for l in lines:
                        param = self.dispatchParam(l.strip())
                        if param:
                            cmd.outParams.append(param)
                        else:
                            cmd.outParams.append(CmdParam(cmd.outFormat, "name"))
                else:
                    cmd.outParams.append(CmdParam(cmd.outFormat, "name"))
            else:
                cmd.outParams.append(CmdParam(cmd.outFormat, "name"))

            if atBlocks.has_key("input"):
                lines = atBlocks["input"].strip().splitlines()
                if len(lines) > 0:
                    for l in lines:
                        param = self.dispatchParam(l.strip())
                        if param:
                            cmd.inParams.append(param)
                        else:
                            cmd.inParams.append(CmdParam(cmd.inFormat, "name"))
                else:
                    cmd.inParams.append(CmdParam(cmd.inFormat, "name"))
            else:
                cmd.inParams.append(CmdParam(cmd.inFormat, "name"))

        # extract cmd body
        cmd.cmdBody = self.extractBody(ccFile, g.group(2))
        self.config.cmds[cmd_idx] = cmd
        return commentBlock

    def process(self, is_contrib = False, mod_name = None, pkg_name = None, file_name = None):
        if not mod_name: mod_name = self.modName
        if not pkg_name: pkg_name = self.pkgName
        if not file_name: file_name = self.fileName
        self.setCmdFile(is_contrib, mod_name, pkg_name, file_name)

        if self.processed: return self.config

        #print "Start processing: " + self.fileName

        s = ""
        if self.isContrib: s = "contrib/"
        file_path = self.codeDir + s + self.modName + "/src/" + self.pkgName + "/" + self.fileName

        className = ""
        #processing
        g = re.search(r'^(.+)_cmds.cc$', self.fileName)
        if g:
            className = g.group(1)
            self.config.header.className = g.group(1)
        else: return None

        ccFile = open(file_path, "rb")
        #tmpFile = open(fileName + ".new", "wb")

        baseInfo = None
        commentBlock = None

        line = ccFile.readline()
        while line:
            g = BASEINFO_REGEX.search(line)
            if g: baseInfo = self._parseBaseInfo(g, baseInfo)
            else:
                g = INCLUDE_REGEX.search(line)
                if g: baseInfo = self._parseInclude(g, baseInfo)
                else:
                    g = CMDFUNCTIONDECL_REGEX.search(line)
                    if g: baseInfo = self._parseCmdFunctionDecl(g, baseInfo)
                    else:
                        g = CMDDOC_REGEX.search(line) #extract documentation
                        if g: commentBlock = self._parseCmdDoc(g, ccFile, commentBlock)
                        else:
                            g = INITCMDFUNCTIONDEF_REGEX.search(line)
                            if g: commentBlock = self._parseInitCmdFunctionDef(className, commentBlock)
                            else:
                                g = ADDCMDFUNCTION_REGEX.search(line)
                                if g: self._parseAddCmdFunction(g)
                                else:
                                    g = CMDFUNCTIONDEF_REGEX.search(line)
                                    if g: commentBlock = self._parseCmdFunctionDef(g, ccFile, commentBlock)
                                    else:
                                        g = SAVECMDDEF_REGEX.search(line)
                                        if g:
                                            c = ""
                                            if commentBlock: c = commentBlock
                                            body = self.extractBody(ccFile, g.group(3))
                                            self.config.saveCmdsFunc = SaveCmdsFunction(g.group(2), body, c)
                                        #else: pass
            line = ccFile.readline() #read next line from file
        ccFile.close()

        #self.config.sortCmds()
        self.processed = True
        return self.config

### testing
##p = CmdFileProcessor("D:\\Projects\\VisualC\\nebula2\\code\\")
##
### D:\Projects\VisualC\nebula2\code\contrib\bombsquadbruce\src\BombsquadBruce\bbcamera_cmds.cc
###p.setCmdFile(True, "bombsquadbruce", "BombsquadBruce", "bbcamera_cmds.cc")
##
### D:\Projects\VisualC\nebula2\code\nebula2\src\scene\nabstractshadernode_cmds.cc
##p.setCmdFile(False, "nebula2", "scene", "nabstractshadernode_cmds.cc")
##
##p.process().saveTo("test.log")
