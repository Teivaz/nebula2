-- ItemT is a table that defines an item to be used in a ItemManagerT
-- Visually, it is composed of a set of "legos"  (texture components that 
-- can be combined to form an arbitrarily large box shape), usually
-- combined with text and/or an image.
--
-- required fields:
-- pos: the items will be ordered by ascending pos
-- lego = { size: the screen size of the legos (which must all be the same size)
--            type: the c++ type
--            Init( brushName ): additional initialization for each lego, e.g. defining brushes
--          }
-- width: the number of legos wide, excluding the edge legos
-- height: the number of legos high, excluding the edge pieces
--
-- optional fields:
-- label: the displayed text (usually defined in strings/[language].lua)
-- alignment: how the text is aligned (left, center, right)
-- margin: the distance between left or right aligned text and the item's edge
-- font = { name, colour = {r,g,b,a} }: name must refer to a font defined with addsystemfont or addcustomfont -- required if label is set
-- shrinkWidthToFit: if true, the final width is adjusted to be no bigger than necessary.
-- pic =
--  { brush: an image, expressed as the name of a brush
--    alignment: whether the image appears 'left' or 'right' of the text
--    size: the screen size of the brush (will be calculated automatically) -- the ItemT will be forced to at least this size
--    label = { text, margin, alignment, font } -- optional text, other fields as above
--  }
-- itemAlignment = (left, center, right) or a value from 0-1 representing the left edge in screen space  -- default is left -- only useful in conjunction with ItemManagerT
--
--
--
-- Note: Requires that lego brushes have been defined, with names of the form
-- [legoBrushBaseName][VerticalPos][HorizontalPos],
-- where legoBrushBaseName is arbitrary (but the same for all legos of a given type)
-- VerticalPos is one of Top, [blank], or Low,
-- HorizontalPos is one of Left, Mid, or Right
-- e.g. inactiveItemTopMid, smurfRight, etc.


ItemT =
{
    alignment = 'center',
    margin = 0.02,
    
--inherited public:
    Create = function( self, upperEdge )
        self.width = self:ShrinkToFit()
        self.width, self.height = self:ExpandToFitPic()
        self:CalcRect(upperEdge)        
        selNew( 'nguiwidget', GenerateUniqueItemName() )
            self.path = getCwd()
            selNew( 'nguiwidget', 'Item' )
                call( 'setrect', self.rect.x1, self.rect.x2, self.rect.y1, self.rect.y2 )
                self:CreateComponents( )                     
            sel( '..' )
            self:CreatePic()
            self:CreateLabel()
        sel('..')
    end,

--public:
    GetScreenWidth = function( self )
        return ( self.width + 2 ) * self.lego.size[1]
    end,
            
    GetScreenHeight = function( self )
        return ( self.height + 2 ) * self.lego.size[2]
    end,
    
    SetText = function( self, newText ) -- for changing the text after the button has been created
        local createLabel = self.label == nil
        self.label = newText
        assert( self.shrinkWidthToFit ~= true ) -- the below doesn't work for some reason
        --[[local cwd = psel()
        sel( self.path )
            sel( '..' )
                delete( self.path )       
                self:Create(self.rect.y1)
        sel(cwd)]]
        if createLabel then
            local cwd = psel()
            sel( self.path )           
                self:CreateLabel()
            sel(cwd)
        else
            self.labelObj:settext( self.label )
        end
    end,
    
    SetHorizontalOffset = function( self, offset )
        assert( self.rect ~= nil )
        self.rect.x1 = offset
        self.rect.x2 = offset + self:GetScreenWidth()   
        local cwd = psel()
        sel( self.path )
            sel( 'Item' )
                call( 'setrect', self.rect.x1, self.rect.x2, self.rect.y1, self.rect.y2 )
            if self.label ~= nil then
                local labelRect = self:CalcLabelRect()
                self.labelObj:setrect( labelRect.x1, labelRect.x2, labelRect.y1, labelRect.y2 )
            end
            if self.pic ~= nil then 
                local picRect = self:CalcPicRect()
                self.picObj:setrect( picRect.x1, picRect.x2, picRect.y1, picRect.y2 )
            end
        sel( cwd )
    end,
    
--protected    
    GetLabelWidth = function( self )
        local retVal = self:GetScreenWidth()
        if self.pic ~= nil then 
            retVal = retVal - self:GetPicWidth()
        end
        return retVal
    end,           

--private
    CreateLabel = function( self )
        if self.label ~= nil then 
            local labelRect = self:CalcLabelRect()
            selNew( 'nguitextlabel', 'Label' )
                self.font = InitFont(self.font)
                self.labelObj = lookup( getCwd() )
                self.labelObj:setrect( labelRect.x1, labelRect.x2, labelRect.y1, labelRect.y2 )
                self.labelObj:settext( self.label )
                self.labelObj:setalignment( self.alignment )
                self.labelObj:setborder( self.margin, 0 )
                self.labelObj:setfont( self.font.name )
                self.labelObj:setcolor( self.font.colour.r, self.font.colour.g, self.font.colour.b, self.font.colour.a )
                self.labelObj:setclipping(false)
            sel('..')
        end
    end,
    
    CreatePic = function( self )
        if self.pic ~= nil then             
            selNew( 'nguitextlabel', 'Pic' )
                self.picObj = lookup( getCwd() )
                if self.pic.margin == nil then
                    self.pic.margin = 0
                end            
                local picRect = self:CalcPicRect()
                self.picObj:setrect( picRect.x1, picRect.x2, picRect.y1, picRect.y2 )
                self.picObj:setdefaultbrush( self.pic.brush )
                if self.pic.label ~= nil then
                    local label = self.pic.label
                    self.picObj:settext( label.text )
                    self.picObj:setalignment( label.alignment )
                    self.picObj:setborder( label.margin, 0 )
                    self.picObj:setfont( label.font.name )
                    self.picObj:setcolor( label.font.colour.r, label.font.colour.g, label.font.colour.b, label.font.colour.a )
                    self.picObj:setclipping(false)
                end
            sel('..')
        end
    end,
    
    GetPicWidth = function( self )
        self:CalcPicSize()
        return self.pic.size[1] + 2*self.pic.margin
    end,
    
    GetPicHeight = function( self )
        self:CalcPicSize()
        return self.pic.size[1]       
    end,
    
    CalcPicSize = function( self )
        if self.pic.size == nil then
            self.pic.size = guiServer:CalcBrushSize(self.pic.brush)
        end
    end,
    
    CalcRect = function( self, upperEdge )
        if self.rect == nil then
            self.rect = {
                x1 = 0,
                y1 = upperEdge }
            self.rect.x2 = self.rect.x1 + self:GetScreenWidth()
            self.rect.y2 = self.rect.y1 + self:GetScreenHeight()
        end
    end,

    CalcLabelRect = function( self )
        local labelRect = Instantiate( self.rect )
        if self.pic ~= nil then
            if self.pic.alignment == 'left' then
                labelRect.x1 = labelRect.x2 - self:GetLabelWidth()
            else
                labelRect.x2 = labelRect.x1 + self:GetLabelWidth()
            end
            assert( labelRect.x1 < labelRect.x2 )
        end
        return labelRect
    end,
    
    CalcPicRect = function( self )
        --centre vertically:
        local height = self.rect.y2 - self.rect.y1
        assert( height >= self:GetPicHeight() )   
        local picRect = { y1 = self.rect.y1 + (height - self:GetPicHeight() ) / 2 }
        picRect.y2 = picRect.y1 + self:GetPicHeight()
        if self.pic.alignment == 'left' then
            picRect.x1 = self.rect.x1 + self.pic.margin
            picRect.x2 = picRect.x1 + (self:GetPicWidth() - 2 * self.pic.margin)
        else
            picRect.x2 = self.rect.x2 - self.pic.margin
            picRect.x1 = picRect.x2 - (self:GetPicWidth() - 2 * self.pic.margin)
        end
        return picRect
    end,

    CreateComponents = function( self )
        local posY = 0
        self:CreateRow( 'Top', posY )
        for i = 1, self.height do
            posY = posY + self.lego.size[2]
            self:CreateRow( '', posY )
        end
        posY = posY + self.lego.size[2]
        self:CreateRow( 'Low', posY )
    end,
    
    CreateRow = function( self, rowName, posY )
    --the rowName is the first element of the suffix for the brush names defined in newGui.lua
        local posX = 0
        self:CreateLego( rowName..'Left', posX, posY )
        for i = 1, self.width do
            posX = posX + self.lego.size[1]
            self:CreateLego( rowName..'Mid', posX, posY )
        end
        posX = posX + self.lego.size[1]
        self:CreateLego( rowName..'Right', posX, posY )
    end,
    
    CreateLego = function( self, brushSuffix, posX, posY )               
        local rect = {
            x1 = posX,
            y1 = posY,
            x2 = posX + self.lego.size[1], 
            y2 = posY + self.lego.size[2]
        }
        selNew( self.lego.type, brushSuffix..posX..posY )
            call( 'setrect', rect.x1, rect.x2, rect.y1, rect.y2 )
            self.lego.Init( brushSuffix )
        sel( '..' )
    end,    
            
    ShrinkToFit = function( self )
        local retVal = self.width
        if self.shrinkWidthToFit then
            retVal = 0
            if self.label ~= nil then
                selNew( 'nguitextlabel', 'Label' )
                    call( 'setfont', self.font.name )         
                    call( 'settext', self.label )
                    retVal = math.max( 0, (call('gettextextent') + 2*self.margin) / self.lego.size[1] ) - 2
                sel( '..' )
                delete( 'Label' )
            end
            if self.pic ~= nil then                
                retVal = retVal + self:GetPicWidth() / self.lego.size[1]
            end
        end
        return math.ceil( retVal )
    end,
    
    ExpandToFitPic = function(self)
        local width, height = self.width, self.height
        if self.pic ~= nil then
            width = math.max( width, math.ceil( self:GetPicWidth() /  self.lego.size[1] ) )
            height = math.max( height, math.ceil( self:GetPicHeight() /  self.lego.size[2] ) )
        end
        return width, height
    end,   
}

itemNameSuffix = -1
function GenerateUniqueItemName()
    itemNameSuffix = itemNameSuffix + 1
    return 'ItemT'..itemNameSuffix
end
