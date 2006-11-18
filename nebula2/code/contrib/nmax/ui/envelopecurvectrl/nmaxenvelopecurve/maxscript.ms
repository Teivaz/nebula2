-------------------------------------------------------------------------------
--
--  MAXScript for testing nmaxenvelopecurve ActiveX control
--
--  (C)2006
--
-------------------------------------------------------------------------------


-------------------------------------------------------------------------------
--  Note, the ActiveX should be register before running this script.
--
--  Click the right mouse button on the envelope curve control to open popup menu.
--  Ctrl button down and drag mouse to move control points of the control.
--
-------------------------------------------------------------------------------
previous_enableAccelerators = enableAccelerators;

rollout roCurveControl "Envelope Curve Control"
(
    activeXControl ax "{EDA6DBCD-A8BB-4709-AB92-40181C0C58CE}" height:130 width:460 
    
    on roCurveControl open do 
    (    
        -- set the following to make the custom control work smooth.
        enableAccelerators = false

        format "PROPERTIES:\n"
        showProperties ax

        format "METHODS:\n"
        showMethods ax

        format "EVENTS:\n"
        showEvents ax
        
        -- Init Control 
        -- if ControlType = 0, EnvelopeCurve
        -- if ControlType = 1, EnvelopeColorCurve
        ax.ControlType = 0
        ax.Name = "Particle Emitter"
        
        -- The following command is used for SetXXX method in the ActiveX control.
        -- There might be an error message box which shows the ActiveX control has Get/Set 
        -- concerned bug in the 3dsmax even the changed values are correctly specified though.
        -- This problem can be solved by patching 3dsmax8 servevice pack2.
        -- setIndexedProperty ax #Value 1 1
        
        ax.SetValue 0 0.9
        testvalue = ax.GetValue 0
        format "testvalue - %\n" testvalue
    )
    
    on roCurveControl close do 
    (    
        enableAccelerators = previous_enableAccelerators
    )
    
    on ax OnChangedValue do
    (
        format "The value of the contorl point is changed!\n"
    )
)

controlFloater = newRolloutFloater "Curve Control" 500 470
addRollout roCurveControl controlFloater
