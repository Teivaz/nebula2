#define N_IMPLEMENTS nConServer
//-------------------------------------------------------------------
//  ncsrv_main.cc
//  (C) 1999 A.Weissflog
//-------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "kernel/nenv.h"
#include "gfx2/ngfxserver2.h"
#include "input/ninputevent.h"
#include "input/ninputserver.h"
#include "misc/nconserver.h"
#include "kernel/nscriptserver.h"
#include "kernel/nloghandler.h"

nNebulaScriptClass(nConServer, "nroot");

// all keys which may not be routed to the application if the console is open
static int keyKillSet[] = {
    N_KEY_BACK,
    N_KEY_RETURN,
    N_KEY_SHIFT,
    N_KEY_CONTROL,
    N_KEY_SPACE,
    N_KEY_END,
    N_KEY_HOME,
    N_KEY_LEFT,
    N_KEY_UP,
    N_KEY_RIGHT,
    N_KEY_DOWN,
    N_KEY_INSERT,
    N_KEY_DELETE,
    N_KEY_1,
    N_KEY_2,
    N_KEY_3,
    N_KEY_4,
    N_KEY_5,
    N_KEY_6,
    N_KEY_7,
    N_KEY_8,
    N_KEY_9,
    N_KEY_0,
    N_KEY_A,
    N_KEY_B,
    N_KEY_C,
    N_KEY_D,
    N_KEY_E,
    N_KEY_F,
    N_KEY_G,
    N_KEY_H,
    N_KEY_I,
    N_KEY_J,
    N_KEY_K,
    N_KEY_L,
    N_KEY_M,
    N_KEY_N,
    N_KEY_O,
    N_KEY_P,
    N_KEY_Q,
    N_KEY_R,
    N_KEY_S,
    N_KEY_T,
    N_KEY_U,
    N_KEY_V,
    N_KEY_W,
    N_KEY_X,
    N_KEY_Y,
    N_KEY_Z,
    N_KEY_NUMPAD0,
    N_KEY_NUMPAD1,
    N_KEY_NUMPAD2,
    N_KEY_NUMPAD3,
    N_KEY_NUMPAD4,
    N_KEY_NUMPAD5,
    N_KEY_NUMPAD6,
    N_KEY_NUMPAD7,
    N_KEY_NUMPAD8,
    N_KEY_NUMPAD9,
    N_KEY_MULTIPLY,
    N_KEY_ADD,
    N_KEY_SEPARATOR,
    N_KEY_SUBTRACT,
    N_KEY_DECIMAL,
    N_KEY_DIVIDE,
    N_KEY_NONE
};

//------------------------------------------------------------------------------
/**
*/
nConServer::nConServer() :
    refInputServer("/sys/servers/input"),
    refGfxServer("/sys/servers/gfx"),
    refScriptServer("/sys/servers/script"),
    consoleOpen(false),
    watchersOpen(false),
    historyIndex(0),
    cursorPos(0),
    overstrike(false),
    ctrlDown(false),
    scrollOffset(0)
{
    envClass = kernelServer->FindClass("nenv");
    memset(this->inputBuffer, 0, sizeof(this->inputBuffer));
}

//------------------------------------------------------------------------------
/**
*/
nConServer::~nConServer()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nConServer::Open()
{
    this->consoleOpen = true;
}

//------------------------------------------------------------------------------
/**
*/
void
nConServer::Close()
{
    this->consoleOpen = false;
}

//------------------------------------------------------------------------------
/**
*/
void
nConServer::Toggle()
{
    if (this->consoleOpen)
    {
        this->Close();
    }
    else
    {
        this->Open();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nConServer::Watch(const char* pattern)
{
    this->watchersOpen = true;
    this->watchPattern = pattern;
}

//------------------------------------------------------------------------------
/**
*/
void
nConServer::Unwatch()
{
    this->watchersOpen = false;
}

//------------------------------------------------------------------------------
/**
    Render the console to the gfx server.
*/
void
nConServer::RenderConsole(int displayHeight, int fontHeight)
{
    n_assert(this->consoleOpen);
    n_assert(fontHeight > 0);
    n_assert(displayHeight > 0);

    // get pointer to the kernel server's line buffer entries
    nLineBuffer* lineBuffer = kernelServer->GetLogHandler()->GetLineBuffer();
    if (!lineBuffer)
    {
        // the current log handler supports no line buffer, so
        // there is nothing to render
        return;
    }
    const int maxLines = 512;
    const char* lineArray[maxLines];
    const int safeBottom = 4;   // 4 lines safety zone
    int numLines = lineBuffer->GetLines(lineArray, maxLines);

    // compute num lines fitting on screen, and the first and last visible line
    int maxLinesOnScreen = (displayHeight / int(fontHeight * 1.6f)) - safeBottom;
    int firstLine = maxLinesOnScreen + this->scrollOffset;
    if (firstLine >= numLines)
    {
        firstLine = numLines - 1;
        this->scrollOffset = firstLine - maxLinesOnScreen;
    }
    int lastLine = firstLine - maxLinesOnScreen;
    if (lastLine < 0)
    {
        this->scrollOffset = 0;
        lastLine = 0;
    }

    // render buffer lines
    nGfxServer2* gfxServer = this->refGfxServer.get();
    char line[1024];
    int i;
    float xPos = -1.0f;
    float yPos = -1.0f;
    float dy   = 2.0f / (maxLinesOnScreen + safeBottom);
    int curLine;
    for (curLine = firstLine; curLine >= lastLine; curLine--)
    {
        n_strncpy2(line, lineArray[curLine], sizeof(line) - 3);
        n_strcat(line, "\n", sizeof(line));
        gfxServer->Text(line, xPos, yPos);
        yPos += dy;
    }

    // finally render the current edit line
    int lenInputBuffer = strlen(this->inputBuffer);

    // start final line with prompt from script server
    this->refScriptServer->Prompt(line, sizeof(line));
    char* to = line + strlen(line);
    const char* from = this->inputBuffer;

    // copy until cursor pos
    for (i = 0; i < this->cursorPos; i++)
    {
        *to++ = *from++;
    }

    // insert cursor
    *to++ = '|';

    // copy the rest of the line
    while ((*to++ = *from++));

    // and render it
    gfxServer->Text(line, xPos, yPos);
}

//------------------------------------------------------------------------------
/**
    Render the watcher variables matching the watcher pattern.
*/
void
nConServer::RenderWatchers(int displayHeight, int fontHeight)
{
    n_assert(this->watchersOpen);
    n_assert(this->envClass);
    n_assert(fontHeight > 0);
    n_assert(displayHeight > 0);

    nRoot* watcherVars = kernelServer->Lookup("/sys/var");
    if (watcherVars)
    {
        // compute num lines fitting on screen, and the first and last visible line
        int maxLinesOnScreen = ((displayHeight * 4) / 5) / fontHeight;

        // for each watcher variable
        nGfxServer2* gfxServer = this->refGfxServer.get();
        char line[N_MAXPATH];
        float xPos = -1.0f;
        float yPos = -1.0f;
        float dy   = 2.0f / float(maxLinesOnScreen);
        nEnv* curVar;
        for (curVar = (nEnv*) watcherVars->GetHead(); curVar; curVar = (nEnv*) curVar->GetSucc())
        {
            if (curVar->IsA(this->envClass))
            {
                const char* varName = curVar->GetName();
                if (n_strmatch(varName, this->watchPattern.Get()))
                {
                    switch (curVar->GetType())
                    {
                        case nArg::ARGTYPE_INT:
                            sprintf(line,"%s: %d\n", varName, curVar->GetI());
                            break;

                        case nArg::ARGTYPE_FLOAT:
                            sprintf(line,"%s: %f\n", varName, curVar->GetF());
                            break;

                        case nArg::ARGTYPE_STRING:
                            sprintf(line,"%s: %s\n", varName, curVar->GetS());
                            break;

                        case nArg::ARGTYPE_BOOL:
                            sprintf(line,"%s: %s\n", varName, curVar->GetB() ? "true" : "false");
                            break;

                        default:
                            sprintf(line,"%s: <unknown data type>\n", varName);
                            break;
                    }
                    gfxServer->Text(line, xPos, yPos);
                    yPos += dy;
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    Main render method of the console server. Render either nothing, the
    command console, or any active watcher variables.
*/
void
nConServer::Render()
{
    int displayHeight = this->refGfxServer->GetDisplayMode().GetHeight();
    const int fontHeight = 10;

    if (this->consoleOpen)
    {
        this->RenderConsole(displayHeight, fontHeight);
    }
    else if (this->watchersOpen)
    {
        this->RenderWatchers(displayHeight, fontHeight);
    }
}

//------------------------------------------------------------------------------
/**
    Insert a char into the input buffer, and advance cursor position. 
*/
void
nConServer::EditInsertChar(char c)
{
    const int inputBufferSize = sizeof(this->inputBuffer);    
    if (this->overstrike)
    {
        // overstrike mode
        if ((0 == this->inputBuffer[this->cursorPos]) && (this->cursorPos < (inputBufferSize - 1)))
        {
            this->inputBuffer[this->cursorPos + 1] = 0;
        }
        this->inputBuffer[this->cursorPos] = c;
    }
    else
    {
        // insert mode
        char* moveFrom = this->inputBuffer + this->cursorPos;
        char* moveTo   = moveFrom + 1;
        int moveCount  = inputBufferSize - (this->cursorPos + 1); // preserve trailing 0
        memmove(moveTo, moveFrom, moveCount);
        this->inputBuffer[this->cursorPos] = c;
    }
    this->cursorPos++;
    if (this->cursorPos >= inputBufferSize)
    {
        this->cursorPos = inputBufferSize - 1;
        this->inputBuffer[this->cursorPos] = 0;
    }
}

//------------------------------------------------------------------------------
/**
    Move the cursor position one to the left.
*/
void
nConServer::EditCursorLeft()
{
    this->cursorPos--;
    if (this->cursorPos < 0)
    {
        this->cursorPos = 0;
    }
}

//------------------------------------------------------------------------------
/**
    Move the cursor position one to the right.
*/
void
nConServer::EditCursorRight()
{
    this->cursorPos++;
    int len = strlen(this->inputBuffer);
    if (this->cursorPos > len)
    {
        this->cursorPos = len;
    }
}

//------------------------------------------------------------------------------
/**
    Move the cursor to the beginning of the previous word.
*/
void
nConServer::EditWordLeft()
{
    while ((--this->cursorPos >= 0) && this->inputBuffer[this->cursorPos] == ' ');
    while ((--this->cursorPos >= 0) && this->inputBuffer[this->cursorPos] != ' ');
    this->cursorPos++;
    if (this->cursorPos < 0)
    {
        this->cursorPos = 0;
    }
}

//------------------------------------------------------------------------------
/**
    Move the cursor to the beginning of the next word.
*/
void
nConServer::EditWordRight()
{
    int len = strlen(this->inputBuffer);
    while ((++this->cursorPos < len) && this->inputBuffer[this->cursorPos] == ' ');
    while ((++this->cursorPos < len) && this->inputBuffer[this->cursorPos] != ' ');
    while ((++this->cursorPos < len) && this->inputBuffer[this->cursorPos] == ' ');
    if (this->cursorPos > len)
    {
        this->cursorPos = len;
    }
}

//------------------------------------------------------------------------------
/**
    Delete the character to the right of the cursor.
*/
void
nConServer::EditDeleteRight()
{
    char* moveTo   = this->inputBuffer + this->cursorPos;
    char* moveFrom = moveTo + 1;
    int moveCount = sizeof(this->inputBuffer) - (this->cursorPos + 1);
    memmove(moveTo, moveFrom, moveCount);
}

//------------------------------------------------------------------------------
/**
    Delete the character to the left of the cursor.
*/
void
nConServer::EditDeleteLeft()
{
    if (this->cursorPos > 0)
    {
        --this->cursorPos;
        this->EditDeleteRight();
    }
}

//------------------------------------------------------------------------------
/**
    Return true if the given input key is in the kill set. If yes, the 
    console server must "swallow" the input event, so that the application
    doesn't see it.
*/
bool
nConServer::KeyIsInKillSet(int key)
{
    int i = 0;
    while (keyKillSet[i] != N_KEY_NONE) 
    {
        if (key == keyKillSet[i]) 
        {
            return true;
        }
        ++i;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Scroll up one line.
*/
void
nConServer::EditScrollUp()
{
    ++this->scrollOffset;
}

//------------------------------------------------------------------------------
/**
    Scroll down one line
*/
void
nConServer::EditScrollDown()
{
    --this->scrollOffset;
}

//------------------------------------------------------------------------------
/**
    Move cursor to beginning of line.
*/
void
nConServer::EditHome()
{
    this->cursorPos = 0;
}

//------------------------------------------------------------------------------
/**
    Move cursor to end of line.
*/
void
nConServer::EditEnd()
{
    this->cursorPos = strlen(this->inputBuffer);
}

//------------------------------------------------------------------------------
/**
    Execute the command in the line buffer.
*/
void
nConServer::ExecuteCommand()
{
    this->AddCommandToHistory();

    // print to log
    char line[1024];
    this->refScriptServer->Prompt(line, sizeof(line));
    n_strcat(line, this->inputBuffer, sizeof(line));
    n_strcat(line, "\n", sizeof(line));
    n_printf(line);

    // execute the command
    if (this->inputBuffer[0])
    {
        const char* result;
        bool failOnError = this->refScriptServer->GetFailOnError();
        this->refScriptServer->SetFailOnError(false);
        this->refScriptServer->Run(this->inputBuffer, result);
        this->refScriptServer->SetFailOnError(failOnError);
        if (result && (*result))
        {
            n_printf("%s\n", result);
        }
    }

    // reset edit parameters
    this->inputBuffer[0] = 0;
    this->cursorPos = 0;
    this->scrollOffset = 0;
}

//------------------------------------------------------------------------------
/**
    Takes one input event and edits the line. If the event has been
    processed, the method returns true, in this case, the event should
    be deleted so that the application doesn't see the event (this would
    confuse the the user when the console server is open).
    The line will be sent off to the script server for evaluation
    once the user hits the return key.
*/
bool
nConServer::EditLine(nInputEvent* inputEvent)
{
    bool killEvent = false;

    // handle alphanumeric characters
    if ((inputEvent->GetType() == N_INPUT_KEY_CHAR) &&
        (inputEvent->GetChar() >= ' '))
    {
        this->EditInsertChar(inputEvent->GetChar());
        killEvent = true;
    }

    // handle control keys
    if (inputEvent->GetType() == N_INPUT_KEY_DOWN)
    {
        if (this->KeyIsInKillSet(inputEvent->GetKey()))
        {
            killEvent = true;
        }

        switch (inputEvent->GetKey())
        {
            case N_KEY_LEFT:
                if (this->ctrlDown) this->EditWordLeft();
                else                this->EditCursorLeft();
                break;

            case N_KEY_RIGHT:
                if (this->ctrlDown) this->EditWordRight();
                else                this->EditCursorRight();
                break;

            case N_KEY_BACK:
                this->EditDeleteLeft();
                break;

            case N_KEY_DELETE:
                this->EditDeleteRight();
                break;
            
            case N_KEY_INSERT:
                this->overstrike = !this->overstrike;
                break;

            case N_KEY_UP:
                if (this->ctrlDown) this->EditScrollUp();
                else                this->RecallPrevCmd();
                break;

            case N_KEY_DOWN:
                if (this->ctrlDown) this->EditScrollDown();
                else                this->RecallNextCmd();
                break;

            case N_KEY_RETURN:
                this->ExecuteCommand();
                break;

            case N_KEY_CONTROL:
                this->ctrlDown = true;
                break;

            case N_KEY_HOME:
                this->EditHome();
                break;

            case N_KEY_END:
                this->EditEnd();
                break;

            default:
                break;
        }
    }
    else if (inputEvent->GetType() == N_INPUT_KEY_UP)
    {
        if (this->KeyIsInKillSet(inputEvent->GetKey()))
        {
            killEvent = true;
        }
        switch (inputEvent->GetKey())
        {
            case N_KEY_CONTROL:
                this->ctrlDown = false;
                break;

            default:
                break;
        }
    }
    return killEvent;
}

//------------------------------------------------------------------------------
/**
    Add the command in the input buffer to the command history.
*/
void
nConServer::AddCommandToHistory()
{
    this->ResetHistory();
    if (this->inputBuffer[0])
    {
        this->historyBuffer.Put(this->inputBuffer);
        this->historyBuffer.Put("\n");
    }
}

//------------------------------------------------------------------------------
/**
    Reset the command history position.
*/
void
nConServer::ResetHistory()
{
    this->historyIndex = 0;
}

//------------------------------------------------------------------------------
/**
    Recall the previous command from the history buffer.
*/
void 
nConServer::RecallPrevCmd()
{
    ++this->historyIndex;

    int i;
    int actLine = this->historyBuffer.GetHeadLine();
    for (i = 0; i < this->historyIndex; i++)
    {
        int prevLine = this->historyBuffer.GetPrevLine(actLine);
        if (-1 == prevLine)
        {
            --this->historyIndex;
            break;
        }
        actLine = prevLine;
    }

    const char* historyCommand = this->historyBuffer.GetLine(actLine);
    if (historyCommand)
    {
        strcpy(this->inputBuffer, historyCommand);
        this->EditEnd();
    }
}

//------------------------------------------------------------------------------
/**
    Recall the next command from the history buffer.
*/
void 
nConServer::RecallNextCmd()
{
    --this->historyIndex;
    if (this->historyIndex < 0)
    {
        this->historyIndex = 0;
    }

    int i;
    int actLine = this->historyBuffer.GetHeadLine();
    for (i = 0; i < this->historyIndex; i++)
    {
        int prevLine = this->historyBuffer.GetPrevLine(actLine);
        if (-1 == prevLine)
        {
            ++this->historyIndex;
            break;
        }
        actLine = prevLine;
    }

    const char* historyCommand = this->historyBuffer.GetLine(actLine);
    if (historyCommand)
    {
        strcpy(this->inputBuffer, historyCommand);
        this->EditEnd();
    }
}

//------------------------------------------------------------------------------
/**
    Trigger the console server. Due to a chicken-egg problem this method
    is called from inside nInputServer::Trigger().
    Don't forget to actually render the console by calling 
    nConServer::Render() somewhere between nGfxServer::BeginScene()/EndScene().
*/
void
nConServer::Trigger()
{
    if (this->consoleOpen)
    {
        // parse input events...
        nInputServer* inputServer = this->refInputServer.get();
        nInputEvent* inputEvent = inputServer->FirstEvent();
        if (inputEvent)
        {
            nInputEvent* nextInputEvent;
            do
            {
                nextInputEvent = inputServer->NextEvent(inputEvent);
                if (this->EditLine(inputEvent))
                {
                    inputEvent->SetDisabled(true);
                }
            }
            while ((inputEvent = nextInputEvent));
        }
    }
}

//------------------------------------------------------------------------------
//  EOF
//------------------------------------------------------------------------------
