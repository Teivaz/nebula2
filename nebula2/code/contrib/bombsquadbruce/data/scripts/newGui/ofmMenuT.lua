--A OFMMenuT differs from a normal MenuT in that it displays
--"scaffolding" (steel bars) around every item.

OFMMenuT = Instantiate( MenuT,
{
--inherited:
    Create = function( self )
        self.knobSize = guiServer:CalcBrushSize( "knobLeft" )
        self.barSize = guiServer:CalcBrushSize("barKnobRight")
        self.verticalGap = self.barSize[2]
        
        MenuT.Create(self)
        --create extra scaffolding at the bottom:
        local cwd = psel()
        sel( self.widgetPath )
            self:CreateLabel( 'barKnobBottom', - self.barSize[1], self.screenHeight, self.barSize )
            self:CreateLabel( 'barKnobBottom', self.screenWidth, self.screenHeight, self.barSize )
        sel( cwd )
    end,
    
    CreateItem = function(self, item,yOffset)
        MenuT.CreateItem(self,item,yOffset)        
        self:CreateItemScaffold( yOffset, item )
    end,
    
--new:
    CreateItemScaffold = function( self, yOffset, item )
        self:CreateBarAbove( yOffset, item )
        for i = 0, item.height + 1 do -- this only works because barSize == item.lego.size
            self:CreateLabel( 'barVertical', - self.barSize[1], yOffset + i * self.barSize[2], self.barSize )
            self:CreateLabel( 'barVertical', item:GetScreenWidth(), yOffset + i * self.barSize[2], self.barSize )
        end
    end,

    CreateBarAbove = function( self, yOffset, item )
        if item.pos == 0 then --the top bar
            self:CreateLabel( 'knobLeft', -self.knobSize[1], yOffset-self.knobSize[2], self.knobSize )
            self:CreateLabel( 'barTTop', item:GetScreenWidth(), yOffset-self.barSize[2], self.barSize )
            self:CreateLabel( 'barKnobRight', item:GetScreenWidth() + 2 * self.barSize[1], yOffset-self.barSize[2], self.barSize )
            self:CreateLabel( 'barHorizontal', item:GetScreenWidth() + self.barSize[1], yOffset-self.barSize[2], self.barSize )
        else
            self:CreateLabel( 'barTLeft', - self.barSize[1], yOffset-self.barSize[2], self.barSize )
            self:CreateLabel( 'barTRight', item:GetScreenWidth(), yOffset-self.barSize[2], self.barSize )
        end
        for i = 0, self.width + 1 do
            if i == 1 or ( i > 0 and i == self.width ) then
                self:CreateLabel( 'barTTop', i * self.barSize[1], yOffset- self.barSize[2], self.barSize )
            else
                self:CreateLabel( 'barHorizontal', i * self.barSize[1], yOffset - self.barSize[2], self.barSize )
            end
        end
    end,
    
    CreateLabel = function( self, brushName, posX, posY, size )
        local rect = {
            x1 = posX,
            y1 = posY,
            x2 = posX + size[1], 
            y2 = posY + size[2]
        }
        selNew( 'nguilabel', brushName..posX..posY )
            call( 'setrect', rect.x1, rect.x2, rect.y1, rect.y2 )
            call( 'setdefaultbrush', brushName )
        sel('..')
    end,
} )