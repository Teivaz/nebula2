//-----------------------------------------------------------------------------
// Copyright (c) Ville Ruusutie, 2004. All Rights Reserved.
//-----------------------------------------------------------------------------
// See the file "nxsi_license.txt" for information on usage and redistribution
// of this file, and for a DISCLAIMER OF ALL WARRANTIES.
//-----------------------------------------------------------------------------
#ifndef N_XSI_OPTIONS_H
#define N_XSI_OPTIONS_H
//-----------------------------------------------------------------------------
#include <util/npathstring.h>
#include <cassert>
//-----------------------------------------------------------------------------

class nXSIOptions
{
public:
    enum OutputFlags {
        OUTPUT_MESH   = 1,
        OUTPUT_ANIM   = 2,
        OUTPUT_SCRIPT = 4,
        OUTPUT_SHADER = 8,
    };

    ~nXSIOptions();
    nXSIOptions();

    void Parse(int argc, char* argv[]);
    void Usage() const;

    inline const nPathString& GetXSIFilename() const;
    inline const nPathString& GetMeshFilename() const;
    inline const nPathString& GetAnimFilename() const;
    inline const nPathString& GetScriptFilename() const;
    inline const nPathString& GetShaderFilename() const;
    inline const nString& GetScriptServerName() const;
    inline bool IsOutputAutomatic() const;
    inline int GetOutputFlags() const;

private:
    bool MatchSwitch(const char* currentSwitch, const char* appSwitch) const;
    bool MatchSwitch(const char* currentSwitch, const char* appSwitch, const char* &argument) const;

private:
    nPathString xsiFilename;        ///< .xsi
    nPathString meshFilename;       ///< .n3d2
    nPathString animFilename;       ///< .nanim2
    nPathString shaderFilename;     ///< .fx
    nPathString scriptFilename;     ///< main script (.n2) filename

    nString scriptServerName;       ///< script server name (default: ntclserver)

    bool isOutputAutomatic;         ///< is output names generated automatically
    int outputFlags;
};

//-----------------------------------------------------------------------------

inline const nPathString& nXSIOptions::GetXSIFilename() const
{
    return this->xsiFilename;
}

inline const nPathString& nXSIOptions::GetMeshFilename() const
{
    return this->meshFilename;
}

inline const nPathString& nXSIOptions::GetAnimFilename() const
{
    return this->animFilename;
}

inline const nPathString& nXSIOptions::GetScriptFilename() const
{
    return this->scriptFilename;
}

inline const nPathString& nXSIOptions::GetShaderFilename() const
{
    return this->shaderFilename;
}

inline const nString& nXSIOptions::GetScriptServerName() const
{
    return this->scriptServerName;
}

inline bool nXSIOptions::IsOutputAutomatic() const
{
    return this->isOutputAutomatic;
}

inline int nXSIOptions::GetOutputFlags() const
{
    return this->outputFlags;
}

//-----------------------------------------------------------------------------
#endif // Eof