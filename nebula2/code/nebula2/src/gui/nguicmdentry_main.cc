//------------------------------------------------------------------------------
//  nguicmdentry_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguicmdentry.h"
#include "util/nlinebuffer.h"
#include "kernel/nloghandler.h"
#include "util/nstring.h"

nNebulaClass(nGuiCmdEntry, "nguitextview");

//------------------------------------------------------------------------------
/**
*/
nGuiCmdEntry::nGuiCmdEntry() :
    refScriptServer("/sys/servers/script"),
    editLine(1024),
    ctrlDown(false),
    historyCursor(0),
    cmdHistory(16, 16)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nGuiCmdEntry::~nGuiCmdEntry()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Initialize the internal line array from the kernel server's log buffer.
*/
void
nGuiCmdEntry::UpdateLineArray()
{
    // get pointer to the kernel server's line buffer entries
    nLineBuffer* lineBuffer = kernelServer->GetLogHandler()->GetLineBuffer();
    if (!lineBuffer)
    {
        // the current log handler supports no line buffer, so
        // there is nothing to render
        return;
    }

    // get max number of visible lines
    int maxLines = (this->GetNumVisibleLines() - 1);
    if (0 >= maxLines)
    {
        return;
    }
    const char* lines[512];
    n_assert(maxLines < 512);
    int numLines = lineBuffer->GetLines(lines, maxLines);

    // fill lines
    int i;
    for (i = (numLines - 1); i >= 0; --i)
    {
        this->AppendLine(lines[i]);
    }
}

//------------------------------------------------------------------------------
/**
    Append the current prompt and command input to the line array.
*/
void
nGuiCmdEntry::AppendCmdLine()
{
    kernelServer->PushCwd(this->GetCwd());
    nString cmdLine = this->refScriptServer->Prompt();
    kernelServer->PopCwd();
    int cursorIndex = this->editLine.GetCursorPos();
    nString lineText = this->editLine.GetContent();
    nString textToCursor = lineText.ExtractRange(0, cursorIndex);
    nString textFromCursor = lineText.ExtractRange(cursorIndex, lineText.Length() - cursorIndex);

    if (!textToCursor.IsEmpty())
    {
        cmdLine.Append(textToCursor);
    }
    cmdLine.Append("|");
    if (!textFromCursor.IsEmpty())
    {
        cmdLine.Append(textFromCursor);
    }
    this->AppendLine(cmdLine);
}

//------------------------------------------------------------------------------
/**
    Add the current command to the history buffer.
*/
void
nGuiCmdEntry::AddCommandToHistory()
{
    if (!this->editLine.IsEmpty())
    {
        this->cmdHistory.Append(this->editLine.GetContent());
        this->historyCursor = this->cmdHistory.Size();
    }
}

//------------------------------------------------------------------------------
/**
    Recall the next command in the command history.
*/
void
nGuiCmdEntry::RecallNextCommand()
{
    const int size = this->cmdHistory.Size();
    if (size > 0)
    {
        if (++this->historyCursor >= size)
        {
            this->historyCursor = size - 1;
        }
        this->editLine.SetContent(this->cmdHistory[this->historyCursor].Get());
        this->editLine.CursorEnd();
    }
}

//------------------------------------------------------------------------------
/**
    Recall the previous command in the command history.
*/
void
nGuiCmdEntry::RecallPrevCommand()
{
    if (this->cmdHistory.Size() > 0)
    {
        if (--this->historyCursor < 0)
        {
            this->historyCursor = 0;
        }
        this->editLine.SetContent(this->cmdHistory[this->historyCursor].Get());
        this->editLine.CursorEnd();
    }
}

//------------------------------------------------------------------------------
/**
    Execute the command in the editLine buffer.
*/
void
nGuiCmdEntry::ExecuteCommand()
{
    nScriptServer* scriptServer = this->refScriptServer.get();

    // make local cwd global
    kernelServer->PushCwd(this->GetCwd());

    // log the current command
    nString line = this->refScriptServer->Prompt();
    line.Append(this->editLine.GetContent());
    line.Append("\n");
    n_printf(line.Get());

    // and run the command through the script server
    nString result = 0;
    bool failOnError = scriptServer->GetFailOnError();
    scriptServer->SetFailOnError(false);
    scriptServer->Run(this->editLine.GetContent(), result);
    scriptServer->SetFailOnError(failOnError);
    if (result.IsValid())
    {
        n_printf("%s\n", result.Get());
    }
    this->AddCommandToHistory();
    this->editLine.ClearContent();

    // set new local cwd
    nRoot* newCwd = kernelServer->GetCwd();
    if (newCwd != this->GetCwd())
    {
        this->SetCwd(kernelServer->GetCwd());
        this->UpdateTabComplete();
    }

    // restore previous cwd
    kernelServer->PopCwd();
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiCmdEntry::OnShow()
{
    // update local cwd
    this->SetCwd(kernelServer->GetCwd());
    this->UpdateTabComplete();
    nGuiTextView::OnShow();
}

//------------------------------------------------------------------------------
/**
    Called per frame, this feeds the N latest lines from the command
    buffer into the parent class.
*/
void
nGuiCmdEntry::OnFrame()
{
    this->BeginAppend();
    this->UpdateLineArray();
    if (this->HasFocus())
    {
        this->AppendCmdLine();
    }
    this->EndAppend();
    nGuiTextView::OnFrame();
}

//------------------------------------------------------------------------------
/**
    Handles character input.
*/
void
nGuiCmdEntry::OnChar(uchar charCode)
{
    if (this->editLine.IsCharValid(charCode))
    {
        this->editLine.InsertChar(charCode);
        this->UpdateTabComplete();
    }
    nGuiTextView::OnChar(charCode);
}

//------------------------------------------------------------------------------
/**
    Handles input of raw keys.
*/
bool
nGuiCmdEntry::OnKeyDown(nKey key)
{
    bool handled = false;
    switch (key)
    {
        case N_KEY_BACK:
            this->editLine.DeleteLeft();
            this->UpdateTabComplete();
            handled = true;
            break;

        case N_KEY_RETURN:
            this->ExecuteCommand();
            this->UpdateTabComplete();
            handled = true;
            break;

        case N_KEY_LEFT:
            if (this->ctrlDown) this->editLine.CursorWordLeft();
            else                this->editLine.CursorLeft();
            this->UpdateTabComplete();
            handled = true;
            break;

        case N_KEY_RIGHT:
            if (this->ctrlDown) this->editLine.CursorWordRight();
            else                this->editLine.CursorRight();
            this->UpdateTabComplete();
            handled = true;
            break;

        case N_KEY_UP:
            this->RecallPrevCommand();
            this->UpdateTabComplete();
            handled = true;
            break;

        case N_KEY_DOWN:
            this->RecallNextCommand();
            this->UpdateTabComplete();
            handled = true;

        case N_KEY_INSERT:
            this->editLine.ToggleOverstrike();
            handled = true;
            break;

        case N_KEY_DELETE:
            this->editLine.Delete();
            this->UpdateTabComplete();
            handled = true;
            break;

        case N_KEY_HOME:
            this->editLine.CursorHome();
            this->UpdateTabComplete();
            handled = true;
            break;

        case N_KEY_END:
            this->editLine.CursorEnd();
            this->UpdateTabComplete();
            handled = true;
            break;

        case N_KEY_CONTROL:
            this->ctrlDown = true;
            break;

        case N_KEY_TAB:
            this->DoTabCompletion();
            break;

        default:
            break;
    }
    return handled;
}

//------------------------------------------------------------------------------
/**
    Handles release of raw keys.
*/
void
nGuiCmdEntry::OnKeyUp(nKey key)
{
    switch (key)
    {
        case N_KEY_CONTROL:
            this->ctrlDown = false;
            break;

        default:
            break;
    }
}

//------------------------------------------------------------------------------
/**
    Perform a tab completion.
*/
void
nGuiCmdEntry::DoTabCompletion()
{
    // get the current tab match
    nString match = this->childTabComplete.GetNextMatch();
    if (!match.IsEmpty())
    {
        this->editLine.DeleteWordLeft(" /");
        this->editLine.InsertString(match.Get());
    }
}

//------------------------------------------------------------------------------
/**
    Update the tab completion objects. Must be called whenever the cwd
    changes.
*/
void
nGuiCmdEntry::UpdateTabComplete()
{
    // interpret the word under the cursor as a filename, we're only interested
    // in the last component
    nString wordUnderCursor = this->editLine.GetWordToCursor().Get();
    this->childTabComplete.SetSearchString(wordUnderCursor.ExtractFileName());

    // interpret the word under the cursor as a filename,
    // we're interested in everything but the last component
    nString path = wordUnderCursor.ExtractToLastSlash();

    // feed child objects of local cwd
    this->childTabComplete.ClearCandidates();
    kernelServer->PushCwd(this->GetCwd());
    nRoot* dir;
    if (path.IsEmpty())
    {
        dir = kernelServer->GetCwd();
    }
    else
    {
        dir = kernelServer->Lookup(path.Get());
    }
    if (dir)
    {
        nRoot* child;
        for (child = dir->GetHead(); child; child = child->GetSucc())
        {
            this->childTabComplete.AppendCandidate(child->GetName());
        }
    }
    kernelServer->PopCwd();
}
