#--------------------------------------
# shdpanel.py
#--------------------------------------

import pynebula as nebula
import os, wx
import wx.lib.foldpanelbar as fpbar
import wx.lib.masked as mask
import wx.lib.colourselect as csel

from xml.dom import minidom, Node

#--------------------------------------

fileSvr = nebula.lookup('/sys/servers/file2')
napp = nebula.lookup('/app')

#--------------------------------------

def GetCollapsedIconData():
    return \
'\x89PNG\r\n\x1a\n\x00\x00\x00\rIHDR\x00\x00\x00\x10\x00\x00\x00\x10\x08\x06\
\x00\x00\x00\x1f\xf3\xffa\x00\x00\x00\x04sBIT\x08\x08\x08\x08|\x08d\x88\x00\
\x00\x01\x8eIDAT8\x8d\xa5\x93-n\xe4@\x10\x85?g\x03\n6lh)\xc4\xd2\x12\xc3\x81\
\xd6\xa2I\x90\x154\xb9\x81\x8f1G\xc8\x11\x16\x86\xcd\xa0\x99F\xb3A\x91\xa1\
\xc9J&\x96L"5lX\xcc\x0bl\xf7v\xb2\x7fZ\xa5\x98\xebU\xbdz\xf5\\\x9deW\x9f\xf8\
H\\\xbfO|{y\x9dT\x15P\x04\x01\x01UPUD\x84\xdb/7YZ\x9f\xa5\n\xce\x97aRU\x8a\
\xdc`\xacA\x00\x04P\xf0!0\xf6\x81\xa0\xf0p\xff9\xfb\x85\xe0|\x19&T)K\x8b\x18\
\xf9\xa3\xe4\xbe\xf3\x8c^#\xc9\xd5\n\xa8*\xc5?\x9a\x01\x8a\xd2b\r\x1cN\xc3\
\x14\t\xce\x97a\xb2F0Ks\xd58\xaa\xc6\xc5\xa6\xf7\xdfya\xe7\xbdR\x13M2\xf9\
\xf9qKQ\x1fi\xf6-\x00~T\xfac\x1dq#\x82,\xe5q\x05\x91D\xba@\xefj\xba1\xf0\xdc\
zzW\xcff&\xb8,\x89\xa8@Q\xd6\xaaf\xdfRm,\xee\xb1BDxr#\xae\xf5|\xddo\xd6\xe2H\
\x18\x15\x84\xa0q@]\xe54\x8d\xa3\xedf\x05M\xe3\xd8Uy\xc4\x15\x8d\xf5\xd7\x8b\
~\x82\x0fh\x0e"\xb0\xad,\xee\xb8c\xbb\x18\xe7\x8e;6\xa5\x89\x04\xde\xff\x1c\
\x16\xef\xe0p\xfa>\x19\x11\xca\x8d\x8d\xe0\x93\x1b\x01\xd8m\xf3(;x\xa5\xef=\
\xb7w\xf3\x1d$\x7f\xc1\xe0\xbd\xa7\xeb\xa0(,"Kc\x12\xc1+\xfd\xe8\tI\xee\xed)\
\xbf\xbcN\xc1{D\x04k\x05#\x12\xfd\xf2a\xde[\x81\x87\xbb\xdf\x9cr\x1a\x87\xd3\
0)\xba>\x83\xd5\xb97o\xe0\xaf\x04\xff\x13?\x00\xd2\xfb\xa9`z\xac\x80w\x00\
\x00\x00\x00IEND\xaeB`\x82'

def GetCollapsedIconBitmap():
    return wx.BitmapFromImage(GetCollapsedIconImage())

def GetCollapsedIconImage():
    import cStringIO
    stream = cStringIO.StringIO(GetCollapsedIconData())
    return wx.ImageFromStream(stream)

#--------------------------------------

def GetExpandedIconData():
    return \
'\x89PNG\r\n\x1a\n\x00\x00\x00\rIHDR\x00\x00\x00\x10\x00\x00\x00\x10\x08\x06\
\x00\x00\x00\x1f\xf3\xffa\x00\x00\x00\x04sBIT\x08\x08\x08\x08|\x08d\x88\x00\
\x00\x01\x9fIDAT8\x8d\x95\x93\xa1\x8e\xdc0\x14EO\xb2\xc4\xd0\xd2\x12\xb7(mI\
\xa4%V\xd1lQT4[4-\x9a\xfe\xc1\xc2|\xc6\xc2~BY\x83:A3E\xd3\xa0*\xa4\xd2\x90H!\
\x95\x0c\r\r\x1fK\x81g\xb2\x99\x84\xb4\x0fY\xd6\xbb\xc7\xf7>=\'Iz\xc3\xbcv\
\xfbn\xb8\x9c\x15 \xe7\xf3\xc7\x0fw\xc9\xbc7\x99\x03\x0e\xfbn0\x99F+\x85R\
\x80RH\x10\x82\x08\xde\x05\x1ef\x90+\xc0\xe1\xd8\ryn\xd0Z-\\A\xb4\xd2\xf7\
\x9e\xfbwoF\xc8\x088\x1c\xbbae\xb3\xe8y&\x9a\xdf\xf5\xbd\xe7\xfem\x84\xa4\
\x97\xccYf\x16\x8d\xdb\xb2a]\xfeX\x18\xc9s\xc3\xe1\x18\xe7\x94\x12cb\xcc\xb5\
\xfa\xb1l8\xf5\x01\xe7\x84\xc7\xb2Y@\xb2\xcc0\x02\xb4\x9a\x88%\xbe\xdc\xb4\
\x9e\xb6Zs\xaa74\xadg[6\x88<\xb7]\xc6\x14\x1dL\x86\xe6\x83\xa0\x81\xba\xda\
\x10\x02x/\xd4\xd5\x06\r\x840!\x9c\x1fM\x92\xf4\x86\x9f\xbf\xfe\x0c\xd6\x9ae\
\xd6u\x8d \xf4\xf5\x165\x9b\x8f\x04\xe1\xc5\xcb\xdb$\x05\x90\xa97@\x04lQas\
\xcd*7\x14\xdb\x9aY\xcb\xb8\\\xe9E\x10|\xbc\xf2^\xb0E\x85\xc95_\x9f\n\xaa/\
\x05\x10\x81\xce\xc9\xa8\xf6><G\xd8\xed\xbbA)X\xd9\x0c\x01\x9a\xc6Q\x14\xd9h\
[\x04\xda\xd6c\xadFkE\xf0\xc2\xab\xd7\xb7\xc9\x08\x00\xf8\xf6\xbd\x1b\x8cQ\
\xd8|\xb9\x0f\xd3\x9a\x8a\xc7\x08\x00\x9f?\xdd%\xde\x07\xda\x93\xc3{\x19C\
\x8a\x9c\x03\x0b8\x17\xe8\x9d\xbf\x02.>\x13\xc0n\xff{PJ\xc5\xfdP\x11""<\xbc\
\xff\x87\xdf\xf8\xbf\xf5\x17FF\xaf\x8f\x8b\xd3\xe6K\x00\x00\x00\x00IEND\xaeB\
`\x82'

def GetExpandedIconBitmap():
    return wx.BitmapFromImage(GetExpandedIconImage())

def GetExpandedIconImage():
    import cStringIO
    stream = cStringIO.StringIO(GetExpandedIconData())
    return wx.ImageFromStream(stream)

#--------------------------------------

class ShaderPanel(wx.Panel):
    def __init__(self, parent):
        wx.Panel.__init__(self, parent)
        self.imageList = wx.ImageList(16, 16)
        self.imageList.Add(GetCollapsedIconBitmap())
        self.imageList.Add(GetExpandedIconBitmap())
        self.shdList = self.LoadShaderFile()
        self.object = None
        self.shader = None
        self.fpb = None
        self.ctrlList = {}
        self.Bind(wx.EVT_SIZE, self.OnSize, self)

    def LoadShaderFile(self):
        doc = minidom.parse(fileSvr.manglepath('home:data/shaders/shaders.xml')).documentElement
        shdList = []
        for node in doc.childNodes:
            if node.nodeType == Node.ELEMENT_NODE and node.nodeName == 'shader':
                shdList.append(node)
        shdList.sort()
        return shdList

    def SetRenderPanel(self, renderPanel):
        self.renderPanel = renderPanel;

    def SetObject(self, obj):
        if obj != self.object:
            self.object = obj
            shd = self.GetObjectShader(obj)
            if shd != self.shader:
                self.shader = shd
                self.BuildFoldPanel(obj)
            else:
                self.UpdateFoldPanel(obj)

    def Reset(self):
        if self.object != None:
            self.object = None
            self.shader = None
            self.DestroyFoldPanel()

    def GetObjectShader(self, obj):
        shdName = obj.getshader()
        for shd in self.shdList:
            if shd.attributes['file'].value == shdName:
                return shd
        return None

    def BuildFoldPanel(self, obj):
        self.DestroyFoldPanel()
        self.fpb = fpbar.FoldPanelBar(self, size = self.GetClientSize())
        for node in self.shader.childNodes:
            if node.nodeType == Node.ELEMENT_NODE and node.nodeName == 'param':
                name = node.attributes['name'].value
                label = node.attributes['label'].value
                type = node.attributes['type'].value
                fp = self.fpb.AddFoldPanel(label, collapsed = True, foldIcons = self.imageList)
                if type == 'Int':
                    self.ctrlList[name] = IntParamCtrl(self.fpb, fp, node, obj)
                elif type == 'Float':
                    self.ctrlList[name] = FloatParamCtrl(self.fpb, fp, node, obj)
                elif type == 'Bool':
                    self.ctrlList[name] = BoolParamCtrl(self.fpb, fp, node, obj)
                elif type == 'Enum':
                    self.ctrlList[name] = EnumParamCtrl(self.fpb, fp, node, obj)
                elif type == 'Vector':
                    self.ctrlList[name] = VectorParamCtrl(self.fpb, fp, node, obj)
                elif type == 'Color':
                    self.ctrlList[name] = ColorParamCtrl(self.fpb, fp, node, obj)
                elif type == 'EnvelopeCurve':
                    self.ctrlList[name] = CurveParamCtrl(self.fpb, fp, node, obj, self.renderPanel)
                elif type == 'ColorEnvelopeCurve':
                    self.ctrlList[name] = ColorCurveParamCtrl(self.fpb, fp, node, obj, self.renderPanel)
                elif type == 'Texture' or type == 'BumpTexture' or type == 'CubeTexture':
                    self.ctrlList[name] = TextureParamCtrl(self.fpb, fp, node, obj, self.renderPanel)

    def UpdateFoldPanel(self, obj):
        for node in self.shader.childNodes:
            if node.nodeType == Node.ELEMENT_NODE and node.nodeName == 'param':
                name = node.attributes['name'].value
                self.ctrlList[name].LoadParam(obj)

    def DestroyFoldPanel(self):
        self.DestroyChildren()
        self.fpb = None
        self.ctrlList.clear()

    def OnSize(self, event):
        if self.fpb != None:
            self.fpb.SetSize(self.GetClientSize())

#--------------------------------------

class IntParamCtrl:
    def __init__(self, fpb, fp, param, obj):
        self.param = param
        min = param.attributes['min'].value
        max = param.attributes['max'].value
        val = param.attributes['def'].value
        self.panel = wx.Panel(fp)
        self.intNum = mask.NumCtrl(self.panel, value = val, integerWidth = 4)
        self.intNum.SetBounds(int(min), int(max))
        self.minText = wx.StaticText(self.panel, label = min)
        self.valSldr = wx.Slider(self.panel, value = int(val), minValue = int(min), maxValue = int(max))
        self.maxText = wx.StaticText(self.panel, label = max)
        self.LoadParam(obj)

        sizer = wx.BoxSizer()
        sizer.Add(self.intNum, 0, wx.EXPAND | wx.LEFT, 2)
        sizer.Add(self.minText, 0, wx.EXPAND | wx.LEFT, 2)
        sizer.Add(self.valSldr, 1, wx.EXPAND)
        sizer.Add(self.maxText, 0, wx.EXPAND | wx.RIGHT, 2)
        self.panel.SetSizer(sizer)
        fpb.AddFoldPanelWindow(fp, self.panel, fpbar.FPB_ALIGN_WIDTH)

        self.panel.Bind(wx.EVT_TEXT, self.OnText, self.intNum)
        self.panel.Bind(wx.EVT_SCROLL, self.OnScroll, self.valSldr)

    def LoadParam(self, obj):
        self.object = obj
        name = self.param.attributes['name'].value
        if name == 'RenderPri':
            self.intNum.SetValue(obj.getrenderpri())
        else:
            self.intNum.SetValue(obj.getint(name.encode('ascii')))

    def ApplyParam(self):
        name = self.param.attributes['name'].value
        if name == 'RenderPri':
            self.object.setrenderpri(self.intNum.GetValue())
        else:
            self.object.setint(name.encode('ascii'), self.intNum.GetValue())

    def OnText(self, event):
        self.valSldr.SetValue(self.intNum.GetValue())
        self.ApplyParam()

    def OnScroll(self, event):
        self.intNum.SetValue(self.valSldr.GetValue())
        self.ApplyParam()

#--------------------------------------

class FloatParamCtrl:
    def __init__(self, fpb, fp, param, obj):
        self.param = param
        min = param.attributes['min'].value
        max = param.attributes['max'].value
        val = param.attributes['def'].value
        self.panel = wx.Panel(fp)
        self.floatNum = mask.NumCtrl(self.panel, value = val, integerWidth = 4, fractionWidth = 2)
        self.floatNum.SetBounds(float(min), float(max))
        self.minText = wx.StaticText(self.panel, label = min)
        self.valSldr = wx.Slider(self.panel, value = float(val)*100.0, minValue = float(min)*100.0, maxValue = float(max)*100.0)
        self.maxText = wx.StaticText(self.panel, label = max)
        self.LoadParam(obj)

        sizer = wx.BoxSizer()
        sizer.Add(self.floatNum, 0, wx.EXPAND | wx.LEFT, 2)
        sizer.Add(self.minText, 0, wx.EXPAND | wx.LEFT, 2)
        sizer.Add(self.valSldr, 1, wx.EXPAND)
        sizer.Add(self.maxText, 0, wx.EXPAND | wx.RIGHT, 2)
        self.panel.SetSizer(sizer)
        fpb.AddFoldPanelWindow(fp, self.panel, fpbar.FPB_ALIGN_WIDTH)

        self.panel.Bind(wx.EVT_TEXT, self.OnText, self.floatNum)
        self.panel.Bind(wx.EVT_SCROLL, self.OnScroll, self.valSldr)

    def LoadParam(self, obj):
        self.object = obj
        name = self.param.attributes['name'].value
        if name == 'ActivityDistance':
            self.floatNum.SetValue(obj.getactivitydistance())
        elif name == 'EmissionDuration':
            self.floatNum.SetValue(obj.getemissionduration())
        elif name == 'ParticleBirthDelay':
            self.floatNum.SetValue(obj.getbirthdelay())
        elif name == 'ParticleSpreadAngle':
            self.floatNum.SetValue(obj.getspreadangle())
        elif name == 'ParticleStartRotation':
            self.floatNum.SetValue(obj.getstartrotation())
        else:
            self.floatNum.SetValue(obj.getfloat(name.encode('ascii')))

    def ApplyParam(self):
        name = self.param.attributes['name'].value
        if name == 'ActivityDistance':
            self.object.setactivitydistance(self.floatNum.GetValue())
        elif name == 'EmissionDuration':
            self.object.setemissionduration(self.floatNum.GetValue())
        elif name == 'ParticleBirthDelay':
            self.object.setbirthdelay(self.floatNum.GetValue())
        elif name == 'ParticleSpreadAngle':
            self.object.setspreadangle(self.floatNum.GetValue())
        elif name == 'ParticleStartRotation':
            self.object.setstartrotation(self.floatNum.GetValue())
        else:
            self.object.setfloat(name.encode('ascii'), self.floatNum.GetValue())

    def OnText(self, event):
        self.valSldr.SetValue(self.floatNum.GetValue()*100.0)
        self.ApplyParam()

    def OnScroll(self, event):
        self.floatNum.SetValue(self.valSldr.GetValue()/100.0)
        self.ApplyParam()

#--------------------------------------

class BoolParamCtrl:
    def __init__(self, fpb, fp, param, obj):
        self.param = param
        val = param.attributes['def'].value
        self.panel = wx.Panel(fp)
        self.trueRbtn = wx.RadioButton(self.panel, label = 'True', style = wx.RB_GROUP)
        self.falseRbtn = wx.RadioButton(self.panel, label = 'False')
        if val == '0':
            self.falseRbtn.SetValue(True)
        elif val == '1':
            self.trueRbtn.SetValue(True)
        self.LoadParam(obj)

        sizer = wx.BoxSizer()
        sizer.Add((0, 0), 1, wx.EXPAND)
        sizer.Add(self.trueRbtn, 0, wx.EXPAND | wx.LEFT, 2)
        sizer.Add((0, 0), 1, wx.EXPAND)
        sizer.Add(self.falseRbtn, 0, wx.EXPAND | wx.RIGHT, 2)
        sizer.Add((0, 0), 1, wx.EXPAND)
        self.panel.SetSizer(sizer)
        fpb.AddFoldPanelWindow(fp, self.panel, fpbar.FPB_ALIGN_WIDTH)

        self.panel.Bind(wx.EVT_RADIOBUTTON, self.OnRadioButton, self.trueRbtn)
        self.panel.Bind(wx.EVT_RADIOBUTTON, self.OnRadioButton, self.falseRbtn)

    def LoadParam(self, obj):
        self.object = obj
        name = self.param.attributes['name'].value
        if name == 'LockViewer':
            self.trueRbtn.SetValue(obj.getlockviewer())
        elif name == 'Loop':
            self.trueRbtn.SetValue(obj.getloop())
        else:
            self.trueRbtn.SetValue(obj.getbool(name.encode('ascii')))

    def ApplyParam(self):
        name = self.param.attributes['name'].value
        if name == 'LookViewer':
            self.object.setlockviewer(self.trueRbtn.GetValue())
        elif name == 'Loop':
            self.object.setloop(self.trueRbtn.GetValue())
        else:
            self.object.setbool(name.encode('ascii'), self.trueRbtn.GetValue())

    def OnRadioButton(self, event):
        self.ApplyParam()

#--------------------------------------

class EnumParamCtrl:
    def __init__(self, fpb, fp, param, obj):
        self.param = param
        list = param.attributes['enum'].value.replace('=1', '').split(':')
        val = param.attributes['def'].value
        self.enumChoice = wx.Choice(fp, choices = list)
        self.enumChoice.SetSelection(int(val))
        self.LoadParam(obj)

        fpb.AddFoldPanelWindow(fp, self.enumChoice, fpbar.FPB_ALIGN_WIDTH, leftSpacing = 4, rightSpacing = 4)
        self.enumChoice.Bind(wx.EVT_CHOICE, self.OnChoice, self.enumChoice)

    def LoadParam(self, obj):
        self.object = obj
        name = self.param.attributes['name'].value
        self.enumChoice.SetSelection(obj.getint(name.encode('ascii')))

    def ApplyParam(self):
        name = self.param.attributes['name'].value
        self.object.setint(name.encode('ascii'), self.enumChoice.GetSelection())

    def OnChoice(self, event):
        self.ApplyParam()

#--------------------------------------

class VectorParamCtrl:
    def __init__(self, fpb, fp, param, obj):
        self.param = param
        vec = param.attributes['def'].value.split()
        self.panel1 = wx.Panel(fp)
        self.xText = wx.StaticText(self.panel1, label = 'X')
        self.xNum = mask.NumCtrl(self.panel1, value = vec[0], integerWidth = 4, fractionWidth = 2)
        self.yText = wx.StaticText(self.panel1, label = 'Y')
        self.yNum = mask.NumCtrl(self.panel1, value = vec[1], integerWidth = 4, fractionWidth = 2)
        self.panel2 = wx.Panel(fp)
        self.zText = wx.StaticText(self.panel2, label = 'Z')
        self.zNum = mask.NumCtrl(self.panel2, value = vec[2], integerWidth = 4, fractionWidth = 2)
        self.wText = wx.StaticText(self.panel2, label = 'W')
        self.wNum = mask.NumCtrl(self.panel2, value = vec[3], integerWidth = 4, fractionWidth = 2)
        self.LoadParam(obj)

        sizer1 = wx.BoxSizer()
        sizer1.Add(self.xText, 0, wx.EXPAND | wx.LEFT, 2)
        sizer1.Add(self.xNum, 0, wx.EXPAND | wx.LEFT | wx.RIGHT, 2)
        sizer1.Add(self.yText, 0, wx.EXPAND | wx.LEFT | wx.RIGHT, 2)
        sizer1.Add(self.yNum, 0, wx.EXPAND | wx.RIGHT, 2)
        self.panel1.SetSizer(sizer1)
        fpb.AddFoldPanelWindow(fp, self.panel1, fpbar.FPB_ALIGN_WIDTH)

        sizer2 = wx.BoxSizer()
        sizer2.Add(self.zText, 0, wx.EXPAND | wx.LEFT, 2)
        sizer2.Add(self.zNum, 0, wx.EXPAND | wx.LEFT | wx.RIGHT, 2)
        sizer2.Add(self.wText, 0, wx.EXPAND | wx.LEFT | wx.RIGHT, 2)
        sizer2.Add(self.wNum, 0, wx.EXPAND | wx.RIGHT, 2)
        self.panel2.SetSizer(sizer2)
        fpb.AddFoldPanelWindow(fp, self.panel2, fpbar.FPB_ALIGN_WIDTH)

        self.panel1.Bind(wx.EVT_TEXT, self.OnText, self.xNum)
        self.panel1.Bind(wx.EVT_TEXT, self.OnText, self.yNum)
        self.panel2.Bind(wx.EVT_TEXT, self.OnText, self.zNum)
        self.panel2.Bind(wx.EVT_TEXT, self.OnText, self.wNum)

    def LoadParam(self, obj):
        self.object = obj
        name = self.param.attributes['name'].value
        vec = obj.getvector(name.encode('ascii'))
        self.xNum.SetValue(vec[0])
        self.yNum.SetValue(vec[1])
        self.zNum.SetValue(vec[2])
        self.wNum.SetValue(vec[3])

    def ApplyParam(self):
        name = self.param.attributes['name'].value
        self.object.setvector(name.encode('ascii'), self.xNum.GetValue(), self.yNum.GetValue(), self.zNum.GetValue(), self.wNum.GetValue())

    def OnText(self, event):
        self.ApplyParam()

#--------------------------------------

class ColorParamCtrl:
    def __init__(self, fpb, fp, param, obj):
        self.param = param
        rgba = param.attributes['def'].value.split()
        self.panel1 = wx.Panel(fp)
        self.rText = wx.StaticText(self.panel1, label = 'R')
        self.rNum = mask.NumCtrl(self.panel1, value = rgba[0], integerWidth = 2, fractionWidth = 1)
        self.rNum.SetBounds(0.0, 1.0)
        self.gText = wx.StaticText(self.panel1, label = 'G')
        self.gNum = mask.NumCtrl(self.panel1, value = rgba[1], integerWidth = 2, fractionWidth = 1)
        self.gNum.SetBounds(0.0, 1.0)
        self.bText = wx.StaticText(self.panel1, label = 'B')
        self.bNum = mask.NumCtrl(self.panel1, value = rgba[2], integerWidth = 2, fractionWidth = 1)
        self.bNum.SetBounds(0.0, 1.0)
        color = wx.Colour(float(rgba[0])*255.0, float(rgba[1])*255.0, float(rgba[2])*255.0)
        self.rgbSel = csel.ColourSelect(self.panel1, -1, colour = color, size = (29, 19))
        self.panel2 = wx.Panel(fp)
        self.aText = wx.StaticText(self.panel2, label = 'A')
        self.aNum = mask.NumCtrl(self.panel2, value = rgba[3], integerWidth = 2, fractionWidth = 1)
        self.aNum.SetBounds(0.0, 1.0)
        self.aSldr = wx.Slider(self.panel2, value = int(float(rgba[3])*100.0), minValue = 0, maxValue = 100)
        self.LoadParam(obj)

        sizer1 = wx.BoxSizer()
        sizer1.Add(self.rText, 0, wx.EXPAND | wx.LEFT, 2)
        sizer1.Add(self.rNum, 0, wx.EXPAND | wx.LEFT | wx.RIGHT, 2)
        sizer1.Add(self.gText, 0, wx.EXPAND | wx.LEFT | wx.RIGHT, 2)
        sizer1.Add(self.gNum, 0, wx.EXPAND | wx.LEFT | wx.RIGHT, 2)
        sizer1.Add(self.bText, 0, wx.EXPAND | wx.LEFT | wx.RIGHT, 2)
        sizer1.Add(self.bNum, 0, wx.EXPAND | wx.LEFT | wx.RIGHT, 2)
        sizer1.Add((0, 0), 1, wx.EXPAND)
        sizer1.Add(self.rgbSel, 0, wx.EXPAND | wx.RIGHT, 2)
        self.panel1.SetSizer(sizer1)
        fpb.AddFoldPanelWindow(fp, self.panel1, fpbar.FPB_ALIGN_WIDTH)

        sizer2 = wx.BoxSizer()
        sizer2.Add(self.aText, 0, wx.EXPAND | wx.LEFT, 2)
        sizer2.Add(self.aNum, 0, wx.EXPAND | wx.LEFT | wx.RIGHT, 2)
        sizer2.Add(self.aSldr, 1, wx.EXPAND)
        self.panel2.SetSizer(sizer2)
        fpb.AddFoldPanelWindow(fp, self.panel2, fpbar.FPB_ALIGN_WIDTH)

        self.panel1.Bind(wx.EVT_TEXT, self.OnText, self.rNum)
        self.panel1.Bind(wx.EVT_TEXT, self.OnText, self.gNum)
        self.panel2.Bind(wx.EVT_TEXT, self.OnText, self.bNum)
        self.panel1.Bind(csel.EVT_COLOURSELECT, self.OnSelColor, self.rgbSel)
        self.panel2.Bind(wx.EVT_TEXT, self.OnText, self.aNum)
        self.panel2.Bind(wx.EVT_SCROLL, self.OnScroll, self.aSldr)

    def LoadParam(self, obj):
        self.object = obj
        name = self.param.attributes['name'].value
        color = obj.getvector(name.encode('ascii'))
        self.rNum.SetValue(color[0])
        self.gNum.SetValue(color[1])
        self.bNum.SetValue(color[2])
        self.aNum.SetValue(color[3])

    def ApplyParam(self):
        name = self.param.attributes['name'].value
        self.object.setvector(name.encode('ascii'), self.rNum.GetValue(), self.gNum.GetValue(), self.bNum.GetValue(), self.aNum.GetValue())

    def OnText(self, event):
        self.ApplyParam()

    def OnSelColor(self, event):
        color = event.GetValue()
        self.rNum.SetValue(color.Red()/255.0)
        self.gNum.SetValue(color.Green()/255.0)
        self.bNum.SetValue(color.Blue()/255.0)
        self.ApplyParam()

    def OnScroll(self, event):
        self.aNum.SetValue(self.aSldr.GetValue()/100.0)
        self.ApplyParam()

#--------------------------------------

class CurveParamCtrl:
    def __init__(self, fpb, fp, param, obj, renderPanel):
        self.param = param
        self.renderPanel = renderPanel
        keys = ['KeyFrameVal0', 'KeyFrameVal1', 'KeyFrameVal2', 'KeyFrameVal3', 'KeyFramePos1', 'KeyFramePos2', 'Frequency', 'Amplitude']
        min = self.param.attributes['min'].value
        max = self.param.attributes['max'].value
        self.curveData = param.attributes['def'].value.split()
        self.panel = wx.Panel(fp)
        self.keyChoice = wx.Choice(self.panel, choices = keys)
        self.keyChoice.SetSelection(0)
        self.floatNum = mask.NumCtrl(self.panel, value = self.curveData[0], integerWidth = 4, fractionWidth = 2)
        self.floatNum.SetBounds(float(min), float(max))
        self.tuneBtn = wx.Button(self.panel, label = 'Tune', size = (29, 19))
        self.LoadParam(obj)

        sizer = wx.BoxSizer()
        sizer.Add(self.keyChoice, 0, wx.EXPAND | wx.LEFT, 2)
        sizer.Add(self.floatNum, 1, wx.EXPAND | wx.LEFT | wx.RIGHT, 2)
        sizer.Add(self.tuneBtn, 0, wx.EXPAND | wx.RIGHT, 2)
        self.panel.SetSizer(sizer)
        fpb.AddFoldPanelWindow(fp, self.panel, fpbar.FPB_ALIGN_WIDTH)

        self.panel.Bind(wx.EVT_CHOICE, self.OnChoice, self.keyChoice)
        self.panel.Bind(wx.EVT_TEXT, self.OnText, self.floatNum)
        self.panel.Bind(wx.EVT_BUTTON, self.OnButton, self.tuneBtn)

    def LoadParam(self, obj):
        self.object = obj
        name = self.param.attributes['name'].value
        if name == 'EmissionFrequency':
            self.curveData = list(obj.getemissionfrequency())
            del self.curveData[8]
        elif name == 'ParticleLifeTime':
            self.curveData = list(obj.getparticlelifetime())
            del self.curveData[8]
        elif name == 'ParticleStartVelocity':
            self.curveData = list(obj.getparticlestartvelocity())
            del self.curveData[8]
        elif name == 'ParticleRotationVelocity':
            self.curveData = list(obj.getparticlerotationvelocity())
            del self.curveData[8]
        elif name == 'ParticleScale':
            self.curveData = list(obj.getparticlescale())
            del self.curveData[8]
        elif name == 'ParticleWeight':
            self.curveData = list(obj.getparticleweight())
            del self.curveData[8]
        elif name == 'ParticleAlpha':
            self.curveData = list(obj.getparticlealpha())
            del self.curveData[8]
        elif name == 'ParticleSideVelocity1':
            self.curveData = list(obj.getparticlesidevelocity1())
            del self.curveData[8]
        elif name == 'ParticleSideVelocity2':
            self.curveData = list(obj.getparticlesidevelocity2())
            del self.curveData[8]
        elif name == 'ParticleAirResistance':
            self.curveData = list(obj.getparticleairresistance())
            del self.curveData[8]
        elif name == 'ParticleVelocityFactor':
            self.curveData = list(obj.getparticlevelocityfactor())
            del self.curveData[8]
        sel = self.keyChoice.GetSelection()
        self.floatNum.SetValue(self.curveData[sel])

    def ApplyParam(self):
        name = self.param.attributes['name'].value
        if name == 'EmissionFrequency':
            self.object.setemissionfrequency(self.curveData[0], self.curveData[1], self.curveData[2], self.curveData[3],
                                             self.curveData[4], self.curveData[5], self.curveData[6], self.curveData[7], 0)
        elif name == 'ParticleLifeTime':
            self.object.setparticlelifetime(self.curveData[0], self.curveData[1], self.curveData[2], self.curveData[3],
                                            self.curveData[4], self.curveData[5], self.curveData[6], self.curveData[7], 0)
        elif name == 'ParticleStartVelocity':
            self.object.setparticlestartvelocity(self.curveData[0], self.curveData[1], self.curveData[2], self.curveData[3],
                                                 self.curveData[4], self.curveData[5], self.curveData[6], self.curveData[7], 0)
        elif name == 'ParticleRotationVelocity':
            self.object.setparticlerotationvelocity(self.curveData[0], self.curveData[1], self.curveData[2], self.curveData[3],
                                                    self.curveData[4], self.curveData[5], self.curveData[6], self.curveData[7], 0)
        elif name == 'ParticleScale':
            self.object.setparticlescale(self.curveData[0], self.curveData[1], self.curveData[2], self.curveData[3],
                                         self.curveData[4], self.curveData[5], self.curveData[6], self.curveData[7], 0)
        elif name == 'ParticleWeight':
            self.object.setparticleweight(self.curveData[0], self.curveData[1], self.curveData[2], self.curveData[3], self.curveData[4], self.curveData[5], self.curveData[6], self.curveData[7], 0)
        elif name == 'ParticleAlpha':
            self.object.setparticlealpha(self.curveData[0], self.curveData[1], self.curveData[2], self.curveData[3],
                                         self.curveData[4], self.curveData[5], self.curveData[6], self.curveData[7], 0)
        elif name == 'ParticleSideVelocity1':
            self.object.setparticlesidevelocity1(self.curveData[0], self.curveData[1], self.curveData[2], self.curveData[3],
                                                 self.curveData[4], self.curveData[5], self.curveData[6], self.curveData[7], 0)
        elif name == 'ParticleSideVelocity2':
            self.object.setparticlesidevelocity2(self.curveData[0], self.curveData[1], self.curveData[2], self.curveData[3],
                                                 self.curveData[4], self.curveData[5], self.curveData[6], self.curveData[7], 0)
        elif name == 'ParticleAirResistance':
            self.object.setparticleairresistance(self.curveData[0], self.curveData[1], self.curveData[2], self.curveData[3],
                                                 self.curveData[4], self.curveData[5], self.curveData[6], self.curveData[7], 0)
        elif name == 'ParticleVelocityFactor':
            self.object.setparticlevelocityfactor(self.curveData[0], self.curveData[1], self.curveData[2], self.curveData[3],
                                                  self.curveData[4], self.curveData[5], self.curveData[6], self.curveData[7], 0)

    def OnChoice(self, event):
        sel = self.keyChoice.GetSelection()
        self.floatNum.SetValue(self.curveData[sel])

    def OnText(self, event):
        sel =  self.keyChoice.GetSelection()
        self.curveData[sel] = self.floatNum.GetValue()
        self.ApplyParam()

    def OnButton(self, event):
        min = float(self.param.attributes['min'].value)
        max = float(self.param.attributes['max'].value)
        self.renderPanel.ShowInfoPopup(False)
        dlg = CurveDialog(self.panel, min, max, self.curveData)
        dlg.CenterOnParent()
        if dlg.ShowModal() == wx.ID_OK:
            self.curveData = dlg.GetCurveData()
            self.ApplyParam()
        dlg.Destroy()
        self.renderPanel.ShowInfoPopup(True)
        sel = self.keyChoice.GetSelection()
        self.floatNum.SetValue(self.curveData[sel])

#--------------------------------------

class CurveDialog(wx.Dialog):
    def __init__(self, parent, min, max, curveData):
        pre = wx.PreDialog()
        pre.Create(parent, title = 'Edit Envelop Curve', size = (600, 480))
        self.PostCreate(pre)
        keys = ['KeyFrame\nValue0', 'KeyFrame\nValue1', 'KeyFrame\nValue2', 'KeyFrame\nValue3',
                'KeyFrame\nPosition1', 'KeyFrame\nPosition2', '\nFrequency', '\nAmplitude']
        self.len = max - min
        self.sliders = []
        self.floatNums = []
        sizer1 = wx.BoxSizer()
        for i in range(8):
            slider = wx.Slider(self, value = curveData[i]/self.len*100.0, minValue = 0, maxValue = 100, size = (-1, 350), style = wx.SL_VERTICAL | wx.SL_INVERSE)
            keyText = wx.StaticText(self, label = keys[i], style = wx.ALIGN_CENTRE)
            floatNum = mask.NumCtrl(self, value = curveData[i], integerWidth = 4, fractionWidth = 2)
            self.sliders.append(slider)
            self.floatNums.append(floatNum)
            sizer = wx.BoxSizer(wx.VERTICAL)
            sizer.Add(slider, 1, wx.ALIGN_CENTRE)
            sizer.Add(keyText, 0, wx.ALIGN_CENTRE | wx.BOTTOM, 2)
            sizer.Add(floatNum, 0, wx.ALIGN_CENTRE | wx.BOTTOM, 2)
            sizer1.Add(sizer, wx.EXPAND | wx.ALL, 10)
            self.Bind(wx.EVT_SCROLL, self.OnScroll, slider)
            self.Bind(wx.EVT_TEXT, self.OnText, keyText)

        okBtn = wx.Button(self, wx.ID_OK)
        cancelBtn = wx.Button(self, wx.ID_CANCEL)
        okBtn.SetDefault()

        sizer2 = wx.BoxSizer()
        sizer2.Add((0, 0), 1)
        sizer2.Add(okBtn, 0, wx.ALIGN_CENTRE)
        sizer2.Add((0, 0), 1)
        sizer2.Add(cancelBtn, 0, wx.ALIGN_CENTRE)
        sizer2.Add((0, 0), 1)
        sizer3 = wx.BoxSizer(wx.VERTICAL)
        sizer3.Add(sizer1, 0, wx.EXPAND | wx.ALL, 10)
        sizer3.Add(sizer2, 0, wx.EXPAND)
        self.SetSizer(sizer3)

    def GetCurveData(self):
        curveData = []
        for i in range(8):
            curveData.append(self.floatNums[i].GetValue())
        return curveData

    def OnScroll(self, event):
        slider = event.GetEventObject()
        for i in range(8):
            if self.sliders[i] == slider:
                self.floatNums[i].SetValue(slider.GetValue()*self.len/100.0)
                break

    def OnText(self, event):
        floatNum = event.GetEventObject()
        for i in range(8):
            if self.floatNums[i] == floatNum:
                self.floatNums[i].SetValue(slider.GetValue()/self.len*100.0)
                break

#--------------------------------------

class ColorCurveParamCtrl:
    def __init__(self, fpb, fp, param, obj, renderPanel):
        self.param = param
        self.renderPanel = renderPanel
        keys = ['keyFrameVal0.x', 'keyFrameVal0.y', 'keyFrameVal0.z', 'keyFrameVal1.x', 'keyFrameVal1.y', 'keyFrameVal1.z',
                'keyFrameVal2.x', 'keyFrameVal2.y', 'keyFrameVal2.z', 'keyFrameVal3.x', 'keyFrameVal3.y', 'keyFrameVal3.z',
                'KeyFramePos1', 'KeyFramePos2']
        self.curveData = []
        self.panel = wx.Panel(fp)
        self.keyChoice = wx.Choice(self.panel, choices = keys)
        self.keyChoice.SetSelection(0)
        self.floatNum = mask.NumCtrl(self.panel, value = 0.0, integerWidth = 2, fractionWidth = 2)
        self.floatNum.SetBounds(0.0, 1.0)
        self.tuneBtn = wx.Button(self.panel, label = 'Tune', size = (29, 19))
        self.LoadParam(obj)

        sizer = wx.BoxSizer()
        sizer.Add(self.keyChoice, 0, wx.EXPAND | wx.LEFT, 2)
        sizer.Add(self.floatNum, 1, wx.EXPAND | wx.LEFT | wx.RIGHT, 2)
        sizer.Add(self.tuneBtn, 0, wx.EXPAND | wx.RIGHT, 2)
        self.panel.SetSizer(sizer)
        fpb.AddFoldPanelWindow(fp, self.panel, fpbar.FPB_ALIGN_WIDTH)

        self.panel.Bind(wx.EVT_CHOICE, self.OnChoice, self.keyChoice)
        self.panel.Bind(wx.EVT_TEXT, self.OnText, self.floatNum)
        self.panel.Bind(wx.EVT_BUTTON, self.OnButton, self.tuneBtn)

    def LoadParam(self, obj):
        self.object = obj
        name = self.param.attributes['name'].value
        if name == 'ParticleRGB':
            self.curveData = list(obj.getparticlergb())
        sel = self.keyChoice.GetSelection()
        self.floatNum.SetValue(self.curveData[sel])

    def ApplyParam(self):
        name = self.param.attributes['name'].value
        if name == 'ParticleRGB':
            self.object.setparticlergb(self.curveData[0], self.curveData[1], self.curveData[2], self.curveData[3],
                                       self.curveData[4], self.curveData[5], self.curveData[6], self.curveData[7],
                                       self.curveData[8], self.curveData[9], self.curveData[10], self.curveData[11],
                                       self.curveData[12], self.curveData[13])

    def OnChoice(self, event):
        sel = self.keyChoice.GetSelection()
        self.floatNum.SetValue(self.curveData[sel])

    def OnText(self, event):
        sel =  self.keyChoice.GetSelection()
        self.curveData[sel] = self.floatNum.GetValue()
        self.ApplyParam()

    def OnButton(self, event):
        self.renderPanel.ShowInfoPopup(False)
        dlg = ColorCurveDialog(self.panel, self.curveData)
        dlg.CenterOnParent()
        if dlg.ShowModal() == wx.ID_OK:
            self.curveData = dlg.GetCurveData()
            self.ApplyParam()
        dlg.Destroy()
        self.renderPanel.ShowInfoPopup(True)
        sel = self.keyChoice.GetSelection()
        self.floatNum.SetValue(self.curveData[sel])

#--------------------------------------

class ColorCurveDialog(wx.Dialog):
    def __init__(self, parent, curveData):
        pre = wx.PreDialog()
        pre.Create(parent, title = 'Edit Envelop Curve', size = (800, 480))
        self.PostCreate(pre)
        keys = ['keyFrame\nValue0.x', 'keyFrame\nValue0.y', 'keyFrame\nValue0.z', 'keyFrame\nValue1.x', 'keyFrame\nValue1.y', 'keyFrame\nValue1.z',
                'keyFrame\nValue2.x', 'keyFrame\nValue2.y', 'keyFrame\nValue2.z', 'keyFrame\nValue3.x', 'keyFrame\nValue3.y', 'keyFrame\nValue3.z',
                'KeyFrame\nPosition1', 'KeyFrame\nPosition2']
        self.sliders = []
        self.floatNums = []
        sizer1 = wx.BoxSizer()
        for i in range(14):
            slider = wx.Slider(self, value = curveData[i]*100.0, minValue = 0, maxValue = 100, size = (-1, 350), style = wx.SL_VERTICAL | wx.SL_INVERSE)
            keyText = wx.StaticText(self, label = keys[i], style = wx.ALIGN_CENTRE)
            floatNum = mask.NumCtrl(self, value = curveData[i], integerWidth = 2, fractionWidth = 2)
            self.sliders.append(slider)
            self.floatNums.append(floatNum)
            sizer = wx.BoxSizer(wx.VERTICAL)
            sizer.Add(slider, 1, wx.ALIGN_CENTRE)
            sizer.Add(keyText, 0, wx.ALIGN_CENTRE | wx.BOTTOM, 2)
            sizer.Add(floatNum, 0, wx.ALIGN_CENTRE | wx.BOTTOM, 2)
            sizer1.Add(sizer, wx.EXPAND | wx.ALL, 10)
            self.Bind(wx.EVT_SCROLL, self.OnScroll, slider)
            self.Bind(wx.EVT_TEXT, self.OnText, keyText)

        okBtn = wx.Button(self, wx.ID_OK)
        cancelBtn = wx.Button(self, wx.ID_CANCEL)
        okBtn.SetDefault()

        sizer2 = wx.BoxSizer()
        sizer2.Add((0, 0), 1)
        sizer2.Add(okBtn, 0, wx.ALIGN_CENTRE)
        sizer2.Add((0, 0), 1)
        sizer2.Add(cancelBtn, 0, wx.ALIGN_CENTRE)
        sizer2.Add((0, 0), 1)
        sizer3 = wx.BoxSizer(wx.VERTICAL)
        sizer3.Add(sizer1, 0, wx.EXPAND | wx.ALL, 10)
        sizer3.Add(sizer2, 0, wx.EXPAND)
        self.SetSizer(sizer3)

    def GetCurveData(self):
        curveData = []
        for i in range(14):
            curveData.append(self.floatNums[i].GetValue())
        return curveData

    def OnScroll(self, event):
        slider = event.GetEventObject()
        for i in range(14):
            if self.sliders[i] == slider:
                self.floatNums[i].SetValue(slider.GetValue()/100.0)
                break

    def OnText(self, event):
        floatNum = event.GetEventObject()
        for i in range(14):
            if self.floatNums[i] == floatNum:
                self.floatNums[i].SetValue(slider.GetValue()*100.0)
                break

#--------------------------------------

class TextureParamCtrl:
    def __init__(self, fpb, fp, param, obj, renderPanel):
        self.param = param
        self.renderPanel = renderPanel
        self.homedir = os.path.abspath(fileSvr.manglepath('textures:1')).rstrip('1')
        txt = param.attributes['def'].value
        self.panel = wx.Panel(fp)
        self.txtText = wx.TextCtrl(self.panel, value = txt, size = (173, -1))
        self.txtBtn = wx.Button(self.panel, label = 'Pick', size = (29, 19))
        self.LoadParam(obj)

        sizer = wx.BoxSizer()
        sizer.Add(self.txtText, 1, wx.EXPAND | wx.LEFT | wx.RIGHT, 2)
        sizer.Add(self.txtBtn, 0, wx.EXPAND | wx.RIGHT, 2)
        self.panel.SetSizer(sizer)
        fpb.AddFoldPanelWindow(fp, self.panel, fpbar.FPB_ALIGN_WIDTH)

        self.panel.Bind(wx.EVT_BUTTON, self.OnButton, self.txtBtn)

    def LoadParam(self, obj):
        self.object = obj
        name = self.param.attributes['name'].value
        self.txtText.SetValue(obj.gettexture(name.encode('ascii')))

    def ApplyParam(self):
        name = self.param.attributes['name'].value
        self.object.settexture(name.encode('ascii'), self.txtText.GetValue().encode('ascii'))

    def OnButton(self, event):
        path = self.txtText.GetValue().replace('textures:', self.homedir).replace('/', '\\')
        if os.path.isdir(path):
            dir = path
            file = ''
        else:
            splitPath = os.path.split(path)
            if os.path.isdir(splitPath[0]):
                dir = splitPath[0]
                file = splitPath[1]
            else:
                dir = '.'
                file = ''
        self.renderPanel.ShowInfoPopup(False)
        dlg = wx.FileDialog(self.panel, message = "Choose a texture file",
                            defaultDir = dir,
                            defaultFile = file,
                            wildcard = "Texture dds file (*.dds)|*.dds|All files (*.*)|*.*")
        if dlg.ShowModal() == wx.ID_OK:
            vpath = dlg.GetPath().replace(self.homedir, 'textures:').replace('\\', '/')
            self.txtText.SetValue(vpath)
            self.ApplyParam()
        dlg.Destroy()
        self.renderPanel.ShowInfoPopup(True)

#--------------------------------------
# Eof
#--------------------------------------