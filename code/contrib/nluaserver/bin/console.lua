-- Functions to make the use of the NOH via the console easier

function conls()
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

function conmangle( assignpath )
  local path = mangle( assignpath )
  puts( path .. '\n' )
  return path
end

function conexists( name )
  local b = exists( name )
  if ( b ) then
    puts( 'true\n' )
  else
    puts( 'false\n' )
  end
  return b
end

function up()
  sel( '..' )
end
