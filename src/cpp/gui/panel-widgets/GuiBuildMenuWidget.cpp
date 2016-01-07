#include <string>
#include "global.h"
#include "map/Structure.h"
#include "gui/Identifiers.h"
#include "gui/components/map/GuiMap.h"
#include "gui/components/GuiAddBuildingWidget.h"
#include "gui/components/GuiButton.h"
#include "gui/components/GuiPushButton.h"
#include "gui/components/GuiStaticGraphicElement.h"
#include "gui/panel-widgets/GuiBuildMenuWidget.h"


GuiBuildMenuWidget::GuiBuildMenuWidget(const Context& context) : GuiPanelWidget(context) {
    // TODO in Config auslagern
    static struct {
        BuildingGroup buildingGroup;
        const std::string name;
        const std::string graphicSetName;
        const std::string graphicSetPressedName; // TODO Zustände nutzen

        const MapObjectType* mapObjectTypes[16];

    } buildingGroups[4] = {
        {
            BuildingGroup::CRAFTSMAN,
            _("Craftsmanship"),
            "add-building-group/craftsman", // TODO build-menu-Grafik verwenden
            "add-building-group/craftsman-pressed", { // TODO build-menu-Grafik verwenden
                nullptr,
                nullptr,
                nullptr,
                nullptr,
                context.configMgr->getMapObjectType("stonemason"),
                nullptr,
                nullptr,
                context.configMgr->getMapObjectType("toolsmiths"),
                context.configMgr->getMapObjectType("tobacco-goods"),
                context.configMgr->getMapObjectType("distillery"),
                nullptr,
                nullptr,
                context.configMgr->getMapObjectType("weaving-mill1"),
                context.configMgr->getMapObjectType("butchers"),
                nullptr,
                nullptr
            }
        }, {
            BuildingGroup::FARM,
            _("Farms & plantations"),
            "add-building-group/farm", // TODO build-menu-Grafik verwenden
            "add-building-group/farm-pressed", { // TODO build-menu-Grafik verwenden
                nullptr,
                nullptr,
                context.configMgr->getMapObjectType("sugarcane-plantation"),
                context.configMgr->getMapObjectType("sugarcane-field"),
                context.configMgr->getMapObjectType("tobacco-plantation"),
                context.configMgr->getMapObjectType("tobacco-field"),
                context.configMgr->getMapObjectType("spices-plantation"),
                context.configMgr->getMapObjectType("spices-field"),
                nullptr,
                nullptr,
                context.configMgr->getMapObjectType("sheep-farm"),
                context.configMgr->getMapObjectType("cattle-farm"),
                context.configMgr->getMapObjectType("hunters-hut"),
                context.configMgr->getMapObjectType("foresters"),
                context.configMgr->getMapObjectType("northern-forest1"),
                nullptr,
            }
        }, {
            BuildingGroup::PORT,
            _("Port facilities"),
            "build-menu/port",
            "build-menu/port-pressed", {
                nullptr,
                nullptr,
                nullptr,
                nullptr,
                nullptr,
                nullptr,
                nullptr,
                nullptr,
                nullptr,
                context.configMgr->getMapObjectType("pier"),
                nullptr,
                nullptr,
                context.configMgr->getMapObjectType("office1"),
                context.configMgr->getMapObjectType("office2"),
                nullptr,
                nullptr
            }
        }, {
            BuildingGroup::PUBLIC,
            _("Public buildings"),
            "build-menu/public",
            "build-menu/public-pressed", {
                context.configMgr->getMapObjectType("square1"),
                context.configMgr->getMapObjectType("square2"),
                context.configMgr->getMapObjectType("square3"),
                nullptr,
                context.configMgr->getMapObjectType("cathedral"),
                context.configMgr->getMapObjectType("tavern"),
                nullptr,
                nullptr,
                nullptr,
                nullptr,
                context.configMgr->getMapObjectType("farm-road"),
                context.configMgr->getMapObjectType("cobbled-street"),
                context.configMgr->getMapObjectType("pioneers-house1"),
                context.configMgr->getMapObjectType("marketplace"),
                context.configMgr->getMapObjectType("chapel"),
                nullptr
            }
        }
    };

    for (int groupIndex = 0; groupIndex < 4; groupIndex++) {
        // Grid
        GuiStaticGraphicElement* addBuildingGrid = new GuiStaticGraphicElement(context);
        const IGraphic* graphicAddBuildingGrid = context.graphicsMgr->getGraphicSet("add-building-grid")->getStatic()->getGraphic();
        addBuildingGrid->setCoords(775, 450, graphicAddBuildingGrid->getWidth(), graphicAddBuildingGrid->getHeight());
        addBuildingGrid->setGraphic(graphicAddBuildingGrid);
        addBuildingGrid->setVisible(false);
        context.guiMgr->registerElement(GUI_ID_ADD_BUILDING_GRID_BASE + groupIndex, addBuildingGrid);

        // Buttons im Grid
        for (int gridY = 0; gridY < 4; gridY++) {
            for (int gridX = 0; gridX < 4; gridX++) {
                int buildingIndex = gridY * 4 + gridX;

                const MapObjectType* mapObjectType = buildingGroups[groupIndex].mapObjectTypes[buildingIndex];
                if (mapObjectType == nullptr) {
                    continue;
                }

                std::string graphicSetName = context.graphicsMgr->getGraphicSetNameForAddBuildingButton(mapObjectType);
                const IGraphic* graphicAddBuildingButton =
                    context.graphicsMgr->getGraphicSet(graphicSetName)->getStatic()->getGraphic();

                GuiButton* addBuildingButton = new GuiButton(context);
                addBuildingButton->setCoords(
                    12 + 58 * gridX, 13 + 58 * gridY,
                    graphicAddBuildingButton->getWidth(), graphicAddBuildingButton->getHeight());
                addBuildingButton->setGraphic(graphicAddBuildingButton);
                addBuildingButton->setGraphicPressed(graphicAddBuildingButton);
                addBuildingButton->setStatusBarText(_(mapObjectType->getTitleMsgid()));
                addBuildingButton->setOnClickFunction([ this, context, mapObjectType ]() {
                    context.guiMgr->panelState.addingMapObject = mapObjectType;
                    context.guiMgr->panelState.buildingMenuOpen = false;
                    context.guiMgr->updateGuiFromPanelState();

                    ((GuiMap*) context.guiMgr->findElement(GUI_ID_MAP))->onStartAddingStructure();
                });

                context.guiMgr->registerElement(
                    GUI_ID_ADD_BUILDING_GRID_BUTTON_BASE + groupIndex * 16 + buildingIndex, addBuildingButton);
                addBuildingGrid->addChildElement(addBuildingButton);
            }
        }

        // Button für die Gruppe
        GuiPushButton* addBuildingPushButton = new GuiPushButton(context);
        addBuildingPushButton->setGraphic(
            context.graphicsMgr->getGraphicSet(buildingGroups[groupIndex].graphicSetName)->getStatic()->getGraphic());
        addBuildingPushButton->setGraphicPressed(
            context.graphicsMgr->getGraphicSet(buildingGroups[groupIndex].graphicSetPressedName)->getStatic()->getGraphic());
        addBuildingPushButton->setCoords(12 + groupIndex * 55, 340, 52, 64);
        addBuildingPushButton->setStatusBarText(buildingGroups[groupIndex].name);
        addBuildingPushButton->setOnClickFunction([ this, context, groupIndex ]() {
            // Wenn man die Gruppe nochmal klickt, die bereits ausgewählt ist und das ausgewählte Gebäude nicht
            // aus dieser Gruppe ist, wird das Gebäude gewechselt und eins aus der Gruppe genommen
            if (context.guiMgr->panelState.selectedBuildingGroup == (BuildingGroup) groupIndex &&
                context.guiMgr->panelState.buildingMenuOpen) {

                bool addingStructureInSelectedBuildingGroup = false;
                for (int i = 0; i < 16; i++) {
                    if (buildingGroups[groupIndex].mapObjectTypes[i] == context.guiMgr->panelState.addingMapObject) {
                        addingStructureInSelectedBuildingGroup = true;
                        break;
                    }
                }

                if (!addingStructureInSelectedBuildingGroup) {
                    // Gebäude unten links nehmen (= Index 12 von 16)
                    context.guiMgr->panelState.addingMapObject = buildingGroups[groupIndex].mapObjectTypes[12];
                }
            }
            else {
                context.guiMgr->panelState.selectedBuildingGroup = (BuildingGroup) groupIndex;
                context.guiMgr->panelState.buildingMenuOpen = true;
            }
            context.guiMgr->updateGuiFromPanelState();
        });
        context.guiMgr->registerElement(GUI_ID_ADD_BUILDING_PUSH_BUTTON_BASE + groupIndex, addBuildingPushButton);
        addChildElement(addBuildingPushButton);
    }

    // Gebäudebau: Infos, über zu platzierendes Gebäude
    GuiAddBuildingWidget* addBuildingWidget = new GuiAddBuildingWidget(context);
    context.guiMgr->registerElement(GUI_ID_ADD_BUILDING_WIDGET, addBuildingWidget);
    addChildElement(addBuildingWidget);
}