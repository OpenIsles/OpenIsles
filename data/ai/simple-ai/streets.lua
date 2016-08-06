--
-- Funktionen zum Straßenbau / Planung
------------------------------------------

-- aiInfo.streetEndPoints   Set mit Endpunkten des Straßensystems
-- aiInfo.blocksWithStreets Set mit Mittelpunkten für 6x6-Blöcke, die wir mit Straßen erschlossen haben.
--                          Siehe findFree6x6Blocks()


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

    -- Merken, welche Blöcke damit neu dazukommen
    aiInfo.blocksWithStreets[coordsToKey(buildCoords(sourceCoords.x + 3, sourceCoords.y + 3))] = true
    aiInfo.blocksWithStreets[coordsToKey(buildCoords(sourceCoords.x - 4, sourceCoords.y + 3))] = true
    aiInfo.blocksWithStreets[coordsToKey(buildCoords(sourceCoords.x + 3, sourceCoords.y - 4))] = true
    aiInfo.blocksWithStreets[coordsToKey(buildCoords(sourceCoords.x - 4, sourceCoords.y - 4))] = true
end

--[[
-- Findet alle komplett freien 6x6-Blöcke im Straßenraster.
-- Zurückgegeben wird die "mittlere" (s.u.) Koordinate. Das ist die von den vieren, die auf x- und y- den kleineren
-- Wert hat.
--
-- Ein Block wird nur als frei gewertet, wenn alle 36 Felder zum bebaubaren Bereich von uns gehören.
--
--
-- ░█░░░░░░█░
-- ██████████
-- ░█░░░░░░█░
-- ░█░░░░░░█░
-- ░█░░x░░░█░
-- ░█░░░░░░█░
-- ░█░░░░░░█░
-- ░█░░░░░░█░
-- ██████████
--]]
function findFree6x6Blocks()
    local free6x6Blocks = {}

    -- Mögliche Koordinatenmitten checken
    for blockMiddleCoords,_ in pairs(aiInfo.blocksWithStreets) do
        local middleCoords = keyToCoords(blockMiddleCoords)

        local blockIsFree = true
        for y = middleCoords.y - 2, middleCoords.y + 3 do
            for x = middleCoords.x - 2, middleCoords.x + 3 do
                local coords = buildCoords(x, y)

                -- unser Gebiet?
                local mapTile = oi.getMapTileAt(coords)
                if mapTile.player ~= aiInfo.playerIndex then
                    blockIsFree = false
                    break
                end

                -- Gelände ok?
                if mapTile.mapTileType ~= BITMASK_GRASS then
                    blockIsFree = false
                    break
                end

                -- schon bebaut?
                local mapObjectFixed = oi.getMapObjectFixedAt(coords)
                if mapObjectFixed ~= nil then
                    if (mapObjectFixed.type ~= "northern-forest1") and
                       (mapObjectFixed.type ~= "northern-forest2") then

                        blockIsFree = false
                        break
                    end
                end
            end

            if blockIsFree == false then
                break
            end
        end

        -- Block frei?
        if blockIsFree == true then
            table.insert(free6x6Blocks, blockMiddleCoords)
        end
    end

    return free6x6Blocks
end

--[[
-- Verbindet ein Gebäude (buildingCoords gibt dessen Koordinaten vor) mit dem Straßennetz.
--
-- Wir probieren gerade Straßenzüge von jedem Punkt des Gebäudes aus und gucken, ob wir es zur Haupt-Staße schaffen.
--]]
function connectBuildingToStreetSystem(buildingCoords)
    local building = oi.getMapObjectFixedAt(buildingCoords)

    local buildingWidth, buildingHeight
    if building.view == "south" or building.view == "north" then
        buildingWidth = building.width
        buildingHeight = building.height
    else
        buildingWidth = building.height
        buildingHeight = building.width
    end

    -- Mögliche Lösungen finden
    local possibleConnections = {}
    local testSolution = function(startCoords, increment)
        local coords = startCoords
        local currentConnection = {}

        for i = 1, 3 do -- maximale 3 Kacheln lang darf die Verbindungsstraße werden
            local mapObjectFixed = oi.getMapObjectFixedAt(coords)
            if mapObjectFixed ~= nil then
                if (mapObjectFixed.type == "farm-road") or
                   (mapObjectFixed.type == "cobbled-street") then
                    -- Hauptstraße gefunden -> Lösung

                    table.insert(possibleConnections, currentConnection)
                    return
                end

                if (mapObjectFixed.type ~= "northern-forest1") and
                   (mapObjectFixed.type ~= "northern-forest2") then

                    -- was im Weg -> keine Lösung
                    return
                end
            end

            -- Koordinaten merken; wichtig: nicht coords direkt nehmen, das verändern wir gleich
            table.insert(currentConnection, buildCoords(coords.x, coords.y))

            increment(coords)
        end
    end

    for y = building.y, building.y + buildingHeight - 1 do
        testSolution(buildCoords(building.x + buildingWidth, y), function(coords) coords.x = coords.x + 1 end)
        testSolution(buildCoords(building.x - 1, y), function(coords) coords.x = coords.x - 1 end)
    end
    for x = building.x, building.x + buildingWidth - 1 do
        testSolution(buildCoords(x, building.y + buildingHeight), function(coords) coords.y = coords.y + 1 end)
        testSolution(buildCoords(x, building.y - 1), function(coords) coords.y = coords.y - 1 end)
    end

    -- Irgendeine Lösung wählen und bauen
    if #possibleConnections == 0 then
        oi.debug("Error: Could not connect building at (" .. building.x .. "," .. building.y .. ") to street network")
    end

    local connection = randomPickFromArray(possibleConnections)
    for _,coords in ipairs(connection) do
        oi.build(aiInfo.playerIndex, "farm-road", coords.x, coords.y, "south")
    end
end