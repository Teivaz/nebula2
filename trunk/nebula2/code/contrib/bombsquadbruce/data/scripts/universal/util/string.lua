function string.Capitalize( str ) -- capitalizes the first letter of the string
    return string.upper( string.sub( str, 1, 1 ) )..string.sub( str, 2 )
end

-- find the last instance of pattern in s that is not later than index init (by default, the end of the string)
function string.findLast( s, pattern, init, simple )
    if init ~= nil then s = string.sub( s, 1, init ) end
    local idx
    for i = 1, string.len( s ) do
        idx = string.find( s, pattern, -i, simple )
        if idx ~= nil then
            break
        end
    end
    return idx
end