#ifndef LOADER_STREAM_H
#define LOADER_STREAM_H
//------------------------------------------------------------------------------
/**
    @class nStream
    @ingroup Util

    A stream provides read/write access to an XML file.

    NOTE: since the stream is based on Nebula2's file routines, the stream
    can be read from an npk archive, but cannot be written to npk archives!

    (C) 2005 Radon Labs GmbH
*/
#include "util/nstring.h"
#include "mathlib/vector.h"
#include "mathlib/matrix.h"
#include "tinyxml/tinyxml.h"

//------------------------------------------------------------------------------
class nStream
{
public:
    /// read/write mode flags
    enum Mode
    {
        InvalidMode,
        Read,
        Write,
        ReadWrite,
        String,
    };

    /// default constructor
    nStream();
    /// constructor with filename
    nStream(const nString& fname);
    /// destructor
    virtual ~nStream();
    ///returns the whole document as string
    void GetDocumentAsString(nString& rsString);
    /// turn automatic UTF8 de/encoding on/off (default is off)
    void SetAutomaticUTF8Coding(bool b);
    /// get automatic UTF8 coding flag
    bool GetAutomaticUTF8Coding() const;
    /// set the filename of the stream
    void SetFilename(const nString& n);
    /// get the filename of the stream
    const nString& GetFilename() const;
    /// open the stream for reading or writing
    virtual bool Open(Mode mode);
    /// open string for parsing
    virtual bool OpenString(const nString& n);
    /// close the stream
    virtual bool Close();
    /// return true if stream is open
    bool IsOpen() const;
    /// return true if new file has been created in Open
    bool FileCreated() const;

    /// begin a new node (write mode only)
    bool BeginNode(const nString& nodeName);
    /// end current node (write mode only)
    void EndNode();

    /// return true if node identified by path exists
    bool HasNode(const nString& p) const;
    /// get the short name of the current node
    nString GetCurrentNodeName() const;
    /// get current node name as full path
    nString GetCurrentNodePath() const;
    /// returns the line number of the current node
    int GetCurrentNodeLineNumber() const;
    /// set current node as path (read/write)
    void SetToNode(const nString& p);
    /// set current node to first child node, return false if no child exists
    bool SetToFirstChild(const nString& name = "");
    /// set current node to next sibling node, returns false if no more children exists
    bool SetToNextChild(const nString& name = "");
    /// set current node to parent node, returns false if no parent exists
    bool SetToParent();
    /// set current node to an node, that has the attribute and the value
    void SetToNodeByAttribute(const nString& nodeName, const nString& attribute, const nString& value);

    /// return true if current node has embedded text
    bool HasText() const;
    /// return the text embedded by the current node
    nString GetText() const;

    /// return true if attribute exists on current node
    bool HasAttr(const nString& name) const;
    /// return names of all attributes on current node
    nArray<nString> GetAttrs() const;

    /// set string attribute in current node
    void SetString(const nString& name, const nString& s);
    /// set bool attribute on current node
    void SetBool(const nString& name, bool b);
    /// set int attribute on current node
    void SetInt(const nString& name, int i);
    /// set float attribute on current node
    void SetFloat(const nString& name, float f);
    /// set vector3 attribute on current node
    void SetVector3(const nString& name, const vector3& v);
    /// set vector4 attribute on current node
    void SetVector4(const nString& name, const vector4& v);
    /// set matrix44 attribute on current node
    void SetMatrix44(const nString& name, const matrix44& m);

    /// get string attribute from current node
    nString GetString(const nString& name) const;
    /// get bool attribute from current node
    bool GetBool(const nString& name) const;
    /// get int attribute from current node
    int GetInt(const nString& name) const;
    /// get float attribute from current node
    float GetFloat(const nString& name) const;
    /// get vector3 attribute from current node
    vector3 GetVector3(const nString& name) const;
    /// get vector4 attribute from current node
    vector4 GetVector4(const nString& name) const;
    /// get matrix44 attribute from stream
    matrix44 GetMatrix44(const nString& name) const;

    /// get optional string attribute from current node
    nString GetOptionalString(const nString& name, const nString& defaulValue) const;
    /// get optional bool attribute from current node
    bool GetOptionalBool(const nString& name, bool defaultValue) const;
    /// get optional int attribute from current node
    int GetOptionalInt(const nString& name, int defaultValue) const;
    /// get optional float attribute from current node
    float GetOptionalFloat(const nString& name, float defaultValue) const;
    /// get optional vector3 attribute from current node
    vector3 GetOptionalVector3(const nString& name, const vector3& defaultValue) const;
    /// get optional vector4 attribute from current node
    vector4 GetOptionalVector4(const nString& name, const vector4& defaultValue) const;
    /// get optional matrix44 attribute from stream
    matrix44 GetOptionalMatrix44(const nString& name, const matrix44& defaultValue) const;

private:
    /// find a node by path, handles relativ paths as well
    TiXmlNode* FindNode(const nString& path) const;

    ///  helper function , to generate a string
    void AddNode2String(nString& rsString, const TiXmlNode* pNode, int iIndention = 0);

    bool utf8Coding;
    nString filename;
    Mode mode;
    bool fileCreated;
    TiXmlDocument* xmlDocument;
    TiXmlElement* curNode;
};
//------------------------------------------------------------------------------
#endif
