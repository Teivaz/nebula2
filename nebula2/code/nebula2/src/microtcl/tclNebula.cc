//------------------------------------------------------------------------------
//  tclNebula.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "kernel/nfileserver2.h"
#include "kernel/nfile.h"

#include "microtcl/tclInt.h"
#include "microtcl/tclPort.h"

/*
 *----------------------------------------------------------------------
 *
 * Tcl_EvalFile --
 *
 *	Read in a file and process the entire file as one gigantic
 *	Tcl command.
 *
 * Results:
 *	A standard Tcl result, which is either the result of executing
 *	the file or an error indicating why the file couldn't be read.
 *
 * Side effects:
 *	Depends on the commands in the file.
 *
 * - 20-Feb-04  floh    get script buffer size from file size
 * - 19-Mar-04  floh    important bugfix: script buffer must be 1 byte
 *                      bigger than filesize, otherwise, Tcl may
 *                      trigger a very subtle internal bug
 *
 *----------------------------------------------------------------------
 */

int
Tcl_EvalFile(Tcl_Interp* interp, char* fileName)
{
    Interp* iPtr = (Interp *) interp;

    // get fileobject
    n_assert(nKernelServer::ks);
    nFileServer2* fileServer = nKernelServer::Instance()->GetFileServer();
    n_assert(fileServer);

    int result = TCL_ERROR;

    // create a file object
    nFile* file = fileServer->NewFileObject();
    if (file->Open(fileName, "r"))
    {
        // get size of buffer to allocate
        const int fileSize = file->GetSize();
        char* scriptBuffer = (char*) n_malloc(fileSize + 1);
        n_assert(scriptBuffer);
        memset(scriptBuffer, 0, fileSize + 1);

        // read file into buffer
        int bytesRead = file->Read(scriptBuffer, fileSize);
        file->Close();
        file->Release();
        n_assert(bytesRead == fileSize);

        // evaluate contents
        result = Tcl_EvalEx(interp, scriptBuffer, fileSize, 0);

        // free buffer
        n_free(scriptBuffer);

        // handle result
        if (result == TCL_RETURN)
        {
	        result = TclUpdateReturnInfo(iPtr);
        }
        else if (result == TCL_ERROR)
        {
	        char msg[200 + TCL_INTEGER_SPACE];

	        /*
	         * Record information telling where the error occurred.
	         */
        	sprintf(msg, "\n    (file \"%.150s\" line %d)", fileName, interp->errorLine);
	        Tcl_AddErrorInfo(interp, msg);
        }
    }
    else
    {
        n_error("Tcl_Eval(): could not open tcl script '%s'\n", fileName);
    }
    return result;
}
