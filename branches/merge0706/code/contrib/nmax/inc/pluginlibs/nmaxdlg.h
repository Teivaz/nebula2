#ifndef N_MAXDLG_H
#define N_MAXDLG_H

//-----------------------------------------------------------------------------
/**
    @class nMaxDlg
    @ingroup NebulaMaxExport2Contrib

    @brief Base class of plugin dialogs.
*/

//-----------------------------------------------------------------------------
class nMaxDlg
{
public:
    nMaxDlg(WORD resID, HINSTANCE hInst, HWND hwndParent);
    virtual ~nMaxDlg();
  
    void Create();
    void Destroy();

    virtual int DoModal();

    void SetHWnd(HWND hwnd);
    HWND GetHWnd() const;

protected:
    nMaxDlg() {};
    nMaxDlg(const nMaxDlg&) {};

    virtual void OnInitDialog();
    virtual void OnClose();
    virtual void OnDestroy();

    virtual BOOL OnCommand(int wParamLow, int wParamHigh, long lParam);
    virtual BOOL OnMessage(unsigned int uiMessage, int wParamLow, int wParamHigh, long lParam);

    static BOOL CALLBACK DlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

protected:
    WORD resourceID;
    HINSTANCE hInstance;
    HWND hWndParent;
    HWND hWnd;

};
//-----------------------------------------------------------------------------
inline
void nMaxDlg::SetHWnd(HWND hwnd)
{
    this->hWnd = hwnd;
}
//-----------------------------------------------------------------------------
inline
HWND nMaxDlg::GetHWnd() const
{
    return this->hWnd;
}
//-----------------------------------------------------------------------------
#endif
