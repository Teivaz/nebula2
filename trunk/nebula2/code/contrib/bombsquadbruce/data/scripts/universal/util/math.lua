function round( val )
    if math.mod( val, 1 ) < 0.5 then
        retVal = math.floor( val )
    else
        retVal = math.ceil( val )
    end
    return retVal
end

function Clamp( val, minVal, maxVal )
    if val > maxVal then val = maxVal
    elseif val < minVal then val = minVal end
    return val
end

function FuzzyEqual( val1, val2, fudgeFactor )
    if nil == fudgeFactor then
        fudgeFactor = 0.0001
    end
    return math.abs( val1 - val2 ) <= fudgeFactor
end
