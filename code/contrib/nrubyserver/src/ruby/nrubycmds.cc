#define N_IMPLEMENTS nRubyServer
//--------------------------------------------------------------------
//  nrubycmds.cc --  implements ruby command extensions
//
//  (C) 2004 Thomas Miskiewicz see 3di_license.txt for usage
//--------------------------------------------------------------------

#include "ruby/nrubyserver.h"
#include "kernel/nkernelserver.h"
#include "kernel/nroot.h"
#include "kernel/narg.h"


#ifdef __cplusplus
extern "C" {
#endif
#include "ruby.h"

extern VALUE NArg2RubyObj (nArg *);

// module, error and nroot wraper objects in ruby
VALUE mNebula;
VALUE cNError;
VALUE cNRoot;

//forward dec
void register_commands(void);
VALUE rubycmd_New(VALUE, VALUE , VALUE );
VALUE rubycmd_Delete(VALUE, VALUE );
VALUE rubycmd_Sel(VALUE, VALUE );
VALUE rubycmd_Psel(VALUE);
VALUE rubycmd_Dir(VALUE);
VALUE rubycmd_Get(VALUE, VALUE );
VALUE rubycmd_Unknown(int , VALUE *, VALUE);
VALUE rubycmd_Exit(VALUE);
VALUE rubycmd_Exists(VALUE, VALUE );
VALUE rubycmd_ToS(VALUE);
VALUE rubycmd_Puts(int , VALUE *, VALUE);
VALUE rubycmd_Class(VALUE);
VALUE rubycmd_Inspect(VALUE );

///dummy, to not free nroot objects could be removed
void free_d(VALUE x)
{
    ;
}
///wraps a nebula object for the ruby world
VALUE setRoot(nRoot*root)
{
    n_assert(root);
    void* vp = (void*)root;
    return Data_Wrap_Struct(cNRoot, 0, free_d, vp);
}

///unwraps a ruby object into a nebula object
nRoot * getRoot(VALUE val)
{
    nRoot *o=NULL;

    switch(TYPE(val))
    {
    case T_STRING:
        o = (nRubyServer::kernelServer)->Lookup(RSTRING(val)->ptr);
        break;
    case T_OBJECT:
    case T_DATA:
        {
            if(CLASS_OF(val) == cNRoot)
            {
                void * vptr;
                Data_Get_Struct(val, void, vptr);
                if(vptr != NULL)
                    o=(nRoot*)vptr;
            }
        }break;
    }

    return o;
}
//--------------------------------------------------------------------
/**
    _getInArgs()
    Convert ruby values back to nebula args returns true if successfull
    false if conversion was not possible

    -TODO
        support for list arguments

    -18-Dec-03  Tom     adapted from ruby
    -10-Feb-04  Tom     redone for N2 args
*/
//--------------------------------------------------------------------
static bool _getInArgs(nCmd *cmd, int argc, VALUE *argv)
{
    long num_args;
    VALUE rbarg;

    num_args = cmd->GetNumInArgs();
    if (num_args == argc)
    {

        int i;
        nArg *arg;
        cmd->Rewind();
        for (i=0; i<num_args; i++)
        {
            arg = cmd->In();
            rbarg = argv[i];

            switch(arg->GetType())
            {
              case nArg::Int:
                arg->SetI(NUM2INT(rbarg));
                break;

              case nArg::Float:
                arg->SetF((float)NUM2DBL(rbarg));
                break;

              case nArg::String:
                rb_check_type(rbarg, T_STRING);
                arg->SetS(RSTRING(rbarg)->ptr);
                break;

              //case nArg::ARGTYPE_CODE:
                //rb_check_type(rbarg, T_STRING);
                //arg->SetC(RSTRING(rbarg)->ptr);
                //break;
                // seems to be gone in N2

              case nArg::Bool:
                arg->SetB((TYPE(rbarg)==T_TRUE)?true:false);
                break;

              case nArg::Object:
                {
                    nRoot *o;
                    o = getRoot(rbarg);
                    arg->SetO(o);
                }
                break;

              case nArg::List:
                {
                    int len = RARRAY(rbarg)->len;
                    if(len <=0)
                        return true;

                    nArg* inargs = n_new_array(nArg, len);
                    n_assert(inargs);

                    VALUE result,str;

                    for(int j=0;j<len;j++)
                    {
                        str = rb_ary_entry(rbarg,j);
                        result = rb_funcall(str, rb_intern("to_s"), 0);
                        inargs[j].SetS(RSTRING(result)->ptr);
                    }
                    arg->SetL(inargs,len);
                }
              case nArg::Void:
                break;
            }
        }
        return true;
    }
    rb_raise(cNError, "%s: Expected %ld arg(s), but got %d",
       cmd->GetProto()->GetProtoDef(), num_args, argc);
    return false;
}

//--------------------------------------------------------------------
/**
    _putOutArgs()
    Converts nebula command objects to native ruby values

    -19-Dec-03  Tom     created
*/
//--------------------------------------------------------------------
static VALUE _putOutArgs(nCmd *cmd)
{
    nArg *arg;
    int num_args = cmd->GetNumOutArgs();
    cmd->Rewind();

    // handle single return args (no need to create list)
    if (1 == num_args)
    {
        arg = cmd->Out();
        //return _arg_to_value(arg);
        return NArg2RubyObj(arg);
    }
    else
    {
        VALUE array = rb_ary_new();

        // more then one output arg, create an Array
        int i;
        for (i=0; i<num_args; i++)
        {
            arg = cmd->Out();
            //rb_ary_push(array, _arg_to_value(arg));
            rb_ary_push(array, NArg2RubyObj(arg));
        }
        return array;
    }
}


//--------------------------------------------------------------------
/**
    rubycmd_New()
    Create a new nebula object from ruby in nebula and return the object created
    (just pass the string back) or return Qnil if failed.

    -19-Dec-03  Tom     created
*/
//--------------------------------------------------------------------
VALUE rubycmd_New(VALUE, VALUE node, VALUE name)
{
    rb_check_type(node, T_STRING);
    rb_check_type(name, T_STRING);

    if (!RSTRING(node)->ptr || !RSTRING(name)->ptr)
    {
        rb_warn("Syntax is 'name = new 'class' , 'name'");
    }
    else
    {
        nRoot *o = (nRubyServer::kernelServer)->NewNoFail(RSTRING(node)->ptr,RSTRING(name)->ptr);
        if (o)
        {
            return setRoot(o);
        }
    }
    return Qnil;
}

//--------------------------------------------------------------------
/**
    rubycmd_Delete()
    Deletes a nebula object returns Qtrue if successfull or Qfalse otherwise

    -19-Dec-03  Tom     created
*/
//--------------------------------------------------------------------
VALUE rubycmd_Delete(VALUE, VALUE name)
{
    nRoot *o = getRoot(name);

    if(o)
    {
        o->Release();
        return Qtrue;
    }
    else
    {
        rb_raise(cNError,"Unable to lookup object %s", RSTRING(name)->ptr);
        return Qfalse;
    }
}


//--------------------------------------------------------------------
/**
    rubycmd_Sel()
    Select a nebula object and return it as a string if successfull or Qnil otherwise.
    Also executes a block if a block was passed with the select command:
    sel '/usr' do
        ..
    end

    -19-Dec-03  Tom     created
    -10-Feb-04  Tom     SetCwd no longer returns a nRoot in N2
*/
//--------------------------------------------------------------------
VALUE rubycmd_Sel(VALUE, VALUE obj)
{
    nRoot *o = getRoot(obj);
    VALUE res = obj;

    if(o)
    {
        (nRubyServer::kernelServer)->SetCwd(o);

        if (rb_block_given_p())
            res = rb_yield(res);

            return res;
    }
    else
    {
        rb_raise(cNError,"Unable to lookup object %s", RSTRING(obj)->ptr);
        return Qnil;
    }
}

//--------------------------------------------------------------------
/**
    rubycmd_Psel()
    Return the currently selected object in nebula (cwd) as a string if
    successfull or Qnil otherwise.

    -19-Dec-03  Tom     created
*/
//--------------------------------------------------------------------
VALUE rubycmd_Psel(VALUE)
{
    nRoot *o = (nRubyServer::kernelServer)->GetCwd();

    if(o)
    {
        return setRoot(o);
    }

    return Qnil;
}

//--------------------------------------------------------------------
/**
    rubycmd_Dir()
    Return a list of all objects beneath the currently selected node

    -28-Dec-03  Tom     created by adopting from Michael Witrant
*/
//--------------------------------------------------------------------
VALUE rubycmd_Dir(VALUE)
{
    nRoot *cwd = (nRubyServer::kernelServer)->GetCwd();

    if(cwd)
    {
        nRoot *o;
        VALUE result = rb_ary_new();

        for (o=cwd->GetHead(); o; o=o->GetSucc())
        {
            rb_ary_push(result, rb_str_new2(o->GetName()));
        }
        return result;
    }
    else
    {
        rb_raise(cNError,"Unable to get current working node");
        return Qnil;
    }
}

//--------------------------------------------------------------------
/**
    rubycmd_Get()
    Load the associated file and return the created topnode

    -TODO not tested

    -19-Dec-03   Tom    created
*/
//--------------------------------------------------------------------
VALUE rubycmd_Get(VALUE, VALUE filename)
{
    rb_check_type(filename, T_STRING);

    nObject *o = (nRubyServer::kernelServer)->Load(RSTRING(filename)->ptr);
    if (o)
    {
        bool isRoot = o->IsA("nroot");
        n_assert(isRoot);
        if (isRoot)
        {
            return setRoot((nRoot*)o);
        }
        rb_raise(cNError,"Unable to load object %s, not an nRoot", RSTRING(filename)->ptr);
    }
    rb_raise(cNError,"Unable to load object %s", RSTRING(filename)->ptr);
    return Qnil;
}


//--------------------------------------------------------------------
/**
    rubycmd_Unknown()
    Ruby first tries to call the issued commands on the object of its scope. If no
    command could be called ruby passes all args over to a method_missing method.
    This is the place where we hook in and pass it over to our nebula objects.
    If we cannot resolve it then ruby will print an error itself.

    -20-Dec-03   Tom    created by adopting from Michael Witrant and tclserver
*/
//--------------------------------------------------------------------
VALUE rubycmd_Unknown(int argc, VALUE *argv, VALUE obj)
{
    if(argc ==0)
    {
        rb_raise(rb_eArgError, "Expected at least 1 arg. None given");
        return Qnil;
    }
    nRoot *o=NULL;
    VALUE res = rb_funcall(obj, rb_intern("to_s"), 0);

    //check if it's from the main ruby object if so use the current working set
    if(strcmp(RSTRING(res)->ptr,"main")==0)
        o = (nRubyServer::kernelServer)->GetCwd();
    else
        o = getRoot(obj);

    if(!o)
    {
//      n_printf(" %s",RSTRING(res)->ptr);
        return Qnil;
    }

    ID id = SYM2ID(argv[0]);
    char *cmd_name = rb_id2name(id);
//n_printf("cmd: %s\n", cmd_name);
    nClass *cl = o->GetClass();
    VALUE retval;
    nCmdProto *cmd_proto = (nCmdProto *) cl->FindCmdByName(cmd_name);
    if (cmd_proto)
    {
        nCmd *cmd = cmd_proto->NewCmd();

        if (!_getInArgs(cmd, argc-1, &argv[1]))
            return Qnil;

        // let object handle the command
        if (o->Dispatch(cmd))
        {
            retval = _putOutArgs(cmd);
        }
        else
        {
            rb_raise(cNError,"Unable to dispatch command %s", cmd_name);
            return Qnil;
        }
        cmd_proto->RelCmd(cmd);
    }
    else
    {
        retval = Qnil;
    }

    return retval;
}

//--------------------------------------------------------------------
/**
    rubycmd_Exit()
    Terminate the nrubyserver from a script call.

    -02-Dec-03  Tom     created
*/
//--------------------------------------------------------------------
VALUE rubycmd_Exit(VALUE)
{
    nRubyServer::Terminate();
    return Qtrue;
}

//--------------------------------------------------------------------
/**
    rubycmd_Puts()
    Print result on console

    -02-Dec-03  Tom     created
    -03-Jan-04  Tom     array and hash prints
*/
//--------------------------------------------------------------------
VALUE rubycmd_Puts(int argc, VALUE *argv, VALUE klass)
{
    VALUE str;
    int i;
    char buffer[N_MAXPATH] ="";
    for (i=0; i<argc; i++)
    {
/*      if(CLASS_OF(argv[i]) == cNRoot)//||CLASS_OF(argv[i]) == cNError||CLASS_OF(argv[i]) == mNebula)
        {
            str = rb_any_to_s(argv[i]);
            (nRubyServer::kernelServer)->Print(RSTRING(str)->ptr);
        }
        else*/ if(TYPE(argv[i]) == T_ARRAY || TYPE(argv[i]) == T_HASH)
        {
            VALUE result;
            int len = RARRAY(argv[i])->len;
            if(len<=0)
                continue;
            for(int j=0;j<len;j++)
            {
                str = rb_ary_entry(argv[i],j);
                result = rb_funcall(str, rb_intern("to_s"), 0);
                (nRubyServer::kernelServer)->Print(RSTRING(result)->ptr);
                (nRubyServer::kernelServer)->Print(" ");
            }
        }
        else
        {
            str = rb_funcall(argv[i], rb_intern("to_s"), 0);
            (nRubyServer::kernelServer)->Print(RSTRING(str)->ptr);
        }
        (nRubyServer::kernelServer)->Print("\n");
    }

    return Qnil;
}

//--------------------------------------------------------------------
/**
    rubycmd_Exists
    Returns qtrue if the specified nebula object exists in the named hierarchy.

    -TODO
        Test it

    -19-Dec-03  Tom     created
*/
//--------------------------------------------------------------------
VALUE rubycmd_Exists(VALUE, VALUE name)
{

  nRoot *o = getRoot(name);
  return (o ? Qtrue : Qfalse);
}

//--------------------------------------------------------------------
/**
    rubycmd_ToS()
    Returns a string representing the object.

    -28-Dec-03  Tom     created
*/
//--------------------------------------------------------------------
VALUE rubycmd_ToS(VALUE val)
{
    nRoot * o=getRoot(val);

    if(o)
    {
        return rb_str_new2(o->GetFullName().Get());
    }

    return rb_str_new2("");
}

//--------------------------------------------------------------------
/**
    register_commands
    Registers our definitions within ruby.

    -18-Dec-03  Tom     created
*/
//--------------------------------------------------------------------
void register_commands(void)
{
    mNebula = rb_define_module("Nebula");
    cNRoot = rb_define_class_under(mNebula, "nRoot", rb_cObject);

    rb_define_module_function(mNebula, "new", RUBY_METHOD_FUNC(rubycmd_New), 2);
    rb_define_module_function(mNebula, "sel", RUBY_METHOD_FUNC(rubycmd_Sel), 1);
    rb_define_module_function(mNebula, "psel", RUBY_METHOD_FUNC(rubycmd_Psel), 0);
    rb_define_module_function(mNebula, "method_missing", RUBY_METHOD_FUNC(rubycmd_Unknown), -1);
    //not realy needed !
    rb_define_method(cNRoot, "to_s", RUBY_METHOD_FUNC(rubycmd_ToS), 0);
//  rb_define_method(cNRoot, "initialize", RUBY_METHOD_FUNC(rubycmd_Init), 0);

    rb_define_module_function(mNebula, "delete", RUBY_METHOD_FUNC(rubycmd_Delete), 1);
    rb_define_module_function(mNebula, "puts", RUBY_METHOD_FUNC(rubycmd_Puts), -1);
    rb_define_module_function(mNebula, "dir", RUBY_METHOD_FUNC(rubycmd_Dir), 0);
    rb_define_module_function(mNebula, "get", RUBY_METHOD_FUNC(rubycmd_Get), 1);
    rb_define_module_function(mNebula, "exit", RUBY_METHOD_FUNC(rubycmd_Exit), 0);
    // not tested
    rb_define_module_function(mNebula, "exists", RUBY_METHOD_FUNC(rubycmd_Exists), 1);

    cNError = rb_define_class_under(mNebula, "Error", rb_eException);
}

//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------

#ifdef __cplusplus
}
#endif
