//------------------------------------------------------------------------------
//  ndshowserver2_main.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "video/ndshowcontroller.h"
#include "video/ndshowserver2.h"
#include "kernel/nfileserver2.h"
#include "gfx2/nd3d9server.h"

nNebulaClass(nDShowServer2, "nvideoserver2");

//------------------------------------------------------------------------------
/**
*/
nDShowServer2::nDShowServer2()
{
}

//------------------------------------------------------------------------------
/**
*/
nDShowServer2::~nDShowServer2()
{
}

//------------------------------------------------------------------------------
/**
*/
void
nDShowServer2::Open()
{
    n_assert(!IsOpen());

    HRESULT hr = CoInitialize(NULL);
    if (FAILED(hr))
    {
        n_printf("Failed to initialized COM!\n");
    }

    nVideoServer2::Open();
}

//------------------------------------------------------------------------------
/**
*/
void
nDShowServer2::Close()
{
    n_assert(IsOpen());

    CoUninitialize();

    nVideoServer2::Close();
}

//------------------------------------------------------------------------------
/**
*/
nVideoController*
nDShowServer2::MakeController(const nString& vn) const
{
    n_assert(vn != "");
    nDShowController* result = new nDShowController;
    n_assert(result != 0);
    result->SetVideoName(vn);

    return result;
}
