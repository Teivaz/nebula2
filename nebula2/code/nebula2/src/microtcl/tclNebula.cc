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
 *
 *  NOTE
 *  This is a Nebula specific replacement file for the MicroTcl
 *  implementation, max size for a single script is 128 kByte!
 *
 *----------------------------------------------------------------------
 */

int
Tcl_EvalFile(Tcl_Interp* interp, char* fileName)
{
    Interp* iPtr = (Interp *) interp;

    // get fileobject
    n_assert(nKernelServer::ks);
    nFileServer2* fileServer = nKernelServer::ks->GetFileServer();
    n_assert(fileServer);

    int result = TCL_ERROR;

    // create a file object
    nFile* file = fileServer->NewFileObject();
    if (file->Open(fileName, "r"))
    {
        // allocate a 512 kByte buffer 
        const int bufSize = (1<<17);
        char* buffer = (char*) n_malloc(bufSize);
        n_assert(buffer);

        // read file into buffer
        int bytesRead = file->Read(buffer, bufSize);
        file->Close();
        file->Release();

        n_assert(bytesRead < (bufSize - 1));
        buffer[bytesRead] = 0;

        // evaluate contents
        result = Tcl_EvalEx(interp, buffer, -1, 0);

        // free buffer
        n_free(buffer);

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
