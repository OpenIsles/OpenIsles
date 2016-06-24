--
-- einfache KI für einen Spieler
------------------------------------

oi = OpenIsles

local playerIndex = 3       -- playerIndex, den diese KI steuert

local map                   -- Karte
local isles                 -- Inseln

local mainIsle              -- Unsere Hauptinsel, wo das Kontor steht
local officeCoords          -- Koordinaten unseres Kontors
local streetOrigin          -- Koordinaten des erste Wegfelds für das Wege-Raster

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
-- Zählt, wie viele Objekte vom Type `mapObjectType` der Spieler `playerIndex` auf der Insel hat, die auf der
-- Koordinate (`mapCoords.x`, `mapCoords.y`) liegt.
--]]
local function countMapObjectOfTypeOnIsle(mapObjectType, playerIndex, mapCoords)
    -- Insel suchen
    local isle = findIsle(mapCoords)
    assert(isle ~= nil, "Invalid coords. There is no isle.");

    -- Nun alle Objekte durchgehen und zählen
    local count = 0;
    for _,object in pairs(oi.getObjects()) do
        if object.player == playerIndex and object.type == mapObjectType then
            if isCoordsOnIsle(isle, mapCoords) == true then
                count = count + 1
            end
        end
    end

    return count
end

-- KI-Logik der verschiedenen Phasen ----------------------------------------------------------------------------------

local function phase0()
    -- einfach im fixen Umkreis Straßen bauen
    local radius = 9

    for y = streetOrigin.y - radius, streetOrigin.y + radius do
        for x = streetOrigin.x - radius, streetOrigin.x + radius do
            if ((y % 6 == streetOrigin.y % 6) or (x % 6 == streetOrigin.x % 6)) then
                -- pcall, um unbebaubare Felder einfach zu ignorieren (TODO verbesserungswürdig)
                pcall(oi.build, playerIndex, "farm-road", x, y, "south");
            end
        end
    end

    -- TODO Förster, Fischerhütte (TODO^2) und Marktplatz

    phase = phase + 1
end

local function phase1()
    -- Resourcen für Haus da?
    local colony = oi.getColonyAt(officeCoords);
    if (colony.goods.wood < 3) then
        return
    end

    -- Zählen, wie viele Häuser schon da sind
    local countHouses = 0
    for i = 1, 5 do
        countHouses = countHouses + countMapObjectOfTypeOnIsle("pioneers-house" .. i, playerIndex, officeCoords)
    end

    -- Häuser bauen, bis wir 12 Stück haben
    if countHouses >= 12 then
        phase = phase + 1
    end

    -- Ein neues Haus bauen
    local coords = {
        x = math.random(mainIsle.x, mainIsle.x + mainIsle.width - 1),
        y = math.random(mainIsle.y, mainIsle.y + mainIsle.height - 1)
    }

    -- pcall, um Fehler einfach zu ignorieren; wir bauen irgendwo auf der Insel, kp, ob wir da dürfen (TODO verbesserungswürdig)
    pcall(oi.build, playerIndex, "pioneers-house1", coords.x, coords.y, "south");
end

local function phase2()
    oi.debug("Reached phase2 :-)")
end

-- Callbacks aus OpenIsles --------------------------------------------------------------------------------------------

function init()
    map = oi.getMap()
    isles = oi.getIsles()

    -- unser Kontor finden
    for _,object in pairs(oi.getObjects()) do
        if object.player == playerIndex and object.type == "office1" then
            officeCoords = { x = object.x, y = object.y }
            streetOrigin = { x = officeCoords.x + 1, y = officeCoords.y + 2 } -- TODO Ausrichtung berücksichtigen
            mainIsle = findIsle(officeCoords)
            break
        end
    end
    assert(officeCoords ~= nil, "Could not find our office");
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