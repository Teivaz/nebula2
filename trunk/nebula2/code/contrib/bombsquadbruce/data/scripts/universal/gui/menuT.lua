--new required parameters:
--width: the number of middle legos (excludes the two edges) for every item in the menu
--height: the number of middle pieces (excludes the two edges) for each individual item in the menu

MenuT = Instantiate( ItemManagerT, 
{  
    Create = function( self )
        for name, item in self.items do
            item.width = self.width
            if item.height == nil then 
                item.height = self.height
            end
        end
        ItemManagerT.Create( self )
    end,
} )
