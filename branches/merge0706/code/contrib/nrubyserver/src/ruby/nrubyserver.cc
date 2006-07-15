#define N_IMPLEMENTS nRubyServer
//--------------------------------------------------------------------
//  nrubyserver.cc
//  (C) 2003/4 Thomas Miskiewicz tom@3d-inferno.com
//--------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>


#include "ruby/nrubyserver.h"
#include "kernel/nfileserver2.h"
#include "kernel/nfile.h"
#include "util/nstring.h"
#include "ruby.h"

nNebulaClass(nRubyServer, "nscriptserver");

extern VALUE rubycmd_New;
extern VALUE rubycmd_Sel;
extern VALUE rubycmd_Psel;
extern VALUE rubycmd_Unknown;
extern VALUE rubycmd_Delete;
extern VALUE rubycmd_Dir;
extern VALUE rubycmd_Get;
extern VALUE rubycmd_Exit;
extern VALUE rubycmd_Exists;
extern "C" void register_commands();

//initialization
bool nRubyServer::finished       = false;

///finish up rubyserver
void nRubyServer::Terminate(void)
{
    finished = true;
}

//--------------------------------------------------------------------
//  nRubyServer()
//--------------------------------------------------------------------
nRubyServer::nRubyServer()
{
    this->refFileServer     = "/sys/servers/file2";
    this->print_error       = false;
    this->indent_level      = 0;
    this->indent_buf[0]     = 0;

    // initialize data internal to RUBY to make encodings work.
    ruby_init();
    // not realy used anymore but extensions could come here
    register_manglepath("home:bin/ruby/ruby1.8/lib");
    register_manglepath("home:bin/ruby/ruby1.8/lib/ruby/1.8");
    register_manglepath("home:bin/ruby/ruby1.8/lib/ruby/1.8/i386-mswin32");
    register_manglepath("home:bin/ruby/ruby1.8/ext");
        
    // restrict ruby in its access to the system a little bit
    rb_set_safe_level(1);
    ruby_script("nRubyServer");
    ruby_show_version();
    ruby_init_loadpath();
    // our calls into the ruby world
    register_commands();
}

//--------------------------------------------------------------------
/**
    register_manglepath()
    resolve assign and register the resulting path with ruby as a library 
    search path. Like "home:bin/ruby/ruby1.8/lib"

    -TODO
        rb_p conversion in N2

    -18-Dec-03  Tom created
*/
//--------------------------------------------------------------------
void nRubyServer::register_manglepath(const char* param_path)
{
    int ret =0;
    char path[N_MAXPATH+10];
    nString str(kernelServer->GetFileServer()->ManglePath(param_path));
    strcpy(path, "$: << \"");
    strcat(path, str.Get());
    strcat(path, "\"");
    //int ret=0;
    rb_p(rb_eval_string_protect(path, &ret));
    if (ret) {
        rb_p(ruby_errinfo);
    }
}
//--------------------------------------------------------------------
/**
    ~nRubyServer()
    finish up the ruby environment

    -18-Dec-03  Tom created
*/
//--------------------------------------------------------------------
nRubyServer::~nRubyServer()
{
    ruby_finalize();
}


//--------------------------------------------------------------------
/**
    BeginWrite()
    start writinig a persistent script object

    -TODO
        TEST it.

    -18-Dec-03  Tom created
*/
//--------------------------------------------------------------------
nFile* 
nRubyServer::BeginWrite(const char* filename, nObject* obj)
{
    n_assert(filename);
    n_assert(obj);

    this->indent_level = 0;

    nFile* file = this->refFileServer->NewFileObject();
    n_assert(file);
    if (file->Open(filename, "w"))
    {
        char buf[N_MAXPATH];
        sprintf(buf, "# $parser:nrubyserver$ $class:%s$\n", obj->GetClass()->GetName());

        file->PutS("# ---\n");
        file->PutS(buf);
        file->PutS("# ---\n");
        return file;
    }
    else
    {
        n_printf("nRubyServer::WriteBegin(): failed to open file '%s' for writing!\n", filename);
        file->Release();
        return 0;
    }
}

//--------------------------------------------------------------------
/**
    EndWrite()
    End writing persistent script file

    -TODO
        TEST it.

    -18-Dec-03  Tom created
*/
//--------------------------------------------------------------------
bool 
nRubyServer::EndWrite(nFile* file)
{
    n_assert(file);
 
    file->PutS("# ---\n");
    file->PutS("# Eof\n");
    
    file->Close();
    file->Release();
    return (this->indent_level == 0);
}

//--------------------------------------------------------------------
//  _indent()
//  04-Nov-98   floh    created
//  18-Dec-03   Tom     copied over from ntclserver.cc
//--------------------------------------------------------------------
static void _indent(long i, char *buf)
{
    long j;
    buf[0] = 0;
    for (j=0; j<i; j++) strcat(buf,"\t");
}

//--------------------------------------------------------------------
/**
    WriteComment()
    Write a ruby comment line to the script file

    -TODO
        TEST it.

    -18-Dec-03  Tom created
*/
//--------------------------------------------------------------------
bool nRubyServer::WriteComment(nFile *file, const char *str)
{
    n_assert(file);
    n_assert(str);
    file->PutS("# ");
    file->PutS(str);
    file->PutS("\n");
    return true;
}

//--------------------------------------------------------------------
/**
    write_select_statement()
    Write the statement to select an object after its creation
    statement.

    -TODO
        TEST it.

    -05-Oct-00  floh    created
    -18-Dec-03  Tom     adopted for ruby
*/
//--------------------------------------------------------------------
void nRubyServer::write_select_statement(nFile* file, nRoot *o, nRoot *owner)
{
    switch (this->GetSelectMethod()) {

        case SELCOMMAND:
            // get relative path from owner to o and write select statement
            _indent(++this->indent_level, this->indent_buf);
            
            file->PutS(this->indent_buf);
            file->PutS("sel \"");
            file->PutS(owner->GetRelPath(o).Get());
            file->PutS("\"\n");
            break;

        case NOSELCOMMAND:
            break;
    }
}

//--------------------------------------------------------------------
/**
    WriteBeginNewObject()
    Write start of persistent object with default constructor.
    
    -TODO
        TEST it.

    -06-Mar-00   floh    created
    -05-Sep-00   floh    + support for optional enter/leave syntax
*/
//--------------------------------------------------------------------
bool nRubyServer::WriteBeginNewObject(nFile* file, nRoot *o, nRoot *owner)
{
    n_assert(file);
    n_assert(o);
    const char *o_name  = o->GetName();

    // write generic 'new' statement with ruby syntax
    const char *o_class = o->GetClass()->GetName();
    _indent(this->indent_level,this->indent_buf);

    file->PutS(this->indent_buf);
    file->PutS("new \"");
    file->PutS(o_class);
    file->PutS("\", \"");
    file->PutS(o_name);
    file->PutS("\"\n");

    // write select object statement
    this->write_select_statement(file, o, owner);
    return true;
}

//--------------------------------------------------------------------
/**
    WriteBeginNewObjectCmd()
    Write start of persistent object with custom constructor
    defined by command.
    
    -TODO
        TEST it.

    -06-Mar-00  floh    created
    -18-Dec-03  Tom     adapted fro ruby
*/
//--------------------------------------------------------------------
bool nRubyServer::WriteBeginNewObjectCmd(nFile* file, nRoot *o, nRoot *owner, nCmd *cmd)
{
    n_assert(file);
    n_assert(o);
    n_assert(cmd);

    // write constructor statement
    this->WriteCmd(file, cmd);

    // write select object statement
    this->write_select_statement(file, o, owner);
    return true;
}

//--------------------------------------------------------------------
/**
    WriteBeginSelObject()
    Write start of persisting object without constructor, only
    write the select statement.

    -TODO
        TEST it.

    -06-Mar-00   floh    created
    -18-Dec-03  Tom     adapted fro ruby
*/
//--------------------------------------------------------------------
bool nRubyServer::WriteBeginSelObject(nFile* file, nRoot *o, nRoot *owner)
{
    n_assert(file);
    n_assert(o);

    // write select object statement
    this->write_select_statement(file, o, owner);
    return true;
}

//--------------------------------------------------------------------
/**
    WriteEndObject()
    
    -TODO
        TEST it.

    -04-Nov-98  floh    created
    -18-Dec-03  Tom     adapted fro ruby
*/
//--------------------------------------------------------------------
bool nRubyServer::WriteEndObject(nFile* file, nRoot *o, nRoot *owner)
{
    n_assert(file);
    n_assert(o);

    // get relative path from owner to o and write select statement
    _indent(--this->indent_level, this->indent_buf);
    file->PutS(this->indent_buf);
    file->PutS("sel \"");
    file->PutS(o->GetRelPath(owner).Get());
    file->PutS("\"\n");

    return true;
}

//--------------------------------------------------------------------
/**
    WriteCmd()
    
    -TODO
        TEST it.

    -04-Nov-98  floh    created
    -18-Dec-03  Tom     adapted to ruby syntax
*/
//--------------------------------------------------------------------
bool nRubyServer::WriteCmd(nFile* file, nCmd *cmd)
{
    n_assert(file);
    n_assert(cmd);
    const char *name = cmd->GetProto()->GetName();
    n_assert(name);
    nArg *arg;

    // write the command name
    file->PutS(this->indent_buf);
    file->PutS(name);
    file->PutS(" ");

    // write command arguments
    cmd->Rewind();
    int num_args = cmd->GetNumInArgs();

    const char* strPtr;
    ushort strLen;
    ushort bufLen;

    int i;
    for (i=0; i<num_args; i++) 
    {
        char buf[N_MAXPATH];
        arg=cmd->In();

        switch(arg->GetType()) {

            case nArg::Int:
                sprintf(buf, "%d", arg->GetI());
                break;

            case nArg::Float:
                sprintf(buf, "%.6f", arg->GetF());
                break;

            case nArg::String:
                strPtr = arg->GetS();
                strLen = strlen(strPtr);
                bufLen = sizeof(buf)-1;
            
                file->PutS("\"");
                if (strLen > bufLen-1) 
                {
                    buf[bufLen] = 0; // Null terminator
                    for (int j=0; j<strLen-2; j+=bufLen)
                    {
                        memcpy((void*)&buf[0], strPtr, bufLen);
                        file->PutS(buf);
                        strPtr += bufLen;     
                    }
                strPtr += bufLen;
                }
                sprintf(buf, "%s\"", strPtr);
                break;

            //case nArg::ARGTYPE_CODE:
            //    sprintf(buf, "do; %s; end;", arg->GetC());
            //    break;

            case nArg::Bool:
                sprintf(buf,"%s",(arg->GetB() ? "Qtrue" : "Qfalse"));
                break;

            case nArg::Object:
                {
                    nRoot *o = (nRoot *) arg->GetO();
                    if (o) {
                        char buf[N_MAXPATH];
                        sprintf(buf, "\"%s\"", o->GetFullName().Get() );
                    } 
                    else 
                    {
                        sprintf(buf, "null");
                    }
                }
                break;

            default:
                sprintf(buf, "Qundef");
                break;
        }
        file->PutS(buf);
        // check for argument seperator
        if(i < (num_args-1))
            file->PutS(" , ");
    }
    return file->PutS("\n");
}

//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------





