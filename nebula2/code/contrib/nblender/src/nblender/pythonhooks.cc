//-----------------------------------------------------------------------------
//  pythonhooks.cc
//  (C) 2005-2006 James Mastro
//-----------------------------------------------------------------------------
#include "python/npythonserver.h"

void
nPythonInitializeEnvironment()
{
    PyRun_SimpleString("from pynebula import *");
}

const char*
nPythonModuleName()
{
    return "nblender";
}
