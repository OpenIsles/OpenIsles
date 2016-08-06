--
-- Allgemeine Hilfsfunktionen
---------------------------------

local ALL_VIEWS = { "south", "east", "north", "west" }


--[[
-- Wandelt zwei Zahlen x, y in eine Map-Koordiante (Table) um
--]]
function buildCoords(x, y)
    local x = tonumber(x)
    local y = tonumber(y)

    assert(x ~= nil)
    assert(y ~= nil)

    return { x = x, y = y }
end

--[[
-- Wenn wir Koordinaten für ein Set brauchen, wandelt diese Funktion die Koordinate (Table mit x und y) in einen
-- String um.
--]]
function coordsToKey(coords)
    local x = tonumber(coords.x)
    local y = tonumber(coords.y)

    assert(x ~= nil)
    assert(y ~= nil)

    return x .. "," .. y
end

--[[
-- Inverse-Operation zu coordsToKey()
--]]
function keyToCoords(coords)
    local x,y = string.gmatch(coords, "(%d+),(%d+)")()
    return buildCoords(tonumber(x), tonumber(y))
end

--[[
-- Gibt einen zufälligen Wert aus einem Set (Table mit Value-Keys und Dummy-Value) zurück
--]]
function randomPickFromSet(set)
    -- Größe des Sets ermitteln
    local count = 0
    for _ in pairs(set) do count = count + 1 end

    -- Index zufällig wählen und Wert raussuchen
    local index = math.random(count)
    local i = 1
    for value in pairs(set) do
        if i == index then return value end
        i = i + 1
    end

    assert(false)
end

--[[
-- Gibt einen zufälligen Wert aus einem Array (Table mit Index-Keys und Value) zurück
--]]
function randomPickFromArray(array)
    local index = math.random(#array)

    return array[index]
end

--[[
-- Gibt eine zufällige Ansicht zurück
--]]
function randomView()
    return randomPickFromArray(ALL_VIEWS)
end