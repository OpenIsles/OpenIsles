#include "map/Structure.h"
#include "gui/GuiMgr.h"
#include "gui/Identifiers.h"
#include "gui/components/GuiAddBuildingWidget.h"
#include "gui/components/GuiButton.h"
#include "gui/components/GuiPushButton.h"
#include "gui/components/GuiStaticElement.h"
#include "gui/panel-widgets/GuiBuildMenuWidget.h"


// Aus main.cpp importiert
extern GraphicsMgr* graphicsMgr;
extern GuiMgr* guiMgr;


GuiBuildMenuWidget::GuiBuildMenuWidget() {
    // TODO in Config auslagern
    static struct {
        BuildingGroup buildingGroup;
        const char* name;
        const char* graphicFilename;
        const char* graphicPressedFilename;

        struct {
            StructureType structureType;
            const char* name;
            OtherGraphic graphic;
        } buildings[16];

    } buildingGroups[4] = {
        {
            BuildingGroup::CRAFTSMAN,
            "Handwerksbetriebe",
            "data/img/gui/button-add-building-craftsman.png",
            "data/img/gui/button-add-building-craftsman-pressed.png", {
            { StructureType::NO_STRUCTURE, nullptr, OtherGraphic::ADD_BUILDING_DUMMY },
            { StructureType::NO_STRUCTURE, nullptr, OtherGraphic::ADD_BUILDING_DUMMY },
            { StructureType::NO_STRUCTURE, nullptr, OtherGraphic::ADD_BUILDING_DUMMY },
            { StructureType::NO_STRUCTURE, nullptr, OtherGraphic::ADD_BUILDING_DUMMY },
            {
                StructureType::STONEMASON,
                "Steinmetz",
                OtherGraphic::ADD_BUILDING_STONEMASON
            },
            { StructureType::NO_STRUCTURE, nullptr, OtherGraphic::ADD_BUILDING_DUMMY },
            { StructureType::NO_STRUCTURE, nullptr, OtherGraphic::ADD_BUILDING_DUMMY },
            {
                StructureType::TOOLSMITHS,
                "Werkzeugschmiede",
                OtherGraphic::ADD_BUILDING_TOOLSMITHS
            },
            { StructureType::NO_STRUCTURE, nullptr, OtherGraphic::ADD_BUILDING_DUMMY },
            { StructureType::NO_STRUCTURE, nullptr, OtherGraphic::ADD_BUILDING_DUMMY },
            { StructureType::NO_STRUCTURE, nullptr, OtherGraphic::ADD_BUILDING_DUMMY },
            { StructureType::NO_STRUCTURE, nullptr, OtherGraphic::ADD_BUILDING_DUMMY },
            {
                StructureType::WEAVING_MILL1,
                "Webstube",
                OtherGraphic::ADD_BUILDING_WEAVING_MILL1
            },
            {
                StructureType::BUTCHERS,
                "Fleischerei",
                OtherGraphic::ADD_BUILDING_BUTCHERS
            },
            { StructureType::NO_STRUCTURE, nullptr, OtherGraphic::ADD_BUILDING_DUMMY },
            { StructureType::NO_STRUCTURE, nullptr, OtherGraphic::ADD_BUILDING_DUMMY }
        }
        }, {
            BuildingGroup::FARM,
            "Farmen & Plantagen",
            "data/img/gui/button-add-building-farm.png",
            "data/img/gui/button-add-building-farm-pressed.png", {
                { StructureType::NO_STRUCTURE, nullptr, OtherGraphic::ADD_BUILDING_DUMMY },
                { StructureType::NO_STRUCTURE, nullptr, OtherGraphic::ADD_BUILDING_DUMMY },
                { StructureType::NO_STRUCTURE, nullptr, OtherGraphic::ADD_BUILDING_DUMMY },
                { StructureType::NO_STRUCTURE, nullptr, OtherGraphic::ADD_BUILDING_DUMMY },
                { StructureType::NO_STRUCTURE, nullptr, OtherGraphic::ADD_BUILDING_DUMMY },
                { StructureType::NO_STRUCTURE, nullptr, OtherGraphic::ADD_BUILDING_DUMMY },
                { StructureType::NO_STRUCTURE, nullptr, OtherGraphic::ADD_BUILDING_DUMMY },
                { StructureType::NO_STRUCTURE, nullptr, OtherGraphic::ADD_BUILDING_DUMMY },
                { StructureType::NO_STRUCTURE, nullptr, OtherGraphic::ADD_BUILDING_DUMMY },
                { StructureType::NO_STRUCTURE, nullptr, OtherGraphic::ADD_BUILDING_DUMMY },
                {
                    StructureType::SHEEP_FARM,
                    "Schaffarm",
                    OtherGraphic::ADD_BUILDING_SHEEP_FARM
                },
                {
                    StructureType::CATTLE_FARM,
                    "Rinderfarm",
                    OtherGraphic::ADD_BUILDING_CATTLE_FARM
                },
                { StructureType::NO_STRUCTURE, nullptr, OtherGraphic::ADD_BUILDING_DUMMY },
                {
                    StructureType::FORESTERS,
                    "Förster",
                    OtherGraphic::ADD_BUILDING_DUMMY
                },
                { StructureType::NO_STRUCTURE, nullptr, OtherGraphic::ADD_BUILDING_DUMMY },
                { StructureType::NO_STRUCTURE, nullptr, OtherGraphic::ADD_BUILDING_DUMMY }
            }
        }, {
            BuildingGroup::PORT,
            "Hafenanlagen",
            "data/img/gui/button-add-building-port.png",
            "data/img/gui/button-add-building-port-pressed.png", {
                { StructureType::NO_STRUCTURE, nullptr, OtherGraphic::ADD_BUILDING_DUMMY },
                { StructureType::NO_STRUCTURE, nullptr, OtherGraphic::ADD_BUILDING_DUMMY },
                { StructureType::NO_STRUCTURE, nullptr, OtherGraphic::ADD_BUILDING_DUMMY },
                { StructureType::NO_STRUCTURE, nullptr, OtherGraphic::ADD_BUILDING_DUMMY },
                { StructureType::NO_STRUCTURE, nullptr, OtherGraphic::ADD_BUILDING_DUMMY },
                { StructureType::NO_STRUCTURE, nullptr, OtherGraphic::ADD_BUILDING_DUMMY },
                { StructureType::NO_STRUCTURE, nullptr, OtherGraphic::ADD_BUILDING_DUMMY },
                { StructureType::NO_STRUCTURE, nullptr, OtherGraphic::ADD_BUILDING_DUMMY },
                { StructureType::NO_STRUCTURE, nullptr, OtherGraphic::ADD_BUILDING_DUMMY },
                { StructureType::NO_STRUCTURE, nullptr, OtherGraphic::ADD_BUILDING_DUMMY },
                { StructureType::NO_STRUCTURE, nullptr, OtherGraphic::ADD_BUILDING_DUMMY },
                { StructureType::NO_STRUCTURE, nullptr, OtherGraphic::ADD_BUILDING_DUMMY },
                {
                    StructureType::OFFICE1,
                    "Kontor I",
                    OtherGraphic::ADD_BUILDING_OFFICE1
                },
                { StructureType::NO_STRUCTURE, nullptr, OtherGraphic::ADD_BUILDING_DUMMY },
                { StructureType::NO_STRUCTURE, nullptr, OtherGraphic::ADD_BUILDING_DUMMY },
                { StructureType::NO_STRUCTURE, nullptr, OtherGraphic::ADD_BUILDING_DUMMY }
            }
        }, {
            BuildingGroup::PUBLIC,
            "Öffentliche Gebäude",
            "data/img/gui/button-add-building-public.png",
            "data/img/gui/button-add-building-public-pressed.png", {
                { StructureType::NO_STRUCTURE, nullptr, OtherGraphic::ADD_BUILDING_DUMMY },
                { StructureType::NO_STRUCTURE, nullptr, OtherGraphic::ADD_BUILDING_DUMMY },
                { StructureType::NO_STRUCTURE, nullptr, OtherGraphic::ADD_BUILDING_DUMMY },
                { StructureType::NO_STRUCTURE, nullptr, OtherGraphic::ADD_BUILDING_DUMMY },
                { StructureType::NO_STRUCTURE, nullptr, OtherGraphic::ADD_BUILDING_DUMMY },
                { StructureType::NO_STRUCTURE, nullptr, OtherGraphic::ADD_BUILDING_DUMMY },
                { StructureType::NO_STRUCTURE, nullptr, OtherGraphic::ADD_BUILDING_DUMMY },
                { StructureType::NO_STRUCTURE, nullptr, OtherGraphic::ADD_BUILDING_DUMMY },
                { StructureType::NO_STRUCTURE, nullptr, OtherGraphic::ADD_BUILDING_DUMMY },
                { StructureType::NO_STRUCTURE, nullptr, OtherGraphic::ADD_BUILDING_DUMMY },
                { StructureType::NO_STRUCTURE, nullptr, OtherGraphic::ADD_BUILDING_DUMMY },
                {
                    StructureType::STREET,
                    "Plasterstraße",
                    OtherGraphic::ADD_BUILDING_STREET
                }, {
                    StructureType::PIONEERS_HOUSE1,
                    "Haus",
                    OtherGraphic::ADD_BUILDING_HOUSE
                }, {
                    StructureType::MARKETPLACE,
                    "Marktplatz",
                    OtherGraphic::ADD_BUILDING_MARKETPLACE
                }, {
                    StructureType::CHAPEL,
                    "Kapelle",
                    OtherGraphic::ADD_BUILDING_CHAPEL
                },
                { StructureType::NO_STRUCTURE, nullptr, OtherGraphic::ADD_BUILDING_DUMMY }
            }
        }
    };

    for (int groupIndex = 0; groupIndex < 4; groupIndex++) {
        // Grid
        GuiStaticElement* addBuildingGrid = new GuiStaticElement();
        PlainGraphic* graphicAddBuildingGrid = graphicsMgr->getOtherGraphic(OtherGraphic::ADD_BUILDING_GRID);
        addBuildingGrid->setCoords(775, 450, graphicAddBuildingGrid->getWidth(), graphicAddBuildingGrid->getHeight());
        addBuildingGrid->setGraphic(graphicAddBuildingGrid);
        addBuildingGrid->setVisible(false);
        guiMgr->registerElement(GUI_ID_ADD_BUILDING_GRID_BASE + groupIndex, addBuildingGrid);

        // Buttons im Grid
        for (int gridY = 0; gridY < 4; gridY++) {
            for (int gridX = 0; gridX < 4; gridX++) {
                int buildingIndex = gridY * 4 + gridX;
                StructureType structureType = buildingGroups[groupIndex].buildings[buildingIndex].structureType;

                if (structureType == NO_STRUCTURE) {
                    continue;
                }

                GuiButton* addBuildingButton = new GuiButton();
                PlainGraphic* graphicAddBuildingButton =
                    graphicsMgr->getOtherGraphic(buildingGroups[groupIndex].buildings[buildingIndex].graphic);
                addBuildingButton->setCoords(
                    12 + 58 * gridX, 13 + 58 * gridY, graphicAddBuildingButton->getWidth(), graphicAddBuildingButton->getHeight());
                addBuildingButton->setGraphic(graphicAddBuildingButton);
                addBuildingButton->setGraphicPressed(graphicAddBuildingButton);
                addBuildingButton->setOnClickFunction([ this, structureType ]() {
                    guiMgr->panelState.addingStructure = structureType;
                    guiMgr->panelState.buildingMenuOpen = false;
                    guiMgr->updateGuiFromPanelState();
                });
                guiMgr->registerElement(GUI_ID_ADD_BUILDING_GRID_BUTTON_BASE + groupIndex * 16 + buildingIndex, addBuildingButton);
                addBuildingGrid->addChildElement(addBuildingButton);
            }
        }

        // Button für die Gruppe
        GuiPushButton* addBuildingPushButton = new GuiPushButton();
        addBuildingPushButton->setGraphic(new PlainGraphic(buildingGroups[groupIndex].graphicFilename));
        addBuildingPushButton->setGraphicPressed(new PlainGraphic(buildingGroups[groupIndex].graphicPressedFilename));
        addBuildingPushButton->setCoords(12 + groupIndex * 55, 378, 52, 64);
        addBuildingPushButton->setOnClickFunction([ this, groupIndex ]() {
            // Wenn man die Gruppe nochmal klickt, die bereits ausgewählt ist und das ausgewählte Gebäude nicht
            // aus dieser Gruppe ist, wird das Gebäude gewechselt und eins aus der Gruppe genommen
            if (guiMgr->panelState.selectedBuildingGroup == (BuildingGroup) groupIndex &&
                guiMgr->panelState.buildingMenuOpen) {

                bool addingStructureInSelectedBuildingGroup = false;
                for (int i = 0; i < 16; i++) {
                    if (buildingGroups[groupIndex].buildings[i].structureType == guiMgr->panelState.addingStructure) {
                        addingStructureInSelectedBuildingGroup = true;
                        break;
                    }
                }

                if (!addingStructureInSelectedBuildingGroup) {
                    // Gebäude unten links nehmen (= Index 12 von 16)
                    guiMgr->panelState.addingStructure = buildingGroups[groupIndex].buildings[12].structureType;
                }
            }
            else {
                guiMgr->panelState.selectedBuildingGroup = (BuildingGroup) groupIndex;
                guiMgr->panelState.buildingMenuOpen = true;
            }
            guiMgr->updateGuiFromPanelState();
        });
        guiMgr->registerElement(GUI_ID_ADD_BUILDING_PUSH_BUTTON_BASE + groupIndex, addBuildingPushButton);
        addChildElement(addBuildingPushButton);
    }

    // Gebäudebau: Infos, über zu platzierendes Gebäude
    GuiAddBuildingWidget* addBuildingWidget = new GuiAddBuildingWidget();
    guiMgr->registerElement(GUI_ID_ADD_BUILDING_WIDGET, addBuildingWidget);
    addChildElement(addBuildingWidget);
}