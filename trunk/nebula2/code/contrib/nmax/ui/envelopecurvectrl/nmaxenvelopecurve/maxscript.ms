-------------------------------------------------------------------------------
--
--  MAXScript for testing nmaxenvelopecurve ActiveX control
--
--  (C)2006 Cho Jun Hyeong 
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
        -- ax.ControlType = 1
            
        -- The following command is used for SetXXX method in the ActiveX control.
        -- There might be an error message box which shows the ActiveX control has Get/Set 
        -- concerned bug in the 3dsmax even the changed values are correctly specified though.
        -- This problem can be solved by patching 3dsmax8 servevice pack2.
        -- setIndexedProperty ax #Value 1 1
        
        ax.SetValue 0 0.9
        testvalue = ax.GetValue 0
        format "testvalue - %\n" testvalue
        
        params = #(0.0, 0.0, 0.0)
        ax.InitCurve (params as string)
        --ax.InitColorCurve (params as string) 

    )
    
    on roCurveControl close do 
    (    
        enableAccelerators = previous_enableAccelerators
    )
        
    on ax OnChangedValue do
    (
        format "The value of the contorl point is changed!\n"

        if( ax.ControlType == 0 ) then 
        (
            v0 = ax.GetValue 0
            v1 = ax.GetValue 1
            v2 = ax.GetValue 2
            v3 = ax.GetValue 3
            p1 = ax.GetPos 1
            p2 = ax.GetPos 2
            freqency = ax.Frequency
            amplitude = ax.amplitude 
            modFunc = ax.ModulationFunc
            
            format " %f %f %f %f %f %f %f %f %f \n" v0 v1 v2 v3 p1 p2 freqency amplitude modFunc
        )
        else
        (
            cr = (bit.and 0xff (ax.GetColor 0) ) / 255.0
            cg = (bit.and 0xff (bit.shift (ax.GetColor 0) -8)) / 255.0
            cb = (bit.and 0xff (bit.shift (ax.GetColor 0) -16)) / 255.0
            c1 = ax.GetColor 1
            c2 = ax.GetColor 2
            c3 = ax.GetColor 3
            p1 = ax.GetPos 1
            p2 = ax.GetPos 2
                        
            format "% % % % % % % % \n" cr cg cb c1 c2 c3 p1 p2
        )
      
    )
)

controlFloater = newRolloutFloater "Curve Control" 500 470
addRollout roCurveControl controlFloater
