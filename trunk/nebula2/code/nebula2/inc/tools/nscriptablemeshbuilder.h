#ifndef N_SCRIPTABLE_MESHBUILDER_H
#define N_SCRIPTABLE_MESHBUILDER_H
//------------------------------------------------------------------------------
/**
    @class nScriptableMeshBuilder
    @ingroup

    The purpose of this class is to allow the use of nMeshBuilder from scripting 
    languages. Deriving from nRoot gives us this ability.

    @note
    This class multiply inherits from both nRoot and nMeshBuilder. 
    This causes ambiguity when both base classes have functions with the same name.
    To let the compiler know which one we want, use scope operator with the base 
    class name:
    @code
    int val = obj->BaseClass::SomeFunction(params);
    @endcode

    (C)2005 James Mastro / Kim, Hyoun Woo

*/
#include "tools/nmeshbuilder.h"

//------------------------------------------------------------------------------
class nScriptableMeshBuilder : public nRoot
                             , public nMeshBuilder
{
public:
    /// constructor.
    nScriptableMeshBuilder();
    /// destructor.
    virtual ~nScriptableMeshBuilder();

    /// @name Vertex Adding Functions
    /// @{
    int BeginAddVertex();
    void AddCoord(int index, float x, float y, float z);
    void AddNormal(int index, float x, float y, float z);
    void AddColor(int index, float r, float g, float b, float a);
    void AddUv(int index, int layer, float u, float v);
    void AddTangent(int index, float x, float y, float z);
    void AddBinormal(int index, float x, float y, float z);
    void AddJointIndices(int index, float a, float b, float c, float d);
    void AddWeights(int index, float a, float b, float c, float d);
    void EndAddVertex();
    /// @}

private:
    /// begin vertex add
    bool begin_vadd;

};
//------------------------------------------------------------------------------
#endif
