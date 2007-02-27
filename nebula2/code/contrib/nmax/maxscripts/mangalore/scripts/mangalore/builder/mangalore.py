#--------------------------------------------------------------------------
#  mangalore.py
#
#  (c)2007  Kim Hyoun Woo
#--------------------------------------------------------------------------
import re
import xml.dom.minidom
from xml.dom.minidom import Node
from maxscript import *


#-------------------------------------------------------------------------- 
#  note:
#    see minidom.py for more details about NamedNodeMap class and 
#    its member functions in '$python/Lib/xml/dom' directory.
#-------------------------------------------------------------------------- 
class Mangalore:

    maxScript = MaxScript()
    categoryList = []
    tableArray = {}


    """ Parse the given XML file and generate MAXScript code.
    """
    def Parse(self, file):
        doc = xml.dom.minidom.parse(file)

        for node in doc.getElementsByTagName('mangalore'):
            tableNodes = node.getElementsByTagName('table')

            # for each tables in the elements
            for tbl in tableNodes:
                # table name
                tblAttr = tbl.attributes['name']

                # retrieved categories are used for '_Category' dropdownlist's items.
                if tblAttr.value == "_Categories":
                    rows = tbl.getElementsByTagName('row')
                    """
                    env = False
                    for r in rows:
                        c = r.attributes['name'].value
                        # see if there is '_Environment' category
                        if c == "_Environment":
                            env = True
                        self.categoryList.append(c)
                    # append '_Envirionment' category
                    if env == False:
                        self.categoryList.append('_Environment')
                    """

                # for each columns in the table element
                if tblAttr.value == "_Entities":
                    self.pblockcolumns = tbl.getElementsByTagName('column')
                elif tblAttr.value == "_Categories":
                    self.categorycolumns = tbl.getElementsByTagName('column')
                else:
                    self.etccolumns = tbl.getElementsByTagName('column')

                self.tableArray[tblAttr.value] = tbl.getElementsByTagName('column')

        #print self.tableArray

    """ Write modifier plugin maxsript code on the disk.
    """
    def WriteEntityScript(self, file):

        #custAttr = "mlEntityCA = attributes \"mlEntity\"\n" 
        rolloutname = "rEntityAttr"
        paramDef = "parameters EntityAttr rollout:%s" % rolloutname
        rolloutDef = "rollout %s \"Mangalore Entity\"" % rolloutname

        openHandler = ""
        openHandler += "\t\ton %s open do \n" % rolloutname
        openHandler += "\t\t(\n"
        openHandler += "\t\t\tlocal classid = genclassid returnvalue:true\n"
        openHandler += "\t\t\tc_Id = ((classid[1] as string) + (classid[2] as string))\n"

        # A generated MAXScript code which is saved on the disk.
        script = ""
        script += "------------------------------------------------------------------------------\n"
        script += "--  !!MACHINE GENERATED FILE!!\n"
        script += "--\n"
        script += "--  %s\n" % file
        script += "--\n"
        script += "--  (c)2007 Kim Hyoun Woo\n"
        script += "------------------------------------------------------------------------------\n"
        script += "\n"

        script += "plugin modifier mlEntitiy\n"
        script += "name:\"Mangalore Entity\"\n"
        script += "classID:#(0x418f91ee, 0x3c44f19b)\n"
        script += "category:\"mangalore\"\n"
        script += "extends:EmptyModifier\n"
        #script += "caMangaloreEntity = attributes \"Mangalore Entity\"\n"
        script += "(\n"

        #script += custAttr
        #script += "(\n"
        script += "\tlocal isLight = false\n"

        attachHandler = ""
        attachHandler += "\ton attachedToNode node do\n"
        attachHandler += "\t(\n"
        attachHandler += "\t\tisLight = iskindof node 'light'\n"
        attachHandler += "\t)\n"

        # A string for rollout MAXScript 
        params = ""
        rollout = ""

        attachHandleForOpen = ""

        params += "\t" + paramDef + "\n" + "\t(\n"
        rollout += "\t" + rolloutDef + "\n" +  "\t(\n"

        for col in self.pblockcolumns:
            attrs = col.attributes
            if attrs.has_key('gui'):
                name = col.attributes['name']
                label = col.attributes['label']
                datatype = col.attributes['type']
                defval = col.attributes['def']
                                
                hasGUI = col.attributes['gui']

                if hasGUI.value == "1":
                    # write parameters
                    #params += "\t\t"
                    params += self.maxScript.GenerateParameter(name.value, label.value, 
                        datatype.value, defval.value)

                    rollout += "\t\t"

                    """
                    items = []
                    if name.value == "_Type":
                        items.append("Template") 
                        items.append("Instance") 

                    if name.value == "_Category":
                        for i in self.categoryList:
                            items.append(i)
                    """

                    # tokenize the string by comma.
                    items = ""
                    if attrs.has_key('itemlist'):
                        items = col.attributes['itemlist'].value

                    itemList = re.split(',', items)

                    # write rollout ui controls
                    rollout += self.maxScript.GenerateRollout(name.value, datatype.value, 
                        label.value, defval.value, itemList)

                    tmp = "" 
                    tmp = self.maxScript.GenerateStrTab(name.value, datatype.value, itemList)
                    if tmp != "" :
                        openHandler +="\t\t\t"
                        openHandler += tmp

                    #FIXME: 
                    ret = name.value.find('Light')
                    if ret >= 0 :
                        uiname = self.maxScript.GetUIControlName(datatype.value, label.value)
                        attachHandleForOpen  += ("\t\t\t\t%s.enabled = false\n" % uiname)

        params += "\t)\n"

        openHandler += "\t\t\tif islight == false do\n"
        openHandler += "\t\t\t(\n"
        openHandler += attachHandleForOpen
        openHandler += "\t\t\t)\n"

        openHandler += "\t\t)\n"
        rollout += openHandler

        rollout += "\t)\n"

        script += params
        script += rollout

        script += attachHandler 

        script += ")\n"

        # for debugging
        #print script

        # write generated script on the disk.
        f = open(file, 'w')
        f.write(script)
        f.close()


    """ Write maxscript function which create table and its related columns of database.
    """
    def WriteTableScript (self, file):
        script = ""
        script += "fn nCreateTablesAndColumns database =\n"
        script += "(\n"
        for key, value in self.tableArray.iteritems():

            keycolumn = ""
            columns = ""
            for i in value:
                name = i.attributes['name']
                columns += name.value
                columns += " "
                keycolumn = name.value

            script += ("\tnCreateTable database \"%s\" \"%s\" \"%s\"\n" % (key, columns, keycolumn))

        script += ")\n"

        # for debugging
        #print script

        # write generated script on the disk.
        f = open(file, 'w')
        f.write(script)
        f.close()

    """
    """
    def WriteParserScript (self, file):
        script = ""

        # A generated MAXScript code which is saved on the disk.
        script = ""
        script += "------------------------------------------------------------------------------\n"
        script += "--  !!MACHINE GENERATED FILE!!\n"
        script += "--\n"
        script += "--  %s\n" % file
        script += "--\n"
        script += "--  (c)2007 Kim Hyoun Woo\n"
        script += "------------------------------------------------------------------------------\n"
        script += "\n"

        script += "fn nParseMangaloreEntity obj =\n" 
        script += "(\n"

        script += "\tcolumns = \"\"\n"
        script += "\tvalues =  \"\"\n"
        script += "\tret = #()\n"

        for col in self.pblockcolumns:
            attrs = col.attributes
            if attrs.has_key('gui'):
                name = col.attributes['name']
                datatype = col.attributes['type']
                label = col.attributes['label']
                hasGUI = col.attributes['gui']

                if hasGUI.value == "1":
                    script += "\t"
                    script += ("val = obj.modifiers[\"Mangalore Entity\"].c%s\n" % name.value)

                    uiname = self.maxScript.GetUIControlName(datatype.value, label.value)
                    script += ("\tstatus = obj.modifiers[\"Mangalore Entity\"].rEntityAttr.%s.enabled\n" % uiname)

                    type = datatype.value

                    if type == "Integer":
                        tmp = ""
                        tmp += "\tif val != \"\" or (val != undefined) do\n"
                        tmp += "\t(\n"
                        tmp += "\t\tif status == true do\n"
                        tmp += "\t\t(\n"
                        tmp += "\t\tcolumns += " + ("\"%s\"\n" % name.value)
                        tmp += ("\t\tvalues += obj.modifiers[\"Mangalore Entity\"].strtabc%s[val]\n" % name.value)
                        tmp += "\t\t)\n"
                        tmp += "\t)\n"
                        script += tmp
                    elif type == "Color":
                        script += ""

                    elif type == "String":
                        tmp = ""
                        tmp += "\tif val != \"\" do\n"
                        tmp += "\t(\n"
                        tmp += "\t\tif status == true do\n"
                        tmp += "\t\t(\n"
                        tmp += "\t\tcolumns += " + ("\"%s\"\n" % name.value)
                        tmp += "\t\tvalues += val as string\n"
                        tmp += "\t\t)\n"
                        tmp += "\t)\n"
                        script += tmp
                    elif type == "Boolean":
                        tmp = ""
                        if name.value == "Transform":
                            tmp += "\tcolumns += \"Transform\"\n"
                            tmp += "\ttm = nConvertTM obj.transform\n"
                            tmp += "\tvalues += tm as string\n" 
                        else:
                            tmp += "\tif status == true do\n"
                            tmp += "\t(\n"
                            tmp += "\tcolumns += " + ("\"%s\"\n" % name.value)
                            tmp += "\tvalues += val as string\n" 
                            tmp += "\t)\n"
                        script += tmp
                    elif type == "Float":
                        tmp = ""
                        tmp += "\tif status == true do\n"
                        tmp += "\t(\n"
                        tmp += "\tcolumns += " + ("\"%s\"\n" % name.value)
                        tmp += "\tvalues += val as string\n"
                        tmp += "\t)\n"
                        script += tmp
                    else:
                        tmp = ""
                        tmp += "\tvalues += val as string\n"
                        script += "\t"
                        script += tmp

                    script += "\tcolumns += \" \"\n"
                    script += "\tvalues += \" \"\n"

        script += "\tappend ret columns\n"
        script += "\tappend ret values\n"
        script += "\treturn ret\n"

        script += ")\n"

        f = open(file, 'w')
        f.write(script)
        f.close()

"""
mangalore = Mangalore()
mangalore.Parse('mangalore.xml')
mangalore.WriteEntityScript('entitytest.ms')
mangalore.WriteTableScript('function.ms')
"""

