-- ButtonT defines a button
--
-- required fields (in addition to those in ItemT):
-- cmd: a string of a Lua function call, e.g. 'DoOnClick()'
--
--
-- Note: Requires that lego brushes have been defined with
-- the following legoBaseNames (see ItemT)
-- inactiveItem, activeItem, invalidItem


ButtonT = Instantiate( ItemT, 
{   
    --inherited methods:
    Create = function(self, upperEdge )
        self.lego = { size = guiServer:CalcBrushSize("inactiveItemTopMid"),
                          type = 'ccguibuttonlego',
                          Init = function( brushSuffix )
                            call( 'setdefaultbrush', "inactiveItem"..brushSuffix )
                            call( 'setpressedbrush', "inactiveItem"..brushSuffix)
                            call( 'sethighlightbrush', "activeItem"..brushSuffix)
                            call( 'setdisabledbrush', "invalidItem"..brushSuffix)
                          end
                        }
    
        ItemT.Create( self, upperEdge )
        self.object = lookup( self.path )
        self:SetCmd( self.cmd )
    end,

    -- new public methods:   
    Disable = function( self )
        self.object:disable()
        self:SetFontColour( self.font.disabledColour )
    end,
    
    Enable = function( self )
        self.object:enable()
        self:SetFontColour( self.font.colour )
    end,
    
    SetCmd = function( self, cmd )
        self.cmd = cmd
        sel( self.path )
            sel( 'Item' )
                call( 'setbuttondowncommand', self.cmd )
            sel('..')
        sel('..')
    end,
    
    --protected:
    
    SetFontColour = function( self, colour )
        self.labelObj:setcolor( colour.r, colour.g, colour.b, colour.a )
    end
} )