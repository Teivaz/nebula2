#ifndef N_LW_EXPORTER_SETTINGS_H
#define N_LW_EXPORTER_SETTINGS_H
//----------------------------------------------------------------------------
#include "util/nstring.h"
#include "util/narray.h"
#include "kernel/narg.h"

class nLWExportNodeType;

//----------------------------------------------------------------------------
/**
    @class nLWExporterSettings
    @brief Facilitates access to the exporter settings stored 
           in N2LightwaveExporter.xml.
*/
class nLWExporterSettings
{
public:
    static nLWExporterSettings* Instance();
    static void FreeInstance();
    ~nLWExporterSettings();

    /// load all settings from disk
    void Load();
    /// save all settings to disk
    void Save();

    // general settings

    nString GetProjectDir() const;
    void SetGeometryScale(float factor);
    float GetGeometryScale() const;
    void SetScriptServer(const nString&);
    nString GetScriptServer() const;
    void SetPreviewRenderer(const nString&);
    nString GetPreviewRenderer() const;
    void SetPreviewWidth(int);
    int GetPreviewWidth() const;
    void SetPreviewHeight(int);
    int GetPreviewHeight() const;
    void SetPreviewFullScreen(bool);
    bool GetPreviewFullScreen() const;
    void SetOutputBinarySceneNode(bool);
    bool GetOutputBinarySceneNode() const;
    void SetOutputBinaryMesh(bool);
    bool GetOutputBinaryMesh() const;
    void SetOutputBinaryAnimation(bool);
    bool GetOutputBinaryAnimation() const;
    
    // preview settings

    /// Get the total number of renderers available
    int GetNumPreviewRenderers() const;
    /// Get the name of the renderer that corresponds to the given index
    const nString& GetPreviewRendererName(int index) const;
    /// Get the feature set used by the renderer that corresponds to the given index
    const nString& GetPreviewRendererFeatureSet(int index) const;
    /// Get the feature set used by the renderer of the given name
    const nString& GetPreviewRendererFeatureSet(const nString&);
    /// Get the total number of sizes available
    int GetNumPreviewSizes() const;
    /// Get the preview size that corresponds to the given index
    const nString& GetPreviewSize(int index) const;

    // export node types

    int GetNumExportNodeTypes() const;
    nLWExportNodeType* GetExportNodeType(int index) const;
    nLWExportNodeType* GetExportNodeType(const nString& nodeType) const;
    void SetExportNodeTypes(nArray<nLWExportNodeType*>&);
    
    // scene node types

    int GetNumSceneNodeTypes() const;
    const nString& GetSceneNodeType(int index) const;

private:
    nLWExporterSettings();
    void SetDefaults();
    void ClearExportNodeTypes();

    static nLWExporterSettings* singleton;
    static const char SETTINGS_FILENAME[];

    nArray<nArg> settingsArray;
    nArray<nLWExportNodeType*> exportNodeTypesArray;
    nArray<nString> sceneNodeTypesArray;
    nArray<nString> rendererNameArray;
    nArray<nString> rendererFeatureSetArray;
    nArray<nString> previewSizeArray;
};

//----------------------------------------------------------------------------
#endif // N_LW_EXPORTER_SETTINGS_H
