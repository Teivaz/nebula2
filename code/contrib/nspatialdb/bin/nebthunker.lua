--
-- This metatable setup can be moved to C++
-- but I don't know if that would actually
-- improve performance.
--

-- This resolves the relative path based on the index and returns a thunk
function relgrab( t, k )
  local path
  path = t:getfullname()
  path = path .. '/' .. k
  return lookup( path )
end

--
--  Try to find a script command that matches the key and return it, if that fails assume
--  the key is the name of a node and return a thunk (or nil if it turns out the node
--  doesn't exist).
--
function neb_index( t, k )
  if not t._class[k] then
--      puts( 'relgrab: ' .. tostring(t) .. ', ' .. k .. '\n' )
    return relgrab( t, k )
  else
    return t._class[k]
  end
end

--  Build a metatable for neb thunks
_nebthunker = { __index = neb_index }

-- Create the default '/' NOH root thunk
nebula = lookup( '/' )

-- Remove zombie thunks from the _nebthunks table
function PruneNebThunks()
  local function KillZombies( t, dr )
    local numZombies = 0
    -- find zombies
    for k, v in pairs( t ) do
      if IsZombieThunk( v ) then
        numZombies = numZombies + 1
        dr.n = dr.n + 1
        dr[dr.n] = k
        puts( 'Found eeevil zombie thunk ' .. tostring(k) .. '\n' )
      end
    end
    -- kill zombies
    while ( numZombies > 0 ) do
      rawset( t, dr[dr.n], nil )
      numZombies = numZombies - 1
      dr.n = dr.n - 1
    end
  end
  
  local deathRow = { n = 0 }

  puts( 'Started nebula tree pruning.\n' )
  KillZombies( _nebthunks, deathRow )
  puts( 'Finished nebula tree pruning.\n' )
end
