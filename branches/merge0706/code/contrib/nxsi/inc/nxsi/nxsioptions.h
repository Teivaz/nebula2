//-----------------------------------------------------------------------------
// Copyright (c) Ville Ruusutie, 2004. All Rights Reserved.
//-----------------------------------------------------------------------------
// See the file "nxsi_license.txt" for information on usage and redistribution
// of this file, and for a DISCLAIMER OF ALL WARRANTIES.
//-----------------------------------------------------------------------------
#ifndef N_XSI_OPTIONS_H
#define N_XSI_OPTIONS_H
//-----------------------------------------------------------------------------
#include <cassert>
//-----------------------------------------------------------------------------

class nXSIOptions
{
public:
    enum OutputFlags {
        OUTPUT_MESH      = (1<<1),
        OUTPUT_ANIM      = (1<<2),
        OUTPUT_SCRIPT    = (1<<3),
        OUTPUT_AUTO      = (1<<4),
        OUTPUT_MERGEALL  = (1<<5),
        OUTPUT_BINARY    = (1<<6),
    };

    ~nXSIOptions();
    nXSIOptions();

    void Parse(int argc, char* argv[]);
    void Usage() const;

    inline const nString& GetXSIFilename() const;
    inline const nString& GetMeshFilename() const;
    inline const nString& GetAnimFilename() const;
    inline const nString& GetScriptFilename() const;
    inline const nString& GetScriptServerName() const;
    inline int GetOutputFlags() const;

private:
    nString xsiFilename;        ///< .xsi
    nString meshFilename;       ///< .n3d2 or .nvx2
    nString animFilename;       ///< .nanim2 or .nax2
    nString scriptFilename;     ///< main script (.n2) filename

    nString scriptServerName;       ///< script server name (default: ntclserver)

    int outputFlags;
};

//-----------------------------------------------------------------------------

inline const nString& nXSIOptions::GetXSIFilename() const
{
    return this->xsiFilename;
}

inline const nString& nXSIOptions::GetMeshFilename() const
{
    return this->meshFilename;
}

inline const nString& nXSIOptions::GetAnimFilename() const
{
    return this->animFilename;
}

inline const nString& nXSIOptions::GetScriptFilename() const
{
    return this->scriptFilename;
}

inline const nString& nXSIOptions::GetScriptServerName() const
{
    return this->scriptServerName;
}

inline int nXSIOptions::GetOutputFlags() const
{
    return this->outputFlags;
}

//-----------------------------------------------------------------------------
#endif // Eof