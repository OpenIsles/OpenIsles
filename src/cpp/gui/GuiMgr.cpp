#include "game/Game.h"
#include "gui/GuiAddBuildingWidget.h"
#include "gui/GuiButton.h"
#include "gui/GuiColonyGoodsWidget.h"
#include "gui/GuiMgr.h"
#include "gui/GuiPushButton.h"
#include "gui/GuiSelectedBuildingWidget.h"
#include "gui/Identifiers.h"
#include "sound/SoundMgr.h"

#ifdef DEBUG_A_STAR
#include "pathfinding/AStar.h"

extern int mouseCurrentMapX, mouseCurrentMapY;
#endif

// Aus main.cpp importiert
extern bool quitGame;
extern Game* game;
extern GraphicsMgr* graphicsMgr;
extern SoundMgr* soundMgr;


GuiMgr::GuiMgr() {
    // Panel
    PlainGraphic* graphic = new PlainGraphic("data/img/gui/panel.png");
	GuiStaticElement* panel = new GuiStaticElement();
    panel->setCoords(768, 0, graphic->getWidth(), graphic->getHeight());
    panel->setGraphic(graphic);
    registerElement(GUI_ID_PANEL, panel);
    
    // Statusleiste
    graphic = new PlainGraphic("data/img/gui/statusbar.png");
	GuiStaticElement* statusBar = new GuiStaticElement();
    statusBar->setCoords(0, 734, graphic->getWidth(), graphic->getHeight());
    statusBar->setGraphic(graphic);
    registerElement(GUI_ID_STATUS_BAR, statusBar);
    
    // Buttons zum Umschalten der Tabs und zugehörige UI-Gruppen
    const char* tabGraphics[4][2] = {
        {
            "data/img/gui/button-build.png",
            "data/img/gui/button-build-pressed.png"
        }, {
            "data/img/gui/button-dummy.png",
            "data/img/gui/button-dummy-pressed.png"
        }, {
            "data/img/gui/button-dummy.png",
            "data/img/gui/button-dummy-pressed.png"
        }, {
            "data/img/gui/button-dummy.png",
            "data/img/gui/button-dummy-pressed.png"
        }
    };
    
    for (int i = 0; i < 4; i++) {
        // Button
        GuiPushButton* panelSwitchPushButton = new GuiPushButton();
        panelSwitchPushButton->setGraphic(new PlainGraphic(tabGraphics[i][0]));
        panelSwitchPushButton->setGraphicPressed(new PlainGraphic(tabGraphics[i][1]));
        panelSwitchPushButton->setCoords(22 + i*55, 235, 48, 64);
        panelSwitchPushButton->setOnClickFunction([this, i]() {
            for (int j = 0; j < 4; j++) {
                bool active = (j == i);
                
                ((GuiPushButton*) findElement(GUI_ID_PANEL_SWITCH_PUSH_BUTTON_BASE + j))->setActive(active);
                findElement(GUI_ID_PANEL_BASE + j)->setVisible(active);
            }
        });
        registerElement(GUI_ID_PANEL_SWITCH_PUSH_BUTTON_BASE + i, panelSwitchPushButton);
        panel->addChildElement(panelSwitchPushButton);
        
        // Container
        GuiStaticElement* panelSwitch = new GuiStaticElement();
        panelSwitch->setCoords(15, 320, 226, 448);
        
        registerElement(GUI_ID_PANEL_BASE + i, panelSwitch);
        panel->addChildElement(panelSwitch);
    }
    ((GuiPushButton*) findElement(GUI_ID_PANEL_SWITCH_PUSH_BUTTON_BUILD))->triggerOnClick();
    
    // Buttons auf dem 1. Tab
    initBuildGui();

    // Buttons auf dem 4. Tab
    GuiPushButton* musicPushButton = new GuiPushButton();
    musicPushButton->setGraphic(new PlainGraphic("data/img/gui/button-music.png"));
    musicPushButton->setGraphicPressed(new PlainGraphic("data/img/gui/button-music-pressed.png"));
    musicPushButton->setCoords(2, 370, 64, 64);
    musicPushButton->setOnClickFunction([this]() {
        bool musicEnabled = ((GuiPushButton*) findElement(GUI_ID_MUSIC_PUSH_BUTTON))->isActive();
        
        if (musicEnabled) {
            soundMgr->enableMusic();
        } else {
            soundMgr->disableMusic();
        }
    });
    registerElement(GUI_ID_MUSIC_PUSH_BUTTON, musicPushButton);
    findElement(GUI_ID_PANEL_4)->addChildElement(musicPushButton);

#ifdef DEBUG
    // Testzeugs
    graphic = new PlainGraphic("data/img/gui/testbutton.png");
    GuiButton* testButton = new GuiButton();
    testButton->setCoords(12, 70, graphic->getWidth(), graphic->getHeight());
    testButton->setGraphic(graphic);
    testButton->setGraphicPressed(new PlainGraphic("data/img/gui/testbutton-pressed.png"));
    testButton->setOnClickFunction([]() {
        std::cout << "Click1" << std::endl;
    });
    testButton->setVisible(false);
    registerElement(GUI_ID_TEST_BUTTON1, testButton);
    findElement(GUI_ID_PANEL_3)->addChildElement(testButton);

    graphic = new PlainGraphic("data/img/gui/testbutton.png");
    GuiButton* testButton2 = new GuiButton();
    testButton2->setCoords(92, 70, graphic->getWidth(), graphic->getHeight());
    testButton2->setGraphic(graphic);
    testButton2->setGraphicPressed(new PlainGraphic("data/img/gui/testbutton-pressed.png"));
    testButton2->setOnClickFunction([]() {
        std::cout << "Click2" << std::endl;
    });
    testButton2->setVisible(false);
    registerElement(GUI_ID_TEST_BUTTON2, testButton2);
    findElement(GUI_ID_PANEL_3)->addChildElement(testButton2);
    
    graphic = new PlainGraphic("data/img/gui/testbutton.png");
    GuiPushButton* testPushButton = new GuiPushButton();
    testPushButton->setCoords(172, 70, graphic->getWidth(), graphic->getHeight());
    testPushButton->setGraphic(graphic);
    testPushButton->setGraphicPressed(new PlainGraphic("data/img/gui/testbutton-pressed.png"));
    testPushButton->setOnClickFunction([this]() {
        GuiPushButton* testPushButton = (GuiPushButton*) findElement(GUI_ID_TEST_PUSH_BUTTON);
        ((GuiPushButton*) findElement(GUI_ID_TEST_BUTTON1))->setVisible(testPushButton->isActive());
        ((GuiPushButton*) findElement(GUI_ID_TEST_BUTTON2))->setVisible(testPushButton->isActive());
        std::cout << "Click im PushButton: " << (testPushButton->isActive() ? "active" : "inactive") << std::endl;
    });
    registerElement(GUI_ID_TEST_PUSH_BUTTON, testPushButton);
    findElement(GUI_ID_PANEL_3)->addChildElement(testPushButton);
#endif
}

GuiMgr::~GuiMgr() {
    // Grafiken wieder wegräumen. Der Konstruktur hat sie alle direkt geladen.
    // TODO Die Grafiken könnte man alle schön in den GraphicsMgr packen, dann muss man hier den Aufwand nicht betreiben
    delete ((GuiStaticElement*) findElement(GUI_ID_PANEL))->getGraphic();
    delete ((GuiStaticElement*) findElement(GUI_ID_STATUS_BAR))->getGraphic();

    for (int i = 0; i < 4; i++) {
        delete ((GuiPushButton*) findElement(GUI_ID_PANEL_SWITCH_PUSH_BUTTON_BASE + i))->getGraphic();
        delete ((GuiPushButton*) findElement(GUI_ID_PANEL_SWITCH_PUSH_BUTTON_BASE + i))->getGraphicPressed();

        delete ((GuiPushButton*) findElement(GUI_ID_ADD_BUILDING_PUSH_BUTTON_BASE + i))->getGraphic();
        delete ((GuiPushButton*) findElement(GUI_ID_ADD_BUILDING_PUSH_BUTTON_BASE + i))->getGraphicPressed();
    }

    delete ((GuiPushButton*) findElement(GUI_ID_MUSIC_PUSH_BUTTON))->getGraphic();
    delete ((GuiPushButton*) findElement(GUI_ID_MUSIC_PUSH_BUTTON))->getGraphicPressed();

#ifdef DEBUG
    // Testzeugs-Grafiken wieder wegräumen
    delete ((GuiButton*) findElement(GUI_ID_TEST_BUTTON1))->getGraphic();
    delete ((GuiButton*) findElement(GUI_ID_TEST_BUTTON1))->getGraphicPressed();
    delete ((GuiButton*) findElement(GUI_ID_TEST_BUTTON2))->getGraphic();
    delete ((GuiButton*) findElement(GUI_ID_TEST_BUTTON2))->getGraphicPressed();
    delete ((GuiPushButton*) findElement(GUI_ID_TEST_PUSH_BUTTON))->getGraphic();
    delete ((GuiPushButton*) findElement(GUI_ID_TEST_PUSH_BUTTON))->getGraphicPressed();
#endif

    // GUI-Elemente wegräumen
    for (auto iter = identifierMap.cbegin(); iter != identifierMap.cend(); iter++) {
		GuiBase* guiElement = iter->second;
        delete guiElement;
    }
    identifierMap.clear();
}

void GuiMgr::initBuildGui() {
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
                { StructureType::NO_STRUCTURE, nullptr, OtherGraphic::ADD_BUILDING_DUMMY },
                { StructureType::NO_STRUCTURE, nullptr, OtherGraphic::ADD_BUILDING_DUMMY },
                { StructureType::NO_STRUCTURE, nullptr, OtherGraphic::ADD_BUILDING_DUMMY },
                { StructureType::NO_STRUCTURE, nullptr, OtherGraphic::ADD_BUILDING_DUMMY },
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
                {
                    StructureType::BRICKYARD,
                    "Steinmetz",
                    OtherGraphic::ADD_BUILDING_DUMMY
                }, {
                    StructureType::BRICKYARD2,
                    "Steinmetz (gedreht)",
                    OtherGraphic::ADD_BUILDING_DUMMY
                }
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
                {
                    StructureType::HERBARY,
                    "Kräuterfeld",
                    OtherGraphic::ADD_BUILDING_DUMMY
                }, {
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
                { StructureType::NO_STRUCTURE, nullptr, OtherGraphic::ADD_BUILDING_DUMMY },
                { StructureType::NO_STRUCTURE, nullptr, OtherGraphic::ADD_BUILDING_DUMMY },
                { StructureType::NO_STRUCTURE, nullptr, OtherGraphic::ADD_BUILDING_DUMMY },
                {
                    StructureType::OFFICE1,
                    "Kontor I",
                    OtherGraphic::ADD_BUILDING_OFFICE1
                }
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
                    StructureType::SIGNALFIRE,
                    "Signalfeuer",
                    OtherGraphic::ADD_BUILDING_DUMMY
                }, {
                    StructureType::STREET,
                    "Plasterstraße",
                    OtherGraphic::ADD_BUILDING_STREET
                }, {
                    StructureType::PIONEERS_HOUSE1,
                    "Pionier-Haus",
                    OtherGraphic::ADD_BUILDING_DUMMY
                }, {
                    StructureType::MARKETPLACE,
                    "Marktplatz",
                    OtherGraphic::ADD_BUILDING_MARKETPLACE
                }, {
                    StructureType::CHAPEL,
                    "Kapelle",
                    OtherGraphic::ADD_BUILDING_CHAPEL
                }
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
        registerElement(GUI_ID_ADD_BUILDING_GRID_BASE + groupIndex, addBuildingGrid);

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
                    12 + 58*gridX, 13 + 58*gridY, graphicAddBuildingButton->getWidth(), graphicAddBuildingButton->getHeight());
                addBuildingButton->setGraphic(graphicAddBuildingButton);
                addBuildingButton->setGraphicPressed(graphicAddBuildingButton);
                addBuildingButton->setOnClickFunction([this, structureType, buildingIndex, groupIndex]() {
                    game->startAddingStructure(structureType);
                    findElement(GUI_ID_ADD_BUILDING_GRID_BASE + groupIndex)->setVisible(false);
                });
                registerElement(GUI_ID_ADD_BUILDING_GRID_BUTTON_BASE + groupIndex * 16 + buildingIndex, addBuildingButton);
                addBuildingGrid->addChildElement(addBuildingButton);
            }
        }

        // Button für die Gruppe
        GuiPushButton* addBuildingPushButton = new GuiPushButton();
        addBuildingPushButton->setGraphic(new PlainGraphic(buildingGroups[groupIndex].graphicFilename));
        addBuildingPushButton->setGraphicPressed(new PlainGraphic(buildingGroups[groupIndex].graphicPressedFilename));
        addBuildingPushButton->setCoords(7 + groupIndex*55, 370, 52, 64);
        addBuildingPushButton->setOnClickFunction([this, groupIndex ]() {
            bool buttonActive = ((GuiPushButton*) findElement(GUI_ID_ADD_BUILDING_PUSH_BUTTON_BASE + groupIndex))->isActive();

            findElement(GUI_ID_ADD_BUILDING_GRID_BASE + groupIndex)->setVisible(buttonActive);
        });
        registerElement(GUI_ID_ADD_BUILDING_PUSH_BUTTON_BASE + groupIndex, addBuildingPushButton);
        findElement(GUI_ID_PANEL_BUILD)->addChildElement(addBuildingPushButton);
    }

    // Gebäudebau: Infos, über zu platzierendes Gebäude
    GuiAddBuildingWidget* addBuildingWidget = new GuiAddBuildingWidget();
    registerElement(GUI_ID_ADD_BUILDING_WIDGET, addBuildingWidget);
    findElement(GUI_ID_PANEL_BUILD)->addChildElement(addBuildingWidget);

    // Aktuell ausgewähltes Gebäude
    GuiSelectedBuildingWidget* selectedBuildingWidget = new GuiSelectedBuildingWidget();
    registerElement(GUI_ID_SELECTED_BUILDING_WIDGET, selectedBuildingWidget);
    findElement(GUI_ID_PANEL_BUILD)->addChildElement(selectedBuildingWidget);

    // Warenanzeige der Siedlung, wenn Marktplatz oder Kontor ausgewählt ist
    GuiColonyGoodsWidget* colonyGoodsWidget = new GuiColonyGoodsWidget();
    registerElement(GUI_ID_COLONY_GOODS_WIDGET, colonyGoodsWidget);
    findElement(GUI_ID_PANEL_BUILD)->addChildElement(colonyGoodsWidget);
}

void GuiMgr::registerElement(int identifier, GuiBase* guiElement) {
    if (identifierMap.count(identifier) > 0) {
        throw new std::runtime_error("Identifier already registered");
    }
    
    identifierMap[identifier] = guiElement;
}

GuiBase* GuiMgr::findElement(int identifier) {
    auto iter = identifierMap.find(identifier);
    
    if (iter == identifierMap.end()) {
        throw new std::runtime_error("Identifier not registered");
    }
    
    return iter->second;
}

void GuiMgr::render(SDL_Renderer* renderer) {
    for (auto iter = identifierMap.cbegin(); iter != identifierMap.cend(); iter++) {
		GuiBase* guiElement = iter->second;
        
        // nur Toplevel-Element zeichnen
        if (guiElement->getParentElement() != nullptr) {
            continue;
        }
        
        guiElement->render(renderer);
    }
}

void GuiMgr::onEvent(SDL_Event& event) {
    // TODO Event besser queuen und nicht sofort abarbeiten

    Map* map = game->getMap();
    
    // Spiel beenden
    if (event.type == SDL_QUIT) {
        quitGame = true;
        return;
    }
    
    if (event.type == SDL_MOUSEBUTTONDOWN) {
        // Bei Linksklick die Koordinaten merken
        if (event.button.button == SDL_BUTTON_LEFT) {
            startClickX = event.button.x; 
            startClickY = event.button.y;
        }
        // Rechtsklick...
        else if (event.button.button == SDL_BUTTON_RIGHT) {
            // Platzieren wir ein Gebäude -> abbrechen
            if (game->isAddingStructure()) {
                game->endAddingStructure();
            }
            // Ist ein Gebäude auf der Karte markieren -> deselektieren
            else if(map->getSelectedMapObject() != nullptr) {
                map->setSelectedMapObject(nullptr);
            }
            
            return;
        }
    }
    
    // Tastaturevents
    if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
            quitGame = true;
        }

        // Scrolling der Karte
        else if (event.key.keysym.scancode == SDL_SCANCODE_UP) {
            map->scroll(0, -16);
        } else if (event.key.keysym.scancode == SDL_SCANCODE_DOWN) {
            map->scroll(0, 16);
        } else if (event.key.keysym.scancode == SDL_SCANCODE_LEFT) {
            map->scroll(-16, 0);
        } else if (event.key.keysym.scancode == SDL_SCANCODE_RIGHT) {
            map->scroll(16, 0);
        }

        // Karte zoomen
        else if (event.key.keysym.scancode == SDL_SCANCODE_F2) {
            map->setScreenZoom(4);
        } else if (event.key.keysym.scancode == SDL_SCANCODE_F3) {
            map->setScreenZoom(2);
        } else if (event.key.keysym.scancode == SDL_SCANCODE_F4) {
            map->setScreenZoom(1);
        }

        // Spielgeschwindigkeit
        else if (event.key.keysym.scancode == SDL_SCANCODE_F5) {
            game->setSpeed(1);
        } else if (event.key.keysym.scancode == SDL_SCANCODE_F6) {
            game->setSpeed(2);
        } else if (event.key.keysym.scancode == SDL_SCANCODE_F7) {
            game->setSpeed(4);
        } else if (event.key.keysym.scancode == SDL_SCANCODE_F8) {
            if (event.key.keysym.mod & KMOD_SHIFT) {
                game->setSpeed(8);
            }
#ifdef DEBUG
            else {
                game->setSpeed(50); // Debug-Ultra-Speed :-)
            }
#endif
        }

#ifdef DEBUG
        // Debug-Zwecke
        if (event.key.keysym.scancode == SDL_SCANCODE_1) {
            game->startAddingStructure(StructureType::CHAPEL);
        } else if (event.key.keysym.scancode == SDL_SCANCODE_2) {
            game->startAddingStructure(StructureType::PIONEERS_HOUSE1);
        } else if (event.key.keysym.scancode == SDL_SCANCODE_3) {
            game->startAddingStructure(StructureType::SIGNALFIRE);
        } else if (event.key.keysym.scancode == SDL_SCANCODE_4) {
            game->startAddingStructure(StructureType::HERBARY);
        } else if (event.key.keysym.scancode == SDL_SCANCODE_5) {
            game->startAddingStructure(StructureType::BRICKYARD);
        } else if (event.key.keysym.scancode == SDL_SCANCODE_6) {
            game->startAddingStructure(StructureType::BRICKYARD2);
        } else if (event.key.keysym.scancode == SDL_SCANCODE_7) {
            game->startAddingStructure(StructureType::OFFICE1);
        } else if (event.key.keysym.scancode == SDL_SCANCODE_8) {
            game->startAddingStructure(StructureType::STREET);
        } else if (event.key.keysym.scancode == SDL_SCANCODE_9) {
            game->startAddingStructure(StructureType::MARKETPLACE);
        } else if (event.key.keysym.scancode == SDL_SCANCODE_0) {
            game->startAddingStructure(StructureType::FORESTERS);
        } else if (event.key.keysym.scancode == SDL_SCANCODE_DELETE) {
            map->deleteSelectedObject();
        }
#endif

#ifdef DEBUG_A_STAR
        bool needToRecalculate = false;

        // A*-Start- und Endkoordinaten festlegen
        if (event.key.keysym.scancode == SDL_SCANCODE_A) {
            AStar::debugAStar_source = MapCoordinate(mouseCurrentMapX, mouseCurrentMapY);
            needToRecalculate = true;
        } else if (event.key.keysym.scancode == SDL_SCANCODE_S) {
            AStar::debugAStar_destination = MapCoordinate(mouseCurrentMapX, mouseCurrentMapY);
            needToRecalculate = true;
        } else if (event.key.keysym.scancode == SDL_SCANCODE_D) {
            MapTile* mapTile = game->getMap()->getMapTileAt(mouseCurrentMapX, mouseCurrentMapY);
            AStar::debugAStar_buildingToUseCatchmentArea = dynamic_cast<Building*>(mapTile->mapObject);
            needToRecalculate = true;
        } else if (event.key.keysym.scancode == SDL_SCANCODE_F) {
            AStar::debugAStar_useStreetOnly = !AStar::debugAStar_useStreetOnly;
            needToRecalculate = true;
        }

        if (needToRecalculate) {
            if (AStar::debugAStar_source.mapX > 0 && AStar::debugAStar_source.mapY > 0 &&
                AStar::debugAStar_destination.mapX > 0 && AStar::debugAStar_destination.mapY > 0) {

                if (AStar::debugAStar_route != nullptr) {
                    delete AStar::debugAStar_route;
                }

                AStar::debugAStar_route = AStar::findRoute(
                    AStar::debugAStar_source, AStar::debugAStar_destination,
                    AStar::debugAStar_buildingToUseCatchmentArea,
                    AStar::debugAStar_useStreetOnly);

                if (AStar::debugAStar_route != nullptr) {
                    AStar::cutRouteInsideBuildings(AStar::debugAStar_route);
                }
            }
        }
#endif
    }
    
    // Maustaste in der Karte geklickt
    if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT) {
        if (abs(event.button.x - startClickX) < 3 && abs(event.button.y - startClickY) < 3) {
            map->onClick(event.button.x, event.button.y);
        }
    }
    
    // Die GUI-Elemente sollen nur Linksklicks mitkriegen, der Rest interessiert die eh nicht
    if (event.type != SDL_MOUSEBUTTONDOWN && event.type != SDL_MOUSEBUTTONUP) {
        return;
    }
    if (event.button.button != SDL_BUTTON_LEFT) {
        return;
    }
    
    for (auto iter = identifierMap.cbegin(); iter != identifierMap.cend(); iter++) {
		GuiBase* guiElement = iter->second;
        
        // nur an Toplevel-Elemente schicken
        if (guiElement->getParentElement() != nullptr) {
            continue;
        }
        
        guiElement->onEvent(event);
    }
}
