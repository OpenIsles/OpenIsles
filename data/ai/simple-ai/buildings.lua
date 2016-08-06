--
-- Funktionen zum Gebäudebau
--------------------------------


--[[
-- Füllt den Einzugsbereich eines Gebäudes (buildingCoords gibt dessen Koordinaten vor). Z.B. den Förster mit Wald.
-- Als Parameter mapObjectTypeName wird der Name des MapObjectTypes benannt, der gebaut werden soll.
--]]
function fillBuildingCatchmentArea(buildingCoords, mapObjectTypeName)
    local catchmentArea = oi.getCatchmentAreaForBuilding(buildingCoords)

    for _,coords in pairs(catchmentArea) do
        local mapObjectFixed = oi.getMapObjectFixedAt(coords)
        if mapObjectFixed == nil then
            oi.build(aiInfo.playerIndex, mapObjectTypeName, coords.x, coords.y, "south")
        end
    end
end