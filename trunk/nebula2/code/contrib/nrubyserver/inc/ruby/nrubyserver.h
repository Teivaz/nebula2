#ifndef N_RUBYSERVER_H
#define N_RUBYSERVER_H
//--------------------------------------------------------------------
//  nrubycmds.cc --  implements ruby command extensions 
//                  
//  (C) 2004 Thomas Miskiewicz see 3di_license.txt for usage
//--------------------------------------------------------------------

//--------------------------------------------------------------------
/** 
    @class nRubyServer
    @ingroup ScriptServices 

    @brief wraps around Ruby interpreter

    Implements an nScriptServer that understands Ruby, extended 
    by a few Nebula specific Ruby commands and the ability
    to route script cmds to Nebula C++ objects. 

    The nRubyServer understands the following commands as seen with the standard 
    tcl script interpreter in nsh.txt

    -new "nodeclass", "nodename" 
        names must be strings for ruby and delimited parameters 
    -dir
        returns a list of subnodes for puts output see startup.rb example
    -exists "nodename"
    -delete "nodename"
    -sel "nodename" 
    -psel "nodename"
    -puts 
        

    -TODO
        Client-Server behavior, ruby extensions like  tcl/tk

    -18-Dec-03   Tom    created 
    -10-Feb-04   Tom    ported to nebula 2
*/
//--------------------------------------------------------------------
#include <ruby.h>
#include <rubyio.h> 

#ifndef N_KERNELSERVER_H
#include "kernel/nkernelserver.h"
#endif

#ifndef N_SCRIPTSERVER_H
#include "kernel/nscriptserver.h"
#endif

#ifndef N_AUTOREF_H 
#include "kernel/nautoref.h"
#endif

#undef N_DEFINES
#define N_DEFINES nRubyServer
#include "kernel/ndefdllclass.h"

//--------------------------------------------------------------------
class nFileServer2; 
class nRubyServer : public nScriptServer {
public: 
    static nClass *local_cl;
    static nKernelServer *kernelServer; 
    //if true signal from ruby was sent to call setquitrequested
    static bool finished;
        
    nAutoRef<nFileServer2> refFileServer;
    long indent_level;
    char indent_buf[N_MAXPATH]; 
    bool print_error;

public: 
    nRubyServer();
    virtual ~nRubyServer(); 
    
    virtual bool Run(const char *, const char*&);
    virtual bool RunScript(const char *, const char*&); 
    /// Not tested so far where is it used anyway ? 
    virtual bool RunCommand(nCmd *);
    virtual char *Prompt(char *, int);
    
    virtual nFile* BeginWrite(const char* filename, nRoot* obj);  
    virtual bool WriteComment(nFile *, const char *);
    virtual bool WriteBeginNewObject(nFile *, nRoot *, nRoot *);
    virtual bool WriteBeginNewObjectCmd(nFile *, nRoot *, nRoot *, nCmd *); 
    virtual bool WriteBeginSelObject(nFile *, nRoot *, nRoot *);
    virtual bool WriteCmd(nFile *, nCmd *); 
    virtual bool WriteEndObject(nFile *, nRoot *, nRoot *); 
    virtual bool EndWrite(nFile *); 
    
    virtual bool Trigger(void); 
    /// sets finished to true and hereby requests a shutdown of the nrubyserver 
    static void Terminate(void);
private:
    void write_select_statement(nFile* file, nRoot *o, nRoot *owner);
    /// registers a library search path with ruby from an existing nebula assign
    void register_manglepath(const char*);
};
//--------------------------------------------------------------------
#endif
