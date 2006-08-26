#ifndef N_CONSERVER_H
#define N_CONSERVER_H
//------------------------------------------------------------------------------
/**
    @class nConServer
    @ingroup NebulaConsoleServer
    @brief Implements the Nebula ingame console.

    Must live under the name /sys/servers/console in the Nebula object
    hierarchy.

    See also @ref N2ScriptInterface_nconserver

    (C) 2003 RadonLabs GmbH
*/
#include "kernel/ntypes.h"
#include "util/nlinebuffer.h"
#include "kernel/nroot.h"
#include "kernel/nautoref.h"
#include "input/ninputevent.h"

class nInputServer;
class nGfxServer2;
class nScriptServer;

//------------------------------------------------------------------------------
class nConServer : public nRoot
{
public:
    /// constructor
    nConServer();
    /// destructor
    virtual ~nConServer();
    /// return instance pointer
    static nConServer* Instance();
    /// per-frame trigger method
    void Trigger();
    /// render the console
    void Render();
    /// open the console
    void Open();
    /// close the console
    void Close();
    /// toggle the console
    void Toggle();
    /// watch a debug watcher variable
    void Watch(const char* watchName);
    /// unwatch all watcher variables
    void Unwatch();

private:
    /// process input events
    bool EditLine(nInputEvent* inputEvent);
    /// add input buffer to command history
    void AddCommandToHistory();
    /// execute command in input buffer
    void ExecuteCommand();
    /// recall the previous command
    void RecallPrevCmd();
    /// recall the next command
    void RecallNextCmd();
    /// reset the command history
    void ResetHistory();
    /// render the console
    void RenderConsole(int displayHeight, int fontHeight);
    /// render watcher variables
    void RenderWatchers(int displayHeight, int fontHeight);
    /// insert a character into the input buffer
    void EditInsertChar(char c);
    /// move the cursor one position to the left
    void EditCursorLeft();
    /// move the cursor one position to the right
    void EditCursorRight();
    /// move the cursor one word to the left
    void EditWordLeft();
    /// move the cursor one word to the right
    void EditWordRight();
    /// delete the character to the right of the cursor
    void EditDeleteRight();
    /// delete the character to the left of the cursor
    void EditDeleteLeft();
    /// scroll up one line
    void EditScrollUp();
    /// scroll down one line
    void EditScrollDown();
    /// move cursor to beginning of line
    void EditHome();
    /// move cursor to end of line
    void EditEnd();
    /// check if given input key is in kill set
    bool KeyIsInKillSet(int key);

private:
    static nConServer* Singleton;

    nAutoRef<nInputServer>  refInputServer;
    nAutoRef<nScriptServer> refScriptServer;
    nClass *envClass;

    bool consoleOpen;
    bool watchersOpen;

    int historyIndex;           // points to current history entry
    nLineBuffer historyBuffer;

    int cursorPos;              // current cursor position in inputBuffer
    bool overstrike;            // overstrike on/off
    bool ctrlDown;              // control key down?
    int scrollOffset;           // current scroll offset
    nString watchPattern;
    char inputBuffer[N_MAXPATH];
};

//------------------------------------------------------------------------------
/**
*/
inline
nConServer*
nConServer::Instance()
{
    n_assert(Singleton);
    return Singleton;
}

//------------------------------------------------------------------------------
#endif
