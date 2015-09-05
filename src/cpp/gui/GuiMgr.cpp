#include <SDL.h>
#include <string>
#include "config/ConfigMgr.h"
#include "game/Game.h"
#include "graphics/graphic/sdl/SDLGraphic.h"
#include "graphics/mgr/IFontMgr.h"
#include "gui/components/map/GuiMap.h"
#include "gui/components/GuiAddBuildingWidget.h"
#include "gui/components/GuiPushButton.h"
#include "gui/components/GuiMapRotateWidget.h"
#include "gui/components/GuiMinimap.h"
#include "gui/components/GuiStatusBar.h"
#include "gui/panel-widgets/GuiBuildMenuWidget.h"
#include "gui/panel-widgets/GuiColonyGoodsWidget.h"
#include "gui/panel-widgets/GuiDummyWidget.h"
#include "gui/panel-widgets/GuiOptionsMenuWidget.h"
#include "gui/panel-widgets/GuiSelectedBuildingWidget.h"
#include "gui/Identifiers.h"
#include "map/Map.h"
#include "utils/Color.h"
#include "utils/Events.h"
#include "utils/Rect.h"

static Color colorWhite = Color(255, 255, 255, 255);
static Color colorBlack = Color(0, 0, 0, 255);
static Color colorLightBrown = Color(223, 216, 183, 255);

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
    delete ((GuiStatusBar*) findElement(GUI_ID_STATUS_BAR))->getGraphic();

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

    // Karte mit Resourcen-Leiste
    GuiResourcesBar* guiResourcesBar = new GuiResourcesBar(context);
    registerElement(GUI_ID_RESOURCES_BAR, guiResourcesBar);

    GuiMap* guiMap = new GuiMap(context, guiResourcesBar);
    guiMap->addChildElement(guiResourcesBar);
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

    // Karte drehen
    GuiMapRotateWidget* guiMapRotateWidget = new GuiMapRotateWidget(context);
    registerElement(GUI_ID_MAP_ROTATE_WIDGET, guiMapRotateWidget);
    panel->addChildElement(guiMapRotateWidget);

    // Statusleiste
    graphic = new SDLGraphic(renderer, "data/img/gui/statusbar.png");
    GuiStatusBar* statusBar = new GuiStatusBar(context);
    statusBar->setCoords(0, 734, graphic->getWidth(), graphic->getHeight());
    statusBar->setGraphic(graphic);
    registerElement(GUI_ID_STATUS_BAR, statusBar);

    // Panel-Widgets
    initPanelWidgets();

    // Buttons zum Umschalten der Tabs und zugehörige UI-Gruppen
    static struct {
        const char* statusBarText;
        const char* graphicFilename;
        const char* graphicPressedFilename;
        PanelButton panelButtonToActive;
        GuiPanelWidget* panelWidgetToActivate;

    } tabGraphics[4][2] = {
        {
            "Baumodus einschalten",
            "data/img/gui/button-build.png",
            "data/img/gui/button-build-pressed.png",
            PanelButton::ADD_BUILDING,
            (GuiPanelWidget*) findElement(GUI_ID_BUILD_MENU_PANEL_WIDGET)
        }, {
            "TODO", // TODO wir wollen den Kampfmodus doch gar nicht ;-)
            "data/img/gui/button-dummy.png",
            "data/img/gui/button-dummy-pressed.png",
            PanelButton::MILITARY,
            (GuiPanelWidget*) findElement(GUI_ID_MILITARY_PANEL_WIDGET)
        }, {
            "Infomodus einschalten",
            "data/img/gui/button-dummy.png",
            "data/img/gui/button-dummy-pressed.png",
            PanelButton::INFO,
            (GuiPanelWidget*) findElement(GUI_ID_PLAYER_STATUS_PANEL_WIDGET)
        }, {
            "Einstellungen",
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
        panelSwitchPushButton->setCoords(22 + i*55, 285, 48, 64);
        panelSwitchPushButton->setStatusBarText(tabGraphics[i]->statusBarText);
        panelSwitchPushButton->setOnClickFunction([this, i]() {
            panelState.selectedPanelButton = tabGraphics[i]->panelButtonToActive;
            panelState.activeGuiPanelWidget = tabGraphics[i]->panelWidgetToActivate;
            updateGuiFromPanelState();

            if (GUI_ID_PANEL_SWITCH_PUSH_BUTTON_BASE + i == GUI_ID_PANEL_SWITCH_PUSH_BUTTON_BUILD) {
                ((GuiMap*) context->guiMgr->findElement(GUI_ID_MAP))->onStartAddingStructure();
            }
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
    panelState.addingMapObject = context->configMgr->getMapObjectType("foresters");
    panelState.addingMapObjectView = Direction::SOUTH;
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
}

#ifndef NO_SDL
void GuiMgr::onEvent(SDL_Event& event) {
    Game* game = context->game;
    Map* map = game->getMap();

    // Vorbereitende Arbeiten. Das Event wird danach trotzdem an die GUI-Element gegeben.

    // Bei Linksklick die Koordinaten merken
    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
        int x = event.button.x;
        int y = event.button.y;

        startClickWindowCoords.setTo(x, y);
        startClickMapCoords = MapCoordUtils::getMapCoordsUnderMouse(*map, x, y);
    }

    // Maus bewegt? MapCoords ermitteln und vergleichen, ob wie die Kachel gewechselt haben
    else if (event.type == SDL_MOUSEMOTION) {
        int x = event.motion.x;
        int y = event.motion.y;

        const MapCoords& newMapCoordsUnderMouse = MapCoordUtils::getMapCoordsUnderMouse(*map, x, y);

        // Kachel verlassen? Dann spezielles Event verschicken.
        if (newMapCoordsUnderMouse != mapCoordsUnderMouse) {
            MouseMotionMapCoordsEvent* motionMapCoordsEvent = new MouseMotionMapCoordsEvent();
            motionMapCoordsEvent->mapCoordsBefore = mapCoordsUnderMouse;
            motionMapCoordsEvent->mapCoordsNow = newMapCoordsUnderMouse;

            SDL_Event eventToPush = {}; // 0-initalisieren
            eventToPush.type = context->userEventBase + USER_EVENT_MOUSEMOTION_MAPCOORDS;
            eventToPush.user.data1 = motionMapCoordsEvent;
            SDL_PushEvent(&eventToPush);

            /*
             * TODO Event-Flut verhindern. Wenn die Maus zu schnell bewegt wird, werden zu viele
             * USER_EVENT_MOUSEMOTION_MAPCOORDS-Events generiert und die Abarbeitung dieser a) dauert lange und b)
             * ist sinnfrei, mehrmals pro Frame zu machen
             */

            mapCoordsUnderMouse = newMapCoordsUnderMouse;
        }
    }

    // Erst alle GUI-Elemente durchgehen, dass die das Event zuerst kriegen
    for (auto iter = identifierMap.cbegin(); iter != identifierMap.cend(); iter++) {
		GuiBase* guiElement = iter->second;

        // nur an Toplevel-Elemente schicken
        if (guiElement->getParentElement() != nullptr) {
            continue;
        }

        if (!guiElement->onEvent(event)) {
            return;
        }
    }

    // Jetzt kann der GuiMgr selber gucken, ob er das Event brauchen kann
    
    // Spiel beenden
    if (event.type == SDL_QUIT) {
        quitGame = true;
        return;
    }

    // Rechtsklick...
    if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_RIGHT) {
        // Platzieren wir ein Gebäude -> abbrechen
        if (panelState.selectedPanelButton == PanelButton::ADD_BUILDING) {
            if (panelState.buildingMenuOpen) {
                panelState.buildingMenuOpen = false;
            } else {
                panelState.selectedPanelButton = PanelButton::INFO;
                panelState.activeGuiPanelWidget = (GuiPanelWidget*) findElement(GUI_ID_PLAYER_STATUS_PANEL_WIDGET);
            }
            updateGuiFromPanelState();
            ((GuiMap*) findElement(GUI_ID_MAP))->onCancelAddingStructure();
        }
        // Ist ein Gebäude auf der Karte markieren -> deselektieren
        else if(map->getSelectedMapObject() != nullptr) {
            game->setSelectedMapObject(nullptr);
        }

        return;
    }

    // Tastaturevents
    if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
            quitGame = true;
        }

        // Scrolling der Karte
        else if (event.key.keysym.scancode == SDL_SCANCODE_UP) {
            scrollMap(map, -1, -1);
        } else if (event.key.keysym.scancode == SDL_SCANCODE_DOWN) {
            scrollMap(map, 1, 1);
        } else if (event.key.keysym.scancode == SDL_SCANCODE_LEFT) {
            scrollMap(map, -1, 1);
        } else if (event.key.keysym.scancode == SDL_SCANCODE_RIGHT) {
            scrollMap(map, 1, -1);
        }

        // Karte zoomen
        else if (event.key.keysym.scancode == SDL_SCANCODE_F2) {
            if(!(event.key.keysym.mod & KMOD_SHIFT)) {
                changeMapZoom(map, 4);
            }
#ifdef DEBUG
            else {
                changeMapZoom(map, 32);
            }
#endif // DEBUG
        } else if (event.key.keysym.scancode == SDL_SCANCODE_F3) {
            if(!(event.key.keysym.mod & KMOD_SHIFT)) {
                changeMapZoom(map, 2);
            }
#ifdef DEBUG
            else {
                changeMapZoom(map, 16);
            }
#endif // DEBUG
        } else if (event.key.keysym.scancode == SDL_SCANCODE_F4) {
            if(!(event.key.keysym.mod & KMOD_SHIFT)) {
                changeMapZoom(map, 1);
            }
#ifdef DEBUG
            else {
                changeMapZoom(map, 8);
            }
#endif // DEBUG
        }

        // Karte drehen
        else if (event.key.keysym.scancode == SDL_SCANCODE_Z) {
            map->rotateViewCounterclockwise();
            ((GuiMinimap*) findElement(GUI_ID_MINIMAP))->onMapCoordsChanged();
            ((GuiMapRotateWidget*) findElement(GUI_ID_MAP_ROTATE_WIDGET))->onMapRotated();
        } else if (event.key.keysym.scancode == SDL_SCANCODE_X) {
            map->rotateViewClockwise();
            ((GuiMinimap*) findElement(GUI_ID_MINIMAP))->onMapCoordsChanged();
            ((GuiMapRotateWidget*) findElement(GUI_ID_MAP_ROTATE_WIDGET))->onMapRotated();
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

            ((GuiMap*) findElement(GUI_ID_MAP))->onStartAddingStructure();
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
            Direction::rotate90DegreesClockwise(panelState.addingMapObjectView);
            ((GuiMap*) findElement(GUI_ID_MAP))->onRotateAddingStructure();
        } else if (event.key.keysym.scancode == SDL_SCANCODE_COMMA) {
            Direction::rotate90DegreesCounterclockwise(panelState.addingMapObjectView);
            ((GuiMap*) findElement(GUI_ID_MAP))->onRotateAddingStructure();
        }

        // offiziellen FPS-Counter an-/ausschalten
        else if (event.key.keysym.scancode == SDL_SCANCODE_F && (event.key.keysym.mod & KMOD_SHIFT)) {
            game->toggleFpsCounter();
        }

#ifdef DEBUG_A_STAR
        bool needToRecalculate = false;

        // A*-Start- und Endkoordinaten festlegen
        if (event.key.keysym.scancode == SDL_SCANCODE_A) {
            AStar::debugAStar_source = mapCoordsUnderMouse;
            needToRecalculate = true;
        } else if (event.key.keysym.scancode == SDL_SCANCODE_S) {
            AStar::debugAStar_destination = mapCoordsUnderMouse;
            needToRecalculate = true;
        } else if (event.key.keysym.scancode == SDL_SCANCODE_D) {
            MapTile* mapTile = game->getMap()->getMapTileAt(mapCoordsUnderMouse);
            AStar::debugAStar_buildingToUseCatchmentArea = dynamic_cast<Building*>(mapTile->mapObjectFixed);
            needToRecalculate = true;
        } else if (event.key.keysym.scancode == SDL_SCANCODE_F) {
            AStar::debugAStar_useStreetOnly = !AStar::debugAStar_useStreetOnly;
            needToRecalculate = true;
        } else if (event.key.keysym.scancode == SDL_SCANCODE_G) {
            AStar::debugAStar_rightAnglesOnly = !AStar::debugAStar_rightAnglesOnly;
            needToRecalculate = true;
        }

        if (needToRecalculate) {
            if (AStar::debugAStar_source.x() > 0 && AStar::debugAStar_source.y() > 0 &&
                AStar::debugAStar_destination.x() > 0 && AStar::debugAStar_destination.y() > 0) {

                AStar aStar(context, AStar::debugAStar_buildingToUseCatchmentArea,
                            true, AStar::debugAStar_useStreetOnly, AStar::debugAStar_rightAnglesOnly);
                AStar::debugAStar_route = aStar.getRoute(AStar::debugAStar_source, AStar::debugAStar_destination);
            }
        }
#endif // DEBUG_A_STAR
    }
}
#endif // NO_SDL

void GuiMgr::onSelectedMapObjectChanged(const MapObject* newSelectedMapObject) {
    if (newSelectedMapObject == nullptr) {
        panelState.activeGuiPanelWidget = (GuiPanelWidget*) findElement(GUI_ID_PLAYER_STATUS_PANEL_WIDGET);
    } else {
        const Building* newSelectedBuilding = dynamic_cast<const Building*>(newSelectedMapObject);
        if (newSelectedBuilding != nullptr) {
            if (newSelectedBuilding->isStorageBuilding()) {
                GuiColonyGoodsWidget* guiColonyGoodsWidget =
                    (GuiColonyGoodsWidget*) findElement(GUI_ID_COLONY_GOODS_PANEL_WIDGET);

                guiColonyGoodsWidget->onSelectedMapBuildingChanged(newSelectedBuilding);
                panelState.activeGuiPanelWidget = guiColonyGoodsWidget;

            } else {
                GuiSelectedBuildingWidget* guiSelectedBuildingWidget =
                    (GuiSelectedBuildingWidget*) findElement(GUI_ID_SELECTED_BUILDING_PANEL_WIDGET);

                guiSelectedBuildingWidget->onSelectedMapBuildingChanged(newSelectedBuilding);
                panelState.activeGuiPanelWidget = guiSelectedBuildingWidget;
            }
        }
    }

    panelState.selectedPanelButton = PanelButton::INFO;
    updateGuiFromPanelState();
}

void GuiMgr::onNewGame() {
    ((GuiMap*) findElement(GUI_ID_MAP))->onNewGame();
    ((GuiMinimap*) findElement(GUI_ID_MINIMAP))->updateMinimapTexture();
    ((GuiMinimap*) findElement(GUI_ID_MINIMAP))->onMapCoordsChanged();
    ((GuiMapRotateWidget*) findElement(GUI_ID_MAP_ROTATE_WIDGET))->onMapRotated();
}

void GuiMgr::onOfficeCatchmentAreaChanged() {
    ((GuiMinimap*) findElement(GUI_ID_MINIMAP))->updateMinimapTexture();
}

void GuiMgr::setStatusBarText(const std::string& text) {
    ((GuiStatusBar*) findElement(GUI_ID_STATUS_BAR))->setText(text);
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

    // Baukosten aktualisieren
    GuiResourcesBar* guiResourcesBar = (GuiResourcesBar*) findElement(GUI_ID_RESOURCES_BAR);
    if (panelState.selectedPanelButton == PanelButton::ADD_BUILDING) {
        const MapObjectType* mapObjectType = context->guiMgr->getPanelState().addingMapObject;
        const BuildingCosts& buildingCosts = mapObjectType->buildingCosts;

        guiResourcesBar->showBuildingCosts(buildingCosts);
    } else {
        guiResourcesBar->hideBuildingCosts();
    }
}

void GuiMgr::drawGoodsBox(int x, int y, const Good* good, double inventory, double capacity) {
    const std::string graphicSetName = context->graphicsMgr->getGraphicSetNameForGoodIcons(good, true);
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

inline void GuiMgr::changeMapZoom(Map* map, int newScreenZoom) {
    map->setScreenZoom(newScreenZoom);
    ((GuiMinimap*) findElement(GUI_ID_MINIMAP))->onMapCoordsChanged();
}

inline void GuiMgr::scrollMap(Map* map, int xDelta, int yDelta) {
    map->scroll(xDelta, yDelta);
    ((GuiMinimap*) findElement(GUI_ID_MINIMAP))->onMapCoordsChanged();
}

void GuiMgr::drawPanelHeader(int x, int y, const std::string& string, const Player* drawPlayersCoatOfArms) {
    context->graphicsMgr->getGraphicSet("panel-header")->getStatic()->getGraphic()->drawAt(x, y);
    context->fontMgr->renderText(renderer, string, x + 118, y + 19,
        &colorLightBrown, &colorBlack, "DroidSans-Bold.ttf", 15, RENDERTEXT_HALIGN_CENTER | RENDERTEXT_VALIGN_MIDDLE);

    if (drawPlayersCoatOfArms != nullptr) {
        const std::string graphicSetName =
            context->graphicsMgr->getGraphicSetNameForCoatOfArms("large", drawPlayersCoatOfArms->getColor());
        const IGraphic* coatOfArmsGraphic =
            context->graphicsMgr->getGraphicSet(graphicSetName)->getStatic()->getGraphic();

        coatOfArmsGraphic->drawShadowAt(x + 209, y + 6);
        coatOfArmsGraphic->drawAt(x + 203, y);
    }
}
