--
-- einfache KI für einen Spieler
------------------------------------

oi = OpenIsles

local playerIndex = 3       -- playerIndex, den diese KI steuert

local map                   -- Karte

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

function phase0()
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

end

function init()
    map = oi.getMap()

    -- unser Kontor finden
    for _,object in ipairs(oi.getObjects()) do
        if object.player == playerIndex and object.type == "office1" then
            officeCoords = { x = object.x, y = object.y }
            streetOrigin = { x = officeCoords.x + 1, y = officeCoords.y + 2 } -- TODO Ausrichtung berücksichtigen
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
        -- TODO
    elseif phase == 2 then
        -- TODO
    elseif phase == 3 then
        -- TODO
    elseif phase == 4 then
        -- TODO
    end
end