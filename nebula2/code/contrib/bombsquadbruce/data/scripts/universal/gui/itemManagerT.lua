-- A canvas on which is displayed a (possibly empty) collection of Items,
-- together with an optional, screen filling background image.
--
--required parameters:
--name: an arbitrary, unique name
--
--optional parameters:
--backgroundImage: an image that will be displayed behind the canvas, filling the screen (regardless of canvas size or position)
--upperEdge: a value between 0 and 1 that determines where the top of the canvas lies wrt to the screen (0 means flush with the top screen edge)
--leftEdge: see above.  If leftEdge == 'centre', the canvas will be centred automatically; 'right' aligns the canvas with the right screen edge 
--items: a list of ItemT objects
--fadeInTime: how many seconds the canvas (and background image, if any) takes to fade in from complete transparency
--verticalGap: the screen distance (0-1) between successive items
--
-- The items must have the following field:
-- pos (items will be arranged in a column from lowest to highest pos)
-- The items must have the following methods:
-- Create(upperEdge): creates the item and positions its upper edge at upperEdge
-- GetScreenWidth(): the item's width in screen coordinates (0-1)
-- GetScreenHeight()

ItemManagerT = 
{  
--public
    upperEdge = 0,
    leftEdge = 0,
    fadeInTime = 0,
    verticalGap = 0,
--protected:
    Create = function( self )
        local cwd=psel()
        sel( guiServer.rootWindow.path )
            selNew( 'nguiwindow', self.name )
                self.winPath = getCwd() 
                self.object = lookup( self.winPath)
                if self.backgroundImage ~= nil then
                    self.object:setdefaultbrush( self.backgroundImage )
                end
                self.object:setfadeintime( self.fadeInTime )
                selNew( 'nguiwidget', 'box' ) -- an extra layer to accommodate a possible background image
                    self.widgetPath = getCwd() -- this is useful because it defines our coordinate frame of reference
                    self.screenHeight = self:CreateItems()
                    self:DetermineScreenWidth()
                    self:AlignItems()
                    self:CalcHorizontalAlignment()
                    local lowerRight = { x = self.leftEdge + self.screenWidth,
                                                  y = self.upperEdge + self.screenHeight }
                    call( 'setrect', self.leftEdge, lowerRight.x, self.upperEdge, lowerRight.y )
                sel('..')
                call( 'hide' )
        sel( cwd )
    end,
         
    AddBrushes = function() end,   -- override this to define brushes for your canvas
    
    Activate = function(self) 
        self.object:show()
    end,
    
    Deactivate = function(self) 
        self.object:hide()
    end,
    
-- private:
    CalcHorizontalAlignment = function(self)
        if self.leftEdge == 'centre' then
            self.leftEdge = (1 - self.screenWidth) / 2
        elseif self.leftEdge == 'right' then
            self.leftEdge = 1 - self.screenWidth
        end
    end,
    
    DetermineScreenWidth = function( self )
        if self.screenWidth == nil then
            local maxWidth = 0
            for name, item in self.items do
                if item:GetScreenWidth() > maxWidth then
                    maxWidth = item:GetScreenWidth()
                end
            end    
            self.screenWidth = maxWidth
        end
    end,

    CreateItems = function(self)
        local yOffset = 0        
        local itemsByPos =  {}
        for name, item in self.items do
            itemsByPos[item.pos] = item
        end
        for i = 0, table.getn( itemsByPos ) do
            local item = itemsByPos[i]
            yOffset = yOffset + self.verticalGap
            self:CreateItem( item, yOffset )
            yOffset = yOffset + item:GetScreenHeight()
        end
        return yOffset
    end,
    
    -- the following method is defined to make inheritance easier:
    CreateItem = function( self, item, yOffset )
        item:Create( yOffset )
    end,
    
    AlignItems = function(self)
        for name, item in self.items do
            if item.itemAlignment == 'right' then
                item:SetHorizontalOffset( self.screenWidth - item:GetScreenWidth() )
            elseif item.itemAlignment == 'center' then
                item:SetHorizontalOffset( (self.screenWidth - item:GetScreenWidth()) / 2 )
            else
                local leftMargin = tonumber( item.itemAlignment )
                if leftMargin ~= nil then
                    item:SetHorizontalOffset( leftMargin )
                end
            end
        end
    end,

}

