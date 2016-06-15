--
-- Test-Script für die KI
------------------------------------

oi = OpenIsles

local firstRun = true
local buildSomething = false
local lastOutputTicks = 0

function main()
    -- TODO eine init()-Methode wäre schön, wenn wir bei Spielstart benachrichtigt werden, z.B. mit Map und Spieleranzahl
    local playerCount = oi.getPlayerCount()

    -- einmalige Begrüßung
    if firstRun == true then
        oi.debug("Hello World from AI :-)")
        firstRun = false
    end

    -- nur alle 5 Sekunden was tun
    local gameTicks = oi.getGameTicks()
    if gameTicks <= lastOutputTicks + 5000 then
        return
    end
    lastOutputTicks = gameTicks

    -- Geld aller Spieler ausgeben
    for i = 1, 4 do
        local player = oi.getPlayer(i)

        oi.debug("Player '" .. player.name .. "' (type " .. player.type .. ") has " .. player.coins .. " coins.")
    end

    -- nach 30 Sekunden bauen wir was
    if (buildSomething == false) and (gameTicks > 30000) then
        for y = 212, 217 do
            oi.build(3, "farm-road", 52, y, "south");
        end
        oi.build(3, "foresters", 53, 215, "north");

        buildSomething = true
    end
end