#define N_IMPLEMENTS nCmdProto
#define N_KERNEL
//------------------------------------------------------------------------------
//  ncmdproto.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "kernel/ncmdproto.h"
#include "kernel/ncmd.h"

//------------------------------------------------------------------------------
/**
*/
nCmdProto::nCmdProto(const char *proto_def, uint id, void (*cmd_proc)(void *, nCmd *))
{
    char tmp[128];
    char inArgs[64];
    char outArgs[64];
    char c;
    
    // copy prototype definition
    this->protoDef = proto_def;
	this->fourcc   = id;
	this->cmdProc  = cmd_proc; // can be NULL if legacy cmd handling used
    
    // isolate and validate outargs, cmd name and in args...
    n_strncpy2(tmp, proto_def, sizeof(tmp));
    char* outArgStr = strtok(tmp, "_");
    char* nameStr   = strtok(0, "_");
    char* inArgStr  = strtok(0, "_");
    n_assert(outArgStr && nameStr && inArgStr);

   	// set cmd proto name in own hash node
    this->SetName(nameStr);
 
	// parse in args
	this->numInArgs = 0;
    while ((c = *inArgStr++)) 
    {
		// ignore void arguments
		if (c != 'v') 
        {
	        n_assert(this->IsValidArg(c)) 
			inArgs[this->numInArgs++] = c;
		}					
    }
	inArgs[this->numInArgs] = 0;
		
	// parse out args
	this->numOutArgs = 0;
    while ((c = *outArgStr++)) 
    {
		// ignore void arguments
		if (c != 'v') 
        {
	        n_assert(this->IsValidArg(c)) 
			outArgs[this->numOutArgs++] = c;
		}					
    }
	outArgs[this->numOutArgs] = 0;


    // create template cmd object
    this->cmdTemplate = n_new nCmd(this);

    // initialize template object in args
    int i;
    for (i = 0; i < this->numInArgs; i++)
    {
        switch (inArgs[i]) 
        {
            case 'i':   this->cmdTemplate->In()->SetI(0);     break;
            case 'f':   this->cmdTemplate->In()->SetF(0.0f);  break;
            case 'b':   this->cmdTemplate->In()->SetB(false); break;
            case 's':   this->cmdTemplate->In()->SetS(NULL);  break;
            case 'o':   this->cmdTemplate->In()->SetO(NULL);  break;
            case 'c':   this->cmdTemplate->In()->SetC(NULL);  break;
            default:    break;
       } 
    }

    // initialize template object out args
    for (i = 0; i < this->numOutArgs; i++)
    {
        switch (outArgs[i]) 
        {
            case 'i':   this->cmdTemplate->Out()->SetI(0);     break;
            case 'f':   this->cmdTemplate->Out()->SetF(0.0f);  break;
            case 'b':   this->cmdTemplate->Out()->SetB(false); break;
            case 's':   this->cmdTemplate->Out()->SetS(NULL);  break;
            case 'o':   this->cmdTemplate->Out()->SetO(NULL);  break;
            case 'c':   this->cmdTemplate->Out()->SetC(NULL);  break;
            default:    break;
       } 
    }
    this->cmdTemplate->Rewind();
    this->cmdLocked = false;
}

//------------------------------------------------------------------------------
/**
*/
nCmdProto::nCmdProto(const nCmdProto& rhs)
{
    this->SetName(rhs.GetName());
    this->fourcc      = rhs.fourcc;
    this->numInArgs   = rhs.numInArgs;
    this->numOutArgs  = rhs.numOutArgs;
    this->cmdTemplate = n_new nCmd(*(rhs.cmdTemplate));
    this->cmdLocked   = false;
    this->protoDef    = rhs.protoDef;
    this->cmdProc     = rhs.cmdProc;
}


//------------------------------------------------------------------------------
/**
*/
nCmdProto::~nCmdProto()
{
    n_delete this->cmdTemplate;
}

//------------------------------------------------------------------------------
/**
    Create a new cmd object rom the internal template object. Optimization:
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
        return n_new nCmd(*(this->cmdTemplate));
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
        n_delete cmd;
    }
    else
    {
        // this was the cached command template, just unlock it
        n_assert(cmdLocked);
        cmdLocked = false;
    }
}

//------------------------------------------------------------------------------
//  EOF
//------------------------------------------------------------------------------
