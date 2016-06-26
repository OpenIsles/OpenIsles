--
-- Funktionen zum Straßenbau / Planung
------------------------------------------

BITMASK_GRASS = 256


--[[
-- Helper, der für ein Koordinatenrechteck ein int-Array (von Koordinaten "x,y"-Keys) ausgibt.
--]]
local function debugOutputMapGrid(coordsStart, coordsEnd, mapArray)
    local output = {}
    for y = coordsStart.y, coordsEnd.y do
        for x = coordsStart.x, coordsEnd.x do
            if mapArray[x .. "," .. y] == 1 then
                output[#output + 1]  = "█"
            elseif mapArray[x .. "," .. y] == 2 then
                output[#output + 1]  = "▓"
            else
                output[#output + 1]  = "░"
            end
        end
        output[#output + 1]  = "\n"
    end

    oi.debug("debugOutputMapGrid\n" .. table.concat(output))
end


--[[
-- Findet die Koordinate für das erste Straßenstück hinter dem Kontor
--
-- Rückgabewert sind Koordinaten (x, y)
--]]
function getStreetOrigin(officeObject)
    if officeObject.view == "south" then
        return buildCoords(officeObject.x + 1, officeObject.y - 1)
    elseif officeObject.view == "east" then
        return buildCoords(officeObject.x + 2, officeObject.y - 1)
    elseif officeObject.view == "north" then
        return buildCoords(officeObject.x + 1, officeObject.y + 2)
    elseif officeObject.view == "west" then
        return buildCoords(officeObject.x - 1, officeObject.y - 1)
    else
        error("Illegal view for office at (" .. officeObject.x .. ", " .. officeObject.y .. ")")
    end
end

--[[
-- Erweitert das Straßennetz um einen Block.
--
---░░░░░░░█░░░░░░░
-- ░░░░░░░█░░░░░░░
-- ░░░░░░░█░░░░░░░
-- ░░░░░░░█░7░░░░░
-- ░░░░░░░█░░░░░░░
-- ░░░░░░░█░░░7░░░
-- ░░░░░░░█░░░░░░░
-- ███████S███████
-- ░░░░░░░█░░░░░░░
-- ░░░░░░░█░░░░░░░
-- ░░░░░░░█░░░░░░░
-- ░░░░░░░█░░░░░░░
-- ░░░░░░░█░░░░░░░
-- ░░░░░░░█░░░░░░░
---░░░░░░░█░░░░░░░
--
--]]
function buildStreetSystemAt(sourceCoords)
    -- Quellstück bauen. Das ist einmalig am Kontor, danach sollte das Feld immer schon belegt sein.
    if oi.getMapObjectFixedAt(sourceCoords) == nil then
        oi.build(aiInfo.playerIndex, "farm-road", sourceCoords.x, sourceCoords.y, "south")
    end

    -- Wir bauen immer ein Kreuz.
    -- Mindestens eine Seite hat schon Straßen bzw. das Kontor ist im Weg.
    -- Ist was im Weg, brechen wir für diese Richtung ab.
    -- Schaffen wir es zum 7.Stück, merken wir uns dieses Ende für spätere Erweiterung.

    for i = 1, 7 do
        if pcall(oi.build, aiInfo.playerIndex, "farm-road", sourceCoords.x + i, sourceCoords.y, "south") == false then
            break
        end
        if i == 7 then
            aiInfo.streetEndPoints[coordsToKey(buildCoords(sourceCoords.x + i, sourceCoords.y))] = true
        end
    end

    for i = 1, 7 do
        if pcall(oi.build, aiInfo.playerIndex, "farm-road", sourceCoords.x - i, sourceCoords.y, "south") == false then
            break
        end
        if i == 7 then
            aiInfo.streetEndPoints[coordsToKey(buildCoords(sourceCoords.x - i, sourceCoords.y))] = true
        end
    end

    for i = 1, 7 do
        if pcall(oi.build, aiInfo.playerIndex, "farm-road", sourceCoords.x, sourceCoords.y + i, "south") == false then
            break
        end
        if i == 7 then
            aiInfo.streetEndPoints[coordsToKey(buildCoords(sourceCoords.x, sourceCoords.y + i))] = true
        end
    end

    for i = 1, 7 do
        if pcall(oi.build, aiInfo.playerIndex, "farm-road", sourceCoords.x, sourceCoords.y - i, "south") == false then
            break
        end
        if i == 7 then
            aiInfo.streetEndPoints[coordsToKey(buildCoords(sourceCoords.x, sourceCoords.y - i))] = true
        end
    end

    -- Endpunkt-Liste aktualisieren
    aiInfo.streetEndPoints[coordsToKey(sourceCoords)] = nil

    -- Debug
    oi.debug("aiInfo.streetEndPoints:")
    for coords,_ in pairs(aiInfo.streetEndPoints) do
        oi.debug(coords)
    end
end