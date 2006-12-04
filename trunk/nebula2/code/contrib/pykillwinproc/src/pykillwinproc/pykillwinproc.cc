//--------------------------------------------------------------------
//  This is an extension module for Python that provides functions
//  to kill processes on a Windows machine.
//
//  Unfortunately there is no easy way in Python to kill a process
//  and it's children on a Windows machine (os.kill() is simply
//  missing on Windows). Using the win32api extensions one is able
//  to kill a process but not it's children (at least not without
//  resorting to using the Performance Data Helpers API and doing
//  unspeakable things).
//
//  (c) 2005    Vadim Macagon
//
//  Contents are licensed under the terms of the Nebula License.
//--------------------------------------------------------------------

#ifdef _DEBUG
#undef _DEBUG
#include <Python.h>
#define _DEBUG 1
#else
#include <Python.h>
#endif

#include <windows.h>
#include <tlhelp32.h>

//------------------------------------------------------------------------------
/**
    Kills a process but not it's children.
*/
static void KillProcess(int pid)
{
    HANDLE hProc = OpenProcess(SYNCHRONIZE|PROCESS_TERMINATE, false, pid);
    if (hProc)
    {
        TerminateProcess(hProc, -1);
        CloseHandle(hProc);
    }
}

//------------------------------------------------------------------------------
/**
    Kills a process but not it's children.
*/
static PyObject* pyKillWinProc_KillProcess(PyObject* self, PyObject* args)
{
    int pid = 0;

    if (!PyArg_ParseTuple(args, "i", &pid))
        return NULL;

    if (pid)
        KillProcess(pid);

    Py_INCREF(Py_None);
    return Py_None;
}

//------------------------------------------------------------------------------
/**
    Kills a process and its immediate children (but not the children's children).
*/
static PyObject* pyKillWinProc_KillProcessTree(PyObject* self, PyObject* args)
{
    int pid = 0;

    if (!PyArg_ParseTuple(args, "i", &pid))
        return NULL;

    if (pid)
    {
        HANDLE hProcSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (INVALID_HANDLE_VALUE == hProcSnapshot)
        {
            PyErr_SetString(PyExc_Exception, "Failed to create process snapshot.");
            return NULL;
        }

        KillProcess(pid);

        PROCESSENTRY32 pe32;
        // set the size of the structure before using it.
        pe32.dwSize = sizeof(PROCESSENTRY32);

        // kill all the children of the process
        if (Process32First(hProcSnapshot, &pe32))
        {
            do
            {
                if (pe32.th32ParentProcessID == pid)
                    KillProcess(pe32.th32ProcessID);
            } while (Process32Next(hProcSnapshot, &pe32));
        }
        CloseHandle(hProcSnapshot);
    }

    Py_INCREF(Py_None);
    return Py_None;
}

//------------------------------------------------------------------------------
// method table
static PyMethodDef PyKillWinProcMethods[] = {
    {"kill_process", pyKillWinProc_KillProcess, METH_VARARGS,
     "kill_process(pid) \npid - process identifier of the process to kill. \nKill a Win32 Process but not its children."},
    {"kill_process_tree", pyKillWinProc_KillProcessTree, METH_VARARGS,
     "kill_process_tree(pid) \npid - process identifier of the process to kill. \nKill a Win32 Process and its immediate children."},
    {NULL, NULL, 0, NULL} /* Sentinel */
};

//------------------------------------------------------------------------------
// module initialisation
extern "C"
__declspec(dllexport)
void initpykillwinproc()
{
    Py_InitModule("pykillwinproc", PyKillWinProcMethods);
}
