//------------------------------------------------------------------------------
//  nebuladotnet.h
//  This file is licensed under the terms of the Nebula License.
//  (C) 2003 Alton Goerby aka PerfectCell.
//------------------------------------------------------------------------------

#ifndef _NEBULA_DOTNET_H
#define _NEBULA_DOTNET_H
#ifdef __cplusplus
extern "C" {
#endif

    class  nArg;
    namespace Nebula2NET
    {
    public __gc __abstract class NebulaObject : public System::Object
        {
        protected:
            NebulaObject()
            {
            }
        public:
            NebulaObject(System::String* name)
            {
                this->CreateOrGetNebulaObject(name);
            }
        public:
            static System::Collections::Hashtable* Table;
        private:
            System::UInt32 _ptr;
            System::String* _noh;
        protected:
            __property System::String* get_Noh(){return this->_noh;}
            __property System::UInt32 get_Ptr(){return this->_ptr;}
        private:
            void PackParamToArg(System::Object* param, nArg* arg);
            System::Object* UnpackArgToRetVal(nArg* arg);
            NebulaObject* Clone();
        protected:
            System::Object* InvokeNebulaCmd(unsigned int fourcc,
                [System::ParamArray]System::Object* params[]);
            void CreateOrGetNebulaObject(System::String* name);
        };
    }

#ifdef __cplusplus
}
#endif
#endif