guiServer.modes.mainMenu = 
    Instantiate( 
        OFMMenuT, 
        {       
            name = 'mainMenu',
            
            width = 9,
            height = 1,
            
            upperEdge = 0.27,
            leftEdge = 'centre',
            
            items = {
                play = Instantiate( ButtonT, { cmd = 'game:Restart()', pos = 0 } ),
                continue = Instantiate( ButtonT, { cmd = 'guiServer.modes.mainMenu:Continue()', pos = 1 } ),
                options = Instantiate( ButtonT, { cmd = 'Options()', pos = 2 } ),
                quit = Instantiate( ButtonT, { cmd = 'Quit()', pos = 3 } ),                
            },                 
            
            Activate = function( self )               
                OFMMenuT.Activate( self )
                self.items.options:Disable()
                if true == game.inProgress then
                    self.items.continue:Enable()
                else
                    self.items.continue:Disable()
                end
                world.Pause(true)
            end,
            
            Up = function( self )
                if true == game.inProgress then
                    self:Continue() -- this is inconsistent, but that's better than accidentally quitting
                else
                    Quit() -- close the app
                end
            end,

            --button methods
            Continue = function()
                writeDebug( 'Continue')
                guiServer:Deactivate()
                world.Pause(false)
            end,
        }
    )
