#define N_IMPLEMENTS nRubyServer
//--------------------------------------------------------------------
//  nrubyrun.cc -- Command evaluation and passing to ruby
//
//  (C) 2003/4 Thomas Miskiewicz tom@3d-inferno.com
//--------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include "ruby.h"

#include "ruby/nrubyserver.h"
#include "kernel/nfileserver2.h"
#include "kernel/narg.h"

extern "C" void free_d(VALUE);
extern "C" VALUE cNRoot;

//--------------------------------------------------------------------
/**
    Prompt
    Pass a prompt string to the interactive shell.
    Will be called each frame so I omitted any fancy stuff

    - 02-01-04   tom    created
*/
//--------------------------------------------------------------------
nString nRubyServer::Prompt()
{
    // no custom prompt as Prompt will be called every frame :-(
    nString prompt;
    prompt.Append("> ");
    return prompt;

}

//--------------------------------------------------------------------
/**
    Run
    Evaluate a ruby command string
    Result will be empty, as any errors get printed out by ruby itself for
    ease of use.
    
    - 02-01-04  Tom     created
    - 11-02-04  Tom     error printing for N2
*/
//--------------------------------------------------------------------
bool nRubyServer::Run(const char *cmd_str, nString& result)
{
    result.Clear();
    int ret = 0;
    rb_p(rb_eval_string_protect((char *)cmd_str, &ret));
    if (ret) 
    {
        //same as rb_p but without rb_default_rs
        if (this->GetFailOnError()) 
        {
            n_error(RSTRING(rb_obj_as_string(rb_inspect(ruby_errinfo)))->ptr);
        }
        else
        {
            n_printf(RSTRING(rb_obj_as_string(rb_inspect(ruby_errinfo)))->ptr);
            //n_printf(rb_default_rs);
        }
        return false;
    }
    return true;
}

#ifdef __cplusplus
extern "C" {
#endif

//--------------------------------------------------------------------
/**
    NArg2RubyObj
    
    Converts a nebula argument into an ruby native object.
    TODO implement LIST Type for nebula 2

    - 02-01-04  Tom     created
    - 11-02-04  Tom     N2 args
*/
//--------------------------------------------------------------------
VALUE NArg2RubyObj(nArg *a) 
{
    switch (a->GetType()) {
        case nArg::Void:
            return Qnil;
        case nArg::Int:
            return INT2NUM(a->GetI());
        case nArg::Float:
            return rb_float_new(a->GetF());
        case nArg::String:
            return rb_str_new2(a->GetS());
        case nArg::Bool:
            return (a->GetB()==true?Qtrue:Qfalse);
        case nArg::Object:
            {
                  nRoot* o = (nRoot *) a->GetO();
                  if(o)
                  {
                      VALUE tst = Data_Wrap_Struct(cNRoot, 0, free_d, (void *)o); 
                        return tst;
                  }
                  else
                  {
                      return Qnil;
                  }
            }
        //case nArg::ARGTYPE_CODE:
        //    return rb_str_new2(a->GetC());
        case nArg::List:
            {
                nArg *args;
                int num_args = a->GetL(args);
                VALUE result = rb_ary_new2(num_args);

                for(int i = 0; i < num_args; i++) 
                {
                    rb_ary_push(result, NArg2RubyObj(&args[i]));
                }
                return result;
            }
        default:
            return Qundef;
    }
    return Qundef;
}

#ifdef __cplusplus
}
#endif

//--------------------------------------------------------------------
/**
    RunCommand
    Execute a nebula command in ruby

    - 02-01-04  Tom     created not tested to be done
    - 11-02-04  Tom     error printing for N2
*/
//--------------------------------------------------------------------
bool nRubyServer::RunCommand(nCmd *c)
{
    //TODO
    nArg *arg;
    int num_args = c->GetNumInArgs();
    c->Rewind();
    VALUE rargs = Qfalse;

    // handle single return args (no need to create list)
    if (1 == num_args) 
    {
        arg = c->In();
        rargs = NArg2RubyObj(arg);
    } 
    else 
    {
//      rargs = rb_ary_new();

        // more then one in arg, create an Array
        int i;
        for (i=0; i<num_args; i++) 
        {
            arg = c->In();
            rb_ary_push(rargs, NArg2RubyObj(arg));
            //rargs[i] = NArg2RubyObj(arg);
        }
    }
    
    //rb_str_new2(c->In()->GetS());

    rb_p(rb_funcall2(rb_mKernel,rb_intern(c->In()->GetS()), num_args, &rargs));
    if (NIL_P(ruby_errinfo)) 
    {
        //same as rb_p but without rb_default_rs
        if (this->GetFailOnError()) 
        {
            n_error(RSTRING(rb_obj_as_string(rb_inspect(ruby_errinfo)))->ptr);
        }
        else
        {
            n_printf(RSTRING(rb_obj_as_string(rb_inspect(ruby_errinfo)))->ptr);
            //n_printf(rb_default_rs);
        }
        return false;
    }
    return true;
}

//--------------------------------------------------------------------
/**
    RunScript
    Load and evaluate a ruby script then switches over to interactive mode
    Result is left empty as errors get printed out by nebula itself for 
    ease of use.

    - 18-12-03  Tom     created
    - 11-02-04  Tom     error printing for N2
*/
//--------------------------------------------------------------------
bool nRubyServer::RunScript(const char *fname, nString& result)
{
    char buf[N_MAXPATH];
    result.Clear();
    strcpy(buf,(kernelServer->GetFileServer()->ManglePath(fname).Get()));
    this->print_error = true;
    ruby_script(buf);  
    //rb_load_file(buf);  

    int ret =0;
    rb_load_protect(rb_str_new2(buf), 0, &ret);
    if (ret) 
    {
        //rb_p(ruby_errinfo);
        //same as rb_p but without rb_default_rs
        if (this->GetFailOnError()) 
        {
            n_error(RSTRING(rb_obj_as_string(rb_inspect(ruby_errinfo)))->ptr);
        }
        else
        {
            n_printf(RSTRING(rb_obj_as_string(rb_inspect(ruby_errinfo)))->ptr);
            //n_printf(rb_default_rs);
        }
        return false;
    }

    return true;
}

//--------------------------------------------------------------------
/**
    @brief Invoke a Ruby function.
*/
bool nRubyServer::RunFunction(const char *funcName, nString& result)
{
    nString cmdStr = funcName;
    cmdStr.Append("()");
    return this->Run(cmdStr.Get(), result);
}

//--------------------------------------------------------------------
/**
    Trigger
    Check for shutdown flag

    - 18-12-03   Tom    created
    - 01-05-04   Tom    cleaned up
*/
//--------------------------------------------------------------------
bool nRubyServer::Trigger(void)
{
    if(!GetQuitRequested() && !finished)
    {
        return true;
    }
    else
    {
        return false;
    }
}

//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------

