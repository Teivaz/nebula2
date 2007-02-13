#--------------------------------------------------------------------------
#  mangalore.py
#
#  (c)2007  Kim Hyoun Woo
#--------------------------------------------------------------------------
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
        openHandler += "\t\t)\n"

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
        script += "(\n"

        #script += custAttr
        #script += "(\n"

        # A string for rollout MAXScript 
        params = ""
        rollout = ""

        params += "\t" + paramDef + "\n" + "\t(\n"
        rollout += "\t" + rolloutDef + "\n" +  "\t(\n"

        for col in self.pblockcolumns:
            attrs = col.attributes
	    if attrs.has_key('gui'):
                name = col.attributes['name']
                label = col.attributes['label']
                datatype = col.attributes['type']
                defval = col.attributes['def']

                # write parameters
                params += "\t\t"
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

		items = ""
		if attrs.has_key('itemlist'):
                    items = col.attributes['itemlist'].value

                # write rollout ui controls
                rollout += self.maxScript.GenerateRollout(name.value, datatype.value, 
					label.value, defval.value, items)

        params += "\t)\n"

        rollout += openHandler

        rollout += "\t)\n"

        script += params
        script += rollout
        #script += ")\n"

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
mangalore = Mangalore()
mangalore.Parse('mangalore.xml')
mangalore.WriteEntityScript('entitytest.ms')
mangalore.WriteTableScript('function.ms')
"""

