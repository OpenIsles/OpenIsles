--
-- Test-Script für die KI
------------------------------------

oi = OpenIsles

local firstRun = true
local buildSomething = false
local demolishSomething = false
local lastOutputTicks = 0

local map
local isles
local ships
local objects

function init()
    map = oi.getMap()

    oi.debug("mapSize = " .. map.width .. "x" .. map.height)
    oi.debug("test: map[34,56] = " .. map['34,56'])

    isles = oi.getIsles()

    for _,isle in ipairs(isles) do
        oi.debug("isle: x = " .. isle.x .. ", y = " .. isle.y ..", size = " .. isle.width .. "x" .. isle.height)
    end

    ships = oi.getShips()

    for _,ship in ipairs(ships) do
        oi.debug("ship: x = " .. ship.x .. ", y = " .. ship.y ..", player = " .. ship.player .. ", name = " .. ship.name)
    end

    objects = oi.getObjects()

    for _,object in ipairs(objects) do
        oi.debug("object: x = " .. object.x .. ", y = " .. object.y ..", player = " .. object.player .. ", type = " .. object.type)
    end
end

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

    -- nach 40 Sekunden reißen wir wieder was ab
    if (demolishSomething == false) and (gameTicks > 40000) then
        oi.demolish(3, 52, 217);
        demolishSomething = true
    end
end