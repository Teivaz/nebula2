#--------------------------------------------------------------------------
#  maxscript.py
#    
#  (c)2007 Kim Hyoun Woo
#--------------------------------------------------------------------------

#--------------------------------------------------------------------------
#
#--------------------------------------------------------------------------
class MaxScript:

    """ Retrieve default value of parameters
    """
    def GetDefVal(self, type, defval):
        if type == "Integer" :
            return ("default:%s" % defval)
        elif type == "String" :
            return (("default:\"%s\"") % defval)
        elif type == "Float" :
            return (("default:%s") % defval)
        elif type == "Boolean" :
            return (("default:%s") % defval)
        elif type == "Color" :
            return ("default:[%s]" % defval)
        else:
            print "No matching data type for the given default value.\n"
            return ("default:\"\"")

    """ Generate MAXScript for parameter blocks
    """
    def GenerateParameter(self, name, label, type, defval):
        # 'c' is prefix which means the column element
        paramname = "c%s" % name
        default = self.GetDefVal(type, defval)
        uiname = self.GetUIControlName(type, label)

	ret = ""
        ret += "\t\t"
        ret +=  "%s type:#%s %s animatable:true ui:%s\n" % (paramname, type.lower(), default, uiname)
	
        if type == "Integer":
            ret += "\t\t"
            ret += "strtab%s type:#stringTab tabSizeVariable:true\n" % (paramname)

        return ret

    """ Make a new UI control name by the given its type and label.
    """
    def GetUIControlName(self, type, label):
        if type == "Integer" :
            return "dlist%s" % label
        elif type == "String" :
            return "edt%s" % label
        elif type == "Float" :
            return "spn%s" % label
        elif type == "Color" :
            return "clr%s" % label
        elif type == "Boolean":
            return "chk%s" % label
        else:
            return "lbl%s" % label

    """ Generate MAXScript code for UI control by the given type of the column
 
        items in-arg is only needed for integer type.
    """
    def GenerateRollout(self, name, type, label, defval, items):

        uiname = self.GetUIControlName(type, label)

        if type == "Integer" :

            str = ""
            str += "items:#("
            for i in items:
                i.strip (' ')
                str += "\"%s\"" % i
                if len(items) > items.index(i) + 1:
                    str += ", "
            str += ")"

            return ("dropdownlist %s \"%s\" %s align:#left \n" % (uiname, label, str))

        elif type == "String" :
            return ("edittext %s \"%s\" align:#left \n" % (uiname, label))

        elif type == "Float" :
            return ("spinner %s \"%s\" align:#left \n" % (uiname, label))

        elif type == "Color" :
            clr = "color:[%s]" % defval
            return ("colorpicker %s \"%s\" align:#left alpha:true %s \n" % (uiname, label, clr))

        elif type == "Boolean":
            return ("checkbox %s \"%s\" align:#left \n" % (uiname, label))

        else:
            return ("label %s \"%s\" align:#left \n" % (uiname, label)) 

    """
    """
    def GenerateStrTab(self, name, type, items):
        if type == "Integer" :
            strItems = ""
            for i in items:
                strItems += "\"%s\"" % i
                if len(items) > items.index(i) + 1:
                    strItems += ", "

            strTab = "strtab"
            # prefix for the parameter
	    strTab += "c"
            strTab += name
            return ("%s = #(%s)\n" % (strTab, strItems))
        else:
            return ""

