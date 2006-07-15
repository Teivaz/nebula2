#ifndef N_VSDEBUGGER_H
#define N_VSDEBUGGER_H

#define ADDIN_API __declspec(dllexport)

extern "C" ADDIN_API
HRESULT WINAPI
AddIn_nString(DWORD, DEBUGHELPER*, int, BOOL, char*, size_t, DWORD);

#endif /* N_VSDEBUGGER_H */
