#include "config/ConfigMgr.h"
#include "game/Colony.h"
#include "game/Game.h"
#include "gui/FontMgr.h"
#include "gui/GuiMgr.h"
#include "gui/components/GuiAddBuildingWidget.h"
#include "gui/components/GuiButton.h"
#include "gui/components/GuiPushButton.h"
#include "gui/panel-widgets/GuiBuildMenuWidget.h"
#include "gui/panel-widgets/GuiColonyGoodsWidget.h"
#include "gui/panel-widgets/GuiDummyWidget.h"
#include "gui/panel-widgets/GuiOptionsMenuWidget.h"
#include "gui/panel-widgets/GuiSelectedBuildingWidget.h"
#include "gui/Identifiers.h"
#include "sound/SoundMgr.h"

static SDL_Color colorWhite = {255, 255, 255, 255};
static SDL_Color colorBlack = {0, 0, 0, 255};

#ifdef DEBUG_A_STAR
#include "pathfinding/AStar.h"

extern int mouseCurrentX, mouseCurrentY;
#endif

// Aus main.cpp importiert
extern ConfigMgr* configMgr;
extern FontMgr* fontMgr;
extern Game* game;
extern GraphicsMgr* graphicsMgr;
extern SoundMgr* soundMgr;


GuiMgr::GuiMgr(SDL_Renderer* renderer) :
    renderer(renderer),
    quitGame(false)
{
}

GuiMgr::~GuiMgr() {
    // Grafiken wieder wegräumen. Der Konstruktur hat sie alle direkt geladen.
    // TODO Die Grafiken könnte man alle schön in den GraphicsMgr packen, dann muss man hier den Aufwand nicht betreiben
    delete ((GuiStaticElement*) findElement(GUI_ID_PANEL))->getGraphic();
    delete ((GuiStaticElement*) findElement(GUI_ID_STATUS_BAR))->getGraphic();

    for (int i = 0; i < 4; i++) {
        delete ((GuiPushButton*) findElement(GUI_ID_PANEL_SWITCH_PUSH_BUTTON_BASE + i))->getGraphic();
        delete ((GuiPushButton*) findElement(GUI_ID_PANEL_SWITCH_PUSH_BUTTON_BASE + i))->getGraphicPressed();
    }

    // GUI-Elemente wegräumen
    for (auto iter = identifierMap.cbegin(); iter != identifierMap.cend(); iter++) {
		GuiBase* guiElement = iter->second;
        delete guiElement;
    }
    identifierMap.clear();
}

void GuiMgr::initGui() {
    //////////////////////////////////////////////////////////////////////////////////////////
    // GUI-Elemente anlegen //////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////

    // Panel
    PlainGraphic* graphic = new PlainGraphic(renderer, "data/img/gui/panel.png");
    GuiStaticElement* panel = new GuiStaticElement();
    panel->setCoords(768, 0, graphic->getWidth(), graphic->getHeight());
    panel->setGraphic(graphic);
    registerElement(GUI_ID_PANEL, panel);

    // Statusleiste
    graphic = new PlainGraphic(renderer, "data/img/gui/statusbar.png");
    GuiStaticElement* statusBar = new GuiStaticElement();
    statusBar->setCoords(0, 734, graphic->getWidth(), graphic->getHeight());
    statusBar->setGraphic(graphic);
    registerElement(GUI_ID_STATUS_BAR, statusBar);

    // Panel-Widgets
    initPanelWidgets();

    // Buttons zum Umschalten der Tabs und zugehörige UI-Gruppen
    static struct {
        const char* graphicFilename;
        const char* graphicPressedFilename;
        PanelButton panelButtonToActive;
        GuiPanelWidget* panelWidgetToActivate;

    } tabGraphics[4][2] = {
        {
            "data/img/gui/button-build.png",
            "data/img/gui/button-build-pressed.png",
            PanelButton::ADD_BUILDING,
            (GuiPanelWidget*) findElement(GUI_ID_BUILD_MENU_PANEL_WIDGET)
        }, {
            "data/img/gui/button-dummy.png",
            "data/img/gui/button-dummy-pressed.png",
            PanelButton::MILITARY,
            (GuiPanelWidget*) findElement(GUI_ID_MILITARY_PANEL_WIDGET)
        }, {
            "data/img/gui/button-dummy.png",
            "data/img/gui/button-dummy-pressed.png",
            PanelButton::INFO,
            (GuiPanelWidget*) findElement(GUI_ID_PLAYER_STATUS_PANEL_WIDGET)
        }, {
            "data/img/gui/button-dummy.png",
            "data/img/gui/button-dummy-pressed.png",
            PanelButton::OPTIONS,
            (GuiPanelWidget*) findElement(GUI_ID_OPTIONS_MENU_PANEL_WIDGET)
        }
    };

    for (int i = 0; i < 4; i++) {
        // Button
        GuiPushButton* panelSwitchPushButton = new GuiPushButton();
        panelSwitchPushButton->setGraphic(new PlainGraphic(renderer, tabGraphics[i]->graphicFilename));
        panelSwitchPushButton->setGraphicPressed(new PlainGraphic(renderer, tabGraphics[i]->graphicPressedFilename));
        panelSwitchPushButton->setCoords(22 + i*55, 235, 48, 64);
        panelSwitchPushButton->setOnClickFunction([this, i]() {
            panelState.selectedPanelButton = tabGraphics[i]->panelButtonToActive;
            panelState.activeGuiPanelWidget = tabGraphics[i]->panelWidgetToActivate;
            updateGuiFromPanelState();
        });
        registerElement(GUI_ID_PANEL_SWITCH_PUSH_BUTTON_BASE + i, panelSwitchPushButton);
        panel->addChildElement(panelSwitchPushButton);
    }

    //////////////////////////////////////////////////////////////////////////////////////////
    // Zustand initialisieren ////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////

    panelState.selectedPanelButton = PanelButton::INFO;

    // Initial ist für den Gebäudebau der Förster ausgewählt
    panelState.selectedBuildingGroup = BuildingGroup::FARM;
    panelState.addingStructure = StructureType::FORESTERS;
    panelState.buildingMenuOpen = false;

    updateGuiFromPanelState();
}

void GuiMgr::initPanelWidgets() {
    GuiBase* panel = findElement(GUI_ID_PANEL);

    // ausgewähltes Gebäude (Infos über Produktion und intere Lagerbestände des Gebäudes)
    GuiSelectedBuildingWidget* selectedBuildingWidget = new GuiSelectedBuildingWidget();
    registerElement(GUI_ID_SELECTED_BUILDING_PANEL_WIDGET, selectedBuildingWidget);
    panel->addChildElement(selectedBuildingWidget);

    // Kolonie-Warenübersicht
    GuiColonyGoodsWidget* colonyGoodsWidget = new GuiColonyGoodsWidget();
    registerElement(GUI_ID_COLONY_GOODS_PANEL_WIDGET, colonyGoodsWidget);
    panel->addChildElement(colonyGoodsWidget);

    // Baumenü
    GuiBuildMenuWidget* buildMenuWidget = new GuiBuildMenuWidget();
    registerElement(GUI_ID_BUILD_MENU_PANEL_WIDGET, buildMenuWidget);
    panel->addChildElement(buildMenuWidget);

    // Kampfmenü
    GuiDummyWidget* dummyWidget1 = new GuiDummyWidget();
    registerElement(GUI_ID_MILITARY_PANEL_WIDGET, dummyWidget1);
    panel->addChildElement(dummyWidget1);

    // Spielerstatus (Infomenü)
    GuiDummyWidget* dummyWidget2 = new GuiDummyWidget();
    registerElement(GUI_ID_PLAYER_STATUS_PANEL_WIDGET, dummyWidget2);
    panel->addChildElement(dummyWidget2);

    // Optionen-Menü
    GuiOptionsMenuWidget* optionsMenuWidget = new GuiOptionsMenuWidget();
    registerElement(GUI_ID_OPTIONS_MENU_PANEL_WIDGET, optionsMenuWidget);
    panel->addChildElement(optionsMenuWidget);
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
    // Alle GUI-Elemente rendern
    for (auto iter = identifierMap.cbegin(); iter != identifierMap.cend(); iter++) {
		GuiBase* guiElement = iter->second;
        
        // nur Toplevel-Element zeichnen
        if (guiElement->getParentElement() != nullptr) {
            continue;
        }
        
        guiElement->render(renderer);
    }

    // Zum Schluss die Resourcen-Leiste oben an die Karte ran
    renderResourcesBar();
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
            if (panelState.selectedPanelButton == PanelButton::ADD_BUILDING) {
                if (panelState.buildingMenuOpen) {
                    panelState.buildingMenuOpen = false;
                } else {
                    panelState.selectedPanelButton = PanelButton::INFO;
                    panelState.activeGuiPanelWidget = (GuiPanelWidget*) findElement(GUI_ID_PLAYER_STATUS_PANEL_WIDGET);
                }
                updateGuiFromPanelState();
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

        // Panel umschalten
        else if (event.key.keysym.scancode == SDL_SCANCODE_B) {
            panelState.selectedPanelButton = PanelButton::ADD_BUILDING;
            panelState.activeGuiPanelWidget = (GuiPanelWidget*) findElement(GUI_ID_BUILD_MENU_PANEL_WIDGET);
            updateGuiFromPanelState();
        } else if (event.key.keysym.scancode == SDL_SCANCODE_K) {
            panelState.selectedPanelButton = PanelButton::MILITARY;
            panelState.activeGuiPanelWidget = (GuiPanelWidget*) findElement(GUI_ID_MILITARY_PANEL_WIDGET);
            updateGuiFromPanelState();
        } else if (event.key.keysym.scancode == SDL_SCANCODE_I) {
            // nur umschalten, wenn wir noch nicht im Info-Modus sind. Z.B. wenn ein Gebäude ausgewählt ist und "I"
            // gedrückt wird, darf sich nichts verändern. Wir zeigen dann auch NICHT die Punkteanzeige
            if (panelState.selectedPanelButton != PanelButton::INFO) {
                panelState.selectedPanelButton = PanelButton::INFO;
                panelState.activeGuiPanelWidget = (GuiPanelWidget*) findElement(GUI_ID_PLAYER_STATUS_PANEL_WIDGET);
                updateGuiFromPanelState();
            }
        } else if (event.key.keysym.scancode == SDL_SCANCODE_O) {
            panelState.selectedPanelButton = PanelButton::OPTIONS;
            panelState.activeGuiPanelWidget = (GuiPanelWidget*) findElement(GUI_ID_OPTIONS_MENU_PANEL_WIDGET);
            updateGuiFromPanelState();
        }

#ifdef DEBUG
        // Debug-Zwecke
        if (event.key.keysym.scancode == SDL_SCANCODE_8) {
            panelState.selectedPanelButton = PanelButton::ADD_BUILDING;
            panelState.selectedBuildingGroup = BuildingGroup::PUBLIC;
            panelState.addingStructure = StructureType::STREET;
            panelState.activeGuiPanelWidget = (GuiPanelWidget*) findElement(GUI_ID_BUILD_MENU_PANEL_WIDGET);
            updateGuiFromPanelState();
        } else if (event.key.keysym.scancode == SDL_SCANCODE_DELETE) {
            map->deleteSelectedObject();
        }
#endif

#ifdef DEBUG_A_STAR
        bool needToRecalculate = false;

        int mouseCurrentMapX, mouseCurrentMapY;
        MapCoordUtils::getMapCoordsUnderMouse(mouseCurrentMapX, mouseCurrentMapY);

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

void GuiMgr::onSelectedMapObjectChanged(MapObject* newSelectedMapObject) {
    if (newSelectedMapObject == nullptr) {
        panelState.activeGuiPanelWidget = (GuiPanelWidget*) findElement(GUI_ID_PLAYER_STATUS_PANEL_WIDGET);
    } else {
        Building* newSelectedBuilding = dynamic_cast<Building*>(newSelectedMapObject);
        if (newSelectedBuilding != nullptr) {
            if (newSelectedBuilding->isStorageBuilding()) {
                panelState.activeGuiPanelWidget = (GuiPanelWidget*) findElement(GUI_ID_COLONY_GOODS_PANEL_WIDGET);
            } else {
                panelState.activeGuiPanelWidget = (GuiPanelWidget*) findElement(GUI_ID_SELECTED_BUILDING_PANEL_WIDGET);
            }
        }
    }

    panelState.selectedPanelButton = PanelButton::INFO;
    updateGuiFromPanelState();
}

void GuiMgr::updateGuiFromPanelState() {
    // Einen der 4 Buttons wählen
    for (int i = 0; i < 4; i++) {
        bool active = (i == (int) (panelState.selectedPanelButton));

        ((GuiPushButton*) findElement(GUI_ID_PANEL_SWITCH_PUSH_BUTTON_BASE + i))->setActive(active);
    }

    // Passendes Panel-Widget anzeigen
    for (int i = GUI_ID_PANEL_WIDGET_START; i <= GUI_ID_PANEL_WIDGET_END; i++) {
        GuiPanelWidget* guiPanelWidget = (GuiPanelWidget*) findElement(i);

        bool active = (panelState.activeGuiPanelWidget == guiPanelWidget);
        guiPanelWidget->setVisible(active);
    }

    // Baumenü: gewählte Kategorie
    for (int i = 0; i < 4; i++) {
        bool active = (
            (panelState.selectedPanelButton == PanelButton::ADD_BUILDING) &&
            (i == (int) (panelState.selectedBuildingGroup))
        );

        ((GuiPushButton*) findElement(GUI_ID_ADD_BUILDING_PUSH_BUTTON_BASE + i))->setActive(active);
        findElement(GUI_ID_ADD_BUILDING_GRID_BASE + i)->setVisible(active && panelState.buildingMenuOpen);
    }
}

void GuiMgr::renderResourcesBar() {
    Player* currentPlayer = game->getCurrentPlayer();
    const BuildingCosts* buildingCosts = (panelState.selectedPanelButton == PanelButton::ADD_BUILDING) ?
        configMgr->getBuildingConfig(panelState.addingStructure)->getBuildingCosts() : nullptr;

    // Münzenguthaben
    graphicsMgr->getOtherGraphic(OtherGraphic::COINS)->drawAt(15, 8);

    std::string outputString = toString(currentPlayer->coins);
    if (buildingCosts != nullptr) {
        outputString += " (";
        outputString += toString(buildingCosts->coins);
        outputString += ")";
    }
    fontMgr->renderText(renderer, outputString, 42, 10,
        &colorWhite, &colorBlack, "DroidSans-Bold.ttf", 18, RENDERTEXT_HALIGN_LEFT);

    // Siedlung, wo der Cursor grade is
    int mouseCurrentMapX, mouseCurrentMapY;
    MapCoordUtils::getMapCoordsUnderMouse(mouseCurrentMapX, mouseCurrentMapY);

    MapTile* mapTileAtCursor = game->getMap()->getMapTileAt(mouseCurrentMapX, mouseCurrentMapY);
    if (mapTileAtCursor == nullptr) {
        return;
    }

    Colony* colony = game->getColony(mapTileAtCursor->player, mapTileAtCursor->isle);
    if (colony == nullptr) {
        return;
    }

    // Waren (nur für den eigenen Spieler)
    if (mapTileAtCursor->player == currentPlayer) {
        GoodsType goodsToDraw[] = { GoodsType::TOOLS, GoodsType::WOOD, GoodsType::BRICKS };
        int x = 290;
        for (unsigned int i = 0; i < sizeof(goodsToDraw); i++, x += 110) {
            GoodsType goodsType = goodsToDraw[i];
            graphicsMgr->getGraphicForGoodsIcon(goodsType)->drawAt(x, 5);

            int goodsInventory = (int) colony->getGoods(goodsType).inventory;
            outputString = toString(goodsInventory);
            if (buildingCosts != nullptr) {
                outputString += " (";
                outputString += toString(
                    (goodsType == GoodsType::TOOLS) ? buildingCosts->tools :
                        (goodsType == GoodsType::WOOD) ? buildingCosts->wood :
                            (goodsType == GoodsType::BRICKS) ? buildingCosts->bricks : 0);
                outputString += ")";
            }

            fontMgr->renderText(renderer, outputString, x + 35, 10,
                &colorWhite, &colorBlack, "DroidSans-Bold.ttf", 18, RENDERTEXT_HALIGN_LEFT);
        }
    }

    // Einwohnerzahl (immer anzeigen)
    PlainGraphic* populationIconGraphic = graphicsMgr->getOtherGraphic((OtherGraphic)
        (OtherGraphic::COAT_OF_ARMS_POPULATION + mapTileAtCursor->player->getColorIndex()));
    populationIconGraphic->drawAt(655, 6);

    outputString = toString(colony->population);
    fontMgr->renderText(renderer, outputString, 690, 10,
        &colorWhite, &colorBlack, "DroidSans-Bold.ttf", 18, RENDERTEXT_HALIGN_LEFT);
}

void GuiMgr::drawGoodsBox(int x, int y, GoodsType goodsType, double inventory, double capacity) {
    graphicsMgr->getGraphicForGoodsMarketplaceIcon(goodsType)->drawAt(x, y);

    if (inventory != -1) {
        // Balken anzeigen
        if (capacity != -1) {
            int barHeight = (int) (inventory / capacity * 42);

            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            SDL_Rect rect = { x + 38, y + (42 - barHeight), 4, barHeight };
            SDL_RenderFillRect(renderer, &rect);
        }
            // Bestand anzeigen
        else {
            char inventoryOutput[10];
            sprintf(inventoryOutput, "%.0ft", floor(inventory));
            fontMgr->renderText(renderer, inventoryOutput, x + 40, y + 42, &colorWhite, &colorBlack,
                "DroidSans.ttf", 12, RENDERTEXT_HALIGN_RIGHT | RENDERTEXT_VALIGN_BOTTOM);
        }

#ifdef DEBUG_ECONOMICS
        char inventoryOutput[10];
        sprintf(inventoryOutput, "%.4ft", inventory);
        fontMgr->renderText(renderer, inventoryOutput, x + 40, y + 55, &colorWhite, &colorBlack,
            "DroidSans.ttf", 12, RENDERTEXT_HALIGN_RIGHT | RENDERTEXT_VALIGN_BOTTOM);
#endif
    }
}