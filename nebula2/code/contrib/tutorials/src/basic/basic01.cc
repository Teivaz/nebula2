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
    nKernelServer* kernelServer = new nKernelServer();

    n_printf("Goodbye Cruel World");

    delete kernelServer;
    return 0;
}
