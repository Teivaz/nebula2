#ifndef N_MESHBUILDER_H
#define N_MESHBUILDER_H
//------------------------------------------------------------------------------
/**
    Nebula mesh builder helper class.
    
    (C) 2002 RadonLabs GmbH
*/
#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifndef N_ARRAY_H
#include "util/narray.h"
#endif

#ifndef N_MATRIX_H
#include "mathlib/matrix.h"
#endif

#ifndef N_VECTOR_H
#include "mathlib/vector.h"
#endif

#ifndef N_STRING_H
#include "util/nstring.h"
#endif

#ifndef N_PLANE_H
#include "mathlib/plane.h"
#endif

#ifndef N_BBOX_H
#include "mathlib/bbox.h"
#endif

class nFileServer2;

//------------------------------------------------------------------------------
class nMeshBuilder
{
public:
    //--- a mesh vertex ---
    class Vertex
    {
    public:
        /// vertex components
        enum Component
        {
            COORD    = (1<<0),
            NORMAL   = (1<<1),
            TANGENT  = (1<<2),
            BINORMAL = (1<<3),
            COLOR    = (1<<4),
            UV0      = (1<<5),
            UV1      = (1<<6),
            UV2      = (1<<7),
            UV3      = (1<<8),
            WEIGHTS  = (1<<9),
            JINDICES = (1<<10), 

            NUM_VERTEX_COMPONENTS = 11,
        };
        enum Flag
        {
            REDUNDANT = (1<<0),
        };

        enum
        {
            MAX_TEXTURE_LAYERS = 4,
        };

        /// constructor
        Vertex();
        /// set coord
        void SetCoord(const vector3& v);
        /// get coord
        const vector3& GetCoord() const;
        /// set normal
        void SetNormal(const vector3& v);
        /// get normal
        const vector3& GetNormal() const;
        /// set tangent
        void SetTangent(const vector3& v);
        /// get tangent
        const vector3& GetTangent() const;
        /// set binormal
        void SetBinormal(const vector3& v);
        /// get binormal
        const vector3& GetBinormal() const;
        /// set color
        void SetColor(const vector4& v);
        /// get color
        const vector4& GetColor() const;
        /// set uv
        void SetUv(int layer, const vector2& v);
        /// get uv
        const vector2& GetUv(int layer) const;
        /// set joint weights
        void SetWeights(const vector4& w);
        /// get joint weights
        const vector4& GetWeights() const;
        /// set joint indices
        void SetJointIndices(const vector4& w);
        /// get joint indices
        const vector4& GetJointIndices() const;
        /// check if component exists
        bool HasComponent(Component c) const;
        /// initialize a component with zero
        void ZeroComponent(Component c);
        /// delete a component
        void DelComponent(Component c);
        /// get component mask
        int GetComponentMask() const;
        /// compare method, return -1, 0 or +1
        int Compare(const Vertex& rhs) const;
        /// equality operator
        bool operator==(const Vertex& rhs) const;
        /// set flag
        void SetFlag(Flag f);
        /// clear flag
        void UnsetFlag(Flag f);
        /// check flag
        bool CheckFlag(Flag f) const;
        /// get vertex width (number of valid floats in vertex)
        int GetWidth() const;
        /// transform the vertex in place
        void Transform(const matrix44& m44, const matrix33& m33);
        /// fill vertex data with interpolated result (ignores weights and joint indices!)
        void Interpolate(const Vertex& v0, const Vertex& v1, float lerp);
        /// component-wise copy from source vertex
        void ComponentCopy(const Vertex& src, int compMask);

        vector3 coord;
        vector3 normal;
        vector3 tangent;
        vector3 binormal;
        vector4 color;
        vector2 uv[MAX_TEXTURE_LAYERS];
        vector4 weights;
        vector4 jointIndices;

        int compMask;       // component mask
        int flags;
    };

    //--- a mesh triangle ---
    class Triangle
    {
    public:
        /// triangle components
        enum Component
        {
            VERTEXINDICES    = (1<<0),
            GROUPID          = (1<<1),
            NORMAL           = (1<<2),
            TANGENT          = (1<<3),
            BINORMAL         = (1<<4),
        };

        /// constructor
        Triangle();
        /// destructor
        ~Triangle();

        /// set vertex indices
        void SetVertexIndices(int i0, int i1, int i2);
        /// get vertex indices
        void GetVertexIndices(int& i0, int& i1, int& i2) const;
        /// set group index
        void SetGroupId(int i);
        /// get group index
        int GetGroupId() const;
        /// check if component is valid
        bool HasComponent(Component c) const;
        /// set triangle normal
        void SetNormal(const vector3& v);
        /// get triangle normal
        const vector3& GetNormal() const;
        /// set triangle tangent
        void SetTangent(const vector3& v);
        /// get triangle tangent
        const vector3& GetTangent() const;
        /// set triangle binormal
        void SetBinormal(const vector3& v);
        /// get triangle binormal
        const vector3& GetBinormal() const;

        int vertexIndex[3];
        int groupId;
        vector3 normal;
        vector3 tangent;
        vector3 binormal;
        int compMask;
    };

    //--- a triangle group ---
    class Group
    {
    public:
        /// default constructor
        Group();
        /// set group id
        void SetId(int i);
        /// get group id
        int GetId() const;
        /// set first triangle index (optional)
        void SetFirstTriangle(int i);
        /// get first triangle index
        int GetFirstTriangle() const;
        /// set number of triangles
        void SetNumTriangles(int i);
        /// get number of triangles
        int GetNumTriangles() const;
    private:
        int id;
        int firstTriangle;
        int numTriangles;
    };

    /// constructor
    nMeshBuilder();
    /// destructor
    ~nMeshBuilder();
    
    //--- loading / saving ---

    /// save to nvx2 file
    bool SaveNvx2(nFileServer2* fileServer, const char* filename);
    /// load from nvx2 file
    bool LoadNvx2(nFileServer2* fileServer, const char* filename);
    /// save to n3d2 file
    bool SaveN3d2(nFileServer2* fileServer, const char* filename);
    /// load from n3d2 file
    bool LoadN3d2(nFileServer2* fileServer, const char* filename);
    /// load from (old-style) n3d file
    bool LoadN3d(nFileServer2* fileServer, const char* filename);
    /// load any of the above (use file extension for format decision)
    bool Load(nFileServer2* fileServer, const char* filename);
    /// save any of the above (use file extension for format decision)
    bool Save(nFileServer2* fileServer, const char* filename);

    //--- mesh building ---
    /// check if vertex component exists
    bool HasVertexComponent(Vertex::Component c) const;
    /// add a vertex
    void AddVertex(const Vertex& v);
    /// add a triangle
    void AddTriangle(const Triangle& t);
    /// get number of triangles
    int GetNumTriangles() const;
    /// get triangle at index
    Triangle& GetTriangleAt(int index) const;
    /// get num vertices
    int GetNumVertices() const;
    /// get vertex at index
    Vertex& GetVertexAt(int index) const;
    /// sort triangles by group id
    void SortTrianglesByGroupId();
    /// find the first triangle matching group id
    int GetFirstGroupTriangle(int groupId) const;
    /// count number of triangles matching group starting at index
    int GetNumGroupTriangles(int groupId, int startTriangleIndex) const;
    /// get the minimum vertex index referenced by a group
    bool GetGroupVertexRange(int groupId, int& minVertexIndex, int& maxVertexIndex) const; 
    /// build a group mapping array
    void BuildGroupMap(nArray<Group>& groupMap);
    /// update triangle group ids from a group map
    void UpdateTriangleGroupIds(const nArray<Group>& groupMap);

    //--- mesh operations ---

    /// erase/duplicate vertex components
    void ForceVertexComponents(int compMask);
    /// transform vertices
    void Transform(const matrix44& m);
    /// remove redundant vertices
    void Cleanup(nArray< nArray<int> >* collapsMap);
    /// build adjacency information (only works on clean meshes)
    void BuildAdjacency();
    /// optimize for t&l hardware vertex cache
    void Optimize();
    /// append mesh from mesh builder object
    void Append(const nMeshBuilder& source);
    /// copy from mesh builder object
    void Copy(const nMeshBuilder& source);
    /// compute the bounding box of the mesh, filtered by a group id
    bbox3 ComputeGroupBBox(int groupId) const;
    /// count vertices in bounding box
    int CountVerticesInBBox(const bbox3& box) const;
    /// split triangle group in place, using a clip plane, return 2 new group indices
    void Split(const plane& clipPlane, int groupId, int posGroupId, int negGroupId);
    /// fill a vertex-triangle mapping array
    void BuildVertexTriangleMap(nArray< nArray<int> >& vertexTriangleMap) const;
    /// create face normals, tangents and binormals (requires a valid uv-mapping at layer 0)
    void BuildTriangleNormals();
    /// generate averaged vertex tangents and binormals
    void BuildVertexTangentBinormals();

private:
    /// static userdata pointer for qsort hook
    static nMeshBuilder* nMeshBuilder::qsortData;
    /// a qsort() hook for generating a sorted index array
    static int __cdecl VertexSorter(const void* elm0, const void* elm1);
    /// qsort hook for sorting triangles by their group index
    static int __cdecl TriangleGroupSorter(const void* elm0, const void* elm1);
    /// do an inflated component copy using a source mesh and a collapsMap
    void InflateCopyComponents(const nMeshBuilder& src, const nArray< nArray<int> >& collapsMap, int compMask);

public:
    nArray<Vertex>   vertexArray;
    nArray<Triangle> triangleArray;
};

//------------------------------------------------------------------------------
/**
*/
inline
nMeshBuilder::Vertex::Vertex() :
    compMask(0),
    flags(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMeshBuilder::Vertex::SetCoord(const vector3& v)
{
    this->coord = v;
    this->compMask |= COORD;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector3&
nMeshBuilder::Vertex::GetCoord() const
{
    return this->coord;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMeshBuilder::Vertex::SetNormal(const vector3& v)
{
    this->normal = v;
    this->compMask |= NORMAL;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector3&
nMeshBuilder::Vertex::GetNormal() const
{
    return this->normal;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMeshBuilder::Vertex::SetTangent(const vector3& v)
{
    this->tangent = v;
    this->compMask |= TANGENT;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector3&
nMeshBuilder::Vertex::GetTangent() const
{
    return this->tangent;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMeshBuilder::Vertex::SetBinormal(const vector3& v)
{
    this->binormal = v;
    this->compMask |= BINORMAL;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector3&
nMeshBuilder::Vertex::GetBinormal() const
{
    return this->binormal;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMeshBuilder::Vertex::SetColor(const vector4& v)
{
    this->color = v;
    this->compMask |= COLOR;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector4&
nMeshBuilder::Vertex::GetColor() const
{
    return this->color;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMeshBuilder::Vertex::SetUv(int layer, const vector2& v)
{
    n_assert((layer >= 0) && (layer < Vertex::MAX_TEXTURE_LAYERS));
    this->uv[layer] = v;
    switch (layer)
    {
        case 0: this->compMask |= UV0; break;
        case 1: this->compMask |= UV1; break;
        case 2: this->compMask |= UV2; break;
        case 3: this->compMask |= UV3; break;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector2&
nMeshBuilder::Vertex::GetUv(int layer) const
{
    n_assert((layer >= 0) && (layer < MAX_TEXTURE_LAYERS));
    return this->uv[layer];
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMeshBuilder::Vertex::SetWeights(const vector4& w)
{
    this->weights = w;
    this->compMask |= WEIGHTS;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector4&
nMeshBuilder::Vertex::GetWeights() const
{
    return this->weights;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMeshBuilder::Vertex::SetJointIndices(const vector4& ji)
{
    this->jointIndices = ji;
    this->compMask |= JINDICES;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector4&
nMeshBuilder::Vertex::GetJointIndices() const
{
    return this->jointIndices;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nMeshBuilder::Vertex::HasComponent(Component c) const
{
    return ((this->compMask & c) == c);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMeshBuilder::Vertex::DelComponent(Component c)
{
    this->compMask &= ~c;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMeshBuilder::Vertex::ZeroComponent(Component c)
{
    this->compMask |= c;
    switch (c)
    {
        case COORD:     this->coord.set(0.0f, 0.0f, 0.0f); break;
        case NORMAL:    this->normal.set(0.0f, 0.0f, 0.0f); break;
        case TANGENT:   this->tangent.set(0.0f, 0.0f, 0.0f); break;
        case BINORMAL:  this->tangent.set(0.0f, 0.0f, 0.0f); break;
        case COLOR:     this->color.set(0.0f, 0.0f, 0.0f, 0.0f); break;
        case UV0:       this->uv[0].set(0.0f, 0.0f); break;
        case UV1:       this->uv[1].set(0.0f, 0.0f); break;
        case UV2:       this->uv[2].set(0.0f, 0.0f); break;
        case UV3:       this->uv[3].set(0.0f, 0.0f); break;
        case WEIGHTS:   this->weights.set(0.0f, 0.0f, 0.0f, 0.0f); break;
        case JINDICES:  this->jointIndices.set(0.0f, 0.0f, 0.0f, 0.0f); break;
        default:        n_error("INVALID VERTEX COMPONENT!"); break;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nMeshBuilder::Vertex::GetComponentMask() const
{
    return this->compMask;
}

//------------------------------------------------------------------------------
/**
    This returns a value suitable for sorting, -1 if the rhs is 'smaller',
    0 if rhs is equal, and +1 if rhs is 'greater'.
*/
inline
int
nMeshBuilder::Vertex::Compare(const Vertex& rhs) const
{
    if (this->HasComponent(COORD) && rhs.HasComponent(COORD))
    {
        int res = this->coord.compare(rhs.coord, 0.001f);
        if (0 != res)
        {
            return res;
        }
    }
    if (this->HasComponent(NORMAL) && rhs.HasComponent(NORMAL))
    {
        int res = this->normal.compare(rhs.normal, 0.001f);
        if (0 != res)
        {
            return res;
        }
    }
    if (this->HasComponent(TANGENT) && rhs.HasComponent(TANGENT))
    {
        int res = this->tangent.compare(rhs.tangent, 0.001f);
        if (0 != res)
        {
            return res;
        }
    }
    if (this->HasComponent(BINORMAL) && rhs.HasComponent(BINORMAL))
    {
        int res = this->binormal.compare(rhs.binormal, 0.001f);
        if (0 != res)
        {
            return res;
        }
    }
    if (this->HasComponent(COLOR) && rhs.HasComponent(COLOR))
    {
        int res = this->color.compare(rhs.color, 0.001f);
        if (0 != res)
        {
            return res;
        }
    }
    if (this->HasComponent(UV0) && rhs.HasComponent(UV0))
    {
        int res = this->uv[0].compare(rhs.uv[0], 0.000001f);
        if (0 != res)
        {
            return res;
        }
    }
    if (this->HasComponent(UV1) && rhs.HasComponent(UV1))
    {
        int res = this->uv[1].compare(rhs.uv[1], 0.000001f);
        if (0 != res)
        {
            return res;
        }
    }
    if (this->HasComponent(UV2) && rhs.HasComponent(UV2))
    {
        int res = this->uv[2].compare(rhs.uv[2], 0.000001f);
        if (0 != res)
        {
            return res;
        }
    }
    if (this->HasComponent(UV3) && rhs.HasComponent(UV3))
    {
        int res = this->uv[3].compare(rhs.uv[3], 0.000001f);
        if (0 != res)
        {
            return res;
        }
    }
    if (this->HasComponent(WEIGHTS) && rhs.HasComponent(WEIGHTS))
    {
        int res = this->weights.compare(rhs.weights, 0.00001f);
        if (0 != res)
        {
            return res;
        }
    }
    if (this->HasComponent(JINDICES) && rhs.HasComponent(JINDICES))
    {
        int res = this->jointIndices.compare(rhs.jointIndices, 0.5f);
        if (0 != res)
        {
            return res;
        }
    }

    // fallthrough: all equal
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nMeshBuilder::Vertex::operator==(const Vertex& rhs) const
{
    return (this->Compare(rhs) == 0);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMeshBuilder::Vertex::SetFlag(Flag f)
{
    this->flags |= f;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMeshBuilder::Vertex::UnsetFlag(Flag f)
{
    this->flags &= ~f;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nMeshBuilder::Vertex::CheckFlag(Flag f) const
{
    return (this->flags & f) == f;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nMeshBuilder::Vertex::GetWidth() const
{
    int w = 0;
    if (this->HasComponent(COORD))      w += 3;
    if (this->HasComponent(NORMAL))     w += 3;
    if (this->HasComponent(TANGENT))    w += 3;
    if (this->HasComponent(BINORMAL))   w += 3;
    if (this->HasComponent(COLOR))      w += 4;
    if (this->HasComponent(UV0))        w += 2;
    if (this->HasComponent(UV1))        w += 2;
    if (this->HasComponent(UV2))        w += 2;
    if (this->HasComponent(UV2))        w += 2;
    if (this->HasComponent(WEIGHTS))    w += 4;
    if (this->HasComponent(JINDICES))   w += 4;
    return w;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMeshBuilder::Vertex::Transform(const matrix44& m44, const matrix33& m33)
{
    if (this->HasComponent(COORD))
    {
        this->coord = m44 * this->coord;
    }
    if (this->HasComponent(NORMAL))
    {
        this->normal = m33 * this->normal;
    }
    if (this->HasComponent(TANGENT))
    {
        this->tangent = m33 * this->tangent;
    }
    if (this->HasComponent(BINORMAL))
    {
        this->binormal = m33 * this->binormal;
    }
}

//------------------------------------------------------------------------------
/**
    Fills the vertex with the interpolated result of the parameters.
    This ignores weights and joint indices. This method is useful
    for clipping.
*/
inline
void
nMeshBuilder::Vertex::Interpolate(const Vertex& v0, const Vertex& v1, float lerp)
{
    if (v0.HasComponent(COORD))
    {
        this->SetCoord(v0.coord + ((v1.coord - v0.coord) * lerp));
    }
    if (v0.HasComponent(NORMAL))
    {
        this->SetNormal(v0.normal + ((v1.normal - v0.normal) * lerp));
        this->normal.norm();
    }
    if (v0.HasComponent(TANGENT))
    {
        this->SetTangent(v0.tangent + ((v1.tangent - v0.tangent) * lerp));
        this->tangent.norm();
    }
    if (v0.HasComponent(BINORMAL))
    {
        this->SetBinormal(v0.binormal + ((v1.binormal - v0.binormal) * lerp));
        this->binormal.norm();
    }
    if (v0.HasComponent(COLOR))
    {
        this->SetColor(v0.color + ((v1.color - v0.color) * lerp));
    }
    if (v0.HasComponent(UV0))
    {
        this->SetUv(0, v0.uv[0] + ((v1.uv[0] - v0.uv[0]) * lerp));
    }
    if (v0.HasComponent(UV1))
    {
        this->SetUv(1, v0.uv[1] + ((v1.uv[1] - v0.uv[1]) * lerp));
    }
    if (v0.HasComponent(UV2))
    {
        this->SetUv(2, v0.uv[2] + ((v1.uv[2] - v0.uv[2]) * lerp));
    }
    if (v0.HasComponent(UV3))
    {
        this->SetUv(3, v0.uv[3] + ((v1.uv[3] - v0.uv[3]) * lerp));
    }
}

//------------------------------------------------------------------------------
/**
    Copy vertex component defined by component mask from source vertex.

    @param  src     the source vertex
    @param  mask    component mask which defines which components to copy
*/
inline
void
nMeshBuilder::Vertex::ComponentCopy(const Vertex& src, int mask)
{
    if (mask & Vertex::COORD)       this->SetCoord(src.coord);
    if (mask & Vertex::NORMAL)      this->SetNormal(src.normal);
    if (mask & Vertex::TANGENT)     this->SetTangent(src.tangent);
    if (mask & Vertex::BINORMAL)    this->SetBinormal(src.binormal);
    if (mask & Vertex::COLOR)       this->SetColor(src.color);
    if (mask & Vertex::UV0)         this->SetUv(0, src.uv[0]);
    if (mask & Vertex::UV1)         this->SetUv(1, src.uv[1]);
    if (mask & Vertex::UV2)         this->SetUv(2, src.uv[2]);
    if (mask & Vertex::UV3)         this->SetUv(3, src.uv[3]);
    if (mask & Vertex::WEIGHTS)     this->SetWeights(src.weights);
    if (mask & Vertex::JINDICES)    this->SetJointIndices(src.jointIndices);
}

//------------------------------------------------------------------------------
/**
*/
inline
nMeshBuilder::Triangle::Triangle() :
    compMask(0)
{
    int i;
    for (i = 0; i < 3; i++)
    {
        this->vertexIndex[i] = 0;
    }
    this->groupId = 0;
}

//------------------------------------------------------------------------------
/**
*/
inline
nMeshBuilder::Triangle::~Triangle()
{
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMeshBuilder::Triangle::SetVertexIndices(int i0, int i1, int i2)
{
    this->vertexIndex[0] = i0;
    this->vertexIndex[1] = i1;
    this->vertexIndex[2] = i2;
    this->compMask |= VERTEXINDICES;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMeshBuilder::Triangle::GetVertexIndices(int& i0, int& i1, int& i2) const
{
    i0 = this->vertexIndex[0];
    i1 = this->vertexIndex[1];
    i2 = this->vertexIndex[2];
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMeshBuilder::Triangle::SetGroupId(int i)
{
    this->groupId = i;
    this->compMask |= GROUPID;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nMeshBuilder::Triangle::GetGroupId() const
{
    return this->groupId;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nMeshBuilder::Triangle::HasComponent(Component c) const
{
    return ((this->compMask & c) == c);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMeshBuilder::Triangle::SetNormal(const vector3& v)
{
    this->normal = v;
    this->compMask |= NORMAL;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector3&
nMeshBuilder::Triangle::GetNormal() const
{
    n_assert(NORMAL & this->compMask);
    return this->normal;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMeshBuilder::Triangle::SetTangent(const vector3& v)
{
    this->tangent = v;
    this->compMask |= TANGENT;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector3&
nMeshBuilder::Triangle::GetTangent() const
{
    n_assert(TANGENT & this->compMask);
    return this->tangent;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMeshBuilder::Triangle::SetBinormal(const vector3& v)
{
    this->binormal = v;
    this->compMask |= BINORMAL;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector3&
nMeshBuilder::Triangle::GetBinormal() const
{
    n_assert(BINORMAL & this->compMask);
    return this->binormal;
}

//------------------------------------------------------------------------------
/**
*/
inline
nMeshBuilder::Group::Group() :
    id(0),
    firstTriangle(0),
    numTriangles(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMeshBuilder::Group::SetId(int i)
{
    this->id = i;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nMeshBuilder::Group::GetId() const
{
    return this->id;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMeshBuilder::Group::SetFirstTriangle(int i)
{
    this->firstTriangle = i;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nMeshBuilder::Group::GetFirstTriangle() const
{
    return this->firstTriangle;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMeshBuilder::Group::SetNumTriangles(int i)
{
    this->numTriangles = i;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nMeshBuilder::Group::GetNumTriangles() const
{
    return this->numTriangles;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nMeshBuilder::HasVertexComponent(Vertex::Component c) const
{
    if (this->GetNumVertices() > 0)
    {
        return this->GetVertexAt(0).HasComponent(c);
    }
    else
    {
        return false;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMeshBuilder::AddVertex(const Vertex& v)
{
    this->vertexArray.PushBack(v);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMeshBuilder::AddTriangle(const Triangle& t)
{
    this->triangleArray.PushBack(t);
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nMeshBuilder::GetNumVertices() const
{
    return this->vertexArray.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline
nMeshBuilder::Vertex&
nMeshBuilder::GetVertexAt(int index) const
{
    return this->vertexArray[index];
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nMeshBuilder::GetNumTriangles() const
{
    return this->triangleArray.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline
nMeshBuilder::Triangle&
nMeshBuilder::GetTriangleAt(int index) const
{
    return this->triangleArray[index];
}

//------------------------------------------------------------------------------
#endif

