//------------------------------------------------------------------------------
//  nmaxoptionparammapdlg.h
//
//  (c)2004 Kim, Hyoun Woo.
//------------------------------------------------------------------------------
#ifndef N_MAXOPTIONPARAMMAPDLG_H
#define N_MAXOPTIONPARAMMAPDLG_H
//------------------------------------------------------------------------------
/**
    @class nMaxOptionParamMapDlg
    @ingroup NebulaMaxExport2Contrib

    @brief Nebula export option dialog in utility pannel.
*/
#include "export2/nmax.h"
#include <iparamm.h>

//------------------------------------------------------------------------------
class nMaxOptionParamMapDlg : public ParamMapUserDlgProc
{
public:
    // supported script server types.
    enum {
        Tcl    = 0,
        Python = 1,
        Lua    = 2,
        Ruby   = 3,
        NumScriptServers = 4,
    };

    nMaxOptionParamMapDlg();
    virtual ~nMaxOptionParamMapDlg();

    // @name 3dsmax functions which should be defined.
    // @{
    BOOL DlgProc(TimeValue t, IParamMap *map, HWND hWnd, UINT msg,
                 WPARAM wParam, LPARAM lParam);
    void DeleteThis() { delete this; }
    // @}

private:
    void InitDialog(HWND hwnd);
    void OnCommand(HWND hwnd, WORD highParam, WORD lowParam);

    bool IsChecked(HWND hwnd, int id);

    void OnVertexComponent(HWND hwnd);

    void OnMeshFileType(HWND hwnd);
    void OnAnimFileType(HWND hwnd);

    void OnHiddenObject(HWND hwnd);

    void OnSelectedScriptServer(HWND hwnd);
    const char* GetScriptServer(const char* item);

private:
    /// spinner control for setting of geometry scaling.
    ISpinnerControl* spinGeomScale;
    /// spinner control for setting of maximum joint palette.
    ISpinnerControl* spinMaxJointPalette;
    /// spinner control for setting of weight trash hold.
    ISpinnerControl* spinWeightTrashold;
    /// spinner control for setting global animation sample rate.
    ISpinnerControl* spinSampleRate;


};
//------------------------------------------------------------------------------
#endif
