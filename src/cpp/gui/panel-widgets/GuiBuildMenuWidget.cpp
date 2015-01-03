#include "map/Structure.h"
#include "gui/Identifiers.h"
#include "gui/components/GuiAddBuildingWidget.h"
#include "gui/components/GuiButton.h"
#include "gui/components/GuiPushButton.h"
#include "gui/components/GuiStaticElement.h"
#include "gui/panel-widgets/GuiBuildMenuWidget.h"


GuiBuildMenuWidget::GuiBuildMenuWidget(const Context* const context) : GuiPanelWidget(context) {
    // TODO in Config auslagern
    static struct {
        BuildingGroup buildingGroup;
        const char* name;
        const std::string graphicSetName;
        const std::string graphicSetPressedName; // TODO Zustände nutzen

        struct {
            StructureType structureType;
            const char* name;
            const std::string graphicSetName;
        } buildings[16];

    } buildingGroups[4] = {
        {
            BuildingGroup::CRAFTSMAN,
            "Handwerksbetriebe",
            "add-building-group/craftsman",
            "add-building-group/craftsman-pressed", {
                { StructureType::NO_STRUCTURE, nullptr, "add-building-button/dummy" },
                { StructureType::NO_STRUCTURE, nullptr, "add-building-button/dummy" },
                { StructureType::NO_STRUCTURE, nullptr, "add-building-button/dummy" },
                { StructureType::NO_STRUCTURE, nullptr, "add-building-button/dummy" },
                {
                    StructureType::STONEMASON,
                    "Steinmetz",
                    "add-building-button/stonemason"
                },
                { StructureType::NO_STRUCTURE, nullptr, "add-building-button/dummy" },
                { StructureType::NO_STRUCTURE, nullptr, "add-building-button/dummy" },
                {
                    StructureType::TOOLSMITHS,
                    "Werkzeugschmiede",
                    "add-building-button/toolsmiths"
                },
                { StructureType::NO_STRUCTURE, nullptr, "add-building-button/dummy" },
                { StructureType::NO_STRUCTURE, nullptr, "add-building-button/dummy" },
                { StructureType::NO_STRUCTURE, nullptr, "add-building-button/dummy" },
                { StructureType::NO_STRUCTURE, nullptr, "add-building-button/dummy" },
                {
                    StructureType::WEAVING_MILL1,
                    "Webstube",
                    "add-building-button/weaving-mill1"
                },
                {
                    StructureType::BUTCHERS,
                    "Fleischerei",
                    "add-building-button/butchers"
                },
                { StructureType::NO_STRUCTURE, nullptr, "add-building-button/dummy" },
                { StructureType::NO_STRUCTURE, nullptr, "add-building-button/dummy" }
            }
        }, {
            BuildingGroup::FARM,
            "Farmen & Plantagen",
            "add-building-group/farm",
            "add-building-group/farm-pressed", {
                { StructureType::NO_STRUCTURE, nullptr, "add-building-button/dummy" },
                { StructureType::NO_STRUCTURE, nullptr, "add-building-button/dummy" },
                { StructureType::NO_STRUCTURE, nullptr, "add-building-button/dummy" },
                { StructureType::NO_STRUCTURE, nullptr, "add-building-button/dummy" },
                { StructureType::NO_STRUCTURE, nullptr, "add-building-button/dummy" },
                { StructureType::NO_STRUCTURE, nullptr, "add-building-button/dummy" },
                { StructureType::NO_STRUCTURE, nullptr, "add-building-button/dummy" },
                { StructureType::NO_STRUCTURE, nullptr, "add-building-button/dummy" },
                { StructureType::NO_STRUCTURE, nullptr, "add-building-button/dummy" },
                { StructureType::NO_STRUCTURE, nullptr, "add-building-button/dummy" },
                {
                    StructureType::SHEEP_FARM,
                    "Schaffarm",
                    "add-building-button/sheep-farm"
                },
                {
                    StructureType::CATTLE_FARM,
                    "Rinderfarm",
                    "add-building-button/cattle-farm"
                },
                {
                    StructureType::HUNTERS_HUT,
                    "Jagdhütte",
                    "add-building-button/hunters-hut"
                },
                {
                    StructureType::FORESTERS,
                    "Förster",
                    "add-building-button/foresters"
                },
                { StructureType::NO_STRUCTURE, nullptr, "add-building-button/dummy" },
                { StructureType::NO_STRUCTURE, nullptr, "add-building-button/dummy" }
            }
        }, {
            BuildingGroup::PORT,
            "Hafenanlagen",
            "add-building-group/port",
            "add-building-group/port-pressed", {
                { StructureType::NO_STRUCTURE, nullptr, "add-building-button/dummy" },
                { StructureType::NO_STRUCTURE, nullptr, "add-building-button/dummy" },
                { StructureType::NO_STRUCTURE, nullptr, "add-building-button/dummy" },
                { StructureType::NO_STRUCTURE, nullptr, "add-building-button/dummy" },
                { StructureType::NO_STRUCTURE, nullptr, "add-building-button/dummy" },
                { StructureType::NO_STRUCTURE, nullptr, "add-building-button/dummy" },
                { StructureType::NO_STRUCTURE, nullptr, "add-building-button/dummy" },
                { StructureType::NO_STRUCTURE, nullptr, "add-building-button/dummy" },
                { StructureType::NO_STRUCTURE, nullptr, "add-building-button/dummy" },
                { StructureType::NO_STRUCTURE, nullptr, "add-building-button/dummy" },
                { StructureType::NO_STRUCTURE, nullptr, "add-building-button/dummy" },
                { StructureType::NO_STRUCTURE, nullptr, "add-building-button/dummy" },
                {
                    StructureType::OFFICE1,
                    "Kontor I",
                    "add-building-button/office1"
                },
                { StructureType::NO_STRUCTURE, nullptr, "add-building-button/dummy" },
                { StructureType::NO_STRUCTURE, nullptr, "add-building-button/dummy" },
                { StructureType::NO_STRUCTURE, nullptr, "add-building-button/dummy" }
            }
        }, {
            BuildingGroup::PUBLIC,
            "Öffentliche Gebäude",
            "add-building-group/public",
            "add-building-group/public-pressed", {
                { StructureType::NO_STRUCTURE, nullptr, "add-building-button/dummy" },
                { StructureType::NO_STRUCTURE, nullptr, "add-building-button/dummy" },
                { StructureType::NO_STRUCTURE, nullptr, "add-building-button/dummy" },
                { StructureType::NO_STRUCTURE, nullptr, "add-building-button/dummy" },
                { StructureType::NO_STRUCTURE, nullptr, "add-building-button/dummy" },
                { StructureType::NO_STRUCTURE, nullptr, "add-building-button/dummy" },
                { StructureType::NO_STRUCTURE, nullptr, "add-building-button/dummy" },
                { StructureType::NO_STRUCTURE, nullptr, "add-building-button/dummy" },
                { StructureType::NO_STRUCTURE, nullptr, "add-building-button/dummy" },
                { StructureType::NO_STRUCTURE, nullptr, "add-building-button/dummy" },
                { StructureType::NO_STRUCTURE, nullptr, "add-building-button/dummy" },
                {
                    StructureType::STREET,
                    "Plasterstraße",
                    "add-building-button/street"
                }, {
                    StructureType::PIONEERS_HOUSE1,
                    "Haus",
                    "add-building-button/house"
                }, {
                    StructureType::MARKETPLACE,
                    "Marktplatz",
                    "add-building-button/marketplace"
                }, {
                    StructureType::CHAPEL,
                    "Kapelle",
                    "add-building-button/chapel"
                },
                { StructureType::NO_STRUCTURE, nullptr, "add-building-button/dummy" }
            }
        }
    };

    for (int groupIndex = 0; groupIndex < 4; groupIndex++) {
        // Grid
        GuiStaticElement* addBuildingGrid = new GuiStaticElement(context);
        const IGraphic* graphicAddBuildingGrid = context->graphicsMgr->getGraphicSet("add-building-grid")->getStatic()->getGraphic();
        addBuildingGrid->setCoords(775, 450, graphicAddBuildingGrid->getWidth(), graphicAddBuildingGrid->getHeight());
        addBuildingGrid->setGraphic(graphicAddBuildingGrid);
        addBuildingGrid->setVisible(false);
        context->guiMgr->registerElement(GUI_ID_ADD_BUILDING_GRID_BASE + groupIndex, addBuildingGrid);

        // Buttons im Grid
        for (int gridY = 0; gridY < 4; gridY++) {
            for (int gridX = 0; gridX < 4; gridX++) {
                int buildingIndex = gridY * 4 + gridX;
                StructureType structureType = buildingGroups[groupIndex].buildings[buildingIndex].structureType;

                if (structureType == NO_STRUCTURE) {
                    continue;
                }

                GuiButton* addBuildingButton = new GuiButton(context);
                const IGraphic* graphicAddBuildingButton = context->graphicsMgr
                    ->getGraphicSet(buildingGroups[groupIndex].buildings[buildingIndex].graphicSetName)->getStatic()->getGraphic();
                addBuildingButton->setCoords(
                    12 + 58 * gridX, 13 + 58 * gridY, graphicAddBuildingButton->getWidth(), graphicAddBuildingButton->getHeight());
                addBuildingButton->setGraphic(graphicAddBuildingButton);
                addBuildingButton->setGraphicPressed(graphicAddBuildingButton);
                addBuildingButton->setOnClickFunction([ this, context, structureType ]() {
                    context->guiMgr->panelState.addingStructure = structureType;
                    context->guiMgr->panelState.buildingMenuOpen = false;
                    context->guiMgr->updateGuiFromPanelState();
                });
                context->guiMgr->registerElement(GUI_ID_ADD_BUILDING_GRID_BUTTON_BASE + groupIndex * 16 + buildingIndex, addBuildingButton);
                addBuildingGrid->addChildElement(addBuildingButton);
            }
        }

        // Button für die Gruppe
        GuiPushButton* addBuildingPushButton = new GuiPushButton(context);
        addBuildingPushButton->setGraphic(
            context->graphicsMgr->getGraphicSet(buildingGroups[groupIndex].graphicSetName)->getStatic()->getGraphic());
        addBuildingPushButton->setGraphicPressed(
            context->graphicsMgr->getGraphicSet(buildingGroups[groupIndex].graphicSetPressedName)->getStatic()->getGraphic());
        addBuildingPushButton->setCoords(12 + groupIndex * 55, 378, 52, 64);
        addBuildingPushButton->setOnClickFunction([ this, context, groupIndex ]() {
            // Wenn man die Gruppe nochmal klickt, die bereits ausgewählt ist und das ausgewählte Gebäude nicht
            // aus dieser Gruppe ist, wird das Gebäude gewechselt und eins aus der Gruppe genommen
            if (context->guiMgr->panelState.selectedBuildingGroup == (BuildingGroup) groupIndex &&
                context->guiMgr->panelState.buildingMenuOpen) {

                bool addingStructureInSelectedBuildingGroup = false;
                for (int i = 0; i < 16; i++) {
                    if (buildingGroups[groupIndex].buildings[i].structureType == context->guiMgr->panelState.addingStructure) {
                        addingStructureInSelectedBuildingGroup = true;
                        break;
                    }
                }

                if (!addingStructureInSelectedBuildingGroup) {
                    // Gebäude unten links nehmen (= Index 12 von 16)
                    context->guiMgr->panelState.addingStructure = buildingGroups[groupIndex].buildings[12].structureType;
                }
            }
            else {
                context->guiMgr->panelState.selectedBuildingGroup = (BuildingGroup) groupIndex;
                context->guiMgr->panelState.buildingMenuOpen = true;
            }
            context->guiMgr->updateGuiFromPanelState();
        });
        context->guiMgr->registerElement(GUI_ID_ADD_BUILDING_PUSH_BUTTON_BASE + groupIndex, addBuildingPushButton);
        addChildElement(addBuildingPushButton);
    }

    // Gebäudebau: Infos, über zu platzierendes Gebäude
    GuiAddBuildingWidget* addBuildingWidget = new GuiAddBuildingWidget(context);
    context->guiMgr->registerElement(GUI_ID_ADD_BUILDING_WIDGET, addBuildingWidget);
    addChildElement(addBuildingWidget);
}