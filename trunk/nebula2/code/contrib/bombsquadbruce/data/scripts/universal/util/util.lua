runscript( 'universal/util/vector.lua' )
runscript( 'universal/util/string.lua' )
runscript( 'universal/util/math.lua' )


function selSafe( class, name ) 
	if sel( name ) == nil then -- then the object doesn't exist
		new( class, name )
		sel( name )
	end
end

function selNew( class, name ) 
	if sel( name ) ~= nil then -- then the object already exists
	  sel('..')
		delete( name )
	end
	new( class, name )
	sel( name )
end

function getCwd()
    --returns the current working directory as a string
    return psel():getfullname()
end

local function writeMsg( msg, indent )
    if type(msg) == 'table' then
        io.write( '\n' )
        local newIndent = indent..'  '
        for key, value in msg do
            io.write( newIndent..key..' = ' )
            writeMsg( value, newIndent )
        end
    else
        io.write( tostring(msg) )
        io.write( '\n' )
    end
end

function writeDebug( msg )
    if logging == true then
        writeMsg( msg, '' )
        io.flush()
    end
end

function conls() --ripped off from Vadim's console.lua
  local t = ls()
  if ( t ~= nil ) then
    for index, value in t do
      if ( value ~= nil ) then
        puts( value )
      end
      puts( ' ' )
    end
  end
  puts( '\n' )
  return t
end

function Instantiate( class, params )
--the params table can be used to add fields to this instance of class, provide
--values, or override methods (because the last two are the same in Lua! Yay!)
    local instance = params or {}
    for field, value in class do
        if nil == instance[field] then --use the class default
            if type( value ) == 'table' then -- do recursive deep copy
                instance[field] = Instantiate( value )
            else
                instance[field] = value 
            end
        end
    end
    if instance.Constructor ~= nil then 
        instance:Constructor()
    end
    return instance
end

function IncIdx( varToIncrement, baseTable )
    return IncInRange( varToIncrement, 1, table.getn( baseTable ) )
end

function IncInRange( varToIncrement, min, max )
    assert( varToIncrement >= min and varToIncrement <= max )
    varToIncrement = varToIncrement + 1
    if varToIncrement > max then
        varToIncrement = min
    end
    return varToIncrement
end

function Screenshot()
    local i = 1
    while io.open( 'screenshot'..i..'.bmp' ) ~= nil do
        i =i + 1
    end
    nebula.sys.servers.gfx:savescreenshot('screenshot'..i..'.bmp')
end
