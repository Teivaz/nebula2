-- vector.lua

--unary operators
function vecLenSquared( vec )
    return vecDot( vec, vec )
end

function vecLen( vec )
    return math.sqrt( vecLenSquared( vec ) )
end

function vecScale( vec, scale )
    local retVal = {}
    for i =1,  table.getn( vec ) do
        retVal[i] = vec[i] * scale
    end
    return retVal
end

function vecNormalize( vec )
    local length = vecLen( vec )
    if 0 == length then
        length = 1 --vec is the zero vector
    end
    return vecScale( vec, 1 / length )
end    

--binary operators
function vecGetDimension( vec1, vec2, operationString )
    if type( vec1 ) ~= 'table' then
        error( "Vec1 isn't a table: '"..tostring( vec1 ).."'" )
    end
    local vecSize = table.getn( vec1 )
    assert( table.getn( vec2 ) == vecSize, "Tried to "..operationString.." two vectors of different dimensions ("..vecSize.." and "..table.getn(vec2)..")!" )
    return vecSize
end

function vecSum( vec1, vec2 )
    vecSize = vecGetDimension( vec1, vec2, 'add' )
    local retVal = {}
    for i =1, vecSize do
        retVal[i] = vec1[i] + vec2[i]
    end
    return retVal
end

function vecDiff( vec1, vec2 )
    vecSize = vecGetDimension( vec1, vec2, 'subtract' )
    local retVal = {}
    for i =1, vecSize do
        retVal[i] = vec1[i] - vec2[i]
    end
    return retVal
end

function vecEqual( vec1, vec2, fudgeFactor )
    vecSize = vecGetDimension( vec1, vec2, 'compare' )
    
    local retVal = true
    for i =1, vecSize do
        if not FuzzyEqual( vec1[i], vec2[i], fudgeFactor ) then
            retVal = false
            break
        end
    end
    return retVal
end

function vecLerp( vec1, vec2, t )   
    return vecSum( vecScale( vec1, 1-t ), vecScale( vec2, t ) )
end

function vecDot( vec1, vec2 )
    vecSize = vecGetDimension( vec1, vec2, 'dot' )
    local retVal = 0
    for i =1, vecSize do
        retVal = retVal + vec1[i] * vec2[i]
    end
    return retVal
end    
