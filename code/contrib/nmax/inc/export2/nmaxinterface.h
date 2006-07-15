//-----------------------------------------------------------------------------
//  nmaxinterface.h
//
//  (c)2004 Kim, Hyoun Woo.
//-----------------------------------------------------------------------------
#ifndef N_MAXINTERFACE_H
#define N_MAXINTERFACE_H
//-----------------------------------------------------------------------------
/**
    @class nMaxInterface
    @ingroup NebulaMaxExport2Contrib

    @brief A wrapper class of 3dsmax Interface class.

*/

//-----------------------------------------------------------------------------
class nMaxInterface
{
public:
    virtual ~nMaxInterface();

    static nMaxInterface* Instance();

    bool Create(Interface *intf);

    int GetCurrentFrame();

    TimeValue GetAnimStartTime() const;
    TimeValue GetAnimEndTime() const;
    int GetNumFrames() const;

    HWND GetHwnd();

    INode* GetRootNode();

    bool ProgressStart(TCHAR* title);
    void ProgressUpdate(int percent);
    bool CheckUserCancel();
    void ProgressEnd();

    Interface* GetInterface() const;

protected:
    Interface* maxInterface;

private:
    nMaxInterface();
    nMaxInterface(const nMaxInterface&) {};

    static nMaxInterface* Singleton;

};
//-----------------------------------------------------------------------------
inline
TimeValue 
nMaxInterface::GetAnimStartTime() const
{
    TimeValue animStart = this->maxInterface->GetAnimRange().Start();
    if (animStart > 0)
        animStart = 0;

    return animStart;
}
//-----------------------------------------------------------------------------
inline
TimeValue 
nMaxInterface::GetAnimEndTime() const
{
    return this->maxInterface->GetAnimRange().End();
}
//-----------------------------------------------------------------------------
inline
int nMaxInterface::GetNumFrames() const
{
    TimeValue animOffset;
    animOffset = (this->GetAnimEndTime() - this->GetAnimStartTime());
    return ((animOffset / GetTicksPerFrame()) + 1);
}
//-----------------------------------------------------------------------------
inline
HWND nMaxInterface::GetHwnd()
{
    return (this->maxInterface->GetMAXHWnd());
}
//-----------------------------------------------------------------------------
inline
INode* nMaxInterface::GetRootNode()
{
    return this->maxInterface->GetRootNode();
}
//-----------------------------------------------------------------------------
inline
Interface* nMaxInterface::GetInterface() const
{
    return this->maxInterface;
}
//-----------------------------------------------------------------------------
#endif
