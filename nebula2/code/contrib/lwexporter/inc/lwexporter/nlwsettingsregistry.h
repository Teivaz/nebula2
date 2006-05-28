#ifndef N_LW_SETTINGS_REGISTRY_H
#define N_LW_SETTINGS_REGISTRY_H
//----------------------------------------------------------------------------
extern "C" 
{
#include <lwsdk/lwsurf.h>
}

#include "util/nstring.h"
#include "util/nkeyarray.h"
#include "util/nhashlist.h"
#include <map>

class nLWObjectExportSettings;
class nLWShaderExportSettings;

typedef std::map<LWSurfaceID, nLWShaderExportSettings*> nLWSurfaceShaderMap;

//----------------------------------------------------------------------------
/**
    @class nLWSettingsRegistry
    @brief Central registry for per-object export settings.

    The object export settings custom handlers add/remove themselves from
    the registry so it's possible to get a handler instance given a LWItemID.
*/
class nLWSettingsRegistry
{
public:
    static nLWSettingsRegistry* Instance();
    static void FreeInstance();
    ~nLWSettingsRegistry();

    // object export settings

    void Add(nLWObjectExportSettings*);
    void Remove(nLWObjectExportSettings*);
    void Update(LWItemID oldId, LWItemID newId);
    nLWObjectExportSettings* Get(LWItemID) const;

    // shader export settings

    //void Add(const nString& objFileName, const nString& surfName, 
    //         nLWShaderExportSettings*);
    void Add(LWSurfaceID, nLWShaderExportSettings*);
    //void Remove(const nString& objFileName, 
    //            const nString& surfName);
    void Remove(LWSurfaceID);
    //nLWShaderExportSettings* Get(const nString& objFileName, 
    //                             const nString& surfName) const;
    nLWShaderExportSettings* Get(LWSurfaceID surfId) const;

private:
    nLWSettingsRegistry();

    static nLWSettingsRegistry* singleton;
    
    nKeyArray<nLWObjectExportSettings*> objExpSettingsArray;
    //nHashList objFileNameList;
    nLWSurfaceShaderMap surfaceShaderMap;
};

//----------------------------------------------------------------------------
#endif // N_LW_SETTINGS_REGISTRY_H
