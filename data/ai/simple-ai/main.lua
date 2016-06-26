--
-- einfache KI für einen Spieler
------------------------------------

oi = OpenIsles

dofile("data/ai/simple-ai/utils.lua")
dofile("data/ai/simple-ai/streets.lua")

aiInfo = {}                 -- Diese globale Tabelle enthält alle Infos, die wir für die KI brauchen -- TODO aktuell is das nur für eine Insel!
aiInfo.playerIndex = 3      -- playerIndex, den diese KI steuert

local map                   -- Karte
local isles                 -- Inseln

local lastRunTicks = 0      -- Zeit, wann diese KI zuletzt was gemacht hat
local phase = 0             -- Phasen, was wir bauen:
--[[
    phase = 0
     Wir bauen ein einfaches Straßengerüstet mit 3 Blöcken
     Wir bauen einen Marktplatz, um Platz zu haben
     Wir bauen einen Förster und eine Fischerhütte (TODO Fischerhütte gibts noch nicht ;-))
    phase = 1
     Wir bauen Häuser neben dem Marktplatz, bis wir 12 Häuser haben,
     danach einen zweiten Förster.
    phase = 2
     Wir reißen Platz ein und bauen 2 Schaffarmen und wenn die erste Wolle fertig ist,
     eine Webstube,
     danach Häuser auf 17 erhöhen
    phase = 3
     Nochmal Straßen erweitern und abreißen und 2 Rindfarmen und danach den Fleischer bauen.
     Weitere 7 Häuser bauen, danach die Kapelle.
     -> Umbau zu Siedler
    phase = 4
     Sind genügend Siedler da, wird eine zweite Insel erschlossen
--]]


-- Helper-Funktionen --------------------------------------------------------------------------------------------------

--[[
-- Prüft, ob die Koordinate (`mapCoords.x`, `mapCoords.y`) auf der Insel `isle` liegt.
--]]
local function isCoordsOnIsle(isle, mapCoords)
    if (mapCoords.x >= isle.x) and (mapCoords.x <= isle.x + isle.width) and
       (mapCoords.y >= isle.y) and (mapCoords.y <= isle.y + isle.height) then

        return true
    else
        return false
    end
end

--[[
-- Liefert die Insel zurück, die sich auf Koordinate (`mapCoords.x`, `mapCoords.y`) befindet.
--]]
local function findIsle(mapCoords)
    for _,isle in pairs(isles) do
        if isCoordsOnIsle(isle, mapCoords) == true then
            return isle
        end
    end

    return nil;
end

--[[
-- Zählt, wie viele Objekte vom Type `mapObjectType` der Spieler `playerIndex` auf der Insel `isle` hat
--]]
local function countMapObjectOfTypeOnIsle(mapObjectType, playerIndex, isle)
    local count = 0;
    for _,object in pairs(oi.getMapObjectsFixed()) do
        if object.player == playerIndex and object.type == mapObjectType then
            if isCoordsOnIsle(isle, buildCoords(object.x, object.y)) == true then
                count = count + 1
            end
        end
    end

    return count
end

-- KI-Logik der verschiedenen Phasen ----------------------------------------------------------------------------------

local function phase0()
    -- Straßen vom Kontor aus bauen
    local streetOrigin = getStreetOrigin(aiInfo.officeObject)
    if oi.getMapObjectFixedAt(streetOrigin) == nil then
        buildStreetSystemAt(streetOrigin)
        return
    end

    -- Straßen erweitern
    if countMapObjectOfTypeOnIsle("farm-road", aiInfo.playerIndex, aiInfo.isle) < 30 then
        local coords = keyToCoords(randomPickFromSet(aiInfo.streetEndPoints))
        buildStreetSystemAt(coords)
        return
    end

    -- TODO Förster, Fischerhütte (TODO^2) und Marktplatz

    phase = phase + 1
end

local function phase1()
    -- Resourcen für Haus da?
    local colony = oi.getColonyAt(aiInfo.officeCoords);
    if (colony.goods.wood < 3) then
        return
    end

    -- Zählen, wie viele Häuser schon da sind
    local countHouses = 0
    for i = 1, 5 do
        countHouses = countHouses + countMapObjectOfTypeOnIsle("pioneers-house" .. i, aiInfo.playerIndex, aiInfo.isle)
    end

    -- Häuser bauen, bis wir 12 Stück haben
    if countHouses >= 12 then
        phase = phase + 1
    end

    -- Ein neues Haus bauen
    local coords = {
        x = math.random(aiInfo.isle.x, aiInfo.isle.x + aiInfo.isle.width - 1),
        y = math.random(aiInfo.isle.y, aiInfo.isle.y + aiInfo.isle.height - 1)
    }

    -- pcall, um Fehler einfach zu ignorieren; wir bauen irgendwo auf der Insel, kp, ob wir da dürfen (TODO verbesserungswürdig)
    pcall(oi.build, aiInfo.playerIndex, "pioneers-house1", coords.x, coords.y, "south");
end

local function phase2()
    oi.debug("Reached phase2 :-)")
end

-- Callbacks aus OpenIsles --------------------------------------------------------------------------------------------

function init()
    isles = oi.getIsles()

    -- unser Kontor finden
    for _,object in pairs(oi.getMapObjectsFixed()) do
        if object.player == aiInfo.playerIndex and object.type == "office1" then
            aiInfo.officeObject = object
            aiInfo.officeCoords = buildCoords(object.x, object.y)
            aiInfo.isle = findIsle(aiInfo.officeCoords)
            aiInfo.streetEndPoints = {}
            break
        end
    end
    assert(aiInfo.officeCoords ~= nil, "Could not find our office");
end

function main()
    -- nur alle x Sekunden was tun
    local ticksNow = oi.getGameTicks()
    if (ticksNow < lastRunTicks + 2000) then return end
    lastRunTicks = ticksNow

    if phase == 0 then
        phase0()
    elseif phase == 1 then
        phase1()
    elseif phase == 2 then
        phase2()
    elseif phase == 3 then
        -- TODO
    elseif phase == 4 then
        -- TODO
    end
end