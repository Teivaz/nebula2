//------------------------------------------------------------------------------
//  nlanguagewrapper.cc
//  This file is licensed under the terms of the Nebula License.
//  (C) 2003 Bruce Mitchener, Jr.
//------------------------------------------------------------------------------
#include "wrapper/nlanguagewrapper.h"

nNebulaClass(nLanguageWrapper, "nroot");

static void dumpArg(nArg *);

//------------------------------------------------------------------------------
/**
*/
nLanguageWrapper::nLanguageWrapper()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nLanguageWrapper::~nLanguageWrapper()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nLanguageWrapper::OpenClass(nClass * cl)
{
    if (NULL != cl->GetSuperClass())
    {
        n_printf("class %s : %s {\n", cl->GetProperName(),
                 cl->GetSuperClass()->GetProperName());
    }
    else
    {
        n_printf("class %s {\n", cl->GetProperName());
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nLanguageWrapper::CloseClass(nClass * cl)
{
    n_printf("};\n");
}

//------------------------------------------------------------------------------
/**
*/
void
nLanguageWrapper::WriteCommand(nClass * cl, nCmdProto * cmdProto)
{
    nCmd * cmd = cmdProto->NewCmd();
    n_printf("    ");
    if (0 == cmdProto->GetNumOutArgs())
    {
        n_printf("void");
    }
    else
    {
        if (cmdProto->GetNumOutArgs() > 1)
        {
            n_printf("(");
        }
        int outArgIndex;
        for (outArgIndex = 0;
             outArgIndex < cmdProto->GetNumOutArgs();
             outArgIndex++)
        {
            nArg * outArg = cmd->Out();
            dumpArg(outArg);
            if (outArgIndex < (cmdProto->GetNumOutArgs() - 1))
            {
                n_printf(", ");
            }
        }
        if (cmdProto->GetNumOutArgs() > 1)
        {   
            n_printf(")");
        }
    }
    n_printf(" %s(", cmdProto->GetName());
    int inArgIndex; 
    for (inArgIndex = 0; inArgIndex < cmdProto->GetNumInArgs(); inArgIndex++)
    {   
        nArg * inArg = cmd->In();
        dumpArg(inArg);
        if (inArgIndex < (cmdProto->GetNumInArgs() - 1))
        {   
            n_printf(", ");
        }
    }
    n_printf(");\n");
    cmd->Rewind();
    cmdProto->RelCmd(cmd);
}

//------------------------------------------------------------------------------
/**
*/
static
void
dumpArg(nArg * arg)
{
    nArg::Type type = arg->GetType();
    switch (type)
    {
        case nArg::Void:
            n_printf("void");
            break;
        case nArg::Int:
            n_printf("int");
            break;
        case nArg::Float:
            n_printf("float");
            break;
        case nArg::String:
            n_printf("string");
            break;
        case nArg::Bool:
            n_printf("bool");
            break;
        case nArg::Object:
            n_printf("object");
            break;
        case nArg::List:
            n_printf("list");
            break;
        case nArg::Float4:
            n_printf("float4");
            break;
        case nArg::Matrix44:
            n_printf("matrix44");
            break;
        default:
            n_error("Unhandled type: %d\n", type);
            break;
    }
}

