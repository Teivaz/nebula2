=begin
XML file format:

  <class name='nGfxServer2' package='gfx2' parent='nroot'>
    <properties>
      <property name="propertyname">
        <doc>...</doc>
        <getterslist>
          <getter [type="count"] name="gettername" fourCC="GJNT">
            [<doc>...</doc>]
          </getter>
          ...
        </getterlist>
        <setterlist>
          <setter type="begin|add|end" name="settername" fourCC="BJNT">
            [<doc>...</doc>]
          </setter>
          ...
        </setterlist>
        <valueformat>
          <value name="valuename" type="integer|string|..." editor="text_input|slider|select|checkbox" [index="yes"]>
            [<doc>...</doc>]
          </value>
          <value name="valuename" type="integer|string|...">
            [<doc>...</doc>]
          </value>
          <value name="valuename" type="string" restricted="enum">
            <enum name="dx7"/>
            <enum name="dx8"/>
            ...
          </value>
          ...
        </valueformat>
      </property>
      <property>
        ...
      </property>
    </properties>
    <methods>
      <method name="methodname" fourCC="GHTR">
        [<doc>...</doc>]
        <output>
          <value name="valuename" type="integer|string|...">
            [<doc>...</doc>]
          </value>
          <value name="valuename" type="string" restricted="enum">
            <enum name="dx7"/>
            <enum name="dx8"/>
            ...
          </value>
          ...
        </output>
        <input>
          <value name="valuename" type="integer|string|...">
            [<doc>...</doc>]
          </value>
          <value name="valuename" type="string" restricted="enum">
            <enum name="dx7"/>
            <enum name="dx8"/>
            ...
          </value>
          ...
        </input>
      </method>
      <method>
        ...
      </method>
    </methods>
  </class>

Command comments format:

/**
    @cppclass <cpp class name>
    
    @superclass <cpp parent class name>

    @classinfo <documentation>
*/

getter/setter:
/**
    @property <property name>

    @format
      type(name;attr1=val1;attr2=val2..) -- comment
      type(name;attr1=val1;attr2=val2..) -- comment
      ...

    @info <documentation>
*/

/**
    @cmd <command name>

    @cmdtype <getter|[count], setter|[begin,add,end]>

    @property <property name>

    @info <documentation>
*/

other commands (methods):
/**
    @cmd <command name>

    @input
      type(name;attr1=val1;attr2=val2..) -- comment
      type(name;attr1=val1;attr2=val2..) -- comment
      ...

    @output
      type(name;attr1=val1;attr2=val2..) -- comment
      type(name;attr1=val1;attr2=val2..) -- comment
      ...

    @info <documentation>
*/

attr=val:
attr - name of the attribute
val - attribute value: val=val1[,val2...,valN]
For example:
  index=yes - used if some property is indexed (array or you can get it by name)

=end

require 'rexml/document'
include REXML

INDIR = "../code/"
OUTDIR = "../code/"

INDENT = "  "

CLASS_REGEX = /^\s*(template\s*<\s*([^,>]+(?:\s*,[^,>]+)*)\s*>\s+)?class\s+(\w[\w\d]*)(\s*:\s*(public|private|protected\s+)?(.+(\s*<\s*[^>]+\s*>)?))?(\s*\{)?\s*$/
ENUM_REGEX = /^\s*enum(\s+\w[\w\d]*)?(\s*\{)?\s*$/
UNION_REGEX = /^\s*union(\s+\w[\w\d]*)?(\s*\{)?\s*$/
STRUCT_REGEX = /^\s*struct(\s+\w[\w\d]*)?(\s*\{)?\s*$/
CLASSCONSTRUCTOR_REGEX = /^\s*(\w[\w\d]*)\s*\(([^\)]+)?\)\s*;\s*(.*)$/
CLASSDESTRUCTOR_REGEX = /^\s*(virtual\s+)?(~\w[\w\d]*)\s*\(\s*\)\s*;\s*(.*)$/
CLASSMETHOD_REGEX = /^\s*(.+?)(\*|&|\s+)(\w[\w\d]*)\s*\(([^\)]+)?\)\s*(const\s*)?;\s*(.*)$/
CLASSPROPERTY_REGEX = /^\s*(.+?)(\*|&|\s+)(\w[\w\d]*)\s*;\s*(.*)$/

CMDFUNCTIONDECL_REGEX = /^\s*static\s+void\s+n_(\w[\w\d]*)\s*\(\s*void\s*\*.*,\s*nCmd\s*\*.*\)\s*;/
INITCMDFUNCTIONDEF_REGEX = /^.*n_initcmds\s*\(.*\)\s*/
CMDFUNCTIONDEF_REGEX = /^.*n_(\w[\w\d]*)\s*\(\s*void\s*\*.*,\s*nCmd\s*\*.*\)\s*/
ADDCMDFUNCTION_REGEX = /^\s*\w[\w\d]*\s*->\s*AddCmd\s*\(\s*"([a-z]+)_([a-z\d]+)_([a-z]+)"\s*,\s*'(....)'\s*,\s*n_([a-z\d]+)\)\s*;/
CMDTYPES_HASH = {"v" => "void", "i" => "integer", "f" => "float", "s" => "string", "b" => "boolean", "o" => "object"}

def getElement(parentElement, elementName, addNew = true)
  result = nil
  result = parentElement.elements[elementName]
  result = parentElement.add_element(elementName) if !result && addNew
  return result
end

def getElementWithAttribute(parentElement, elementName, attrName, attrValue, addNew = false)
  result = nil
  parentElement.each_element_with_attribute(attrName, attrValue, 1, elementName){|e|
    result = e
  }
  result = parentElement.add_element(elementName, {attrName => attrValue}) if !result && addNew
  return result
end
  
class Processor
  def initialize
    @indent = 0
    @newline = true
  end
  
  def dispatchExtComment(hf, firstStr)
    commentStr = ""
    curStr = firstStr
    while !(curStr =~ /^(.*?)\*\//)
      commentStr += curStr
      curStr = hf.gets
    end
    commentStr += $1
    return commentStr
  end
    
  def setDirs(inD, outD)
    @inDir = inD
    @outDir = outD
  end
  
  def showProcessInfo(modName, packName, fileName)
    $stderr.print " " * 79 + "\r"
    formatStr = "m[%d/%#{@modsCount.to_s.size}d] #{modName}, p[%d/%#{@packsCount.to_s.size}d] #{packName}, f[%d/%#{@filesCount.to_s.size}d] #{fileName}\r"
    $stderr.printf formatStr, @modsCount, @curModNum, @packsCount, @curPackNum, @filesCount, @curFileNum
  end
  
  alias oldPrint print
  alias oldPuts puts
  
  def print(*s)
    if s
      oldPrint INDENT * @indent if @newline
      str = s.join
      @newline = (str[-1..-1] == "\n")
      oldPrint str
    end
  end
  
  def puts(*s)
    if s
      print s, "\n"
      #oldPrint "\n"
      #@newline = true
    end
  end
  
  attr_reader :indent
  def indentInc
    @indent += 1    
  end
  
  def indentDec
    @indent -= 1 if @indent > 0
  end

  def processModule(mod, outShiftDir = "")
  end

  def process(modules)
    if !(modules && modules.index("nebula2") == nil)
      $stderr.puts "Main package processing."
      puts "--------<<Start main>>----------"
      Dir.chdir(@inDir)
      @modsCount = 1
      @curModNum = 1
      processModule("nebula2")
      puts "---------<<End main>>-----------"
    end

    $stderr.puts "\nContrib modules processing."
    puts "\n--------<<Start contrib>>----------"
    Dir.chdir(@inDir + "contrib")
    @curModNum = 1
    @modsCount = Dir["*"].size
    Dir["*"].sort.each {|mod|
      if FileTest.directory?(mod) &&
         mod != "." && mod != ".." && mod != "CVS" &&
         !(modules && modules.index(mod) == nil)
      then
        processModule(mod, "contrib/")
      end
      @curModNum += 1
    }
    puts "---------<<End contrib>>-----------"
  end
end

#--------------------------------------------------------------------------------------

class CmdProcessor < Processor
  def initialize
    super
    @fourCC = []
  end
  
  def saveFourCC
    @fourCC.sort!{|a, b| a[0]<=>b[0]}
    
    fourCCElements = Element.new("elements")
    fourCCElements.add_attribute("size", @fourCC.size.to_s)
    prevCode = nil
    @fourCC.each{|e|
      code = e[0]
      if prevCode && prevCode == code
        code += "(duplicate)"
      end
      fourCCElements.add_element("fourCC", {"code" => code, "cmd" => e[1], "class" => e[2], "package" => e[3]})
      prevCode = e[0]
    }
    fourCCDoc = Document.new
    fourCCDoc.add_element(fourCCElements)
    Dir.chdir(@outDir)
    puts "Writing fourCC file (" + @outDir + "fourCC.xml)"
    File.open("fourCC.xml", "w"){|f|
      fourCCDoc.write(f, 2, false, true)
    }
  end
  
  def getFourCC(cmdName, pack)
    res = @fourCC.find{|e| e[1] == cmdName && e[3] == pack}
    return res[0] if res
    return "----"
  end
  
  def dispatchDoc(docStr)
    indentInc
    
    atBlocks = {}
    restBlocks = ""
    atName = ""
    needEnd = ""
    
    docStr.each{|line|
      #line.strip!
      if line != ""
        if line =~ /^\s*@\s*(\w[\w\d]*)\s+(.*)\s*$/
          if needEnd == ""
            atName = $1
            if $2 then atBlocks[atName] = $2 + " " else atBlocks[atName] = "" end
            needEnd = "verbatim" if line == "verbatim"
            needEnd = "code" if line == "code"
          else
            if $1 == "end" + needEnd
              needEnd = ""
            else
              if atName != ""
                atBlocks[atName] += line + " "
              else
                restBlocks += line + " "
              end
            end
          end
        elsif atName != ""
          atBlocks[atName] += line + " "
        else
          restBlocks += line + " "
        end
      else
        atName = ""
      end
    }
    
    indentDec
    return [atBlocks, restBlocks]
  end
  
  def dispatchCmdParamDesc(descStr, valueFormatElement)
    indentInc
    
    valueElement = valueFormatElement.add_element("value")
    if descStr.strip == 'v'
      valueElement.add_attribute("type", "void")
    elsif descStr =~ /^([ifsbo]+)\s*\(\s*(\w[\w\d]*)\s*(;\s*\w[\w\d]*\s*=\s*[^;]+\s*)*\)\s*(?:--\s*(.+)\s*)?$/
      typeSize = $1.size
      if typeSize > 1
        if (typeSize <= 4) && ($1 == 'f' * typeSize) # fff or ffff
          valueElement.add_attribute("type", "vector" + typeSize.to_s)
        else
          valueElement.add_element("error").add_text("Wrong parameter type!")
        end
      else
        valueElement.add_attribute("type", CMDTYPES_HASH[$1])
      end
      valueElement.add_attribute("name", $2)
      valueElement.add_element("doc").add_text(CData.new($4)) if $4
      if $3
        attrList = $3.slice(1..-1)
        attrList.split(';').each{|av|
          if av =~ /^\s*(\w[\w\d]*)\s*=\s*(.+?\s*(,.+?)?)\s*$/
            attrName = $1
            if $3
              valueElement.add_attribute("restricted", attrName)
              $2.split(',').each{|e|
                valueElement.add_element(attrName, {"name" => e.strip})
              }
            else
              valueElement.add_attribute($1, $2)
            end
          else
            valueElement.add_element("error").add_text("Wrong <attr=val> statement: " + attrList)
          end
        }
      end
    else
      valueElement.add_element("error").add_text("Wrong parameter description!")
    end

    indentDec
  end
  
  def processCmdFile(fileName, destDir, pack)
    indentInc
    
    fileName =~ /(.+)_cmds.cc/
    xmlFileName = $1
    
    begin
      File.open(fileName, "r"){|cf|
        #puts file + " cmds present"
        scriptElement = Element.new("class")
        scriptElement.add_attribute("name", "")
        scriptElement.add_attribute("parent", "")
        scriptElement.add_attribute("package", pack)
        
        scriptPropertiesElement = scriptElement.add_element("properties")
        scriptMethodsElement = scriptElement.add_element("methods")
        while cf.gets
          case $_
            when CMDFUNCTIONDECL_REGEX
              # do nothing
            when ADDCMDFUNCTION_REGEX
              @fourCC.push [$4, $2, $5, pack]
            when /^\s*\/\*\*(.*)$/ #extract documentation
              commentLine = dispatchExtComment(cf, $1)
              dDoc = dispatchDoc(commentLine)
              atBlocks = dDoc[0]
              restBlocks = dDoc[1].strip
              scriptPropertyElement = nil
              if atBlocks.has_key? "cppclass"
                superClass = atBlocks["superclass"].strip
                classInfo = atBlocks["classinfo"].strip
                
                if classInfo && classInfo != ""
                  docElement = scriptElement.add_element("doc")
                  docElement.add_text(CData.new(classInfo))
                end
                scriptElement.attributes["name"] = atBlocks["cppclass"].strip
                scriptElement.attributes["parent"] = superClass if superClass && superClass != ""
              elsif atBlocks.has_key? "cmd"
                if atBlocks.has_key? "property"
                  cmdName = atBlocks["cmd"].strip
                  cmdType = atBlocks["cmdtype"].strip
                  propertyName = atBlocks["property"].strip
                  info = atBlocks["info"].strip
                  
                  scriptPropertyElement = getElementWithAttribute(scriptPropertiesElement, "property", "name", propertyName, true)
                  
                  fourCC = getFourCC(cmdName, pack)
                  type = cmdType.split
                  if type[0] == "getter"
                    getterListElement = getElement(scriptPropertyElement, "getterlist")
                    getterElement = getElementWithAttribute(getterListElement, "getter", "name", cmdName, true)
                    getterElement.add_attribute("fourCC", fourCC)
                    if info
                      docElement = getElement(getterElement, "doc")
                      docElement.add_text(CData.new(info))
                    end
                    if type[1] && type[1] == "count"
                      getterElement.add_attribute("type", "count")
                    end
                  elsif type[0] == "setter"
                    setterListElement = getElement(scriptPropertyElement, "setterlist")
                    setterElement = getElementWithAttribute(setterListElement, "setter", "name", cmdName, true)
                    setterElement.add_attribute("fourCC", fourCC)
                    if info
                      docElement = getElement(setterElement, "doc")
                      docElement.add_text(CData.new(info))
                    end
                    if type[1] && (type[1] == "begin" || type[1] == "add" || type[1] == "end")
                      setterElement.add_attribute("type", type[1])
                    end
                  else
                    docElement = getElement(scriptPropertyElement, "doc")
                    docElement.add_element("error").add_text("Wrong command type!")
                  end
                elsif atBlocks.has_key?("output") && atBlocks.has_key?("input")
                  cmdName = atBlocks["cmd"].strip
                  output = atBlocks["output"].strip
                  input = atBlocks["input"].strip
                  info = atBlocks["info"].strip
                  
                  scriptMethodElement = getElementWithAttribute(scriptMethodsElement, "method", "name", cmdName, true)
                  scriptMethodElement.add_attribute("fourCC", getFourCC(cmdName, pack))
                  
                  if info
                    docElement = getElement(scriptMethodElement, "doc")
                    docElement.add_text(CData.new(info))
                  end
                  
                  outputElement = getElement(scriptMethodElement, "output")
                  output.each{|l|
                    dispatchCmdParamDesc(l.strip, outputElement)
                  }
                  
                  inputElement = getElement(scriptMethodElement, "input")
                  input.each{|l|
                    dispatchCmdParamDesc(l.strip, inputElement)
                  }
                end
              elsif atBlocks.has_key? "property"
                propertyName = atBlocks["property"].strip
                format = atBlocks["format"].strip
                info = atBlocks["info"].strip
                
                scriptPropertyElement = getElementWithAttribute(scriptPropertiesElement, "property", "name", propertyName, true)
                
                if info
                  docElement = getElement(scriptPropertyElement, "doc")
                  docElement.add_text(CData.new(info))
                end
                if format
                  valueFormatElement = getElement(scriptPropertyElement, "valueformat")
                  format.each{|l|
                    dispatchCmdParamDesc(l.strip, valueFormatElement)
                  }
                  # trick
                  valueFormatElement.each_element("value"){|valueElement|
                    case valueElement.attributes["type"]
                      #when "void"
                      when "integer"
                        valueElement.add_element("editor", {"type" => "text_input"})
                      when "float"
                        valueElement.add_element("editor", {"type" => "slider"})
                      when "string"
                        valueElement.add_element("editor", {"type" => "select"})
                      when "boolean"
                        valueElement.add_element("editor", {"type" => "checkbox"})
                      when "object"
                        valueElement.add_element("editor", {"type" => "text_input"})
                    end
                  }
                end
              end
            when INITCMDFUNCTIONDEF_REGEX
              # do nothing
            when CMDFUNCTIONDEF_REGEX
              # do nothing
          end
        end
        
        if scriptMethodsElement.size + scriptPropertiesElement.size > 0
          puts "There are script commands for " + xmlFileName
          
          xmlScriptFile = destDir + "/" + xmlFileName + ".xml"
          
          xmlScriptDoc = Document.new
          xmlScriptDoc.add_element(scriptElement)
          File.open(xmlScriptFile, "w"){|xf| xmlScriptDoc.write(xf, 2, false, true) }
          indentDec
          return scriptMethodsElement.size + scriptPropertiesElement.size
        else
          indentDec
          return 0
        end
      }
    rescue SystemCallError
      indentDec
      return 0
    end
    
    indentDec
    return 0
  end

  def processModule(mod, outShiftDir = "")
    puts "\nProcess: " + mod
    
    indentInc
    
    outPath = @outDir + outShiftDir + mod
    
    begin
      Dir.mkdir(outPath)
      rescue SystemCallError
        #do nothing
    end
    
    begin
      Dir.mkdir(outPath + "/desc")
      rescue SystemCallError
        #do nothing
    end
    
    begin
      Dir.chdir(mod + "/src")
      rescue SystemCallError
        puts "Error (" + $! + ") while changing to <inc> dir:\n" + @inDir + mod + "/src\n"
        return
    end
        
    @curPackNum = 1
    @packsCount = Dir["*"].size
    packagesProcessed = 0
    Dir["*"].sort.each {|pack|
      if !FileTest.file?(pack) && pack != "." && pack != ".." && pack != "CVS"
        puts "Module: " + pack

        begin
          Dir.mkdir(outPath + "/desc/" + pack)
          rescue SystemCallError
            #do nothing
        end
    
        begin
          Dir.chdir(pack)
          rescue SystemCallError
            puts "Error (" + $! + ") while changing to <mod> dir:\n" + @inDir + mod + "/src/" + pack + "\n"
            next
        end
        
        # files processing
        @curFileNum = 1
        @filesCount = Dir["*.h"].size # n*.h
        elementsProcessed = 0
        Dir["*_cmds.cc"].sort.each{|file| # n*.h
          showProcessInfo(mod, pack, file)
          scriptElement = processCmdFile(file, outPath + "/desc/" + pack, pack)
          @curFileNum += 1
        }
        Dir.chdir("..")
      end
      @curPackNum += 1
    }
    Dir.chdir("../..")
    
    indentDec
  end
end

#--------------------------------------------------------------------------------------

class CCProcessor < Processor
  def dispatchComment(commentStr)
    return [$2.strip, false] if commentStr =~ /^\/\/(\/?<?)?(.+)$/      
    return [$1.strip, true]  if commentStr =~ /^\/\*(.*)$/
    return [nil, false]
  end
  
  def dispatchMethodOut(methodElement, outStr)
    outElement = Element.new("out", methodElement)
    if outStr =~ /^\s*(virtual\s+)?(friend\s+)?(static\s+)?(inline\s+)?(.+)\s*(\*|&)?$/
      methodElement.add_attribute("virtual", "yes") if $1
      methodElement.add_attribute("friend", "yes") if $2
      methodElement.add_attribute("static", "yes") if $3
      methodElement.add_attribute("inline", "yes") if $4
      typeStr = $5
      typeStr += $6 if $6
      dispatchType(outElement, typeStr)
    else
      outElement.add_attribute("outerror", outStr)
    end
    return outElement
  end
  
  def dispatchMethodIn(methodElement, inStr, num)
    inElement = Element.new("in", methodElement)
    if inStr =~ /^\s*(.+?)(\*|&|\s+)(\w[\w\d]*(?:\[\s*\])?)(\s*=\s*([^\)]+))?$/
      dispatchType(inElement, $1.strip + $2.strip)
      inElement.add_attribute("name", $3)
      inElement.add_attribute("defval", $5) if $4
    elsif inStr =~ /^\s*(.+?)\s*(\*|&)?$/
      typeStr = $1.strip
      typeStr += $2 if $2
      dispatchType(inElement, typeStr)
      inElement.add_attribute("name", "")
    else
      inElement.add_attribute("inerror", inStr)
    end
    inElement.add_attribute("num", num.to_s)
    return inElement
  end
  
  def dispatchType(propertyElement, typeStr)
    if typeStr =~ /^\s*(static\s+)?(const\s+)?(.+)\s*(\*|&)?$/
      propertyElement.add_attribute("static", "yes") if $1
      propertyElement.add_attribute("const", "yes") if $2
      t = $3
      t += $4 if $4
      propertyElement.add_attribute("type", t)
    else
      propertyElement.add_attribute("typeerror", typeStr)    
    end
  end
  
  def processEnum(hf, inEnum)
    while hf.gets
      if !inEnum
        inEnum = true if $_ =~ /^\s*\{/
      elsif $_ =~ /^\s*};/
        return
      end
    end
  end
  
  def processUnion(hf, inUnion)
    while hf.gets
      if !inUnion
        inUnion = true if $_ =~ /^\s*\{/
      elsif $_ =~ /^\s*};/
        return
      end
    end
  end
  
  def processStruct(hf, inStruct)
    while hf.gets
      if !inStruct
        inStruct = true if $_ =~ /^\s*\{/
      elsif $_ =~ /^\s*};/
        return
      end
    end
  end
  
  def processClass(hf, xmlElement, nameOfClass, inClass, modName)
    indentInc
    
    elementsProcessed = 1
    commentLine = ""
    extCommentMode = false
    accessMode = "private"
    methodsElement = Element.new("methods", xmlElement)
    propertiesElement = Element.new("properties", xmlElement)
    while hf.gets
      if !inClass
        inClass = true if $_ =~ /^\s*\{/
      else
        case $_
          when /^\s*};/
            break
          when /^\s*(public|private|protected)\s*:/ # public:, private: or protected: accessors
            accessMode = $1
            commentLine = ""
          when /^\s*\/\/\/?(.*)$/ # comment
            commentLine += $1
          when /^\s*\/\*(.*)/ # extended comment
            commentLine += dispatchExtComment(hf, $1)
          when CLASSCONSTRUCTOR_REGEX
            methodElement = methodsElement.add_element("constructor", {"name"=>$1, "access"=>accessMode})
            methodElement.add_element("doc").add_text(commentLine.strip) if commentLine != ""
            commentLine = ""
            n = 1
            $2.split(",").each{|inBlock|
              dispatchMethodIn(methodElement, inBlock.strip, n)
              n += 1
            } if $2
          when CLASSDESTRUCTOR_REGEX
            methodElement = methodsElement.add_element("destructor", {"name"=>$2, "access"=>accessMode})
            methodElement.add_element("doc").add_text(commentLine.strip) if commentLine != ""
            commentLine = ""
            methodElement.add_attribute("virtual", "yes") if $1
          when CLASSMETHOD_REGEX
            methodElement = methodsElement.add_element("method", {"name"=>$3, "access"=>accessMode})
            methodElement.add_element("doc").add_text(commentLine.strip) if commentLine != ""
            commentLine = ""
            methodElement.add_attribute("const", "yes") if $5
            outStr = $1.strip
            outStr += $2.strip if $2
            dispatchMethodOut(methodElement, outStr)
            n = 1
            $4.split(",").each{|inBlock|
              dispatchMethodIn(methodElement, inBlock.strip, n)
              n += 1
            } if $4
          when CLASSPROPERTY_REGEX
            propertyElement = propertiesElement.add_element("property", {"name"=>$3})
            typeStr = $1.strip
            typeStr += $2.strip if $2
            dispatchType(propertyElement, typeStr)
            p = dispatchComment($4.strip)
            if p[0] then
              if !p[1]
                commentLine += p[0]
              else
                commentLine += dispatchExtComment(hf, p[0])
              end
            end
            propertyElement.add_element("doc").add_text(commentLine.strip) if commentLine != ""
            commentLine = ""
          when ENUM_REGEX
            processEnum(hf, $2 != nil)
            commentLine = ""
          when UNION_REGEX
            processUnion(hf, $2 != nil)
            commentLine = ""
          when STRUCT_REGEX
            processStruct(hf, $2 != nil)
            commentLine = ""
          when CLASS_REGEX
            nameOfSubClass = $3
            parentClass = $4 ? $6 : nil
            xmlSubElement = Element.new("class", xmlElement)
            xmlSubElement.add_element("template", {"val"=>$2}) if $1
            xmlSubElement.add_attribute("name", nameOfSubClass)
            if parentClass
              parentElement = xmlSubElement.add_element("parent", {"class" => parentClass})
              if $5
                parentElement.add_attribute("access", $5.strip)
              else
                parentElement.add_attribute("access", "private") #???
              end
            end
            xmlSubElement.add_attribute("package", modName)
            xmlSubElement.add_attribute("access", accessMode)
            xmlSubElement.add_element("doc").add_text(commentLine.strip) if commentLine != ""
            elementsProcessed += processClass(hf, xmlSubElement, nameOfSubClass, $8 != nil, modName)
            commentLine = ""
        end
      end
      @lineNum += 1
    end
    
    indentDec
    return elementsProcessed
  end

  def processFile(hFile, destDir, modName)
    indentInc
    
    elementsProcessed = 0
    commentLine = ""
    nameOfClass = nil
    parentClass = nil
    cppElement = Element.new("classes")
    xmlElement = nil
    @lineNum = 1
    @indent = 2
    File.open(hFile, "r"){|hf|
      while hf.gets
        if $_ =~ /^\s*\/\*\*(.*)/
          commentLine += dispatchExtComment(hf, $1)
        elsif $_ =~ CLASS_REGEX
          nameOfClass = $3
          parentClass = $4 ? $6 : nil
          xmlElement = Element.new("class", cppElement)
          xmlElement.add_element("doc").add_text("\n" + commentLine + "      ") if commentLine != ""
          xmlElement.add_element("template", {"val"=>$2}) if $1
          xmlElement.add_attribute("name", nameOfClass)
          if parentClass
            parentElement = xmlElement.add_element("parent", {"class" => parentClass})
            if $5
              parentElement.add_attribute("access", $5.strip)
            else
              parentElement.add_attribute("access", "private") #???
            end
          end
          xmlElement.add_attribute("package", modName)
          elementsProcessed += processClass(hf, xmlElement, nameOfClass, $8 != nil, modName)
          commentLine = ""
        end
      @lineNum += 1
      end
    }
    
    if elementsProcessed > 0
      xmlCppFile = destDir + "/" + hFile[0..-3] + ".xml"
      
      begin
        Dir.mkdir(destDir)
        rescue SystemCallError
          #do nothing
      end

      xmlCppDoc = Document.new
      xmlCppDoc.add_element(cppElement)
      File.open(xmlCppFile, "w"){|xf| xmlCppDoc.write(xf, 2, false, true) }      
    end
    
    puts elementsProcessed.to_s + " class" + (elementsProcessed != 1 ? "es" : "  ") + " in " + hFile
    
    indentDec
    return elementsProcessed
  end
  
  def processModule(mod, outShiftDir = "")
    puts "\nProcess: " + mod
    
    indentInc
    
    outPath = @outDir + outShiftDir + mod
    
    begin
      Dir.mkdir(outPath)
      rescue SystemCallError
        #do nothing
    end
    
    begin
      Dir.mkdir(outPath + "/desc")
      rescue SystemCallError
        #do nothing
    end
    
    begin
      Dir.chdir(mod + "/inc")
      rescue SystemCallError
        puts "Error (" + $! + ") while changing to <inc> dir:\n" + @inDir + mod + "/inc\n"
        return
    end
        
    @curPackNum = 1
    @packsCount = Dir["*"].size
    packagesProcessed = 0
    Dir["*"].sort.each {|pack|
      if !FileTest.file?(pack) && pack != "." && pack != ".." && pack != "CVS"
        puts "Module: " + pack

        begin
          Dir.mkdir(outPath + "/desc/" + pack)
          rescue SystemCallError
            #do nothing
        end
    
        begin
          Dir.chdir(pack)
          rescue SystemCallError
            puts "Error (" + $! + ") while changing to <mod> dir:\n" + @inDir + mod + "/inc/" + pack + "\n"
            next
        end
        
        # files processing
        @curFileNum = 1
        @filesCount = Dir["*.h"].size # n*.h
        elementsProcessed = 0
        Dir["*.h"].sort.each{|file| # n*.h
          showProcessInfo(mod, pack, file)
          elementsProcessed += processFile(file, outPath + "/desc/" + pack + "/cpp", mod)
          @curFileNum += 1
        }
        if elementsProcessed == 0
          Dir.delete(outPath + "/desc/" + pack)
        else
          packagesProcessed += 1
        end
        Dir.chdir("..")
      end
      @curPackNum += 1
    }
    if packagesProcessed == 0
      Dir.delete(outPath + "/desc/")
    end
    Dir.chdir("../..")
    
    indentDec
  end
end

workdir = Dir.pwd.sub(/\/bin$/, "/code/")

modules = ["nebula2"]
Dir.chdir(workdir + "contrib")
Dir["*"].sort.each {|mod|
  if FileTest.directory?(mod) && mod != "." && mod != ".." && mod != "CVS"
    modules.push(mod)
  end
}
Dir.chdir(workdir + "../bin/")

selmodules = nil
print "\nChoose action:\n1. Process all modules.\n2. Select modules to process.\n: "
choice = STDIN.gets

if choice.to_i == 2
  selmodules = []
  modules.each_with_index{|m, i|
    printf "%3d: %s\n", i + 1, m
  }
  puts "Choose modules number separated by space:"
  STDIN.gets.split.each{|n|
    num = n.to_i
    if num > 0 && num <= modules.size
      selmodules.push(modules[num - 1])
    end
  }
end

$stdout = File.open("xmlgenerator.log", "w")

$stderr.puts "\n---------<<Cpp processing>>----------"
ccp = CCProcessor.new
ccp.setDirs(workdir, workdir)
ccp.process selmodules

$stderr.puts "\n\n---------<<Cmd processing>>----------"
cmdp = CmdProcessor.new
cmdp.setDirs(workdir, workdir)
cmdp.process selmodules
cmdp.saveFourCC

$stdout.close

$stderr.puts "\n\nPress ENTER to exit..."
STDIN.getc
