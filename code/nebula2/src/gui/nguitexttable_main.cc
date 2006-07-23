//------------------------------------------------------------------------------
//  nguitexttable_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguitexttable.h"
#include "resource/nresourceserver.h"
#include "gfx2/ngfxserver2.h"
#include "gui/nguiserver.h"

nNebulaClass(nGuiTextTable, "gui::nguiformlayout");

//------------------------------------------------------------------------------
/**
*/
nGuiTextTable::nGuiTextTable() :
    fontName("GuiSmall"),
    lineHeight(0.0f),
    border(0.005f)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nGuiTextTable::~nGuiTextTable()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiTextTable::SetColAttrs(int colIndex, const vector4& textColor, Alignment align, float relWidth)
{
    ColAttrs colAttrs;
    colAttrs.textColor = textColor;
    colAttrs.align = align;
    colAttrs.relWidth = relWidth;
    this->columnAttributes[colIndex] = colAttrs;
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiTextTable::SetText(int colIndex, int rowIndex, const nString& text)
{
    this->textArray.Set(colIndex, rowIndex, text);
}

//------------------------------------------------------------------------------
/**
*/
const nString&
nGuiTextTable::GetText(int colIndex, int rowIndex)
{
    return this->textArray.At(colIndex, rowIndex);
}

//------------------------------------------------------------------------------
/**
    This validates the font resource if not happened yet.
*/
void
nGuiTextTable::ValidateFont()
{
    if (!this->refFont.isvalid())
    {
        this->refFont = (nFont2*) nResourceServer::Instance()->FindResource(this->fontName.Get(), nResource::Font);
        if (!this->refFont.isvalid())
        {
            n_error("nGuiTextView %s: Unknown font '%s'!", this->GetName(), this->fontName.Get());
        }
        else
        {
            this->refFont->AddRef();
            nGfxServer2::Instance()->SetFont(this->refFont.get());
            this->lineHeight = nGfxServer2::Instance()->GetTextExtent("X").y;
        }
    }
}

//------------------------------------------------------------------------------
/**
    Compute the number of lines that would fully fit into this widget.
*/
int
nGuiTextTable::GetNumVisibleLines()
{
    this->ValidateFont();
    return int(this->rect.height() / this->lineHeight);
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiTextTable::OnShow()
{
    // compute vertical size
    this->ValidateFont();
    float height = this->lineHeight * this->GetNumRows();
    this->SetMinSize(vector2(0.0f, height));
    this->SetMaxSize(vector2(1.0f, height));

    nGuiFormLayout::OnShow();
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiTextTable::OnHide()
{
    nGuiFormLayout::OnHide();
}

//------------------------------------------------------------------------------
/**
    Render the widget.
*/
bool
nGuiTextTable::Render()
{
    if (this->IsShown())
    {
        // render background
        nGuiServer::Instance()->DrawBrush(this->GetScreenSpaceRect(), this->defaultBrush);

        // set active font
        this->ValidateFont();
        nGfxServer2* gfxServer = nGfxServer2::Instance();
        gfxServer->SetFont(this->refFont.get());

        // initial rectangle
        rectangle screenSpaceRect = this->GetScreenSpaceRect();
        screenSpaceRect.v0.x += this->border;
        screenSpaceRect.v1.x -= this->border;
        rectangle curTextRect = screenSpaceRect;

        // for each column...
        int colIndex;
        int numColumns = this->textArray.GetWidth();
        for (colIndex = 0; colIndex < numColumns; colIndex++)
        {
            // initial rectangle
            curTextRect.v0.y = screenSpaceRect.v0.y;
            curTextRect.v1.x = curTextRect.v0.x + screenSpaceRect.width() * this->columnAttributes[colIndex].relWidth;
            curTextRect.v1.y = curTextRect.v0.y + this->lineHeight;

            // render flags...
            uint renderFlags = nFont2::ExpandTabs;
            switch (this->columnAttributes[colIndex].align)
            {
                case Left:  renderFlags |= nFont2::Left; break;
                case Right: renderFlags |= nFont2::Right; break;
                default:    renderFlags |= nFont2::Center; break;
            }

            // for each row in column...
            int rowIndex;
            int numRows = this->textArray.GetHeight();
            for (rowIndex = 0; rowIndex < numRows; rowIndex++)
            {
                nGuiServer::Instance()->DrawText(this->textArray.At(colIndex, rowIndex).Get(),
                                                 this->columnAttributes[colIndex].textColor,
                                                 curTextRect,
                                                 renderFlags);

                // update text rectangle
                curTextRect.v0.y = curTextRect.v1.y;
                curTextRect.v1.y += this->lineHeight;
            }
            curTextRect.v0.x = curTextRect.v1.x;
        }
        return nGuiFormLayout::Render();
    }
    return false;
}

