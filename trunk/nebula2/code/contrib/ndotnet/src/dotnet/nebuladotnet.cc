//------------------------------------------------------------------------------
//  nebuladotnet.cc
//  This file is licensed under the terms of the Nebula License.
//  (C) 2003 Alton Goerby aka PerfectCell.
//------------------------------------------------------------------------------
#using <mscorlib.dll>
#include "dotnet/nebuladotnet.h"
#include "kernel/nclass.h"
#include "kernel/ncmd.h"
#include "kernel/nkernelserver.h"
#include "kernel/nroot.h"

void QuickInit(void)
{
    //create minimal Nebula runtime
    nKernelServer* kernelServer = new nKernelServer;
}

void CharsFromString(nString& dst, System::String* src)
{
    using namespace System::Runtime::InteropServices;
    const char* chars = (const char*)(Marshal::StringToHGlobalAnsi(src)).ToPointer();
    dst += chars;
    Marshal::FreeHGlobal(System::IntPtr((void*)chars));
}

Nebula2::NebulaObject* Nebula2::NebulaObject::Clone()
{
    return static_cast<NebulaObject*>(this->MemberwiseClone());
}

void Nebula2::NebulaObject::PackParamToArg(System::Object* param, nArg* arg)
{
    n_assert(param && arg);

    System::Type* type = param->GetType();
    n_assert(type);
    System::TypeCode typeCode = System::Type::GetTypeCode(type);

    switch(typeCode)
    {
    case System::TypeCode::Object: 
        {
            //param could be a list of arbitrary objects or a single nebula derived object
            NebulaObject* obj = dynamic_cast<NebulaObject*>(param);
            if(obj)
            {
                //its a nroot derived object
                arg->SetO((void*)obj->_ptr);
            }
            else
            {
                //it must be a list(System::Object* array)
                System::Object* params[] = static_cast<System::Object*[]>(param);
                n_assert(params); //params.Length can be 0
                nArg* args = new nArg[params->Length]; //dtor of nArg frees mem
                arg->SetL(args, params->Length); //SetL does not internally realloc args
                for(int i = 0; i < params->Length; i++)    
                    this->PackParamToArg(params[i], &args[i]);
            }
        }
        break;
    case System::TypeCode::String:
        {
            System::String* str = static_cast<System::String*>(param);    
            n_assert(str);
            nString s;
            CharsFromString(s, str);
            arg->SetS(s.Get());
        }
        break;
    case System::TypeCode::Decimal:
        {
            System::Decimal val = *static_cast<__box System::Decimal*>(param);
            arg->SetF((float)val);
        }
        break;
    case System::TypeCode::Int32:
        {
            System::Int32 val = *static_cast<__box System::Int32*>(param);
            arg->SetI((int)val);
        }
        break;
    case System::TypeCode::Boolean:
        {
            System::Boolean val = *static_cast<__box System::Boolean*>(param);
            arg->SetB((bool)val);
        }
        break;
    default:
        n_assert(0); // we should never get here
    }
}
System::Object* Nebula2::NebulaObject::UnpackArgToRetVal(nArg* arg)
{
    n_assert(arg);
    System::Object* retval = 0;

    switch(arg->GetType())
    {
    case arg->ARGTYPE_LIST:
        {
            nArg* args;
            int len = arg->GetL(args);
            if(len == 0) return __gc new System::Object*[0]; //empty list, possible, no error
            
            n_assert(args);

            System::Object* array[] = __gc new System::Object*[len];
            n_assert(array);

            for(int i = 0; i < len; i++)
                array[i] = this->UnpackArgToRetVal(&args[i]);

            retval = array;
        }
        break;
    case arg->ARGTYPE_OBJECT:
        {
            nRoot* nobj = (nRoot*)arg->GetO();
            if(!nobj) return retval;//null pointer, possible, no error

            const char* name = nobj->GetClass()->GetName();
            n_assert(name);
            System::String* clname = new System::String(name);
            n_assert(clname);
            System::Collections::Hashtable* h = NebulaObject::Table;
            n_assert(h);
            System::Object* obj = h->Item[clname];
            n_assert(obj);
            NebulaObject* inst = static_cast<NebulaObject*>(obj);
            n_assert(inst);
            inst = inst->Clone();
            n_assert(inst);
            //set noh and ptr fields
            inst->_ptr = (uint)nobj;
            char buf[N_MAXPATH];
            nobj->GetFullName(buf, N_MAXPATH);
            inst->_noh = new System::String(buf);
            retval = inst;
        }
        break;
    case arg->ARGTYPE_STRING:
        {
            retval = new System::String(arg->GetS());
        }
        break;
    case arg->ARGTYPE_BOOL:
        {
            retval = __box(arg->GetB());
        }
        break;
    case arg->ARGTYPE_INT:
        {
            retval = __box(arg->GetI());
        }
        break;
    case arg->ARGTYPE_FLOAT:
        {
            retval = __box(arg->GetF());
        }
        break;
    default:
        n_assert(0);//we never should get here
    
    }

    return retval;
}

System::Object* Nebula2::NebulaObject::InvokeNebulaCmd(System::UInt32 fourcc,
    [System::ParamArray]System::Object* params[])
{
    n_assert(this)
    nRoot* nobj = (nRoot*)this->_ptr; //better to use noh name?
    nClass* ncls = nobj->GetClass();
    nCmdProto* cmdProto = ncls->FindCmdById((uint)fourcc);
    if(!cmdProto) return 0; //better throw an exception
    nCmd* cmd = cmdProto->NewCmd();
    n_assert(cmd);

    //pack params into cmd in_args, if any
    int numInArgs = cmdProto->GetNumInArgs();
    if(numInArgs > 0)
    {
        n_assert(params);
        int len = params->Length;
        n_assert(numInArgs == len);
        for(int i = 0; i < len; i++)    
            this->PackParamToArg(params[i], cmd->In());
    }

    //execute cmd and rewind on success
    bool success = nobj->Dispatch(cmd);
    if(!success) return 0; //better throw an exception
    cmd->Rewind();

    //unpack cmd out_args into retval, if any
    System::Object* retval = 0;
    int numOutArgs = cmdProto->GetNumOutArgs();
    if(numOutArgs > 0)
    {
        if(numOutArgs == 1)
            retval = this->UnpackArgToRetVal(cmd->Out());
        else
        {
            //multiple out_args
            int len = numOutArgs;
            System::Object* array[] = __gc new System::Object* __gc[len];
            n_assert(array);
            for(int i = 0; i < len; i++)
                array[i] = this->UnpackArgToRetVal(cmd->Out());
        }
    }
    
    cmd->Rewind();
    cmdProto->RelCmd(cmd);

    return retval;
}

void Nebula2::NebulaObject::CreateOrGetNebulaObject(System::String* name)
{
    nKernelServer* kernelServer = nKernelServer::ks;
    if(!kernelServer) 
    { 
        QuickInit(); 
        kernelServer = nKernelServer::ks; 
    }
    n_assert(kernelServer);
    n_assert(this);
    if(!name)
        throw new System::ArgumentNullException(S"name");

    nString nohName;
    CharsFromString(nohName, name);

    System::Type* type = this->GetType();
    n_assert(type);
    System::String* s = type->Name;
    n_assert(s);
    nString typeName;
    CharsFromString(typeName, s);
    nClass* ncls = kernelServer->FindClass(typeName.Get());
    n_assert(ncls);

    //does name exist ?
    nRoot* nobj = kernelServer->Lookup(nohName.Get());
    if(nobj)
    {
        //check if types match
        if(!nobj->IsA(ncls))
            throw new System::Exception(S"Wrapper type does not match Nebula type"); 
    }
    else
    {
        //create new nebula(c++) object
        nobj = kernelServer->NewNoFail(typeName.Get(),nohName.Get());
        if(!nobj)
            throw new System::Exception(S"Failed to create Nebula object"); 
    }

    this->_ptr = (uint)nobj;
    char buf[N_MAXPATH];
    nobj->GetFullName(buf, N_MAXPATH);
    this->_noh = new System::String(buf);
}