//------------------------------------------------------------------------------
//  nstream.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "util/nstream.h"
#include "kernel/nfileserver2.h"

//------------------------------------------------------------------------------
/**
*/
nStream::nStream() :
    mode(InvalidMode),
    fileCreated(false),
    xmlDocument(0),
    curNode(0),
    utf8Coding(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nStream::nStream(const nString& fname) :
    filename(fname),
    mode(InvalidMode),
    xmlDocument(0),
    curNode(0),
    utf8Coding(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nStream::~nStream()
{
    if (this->IsOpen())
    {
        this->Close();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nStream::SetAutomaticUTF8Coding(bool b)
{
    this->utf8Coding = b;
}

//------------------------------------------------------------------------------
/**
*/
bool
nStream::GetAutomaticUTF8Coding() const
{
    return this->utf8Coding;
}

//------------------------------------------------------------------------------
/**
*/
void
nStream::SetFilename(const nString& n)
{
    this->filename = n;
}

//------------------------------------------------------------------------------
/**
*/
const nString&
nStream::GetFilename() const
{
    return this->filename;
}

//------------------------------------------------------------------------------
/**
*/
bool
nStream::IsOpen() const
{
    return (0 != this->xmlDocument);
}

//------------------------------------------------------------------------------
/**
*/
bool
nStream::FileCreated() const
{
    return this->fileCreated;
}



//------------------------------------------------------------------------------
/**
    Generates a string from the document
*/
void
nStream::GetDocumentAsString(nString& rsString)
{
    rsString = "";
    AddNode2String(rsString,this->xmlDocument->FirstChild());
}


//------------------------------------------------------------------------------
/**
    helper function , to generate a string
*/

void
nStream::AddNode2String(nString& rsString, const TiXmlNode* pNode, int iIndention)
 {
      const char* pcValue = pNode->Value();

      /*
      for(int i=0; i<iIndention; ++i)
      {
           rsString += "\t";
      } */

      switch(pNode->Type())
      {
      case TiXmlNode::COMMENT:
           //assert(pNode->NoChildren()  &&  "Node Type COMMENT should not have children");
           rsString += "<!-- ";
           rsString += pcValue;
           rsString += " -->";
           break;

      case TiXmlNode::DECLARATION:
           //assert(pNode->NoChildren()  &&  "Node Type DECLARATION should not have children");
           rsString += "<?";
           rsString += pcValue;
           rsString += "?>";
           break;

      case TiXmlNode::DOCUMENT:
          // assert (false && "No Documents should be passed into this function; pass the root element instead");
           return;

      case TiXmlNode::ELEMENT:
           rsString += "<";
           rsString += pcValue;

           for(const TiXmlAttribute* pAttr = pNode->ToElement()->FirstAttribute(); pAttr!= 0; pAttr = pAttr->Next())
           {
                rsString += " ";
                rsString += pAttr->Name();
                rsString += "=\"";
                rsString += pAttr->Value();
                rsString += "\"";
           }

           if(pNode->FirstChild()  &&  pNode->FirstChild()->NextSibling() == 0  &&
              pNode->FirstChild()->Type() == TiXmlNode::TEXT)
           {
                rsString += ">";
                rsString += pNode->FirstChild()->ToText()->Value();
                rsString += "</";
                rsString += pcValue;
                rsString += ">";

                return;
           }

           if(pNode->NoChildren())
           {
                rsString += "/>";
           }
           else
           {
                rsString += ">";
           }
           break;

      case TiXmlNode::TEXT:
           //assert(pNode->NoChildren()  &&  "Node Type TEXT should not have children");
           rsString += pcValue;
           break;

      case TiXmlNode::UNKNOWN:
           //assert(pNode->NoChildren()  &&  "Node Type UNKNOWN should not have children");
           rsString += "<";
           rsString += pcValue;
           rsString += ">";
           break;

      default:
           //assert (false && "Unknown Node type");
           return;
      }

      if(pNode->Type() == TiXmlNode::ELEMENT  &&  !pNode->NoChildren())
      {
           for(const TiXmlNode* pChild = pNode->FirstChild(); pChild != 0; pChild = pChild->NextSibling())
           {
                AddNode2String(rsString, pChild, iIndention + 1);
           }


           /*
           for(int i=0; i<iIndention; ++i)
           {
                rsString += "\t";
           } */

           rsString += "</";
           rsString += pcValue;
           rsString += ">";
      }
 }

//------------------------------------------------------------------------------
/**
    Opens a string for parsing
*/
bool
nStream::OpenString(const nString& n)
{
    n_assert(!this->IsOpen());
    n_assert(this->filename.IsValid());
    n_assert(0 == this->curNode);
    this->fileCreated = false;
    this->mode = String;

    // create xml document object
    this->xmlDocument = n_new(TiXmlDocument(this->filename.Get()));

    if (!n.IsEmpty())
    {
        // parse the string
        this->xmlDocument->Parse(n.Get());
        if (this->xmlDocument->Error())
        {
            //n_error("nStream::OpenString() Failed to parse string !");
            return false;
        }

        // set the current node to the root node
        this->curNode = this->xmlDocument->RootElement();
    }
    else
    {
        // no empty string allowed
        return false;
    }
    return true;
}
//------------------------------------------------------------------------------
/**
    Open the stream in read, write or read/write mode. In Read mode the
    stream file must exist, otherwise the method will fail hard with an error. In
    Write mode an existing file will be overwritten. In ReadWrite mode
    an existing file will be read, may be modified and will be
    written back on Close().
*/
bool
nStream::Open(Mode m)
{
    n_assert(!this->IsOpen());
    n_assert(this->filename.IsValid());
    n_assert(0 == this->curNode);
    this->fileCreated = false;
    this->mode = m;

    // check if the file already exists
    bool fileExists = nFileServer2::Instance()->FileExists(this->filename);
    if (!fileExists && ((Write == this->mode) || (ReadWrite == this->mode)))
    {
        this->fileCreated = true;
    }

    // create xml document object
    this->xmlDocument = n_new(TiXmlDocument(this->filename.Get()));
    if ((Read == this->mode) || ((ReadWrite == this->mode) && fileExists))
    {
        // read existing xml document
        if (!this->xmlDocument->LoadFile())
        {
            n_error("nStream::Open() Failed to read file '%s'!", this->filename.Get());
            return false;
        }

        // set the current node to the root node
        this->curNode = this->xmlDocument->RootElement();
    }
    else
    {
        // when in write mode, we add a declaration to the XML stream
        // (this creates the line <?xml version="1.0" encoding="UTF-8"?> to the file
        this->xmlDocument->InsertEndChild(TiXmlDeclaration("1.0", "UTF-8", ""));
    }

    return true;
}

//------------------------------------------------------------------------------
/**
    Close the stream. When in Write or ReadWrite mode, the xml document
    will be saved out before being destroyed.
*/
bool
nStream::Close()
{
    n_assert(this->IsOpen());
    n_assert(this->filename.IsValid());

    // if in write or read/write mode, save out the xml document
    if ((Write == this->mode) || (ReadWrite == this->mode))
    {
        nFileServer2* fileServer = nFileServer2::Instance();

        // make sure the path to the file exists
        fileServer->MakePath(this->filename.ExtractDirName());

        // FIXME: TinyXml doesn't support Nebula2 filenames for writing
        nString mangledPath = fileServer->ManglePath(this->filename.Get());
        if (!this->xmlDocument->SaveFile(mangledPath.Get()))
        {
            n_delete(this->xmlDocument);
            this->xmlDocument = 0;
            n_error("nStream::Close(): Failed to write file '%s'!", this->filename.Get());
            return false;
        }
    }

    // delete the xml document
    n_delete(this->xmlDocument);
    this->xmlDocument = 0;
    this->curNode = 0;
    this->mode = InvalidMode;
    return true;
}

//------------------------------------------------------------------------------
/**
    This method returns the line number of the current node.
*/
int
nStream::GetCurrentNodeLineNumber() const
{
    if (this->curNode)
    {
        return this->curNode->Row();
    }
    else
    {
        return 0;
    }
}

//------------------------------------------------------------------------------
/**
    This method finds an xml node by path name. It can handle absolute
    paths and paths relative to the current node. All the usual file system
    path conventions are valid: "/" is the path separator, "." is the
    current directory, ".." the parent directory.
*/
TiXmlNode*
nStream::FindNode(const nString& path) const
{
    n_assert(this->IsOpen());
    n_assert(path.Length() > 0);

    bool absPath = (path[0] == '/');
    nArray<nString> tokens;
    path.Tokenize("/", tokens);

    TiXmlNode* node;
    if (absPath)
    {
        node = this->xmlDocument;
    }
    else
    {
        n_assert(0 != this->curNode);
        node = this->curNode;
    }

    int i;
    int num = tokens.Size();
    for (i = 0; i < num; i++)
    {
        const nString& cur = tokens[i];
        if ("." == cur)
        {
            // do nothing
        }
        else if (".." == cur)
        {
            // go to parent directory
            node = node->Parent();
            if (node == this->xmlDocument)
            {
                n_error("nStream::FindNode(%s): path points above root node!", path.Get());
                return 0;
            }
        }
        else
        {
            // find child node
            node = node->FirstChild(cur.Get());
            if (0 == node)
            {
                return 0;
            }
        }
    }
    return node;
}

//------------------------------------------------------------------------------
/**
    Begin a new node in write mode. The new node will be set as the current
    node. Nodes may form a hierarchy. Make sure to finalize a node
    with a corresponding call to EndNode()!
*/
bool
nStream::BeginNode(const nString& name)
{
    n_assert(this->IsOpen());
    n_assert(Read != this->mode);
    if (0 == this->curNode)
    {
        // create the root node
        this->curNode = this->xmlDocument->InsertEndChild(TiXmlElement(name.Get()))->ToElement();
    }
    else
    {
        // create a child node
        this->curNode = this->curNode->InsertEndChild(TiXmlElement(name.Get()))->ToElement();
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    End a node in write mode. This will set the parent of the current node as
    new current node so that correct hierarchical behavior is implemented.
*/
void
nStream::EndNode()
{
    n_assert(this->IsOpen());
    n_assert(Read != this->mode);
    n_assert(0 != this->curNode);

    TiXmlNode* parent = this->curNode->Parent();
    n_assert(parent);
    if (parent == this->xmlDocument)
    {
        // we're back at the root
        this->curNode = 0;
    }
    else
    {
        this->curNode = parent->ToElement();
    }
}

//------------------------------------------------------------------------------
/**
    This method returns true if the node identified by path exists. Path
    follows the normal filesystem path conventions, "/" is the separator,
    ".." is the parent node, "." is the current node. An absolute path
    starts with a "/", a relative path doesn't.
*/
bool
nStream::HasNode(const nString& n) const
{
    return (this->FindNode(n) != 0);
}

//------------------------------------------------------------------------------
/**
    Get the short name (without path) of the current node. Node that
    (as a special case), that the name of the root node is the filename
    of the XML document. This is a quirk of TinyXML and isn't handled
    separatly here.
*/
nString
nStream::GetCurrentNodeName() const
{
    n_assert(this->IsOpen());
    n_assert(this->curNode);
    return nString(this->curNode->Value());
}

//------------------------------------------------------------------------------
/**
    This returns the full absolute path of the current node. Path components
    are separated by slashes.
*/
nString
nStream::GetCurrentNodePath() const
{
    n_assert(this->IsOpen());
    n_assert(this->curNode);

    // build bottom-up array of node names
    nArray<nString> components;
    TiXmlNode* node = this->curNode;
    while (node != this->xmlDocument)
    {
        components.Append(node->Value());
        node = node->Parent();
    }

    // build top down path
    nString path = "/";
    int i;
    for (i = components.Size() - 1; i >= 0; --i)
    {
        path.Append(components[i]);
        if (i > 0)
        {
            path.Append("/");
        }
    }
    return path;
}

//------------------------------------------------------------------------------
/**
    Set the node pointed to by the path string as current node. The path
    may be absolute or relative, following the usual filesystem path
    conventions. Separator is a slash.
*/
void
nStream::SetToNode(const nString& p)
{
    n_assert(this->IsOpen());
    n_assert(p.IsValid());
    this->curNode = this->FindNode(p)->ToElement();
    if (0 == this->curNode)
    {
        n_error("nStream::SetToNode(%s): node to found!", p.Get());
    }
}

//------------------------------------------------------------------------------
/**
    Sets the current node to the first child node. If no child node exists,
    the current node will remain unchanged and the method will return false.
    If name is a valid string, only child element matching the name will
    be returned. If name is empty, all child nodes will be considered.
*/
bool
nStream::SetToFirstChild(const nString& name)
{
    n_assert(this->IsOpen());
    n_assert(this->curNode);
    TiXmlElement* child = 0;
    if (name.IsEmpty())
    {
        child = this->curNode->FirstChildElement();
    }
    else
    {
        child = this->curNode->FirstChildElement(name.Get());
    }
    if (child)
    {
        this->curNode = child;
        return true;
    }
    else
    {
        return false;
    }
}

//------------------------------------------------------------------------------
/**
    Sets the current node to the next sibling. If no more children exist,
    the current node will be reset to the parent node and the method will
    return false. If name is a valid string, only child element matching the
    name will be returned. If name is empty, all child nodes will be considered.
*/
bool
nStream::SetToNextChild(const nString& name)
{
    n_assert(this->IsOpen());
    n_assert(this->curNode);

    TiXmlElement* sib = 0;
    if (name.IsEmpty())
    {
        sib = this->curNode->NextSiblingElement();
    }
    else
    {
        sib = this->curNode->NextSiblingElement(name.Get());
    }
    if (sib)
    {
        this->curNode = sib;
        return true;
    }
    else
    {
        this->SetToParent();
        return false;
    }
}

//------------------------------------------------------------------------------
/**
    Sets the current node to its parent. If no parent exists, the
    current node will remain unchanged and the method will return false.
*/
bool
nStream::SetToParent()
{
    n_assert(this->IsOpen());
    n_assert(this->curNode);
    TiXmlNode* parent = this->curNode->Parent();
    if (parent)
    {
        this->curNode = parent->ToElement();
        return true;
    }
    else
    {
        return false;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nStream::SetToNodeByAttribute(const nString& nodeName, const nString& attribute, const nString& value)
{
    n_assert(this->IsOpen());
    n_assert(nodeName.IsValid());
    n_assert(attribute.IsValid());
    n_assert(value.IsValid());

    this->SetToNode(nodeName);
    while(this->GetString(attribute) != value)
    {
        if(this->SetToNextChild() == false)
        {
            return;
        }
    }
}

//------------------------------------------------------------------------------
/**
    Return true if an attribute of the given name exists on the current
    node.
*/
bool
nStream::HasAttr(const nString& name) const
{
    n_assert(this->IsOpen());
    n_assert(this->curNode);
    n_assert(name.IsValid());
    return (0 != this->curNode->Attribute(name.Get()));
}

//------------------------------------------------------------------------------
/**
    Return the provided attribute as string. If the attribute does not exist
    the method will fail hard (use HasAttr() to check for its existance).
*/
nString
nStream::GetString(const nString& name) const
{
    n_assert(this->IsOpen());
    n_assert(this->curNode);
    n_assert(name.IsValid());
    nString str;
    const char* val = this->curNode->Attribute(name.Get());
    if (0 == val)
    {
        n_error("Stream: attribute '%s' doesn't exist on node '%s'!", name.Get(), this->curNode->Value());
    }
    else
    {
        str = val;
        if (this->utf8Coding)
        {
            str.UTF8toANSI();
        }
    }
    return str;
}

//------------------------------------------------------------------------------
/**
    Return the provided optional attribute as string. If the attribute doesn't
    exist, the default value will be returned.
*/
nString
nStream::GetOptionalString(const nString& name, const nString& defaultValue) const
{
    if (this->HasAttr(name))
    {
        return this->GetString(name);
    }
    else
    {
        return defaultValue;
    }
}

//------------------------------------------------------------------------------
/**
    Return the provided attribute as a bool. If the attribute does not exist
    the method will fail hard (use HasAttr() to check for its existance).
*/
bool
nStream::GetBool(const nString& name) const
{
    // NOTE: the check against 1 is for backward compatibility
    nString value = this->GetString(name);
    return ("1" == value) || ("true" == value);
}

//------------------------------------------------------------------------------
/**
    Return the provided optional attribute as bool. If the attribute doesn't
    exist, the default value will be returned.
*/
bool
nStream::GetOptionalBool(const nString& name, bool defaultValue) const
{
    if (this->HasAttr(name))
    {
        return this->GetBool(name);
    }
    else
    {
        return defaultValue;
    }
}

//------------------------------------------------------------------------------
/**
    Return the provided attribute as int. If the attribute does not exist
    the method will fail hard (use HasAttr() to check for its existance).
*/
int
nStream::GetInt(const nString& name) const
{
    return this->GetString(name).AsInt();
}

//------------------------------------------------------------------------------
/**
    Return the provided optional attribute as int. If the attribute doesn't
    exist, the default value will be returned.
*/
int
nStream::GetOptionalInt(const nString& name, int defaultValue) const
{
    if (this->HasAttr(name))
    {
        return this->GetInt(name);
    }
    else
    {
        return defaultValue;
    }
}

//------------------------------------------------------------------------------
/**
    Return the provided attribute as float. If the attribute does not exist
    the method will fail hard (use HasAttr() to check for its existance).
*/
float
nStream::GetFloat(const nString& name) const
{
    return this->GetString(name).AsFloat();
}

//------------------------------------------------------------------------------
/**
    Return the provided optional attribute as float. If the attribute doesn't
    exist, the default value will be returned.
*/
float
nStream::GetOptionalFloat(const nString& name, float defaultValue) const
{
    if (this->HasAttr(name))
    {
        return this->GetFloat(name);
    }
    else
    {
        return defaultValue;
    }
}

//------------------------------------------------------------------------------
/**
    Return the provided attribute as vector3. If the attribute does not exist
    the method will fail hard (use HasAttr() to check for its existance).
*/
vector3
nStream::GetVector3(const nString& name) const
{
    return this->GetString(name).AsVector3();
}

//------------------------------------------------------------------------------
/**
    Return the provided optional attribute as vector3. If the attribute doesn't
    exist, the default value will be returned.
*/
vector3
nStream::GetOptionalVector3(const nString& name, const vector3& defaultValue) const
{
    if (this->HasAttr(name))
    {
        return this->GetVector3(name);
    }
    else
    {
        return defaultValue;
    }
}

//------------------------------------------------------------------------------
/**
    Return the provided attribute as vector4. If the attribute does not exist
    the method will fail hard (use HasAttr() to check for its existance).
*/
vector4
nStream::GetVector4(const nString& name) const
{
    return this->GetString(name).AsVector4();
}

//------------------------------------------------------------------------------
/**
    Return the provided optional attribute as vector4. If the attribute doesn't
    exist, the default value will be returned.
*/
vector4
nStream::GetOptionalVector4(const nString& name, const vector4& defaultValue) const
{
    if (this->HasAttr(name))
    {
        return this->GetVector4(name);
    }
    else
    {
        return defaultValue;
    }
}

//------------------------------------------------------------------------------
/**
    Return the provided attribute as matrix44. If the attribute does not exist
    the method will fail hard (use HasAttr() to check for its existance).
*/
matrix44
nStream::GetMatrix44(const nString& name) const
{
    nString matrixString = this->GetString(name);
    nArray<nString> tokens;
    matrixString.Tokenize(";", tokens);
    n_assert(tokens.Size() == 4);
    matrix44 m;
    m.x_component().set(tokens[0].AsVector3());
    m.y_component().set(tokens[1].AsVector3());
    m.z_component().set(tokens[2].AsVector3());
    m.pos_component().set(tokens[3].AsVector3());
    return m;
}

//------------------------------------------------------------------------------
/**
    Return the provided optional attribute as matrix44. If the attribute doesn't
    exist, the default value will be returned.
*/
matrix44
nStream::GetOptionalMatrix44(const nString& name, const matrix44& defaultValue) const
{
    if (this->HasAttr(name))
    {
        return this->GetMatrix44(name);
    }
    else
    {
        return defaultValue;
    }
}

//------------------------------------------------------------------------------
/**
    Set the provided attribute to a string value. The stream must be
    in Write or ReadWrite mode for this.
*/
void
nStream::SetString(const nString& name, const nString& value)
{
    n_assert(this->IsOpen());
    n_assert(this->curNode);
    n_assert(name.IsValid());
    n_assert(value.IsValid());
    n_assert((Write == this->mode) || (ReadWrite == this->mode || (String == this->mode)));
    if (this->utf8Coding)
    {
        nString utf8 = value;
        utf8.UTF8toANSI();
        this->curNode->SetAttribute(name.Get(), utf8.Get());
    }
    else
    {
        this->curNode->SetAttribute(name.Get(), value.Get());
    }
}

//------------------------------------------------------------------------------
/**
    Set the provided attribute to a bool value. The stream must be
    in Write or ReadWrite mode for this.
*/
void
nStream::SetBool(const nString& name, bool b)
{
    nString s;
    s.SetBool(b);
    this->SetString(name, s);
}

//------------------------------------------------------------------------------
/**
    Set the provided attribute to a int value. The stream must be
    in Write or ReadWrite mode for this.
*/
void
nStream::SetInt(const nString& name, int i)
{
    nString s;
    s.SetInt(i);
    this->SetString(name, s);
}

//------------------------------------------------------------------------------
/**
    Set the provided attribute to a float value. The stream must be
    in Write or ReadWrite mode for this.
*/
void
nStream::SetFloat(const nString& name, float f)
{
    nString s;
    s.SetFloat(f);
    this->SetString(name, s);
}

//------------------------------------------------------------------------------
/**
    Set the provided attribute to a vector3 value. The stream must be
    in Write or ReadWrite mode for this.
*/
void
nStream::SetVector3(const nString& name, const vector3& v)
{
    nString s;
    s.SetVector3(v);
    this->SetString(name, s);
}

//------------------------------------------------------------------------------
/**
    Set the provided attribute to a vector4 value. The stream must be
    in Write or ReadWrite mode for this.
*/
void
nStream::SetVector4(const nString& name, const vector4& v)
{
    nString s;
    s.SetVector4(v);
    this->SetString(name, s);
}

//------------------------------------------------------------------------------
/**
    Set the provided attribute to a matrix44 value.  The stream must be
    in Write or ReadWrite mode for this. Note: the matrix will be written
    as 4x3 (without rightmost column), so it's not suitable for projection
    matrices.
*/
void
nStream::SetMatrix44(const nString& name, const matrix44& m)
{
    nString x,y,z,p;
    x.SetVector3(m.x_component());
    y.SetVector3(m.y_component());
    z.SetVector3(m.z_component());
    p.SetVector3(m.pos_component());
    nString matrixString;
    matrixString.Format("%s;%s;%s;%s", x.Get(), y.Get(), z.Get(), p.Get());
    this->SetString(name, matrixString);
}

//------------------------------------------------------------------------------
/**
    Returns the names of all attributes on the node.
*/
nArray<nString>
nStream::GetAttrs() const
{
    n_assert(this->IsOpen());
    n_assert(this->curNode);
    nArray<nString> attrs(32, 32);
    TiXmlAttribute* attr = this->curNode->FirstAttribute();
    if (attr) do
    {
        attrs.Append(attr->Name());
    }
    while ((attr = attr->Next()));
    return attrs;
}

//------------------------------------------------------------------------------
/**
    Returns true if the current node has embedded text (in the form
    \<Node\>Embedded Text\</Node\>)
*/
bool
nStream::HasText() const
{
    n_assert(this->IsOpen());
    n_assert(this->curNode);
    TiXmlNode* child = this->curNode->FirstChild();
    return child && (child->Type() == TiXmlNode::TEXT);
}

//------------------------------------------------------------------------------
/**
    Return the embedded text of the current node as a string. See
    HasText() for details.
*/
nString
nStream::GetText() const
{
    n_assert(this->IsOpen());
    n_assert(this->curNode);
    TiXmlNode* child = this->curNode->FirstChild();
    n_assert(child->Type() == TiXmlNode::TEXT);
    return nString(child->Value());
}
