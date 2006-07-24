//------------------------------------------------------------------------------
/**
    @file
    @ingroup Nebula2TutorialsBasic

    @brief Basic Example 01 - Creating the Kernel Server.

    The simplest possible Nebula2 program. Creates the nKernelServer,
    logs a message which is sent to stdout and deletes the nKernelServer.
*/

#include "kernel/nkernelserver.h"

int
main(int /*argc*/, const char** /*argv*/)
{
    nKernelServer* kernelServer = n_new (nKernelServer());

    n_printf("Goodbye Cruel World\n");

    n_delete (kernelServer);
    return 0;
}
