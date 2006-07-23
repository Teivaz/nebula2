//------------------------------------------------------------------------------
//  nguitextentry_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguitextentry.h"
#include "gui/nguiserver.h"
#include "resource/nresourceserver.h"
#include "input/ninputserver.h"

nNebulaScriptClass(nGuiTextEntry, "gui::nguitextlabel");

//------------------------------------------------------------------------------
/**
*/
nGuiTextEntry::nGuiTextEntry() :
    refInputServer("/sys/servers/input"),
    mouseOver(false),
    active(false),
    firstFrameActive(false),
    lineEditor(0),
    fileMode(false),
    overstrikeDefault(false),
    passwordMode(false),
    initialCursorPos(Right)
{
    this->SetMaxLength(48);
}

//------------------------------------------------------------------------------
/**
*/
nGuiTextEntry::~nGuiTextEntry()
{
    if (this->lineEditor)
    {
        n_delete(this->lineEditor);
        this->lineEditor = 0;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiTextEntry::SetText(const char* text)
{
    n_assert(text);
    this->lineEditor->SetContent(text);
}

//------------------------------------------------------------------------------
/**
*/
const char*
nGuiTextEntry::GetText() const
{
    // replace with empty text if no content
    const char* content = this->lineEditor->GetContent();
    n_assert(content);
    return content;
}

//------------------------------------------------------------------------------
/**
    @brief Set optional "empty" replacement text
    This will not appear until focus is gained *and lost again*.
    If you want initial default text, use SetText.
*/
void
nGuiTextEntry::SetEmptyText(const char* text)
{
    n_assert(text);
    this->emptyText = text;
}

//------------------------------------------------------------------------------
/**
*/
const char*
nGuiTextEntry::GetEmptyText() const
{
    return this->emptyText.Get();
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiTextEntry::SetOverstrike(bool overstrike)
{
    this->overstrikeDefault = overstrike;
    this->lineEditor->SetOverstrike( overstrike );
}

//------------------------------------------------------------------------------
/**
    @brief Whenever the field is activated, the cursor is reset to this position
    Left means "before the first character"
    Right means "after the last character"
*/
void nGuiTextEntry::SetInitialCursorPos(Alignment pos)
{
    n_assert( pos != Center )
    this->initialCursorPos = pos;
}
//------------------------------------------------------------------------------
/**
    Set max number of chars in text buffer. Discards old text buffer
    content.
*/
void
nGuiTextEntry::SetMaxLength(int l)
{
    if (this->lineEditor)
    {
        n_delete(this->lineEditor);
        this->lineEditor = 0;
    }
    this->lineEditor = n_new(nEditLine(l));
    this->lineEditor->SetOverstrike(this->overstrikeDefault);
}

//------------------------------------------------------------------------------
/**
*/
int
nGuiTextEntry::GetMaxLength() const
{
    n_assert(this->lineEditor);
    return this->lineEditor->GetMaxLineLength();
}

//------------------------------------------------------------------------------
/**
    Check whether the line edit object contains empty text, and if yes,
    replaces it with the empty replacement text.
*/
void
nGuiTextEntry::CheckEmptyText()
{
    const char* content = this->lineEditor->GetContent();
    n_assert(content);
    if (0 == content[0])
    {
        this->lineEditor->SetContent(this->emptyText.Get());
    }
}

//------------------------------------------------------------------------------
/**
    Handle a moused moved event. This checks whether the mouse is over
    the sensitive area, if yes, the mouseOver flag will be set.
*/
bool
nGuiTextEntry::OnMouseMoved(const vector2& mousePos)
{
    if (this->Inside(mousePos))
    {
        this->mouseOver = true;
    }
    else
    {
        this->mouseOver = false;
    }
    return nGuiWidget::OnMouseMoved(mousePos);
}

//------------------------------------------------------------------------------
/**
    Activate the text entry widget, so that text can be entered.
*/
void
nGuiTextEntry::SetActive(bool b)
{
    if (b)
    {
        if (!this->active)
        {
            // FIXME: this may cause trouble when the
            // widget isn't cleanly deactivated for some reason.
            // this->refInputServer->SetMute(true);
            this->active = true;
            this->firstFrameActive = true;
            switch( this->initialCursorPos )
            {
                case Left:  this->lineEditor->CursorHome(); break;
                case Right: this->lineEditor->CursorEnd(); break;
                default:
                    n_error( "Invalid initialCursorPos: %d",
                             this->initialCursorPos );
            }
        }
    }
    else
    {
        // this->refInputServer->SetMute(false);
        this->active = false;
    }
}

//------------------------------------------------------------------------------
/**
    Handle the mouse button down event. If the mouse is currently over
    the text widget, the text widget will become activated so that text
    can be entered. If the mouse is outside the text widget, and the
    text widget was active, it will be deactivated and the callback
    command will be invoked.
*/
bool
nGuiTextEntry::OnButtonDown(const vector2& /*mousePos*/)
{
    if (this->mouseOver)
    {
        this->SetActive(true);
    }
    else
    {
        // if we are currently active, deactivate and call callback hook
        if (this->active)
        {
            this->CheckEmptyText();
            this->OnAction();
            this->SetActive(false);
        }
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Called when the widget is becoming invisible.
*/
void
nGuiTextEntry::OnHide()
{
    if (this->active)
    {
        this->CheckEmptyText();
        this->OnAction();
        this->SetActive(false);
    }
}

//------------------------------------------------------------------------------
/**
    Handles character keyboard input if we are active.
*/
void
nGuiTextEntry::OnChar(uchar charCode)
{
    if (this->active)
    {
        this->lineEditor->SetFileMode(this->GetFileMode());
        this->lineEditor->InsertChar(charCode);
    }
}

//------------------------------------------------------------------------------
/**
    Handles control key input (cursor left/right, backspace, etc...).
*/
bool
nGuiTextEntry::OnKeyDown(nKey key)
{
    n_assert(this->lineEditor);
    bool handled = false;
    if (this->active)
    {
        switch (key)
        {
            case N_KEY_BACK:
                this->lineEditor->DeleteLeft();
                handled = true;
                break;

            case N_KEY_RETURN:
            case N_KEY_ESCAPE:
                if (!this->firstFrameActive)
                {
                    this->CheckEmptyText();
                    this->OnAction();
                    this->SetActive(false);
                    handled = true;
                }
                break;

            case N_KEY_LEFT:
                this->lineEditor->CursorLeft();
                handled = true;
                break;

            case N_KEY_RIGHT:
                this->lineEditor->CursorRight();
                handled = true;
                break;

            case N_KEY_INSERT:
                this->lineEditor->ToggleOverstrike();
                handled = true;
                break;

            case N_KEY_DELETE:
                this->lineEditor->Delete();
                handled = true;
                break;

            case N_KEY_HOME:
                this->lineEditor->CursorHome();
                handled = true;
                break;

            case N_KEY_END:
                this->lineEditor->CursorEnd();
                handled = true;
                break;

            default:
                break;
        }
    }
    return handled;
}

//------------------------------------------------------------------------------
/**
    Returns a text for display in a password entry field.
*/
nString
nGuiTextEntry::GetPwdText() const
{
    nString text = this->GetText();
    nString pwdText;
    int length = text.Length();
    int i;
    for (i = 0; i < length; i++)
    {
        pwdText.Append("*");
    }
    return pwdText;
}

//------------------------------------------------------------------------------
/**
    Returns the displayed text. When the widget is in password mode,
    this will consist of dummy characters.
*/
nString
nGuiTextEntry::GetDisplayedText() const
{
    if (this->passwordMode)
    {
        return this->GetPwdText();
    }
    else
    {
        return this->GetText();
    }
}

//------------------------------------------------------------------------------
/**
    Renders the text entry widget.
*/
bool
nGuiTextEntry::Render()
{
    if (this->firstFrameActive)
    {
        this->firstFrameActive = false;
    }

    if (this->IsShown())
    {
        rectangle screenSpaceRect = this->GetScreenSpaceRect();
        if (this->active)
        {
            nGuiServer::Instance()->DrawBrush(screenSpaceRect, this->pressedBrush);
        }
        else if (this->mouseOver)
        {
            nGuiServer::Instance()->DrawBrush(screenSpaceRect, this->highlightBrush);
        }
        else
        {
            nGuiServer::Instance()->DrawBrush(screenSpaceRect, this->defaultBrush);
        }

        // (re-)validate the font object
        if (!this->refFont.isvalid())
        {
            this->refFont = (nFont2*) nResourceServer::Instance()->FindResource(this->fontName.Get(), nResource::Font);
            if (!this->refFont.isvalid())
            {
                n_error("nGuiTextLabel %s: Unknown font '%s'!", this->GetName(), this->fontName.Get());
            }
        }

        // compute the text position
        nGfxServer2::Instance()->SetFont(this->refFont.get());
        uint renderFlags = nFont2::VCenter;
        switch (this->align)
        {
            case Left:  renderFlags |= nFont2::Left; break;
            case Right: renderFlags |= nFont2::Right; break;
            default:    renderFlags |= nFont2::Center; break;
        }

        // add text border
        screenSpaceRect.v0 += this->border;
        screenSpaceRect.v1 -= this->border;

        // render the cursor, if necessary
        if (this->IsCursorVisible())
        {
            // get x position and width of cursor
            int cursorIndex = this->lineEditor->GetCursorPos();
            nString lineText = this->GetDisplayedText();

            // build a string from the character under the cursor
            char charUnderCursor[2];
            charUnderCursor[0] = (lineText.Get()[cursorIndex] == 0) ? ' ' : lineText.Get()[cursorIndex];
            charUnderCursor[1] = 0;

            // get text extents
            vector2 cursorSize = nGfxServer2::Instance()->GetTextExtent(charUnderCursor);

            // FIXME: GetTextExtent() thinks that spaces are 0 pixels wide!!!
            if (cursorSize.x == 0)
            {
                cursorSize.x = 0.005f;
            }

            // prepare matrix to render cursor resource
            float leftMargin = 0;
            switch (this->align)
            {
                case Center:
                {
                    vector2 totalTextSize = nGfxServer2::Instance()->GetTextExtent(lineText.Get());
                    leftMargin = (screenSpaceRect.v1.x - screenSpaceRect.v0.x - totalTextSize.x) * 0.5f;
                    // fallthrough!
                }
                case Left:
                {
                    nString textToCursor = lineText.ExtractRange(0, cursorIndex);
                    vector2 textToCursorSize = nGfxServer2::Instance()->GetTextExtent(textToCursor.Get());
					// check for whitespaces at end of text
					if (textToCursor.Length() > 0)
					{
						// make sure the cursor moves even if whitespaces are entered
						while (cursorIndex > 0 && textToCursor[cursorIndex-1] == ' ')
						{
							textToCursorSize.x += 0.005f;
							cursorIndex--;
						}
					}
                    leftMargin += screenSpaceRect.v0.x + textToCursorSize.x;
                    break;
                }
                case Right:
                {
                    nString textAfterCursor = lineText.ExtractRange(cursorIndex, lineText.Length() - cursorIndex);
                    vector2 textAfterCursorSize = nGfxServer2::Instance()->GetTextExtent(textAfterCursor.Get());
                    leftMargin = screenSpaceRect.v1.x - textAfterCursorSize.x;
					// check for whitespaces at end of text
					nString textToCursor = lineText.ExtractRange(0, cursorIndex);
					if (textToCursor.Length() > 0)
					{
						// make sure the text moves left even if whitespaces are entered
						while (cursorIndex > 0 && textToCursor[cursorIndex-1] == ' ')
						{
							screenSpaceRect.v0.x -= 0.005f;
							screenSpaceRect.v1.x -= 0.005f;
							cursorIndex--;
						}
					}
					break;
                }
            }

            // prepare matrix to render cursor resource
            rectangle cursorRect(vector2(leftMargin,
                                         screenSpaceRect.v0.y + (screenSpaceRect.height() - cursorSize.y) * 0.5f),
                                 vector2(leftMargin + cursorSize.x,
                                         screenSpaceRect.v0.y + (screenSpaceRect.height() + cursorSize.y) * 0.5f));

            nGuiServer::Instance()->DrawBrush(cursorRect, this->cursorBrush);
        }

        // draw the text
        nGuiServer::Instance()->DrawText(this->GetDisplayedText().Get(), this->color, screenSpaceRect, renderFlags);
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    @brief Returns true iff the cursor should be rendered this frame.
    For the cursor to be shown, the nGuiTextEntry must be active
    and the cursor itself unblinking or in the "on" phase of a blink.
*/
bool nGuiTextEntry::IsCursorVisible() const
{
    bool retVal = false;
    if( this->active )
    {
        if( this->blinking )
        {
            double time = nGuiServer::Instance()->GetTime();
            if (fmod(time, this->blinkRate) > this->blinkRate/2.0)
            {
                retVal = true;
            }
        }
        else
        {
            retVal = true;
        }
    }
    return retVal;
}
