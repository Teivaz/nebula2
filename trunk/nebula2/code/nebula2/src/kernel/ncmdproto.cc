//------------------------------------------------------------------------------
//  ncmdproto.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "kernel/ncmdproto.h"
#include "kernel/ncmd.h"

//------------------------------------------------------------------------------
/**
*/
nCmdProto::nCmdProto(const char *proto_def, nFourCC id)
{
    // check prototype definition
    ProtoDefInfo info(proto_def);

    // parse proto definition and divided in outArgs, name and inArgs. @returns true if OK
    if (!info.valid)
    {
        n_assert2(0, "Invalid prototype definition for nCmdProto");
    }

    // copy prototype definition
    this->protoDef = proto_def;
    this->fourcc   = id;
    this->numInArgs = info.numInArgs;
    this->numOutArgs = info.numOutArgs;

    // set cmd proto name in own hash node
    this->SetName(info.name);

    // create template cmd object
    this->cmdTemplate = n_new(nCmd(this));

    // initialize template object in args
    int i;
    for (i = 0; i < this->numInArgs; i++)
    {
        this->cmdTemplate->In()->Reset(info.inArgs[i]);
    }

    // initialize template object out args
    for (i = 0; i < this->numOutArgs; i++)
    {
        this->cmdTemplate->Out()->Reset(info.outArgs[i]);
    }
    this->cmdTemplate->Rewind();
    this->cmdLocked = false;
}

//------------------------------------------------------------------------------
/**
*/
nCmdProto::nCmdProto(const nCmdProto& rhs) :
    nHashNode(rhs.GetName())
{
    this->fourcc      = rhs.fourcc;
    this->numInArgs   = rhs.numInArgs;
    this->numOutArgs  = rhs.numOutArgs;
    this->cmdTemplate = n_new(nCmd(*(rhs.cmdTemplate)));
    this->cmdLocked   = false;
    this->protoDef    = rhs.protoDef;
}


//------------------------------------------------------------------------------
/**
*/
nCmdProto::~nCmdProto()
{
    n_delete(this->cmdTemplate);
}

//------------------------------------------------------------------------------
/**
    Create a new cmd object from the internal template object. Optimization:
    normally just returns the template object (if nobody else uses it), instead
    of creating a new object.

    IMPORTANT: the nCmdProto::RelCmd() method MUST be used to release the
    nCmd object!!!
*/
nCmd*
nCmdProto::NewCmd()
{
    if (this->cmdLocked)
    {
        // template object is locked, create a new object
        return n_new(nCmd(*(this->cmdTemplate)));
    }
    else
    {
        this->cmdLocked = true;
        this->cmdTemplate->Rewind();
        return this->cmdTemplate;
    }
}

//------------------------------------------------------------------------------
/**
    Release a nCmd object created with nCmdProto::NewCmd()
*/
void
nCmdProto::RelCmd(nCmd* cmd)
{
    n_assert(cmd);
    if (cmd != this->cmdTemplate)
    {
        // not the template object, release with n_delete
        n_delete(cmd);
    }
    else
    {
        // this was the cached command template, just unlock it
        n_assert(cmdLocked);
        cmdLocked = false;
    }
}

//------------------------------------------------------------------------------
/**
    ProtoDefInfo constructor which parses and checks validity of the prototype
    definition. Initializes info resulting information from the parsing process.

    @param proto_def char array containing prototype definition to check & parse
*/
ProtoDefInfo::ProtoDefInfo(const char * proto_def)
{
    n_assert(proto_def);

    const char * ptr;
    char c;

    // initialize all output strings & argument counters
    memset(this, 0, sizeof(ProtoDefInfo));

    // check out args
    ptr = proto_def;
    while ((0 != (c = *ptr++)) && ('_' != c))
    {
        if (!nArg::IsVoid(c))
        {
            if (nArg::IsValidArg(c))
            {
                this->outArgs[this->numOutArgs++] = c;
            }
            else
            {
                return;
            }
        }
    }
    this->outArgs[this->numOutArgs] = 0;

    // check if found separator '_'
    if ('_' != c)
    {
        return;
    }

    // copy name
    int i = 0;
    while ((0 != (c = *ptr++)) && ('_' != c))
    {
        this->name[i++] = c;
    }
    this->name[i++] = 0;

    // check if found separator '_'
    if ('_' != c)
    {
        return;
    }

    // copy out args if buffer provided
    while ((0 != (c = *ptr++)) && ('_' != c))
    {
        if (!nArg::IsVoid(c))
        {
            if (nArg::IsValidArg(c))
            {
                this->inArgs[this->numInArgs++] = c;
            }
            else
            {
                return;
            }
        }
    }
    this->inArgs[this->numInArgs] = 0;

    // check if found end of string
    if (0 != c)
    {
        return;
    }

    valid = true;
}

//------------------------------------------------------------------------------
//  EOF
//------------------------------------------------------------------------------
