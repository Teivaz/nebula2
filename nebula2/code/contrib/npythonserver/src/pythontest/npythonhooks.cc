//------------------------------------------------------------------------------
/**
*/
//------------------------------------------------------------------------------
#include "python/npythonserver.h"

void
nPythonInitializeEnvironment()
{
    PyRun_SimpleString("from pynebula import *");
}

const char*
nPythonModuleName()
{
    return "pynebula";
}

