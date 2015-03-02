#include <string>
#include "config/ConfigMgr.h"
#include "game/Colony.h"
#include "game/Game.h"
#include "graphics/graphic/sdl/SDLGraphic.h"
#include "graphics/mgr/IFontMgr.h"
#include "gui/components/GuiAddBuildingWidget.h"
#include "gui/components/GuiButton.h"
#include "gui/components/GuiPushButton.h"
#include "gui/components/GuiMap.h"
#include "gui/components/GuiMinimap.h"
#include "gui/panel-widgets/GuiBuildMenuWidget.h"
#include "gui/panel-widgets/GuiColonyGoodsWidget.h"
#include "gui/panel-widgets/GuiDummyWidget.h"
#include "gui/panel-widgets/GuiOptionsMenuWidget.h"
#include "gui/panel-widgets/GuiSelectedBuildingWidget.h"
#include "gui/Identifiers.h"
#include "map/Map.h"
#include "utils/Color.h"
#include "utils/Rect.h"

static Color colorWhite = Color(255, 255, 255, 255);
static Color colorBlack = Color(0, 0, 0, 255);

#ifdef DEBUG_A_STAR
#include "map/coords/MapCoords.h"
#include "pathfinding/AStar.h"
#endif


GuiMgr::GuiMgr(const Context* const context, IRenderer* renderer) :
    ContextAware(context),
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

    // Karte
    GuiMap* guiMap = new GuiMap(context);
    registerElement(GUI_ID_MAP, guiMap);

    // Panel
    IGraphic* graphic = new SDLGraphic(renderer, "data/img/gui/panel.png");
    GuiStaticElement* panel = new GuiStaticElement(context);
    panel->setCoords(768, 0, graphic->getWidth(), graphic->getHeight());
    panel->setGraphic(graphic);
    registerElement(GUI_ID_PANEL, panel);

    // Minimap
    GuiMinimap* guiMinimap = new GuiMinimap(context);
    registerElement(GUI_ID_MINIMAP, guiMinimap);
    panel->addChildElement(guiMinimap);

    // Statusleiste
    graphic = new SDLGraphic(renderer, "data/img/gui/statusbar.png");
    GuiStaticElement* statusBar = new GuiStaticElement(context);
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
        GuiPushButton* panelSwitchPushButton = new GuiPushButton(context);
        panelSwitchPushButton->setGraphic(new SDLGraphic(renderer, tabGraphics[i]->graphicFilename));
        panelSwitchPushButton->setGraphicPressed(new SDLGraphic(renderer, tabGraphics[i]->graphicPressedFilename));
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

    // Initial ist für den Gebäudebau der Förster ausgewählt...
    panelState.selectedBuildingGroup = BuildingGroup::FARM;
    panelState.addingStructure = StructureType::FORESTERS;
    panelState.addingStructureView = FourDirectionsView(FourDirectionsView::SOUTH);
    panelState.buildingMenuOpen = false;

    // ...und wir zeigen das Spielerstatus-Widget
    panelState.activeGuiPanelWidget = (GuiPanelWidget*) findElement(GUI_ID_PLAYER_STATUS_PANEL_WIDGET);

    updateGuiFromPanelState();
}

void GuiMgr::initPanelWidgets() {
    GuiBase* panel = findElement(GUI_ID_PANEL);

    // ausgewähltes Gebäude (Infos über Produktion und intere Lagerbestände des Gebäudes)
    GuiSelectedBuildingWidget* selectedBuildingWidget = new GuiSelectedBuildingWidget(context);
    registerElement(GUI_ID_SELECTED_BUILDING_PANEL_WIDGET, selectedBuildingWidget);
    panel->addChildElement(selectedBuildingWidget);

    // Kolonie-Warenübersicht
    GuiColonyGoodsWidget* colonyGoodsWidget = new GuiColonyGoodsWidget(context);
    registerElement(GUI_ID_COLONY_GOODS_PANEL_WIDGET, colonyGoodsWidget);
    panel->addChildElement(colonyGoodsWidget);

    // Baumenü
    GuiBuildMenuWidget* buildMenuWidget = new GuiBuildMenuWidget(context);
    registerElement(GUI_ID_BUILD_MENU_PANEL_WIDGET, buildMenuWidget);
    panel->addChildElement(buildMenuWidget);

    // Kampfmenü
    GuiDummyWidget* dummyWidget1 = new GuiDummyWidget(context);
    registerElement(GUI_ID_MILITARY_PANEL_WIDGET, dummyWidget1);
    panel->addChildElement(dummyWidget1);

    // Spielerstatus (Infomenü)
    GuiDummyWidget* dummyWidget2 = new GuiDummyWidget(context);
    registerElement(GUI_ID_PLAYER_STATUS_PANEL_WIDGET, dummyWidget2);
    panel->addChildElement(dummyWidget2);

    // Optionen-Menü
    GuiOptionsMenuWidget* optionsMenuWidget = new GuiOptionsMenuWidget(context);
    registerElement(GUI_ID_OPTIONS_MENU_PANEL_WIDGET, optionsMenuWidget);
    panel->addChildElement(optionsMenuWidget);
}

void GuiMgr::registerElement(int identifier, GuiBase* guiElement) {
    if (identifierMap.count(identifier) > 0) {
        throw std::runtime_error("Identifier already registered");
    }
    
    identifierMap[identifier] = guiElement;
    guiElement->id = identifier;
}

GuiBase* GuiMgr::findElement(int identifier) {
    auto iter = identifierMap.find(identifier);
    
    if (iter == identifierMap.end()) {
        throw std::runtime_error("Identifier not registered");
    }
    
    return iter->second;
}

void GuiMgr::render() {
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

#ifndef NO_SDL
void GuiMgr::onEvent(SDL_Event& event) {
    // TODO Event besser queuen und nicht sofort abarbeiten

    Game* game = context->game;
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
                game->setSelectedMapObject(nullptr);
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
            map->scroll(-1, -1);
        } else if (event.key.keysym.scancode == SDL_SCANCODE_DOWN) {
            map->scroll(1, 1);
        } else if (event.key.keysym.scancode == SDL_SCANCODE_LEFT) {
            map->scroll(-1, 1);
        } else if (event.key.keysym.scancode == SDL_SCANCODE_RIGHT) {
            map->scroll(1, -1);
        }

        // Karte zoomen
        else if (event.key.keysym.scancode == SDL_SCANCODE_F2) {
            if(!(event.key.keysym.mod & KMOD_SHIFT)) {
                map->setScreenZoom(4);
            }
#ifdef DEBUG
            else {
                map->setScreenZoom(32);
            }
#endif // DEBUG
        } else if (event.key.keysym.scancode == SDL_SCANCODE_F3) {
            if(!(event.key.keysym.mod & KMOD_SHIFT)) {
                map->setScreenZoom(2);
            }
#ifdef DEBUG
            else {
                map->setScreenZoom(16);
            }
#endif // DEBUG
        } else if (event.key.keysym.scancode == SDL_SCANCODE_F4) {
            if(!(event.key.keysym.mod & KMOD_SHIFT)) {
                map->setScreenZoom(1);
            }
#ifdef DEBUG
            else {
                map->setScreenZoom(8);
            }
#endif // DEBUG
        }

        // Karte drehen
        else if (event.key.keysym.scancode == SDL_SCANCODE_Z) {
            map->rotateViewCounterclockwise();
        } else if (event.key.keysym.scancode == SDL_SCANCODE_X) {
            map->rotateViewClockwise();
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
#endif // DEBUG
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

        // Drehen des Gebäudes, was wir bauen wollen
        else if (event.key.keysym.scancode == SDL_SCANCODE_PERIOD) {
            panelState.addingStructureView++;
        } else if (event.key.keysym.scancode == SDL_SCANCODE_COMMA) {
            panelState.addingStructureView--;
        }

#ifdef DEBUG
        // Debug-Zwecke
        if (event.key.keysym.scancode == SDL_SCANCODE_8) {
            panelState.selectedPanelButton = PanelButton::ADD_BUILDING;
            panelState.selectedBuildingGroup = BuildingGroup::PUBLIC;
            panelState.addingStructure = StructureType::STREET;
            panelState.activeGuiPanelWidget = (GuiPanelWidget*) findElement(GUI_ID_BUILD_MENU_PANEL_WIDGET);
            updateGuiFromPanelState();
        }
#endif // DEBUG

#ifdef DEBUG_A_STAR
        bool needToRecalculate = false;

        MapCoords mouseCurrentMapCoords =
            MapCoordUtils::getMapCoordsUnderMouse(*map, context->mouseCurrentX, context->mouseCurrentY);

        // A*-Start- und Endkoordinaten festlegen
        if (event.key.keysym.scancode == SDL_SCANCODE_A) {
            AStar::debugAStar_source = mouseCurrentMapCoords;
            needToRecalculate = true;
        } else if (event.key.keysym.scancode == SDL_SCANCODE_S) {
            AStar::debugAStar_destination = mouseCurrentMapCoords;
            needToRecalculate = true;
        } else if (event.key.keysym.scancode == SDL_SCANCODE_D) {
            MapTile* mapTile = game->getMap()->getMapTileAt(mouseCurrentMapCoords);
            AStar::debugAStar_buildingToUseCatchmentArea = dynamic_cast<Building*>(mapTile->mapObject);
            needToRecalculate = true;
        } else if (event.key.keysym.scancode == SDL_SCANCODE_F) {
            AStar::debugAStar_useStreetOnly = !AStar::debugAStar_useStreetOnly;
            needToRecalculate = true;
        }

        if (needToRecalculate) {
            if (AStar::debugAStar_source.x() > 0 && AStar::debugAStar_source.y() > 0 &&
                AStar::debugAStar_destination.x() > 0 && AStar::debugAStar_destination.y() > 0) {

                if (AStar::debugAStar_route != nullptr) {
                    delete AStar::debugAStar_route;
                }

                AStar* aStar = new AStar(context, AStar::debugAStar_source, AStar::debugAStar_destination,
                    AStar::debugAStar_buildingToUseCatchmentArea, AStar::debugAStar_useStreetOnly);
                aStar->cutRouteInsideBuildings();
                AStar::debugAStar_route = aStar->getRoute();
                delete aStar;
            }
        }
#endif // DEBUG_A_STAR
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
#endif // NO_SDL

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

void GuiMgr::onNewGame() {
    ((GuiMap*) findElement(GUI_ID_MAP))->onNewGame();
    ((GuiMinimap*) findElement(GUI_ID_MINIMAP))->updateMinimapTexture();
}

void GuiMgr::onOfficeCatchmentAreaChanged() {
    ((GuiMinimap*) findElement(GUI_ID_MINIMAP))->updateMinimapTexture();
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
    Player* currentPlayer = context->game->getCurrentPlayer();
    const BuildingCosts* buildingCosts = (panelState.selectedPanelButton == PanelButton::ADD_BUILDING) ?
        context->configMgr->getBuildingConfig(panelState.addingStructure)->getBuildingCosts() : nullptr;

    // Münzenguthaben
    context->graphicsMgr->getGraphicSet("coin")->getStatic()->getGraphic()->drawAt(15, 8);

    std::string outputString = toString(currentPlayer->coins);
    if (buildingCosts != nullptr) {
        outputString += " (";
        outputString += toString(buildingCosts->coins);
        outputString += ")";
    }
    context->fontMgr->renderText(renderer, outputString, 42, 10,
        &colorWhite, &colorBlack, "DroidSans-Bold.ttf", 18, RENDERTEXT_HALIGN_LEFT);

    // Siedlung, wo der Cursor grade is
    MapCoords mouseCurrentMapCoords =
        MapCoordUtils::getMapCoordsUnderMouse(*context->game->getMap(), context->mouseCurrentX, context->mouseCurrentY);

    MapTile* mapTileAtCursor = context->game->getMap()->getMapTileAt(mouseCurrentMapCoords);
    if (mapTileAtCursor == nullptr) {
        return;
    }

    Colony* colony = context->game->getColony(mapTileAtCursor->player, mapTileAtCursor->isle);
    if (colony == nullptr) {
        return;
    }

    // Waren (nur für den eigenen Spieler)
    if (mapTileAtCursor->player == currentPlayer) {
        GoodsType goodsToDraw[] = { GoodsType::TOOLS, GoodsType::WOOD, GoodsType::BRICKS };
        int x = 290;
        for (unsigned int i = 0; i < sizeof(goodsToDraw); i++, x += 110) {
            GoodsType goodsType = goodsToDraw[i];
            const std::string graphicSetName = context->graphicsMgr->getGraphicSetNameForGoodIcons(goodsType, false);
            context->graphicsMgr->getGraphicSet(graphicSetName)->getStatic()->getGraphic()->drawAt(x, 5);

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

            context->fontMgr->renderText(renderer, outputString, x + 35, 10,
                &colorWhite, &colorBlack, "DroidSans-Bold.ttf", 18, RENDERTEXT_HALIGN_LEFT);
        }
    }

    // Einwohnerzahl (immer anzeigen)
    const std::string graphicSetName =
        context->graphicsMgr->getGraphicSetNameForCoatOfArmsPopulation(mapTileAtCursor->player->getColor());
    const IGraphic* populationIconGraphic = context->graphicsMgr->getGraphicSet(graphicSetName)->getStatic()->getGraphic();
    populationIconGraphic->drawAt(655, 6);

    outputString = toString(colony->population);
    context->fontMgr->renderText(renderer, outputString, 690, 10,
        &colorWhite, &colorBlack, "DroidSans-Bold.ttf", 18, RENDERTEXT_HALIGN_LEFT);
}

void GuiMgr::drawGoodsBox(int x, int y, GoodsType goodsType, double inventory, double capacity) {
    const std::string graphicSetName = context->graphicsMgr->getGraphicSetNameForGoodIcons(goodsType, true);
    context->graphicsMgr->getGraphicSet(graphicSetName)->getStatic()->getGraphic()->drawAt(x, y);

    if (inventory != -1) {
        // Balken anzeigen
        if (capacity != -1) {
            int barHeight = (int) (inventory / capacity * 42);

            renderer->setDrawColor(Color(255, 0, 0, 255));
            Rect rect(x + 38, y + (42 - barHeight), 4, barHeight);
            renderer->fillRect(rect);
        }
            // Bestand anzeigen
        else {
            char inventoryOutput[10];
            sprintf(inventoryOutput, "%.0ft", floor(inventory));
            context->fontMgr->renderText(renderer, inventoryOutput, x + 40, y + 42, &colorWhite, &colorBlack,
                "DroidSans.ttf", 12, RENDERTEXT_HALIGN_RIGHT | RENDERTEXT_VALIGN_BOTTOM);
        }

#ifdef DEBUG_ECONOMICS
        char inventoryOutput[10];
        sprintf(inventoryOutput, "%.4ft", inventory);
        context->fontMgr->renderText(renderer, inventoryOutput, x + 40, y + 55, &colorWhite, &colorBlack,
            "DroidSans.ttf", 12, RENDERTEXT_HALIGN_RIGHT | RENDERTEXT_VALIGN_BOTTOM);
#endif
    }
}