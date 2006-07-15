//-------------------------------------------------------------------
//  nvsdebugger.cc
//  (C) 2004 Bruce Mitchener, Jr.
//-------------------------------------------------------------------
#include "vsaddin/vsaddin.h"
#include "vsaddin/nvsdebugger.h"

#define private public
#define protected public
#include "util/nstring.h"

static bool
GetMemory(DEBUGHELPER* pHelper, void* address, DWORD bytesWanted, VOID* pWhere)
{
    DWORD bytesCopied;
    HRESULT hr;
    if ((pHelper->dwVersion < 0x20000) || (sizeof(void*) == sizeof(DWORD)))
    {
        // VC6 and 32 bit code path
        if (address == (void*)0xccCCccCC)
        {
            // Uninitialized stack
            return false;
        }
        hr = pHelper->ReadDebuggeeMemory(pHelper, (DWORD)address, bytesWanted,
                                         pWhere, &bytesCopied);
    }
    else
    {
        // VC7 and later only, for 64 bit pointers
        if (address == (void*)0xccCCccCCccCCccCC)
        {
            // Uninitialized stack
            return false;
        }
        hr = pHelper->ReadDebuggeeMemoryEx(pHelper, (DWORDLONG)address,
                                           bytesWanted, pWhere, &bytesCopied);
    }
    if (hr != S_OK)
    {
        return false;
    }

    if (bytesCopied != bytesWanted)
    {
        return false;
    }
    return true;
}

static void*
GetRealAddress(DEBUGHELPER* pHelper, DWORD dwAddress)
{
    void* realAddress;
    if ((pHelper->dwVersion < 0x20000) || sizeof(void*) == sizeof(DWORD))
    {
        // VC6 and 32 bit code path
        realAddress = (void*)dwAddress;
    }
    else
    {
        realAddress = (void*)pHelper->GetRealAddress(pHelper);
    }
    return realAddress;
}

static bool
BridgeStringObject(DEBUGHELPER* pHelper, void* realAddress,
                   nString* stringObject)
{
    // Read the main body of the nString
    if (false == GetMemory(pHelper, realAddress, sizeof(nString), stringObject))
    {
        return false;
    }
    // Does it use an allocated string buffer?
    if (NULL != stringObject->string)
    {
        // If so, we'll need to bridge that as well
        char* stringData = (char*)n_malloc(stringObject->strLen + 1);
        stringData[0] = '\0';
        if (false == GetMemory(pHelper, (void*)(stringObject->string),
                               stringObject->strLen + 1, stringData))
        {
            return false;
        }
        stringObject->string = stringData;
    }
    return true;
}

HRESULT WINAPI
AddIn_nString(DWORD dwAddress, DEBUGHELPER* pHelper,
              int /*nBase*/, BOOL /*bUniStrings*/,
              char* pResult, size_t max,
              DWORD /*reserved*/)
{
    *pResult = '\0';
    __try
    {
        // We can't use a stack allocated nString here due to the use of SEH
        char stringBuffer[sizeof(nString)];
        nString* string = (nString*)stringBuffer;
        void* realAddress = GetRealAddress(pHelper, dwAddress);
        if (false == BridgeStringObject(pHelper, realAddress, string))
        {
            return E_FAIL;
        }
        snprintf(pResult, max, "%s", string->Get());
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        return E_FAIL;
    }
    return S_OK;
}

