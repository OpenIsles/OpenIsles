#include "game/Game.h"
#include "gui/GuiButton.h"
#include "gui/Identifiers.h"
#include "gui/GuiMgr.h"
#include "gui/GuiStaticElement.h"
#include "map/Map.h"
#include "sound/SoundMgr.h"
#include "Graphic.h"
#include "GuiPushButton.h"

// Aus main.cpp importiert
extern bool quitGame;
extern Game* game;
extern Map* map;
extern SoundMgr* soundMgr;


GuiMgr::GuiMgr() {
    Graphic* graphic = new Graphic("data/img/gui/panel.png");
	GuiStaticElement* panel = new GuiStaticElement();
    panel->setWindowCoords(768, 0, graphic->getWidth(), graphic->getHeight());
    panel->setGraphic(graphic);
    registerElement(GUI_ID_PANEL, panel);
    
    graphic = new Graphic("data/img/gui/statusbar.png");
	GuiStaticElement* statusBar = new GuiStaticElement();
    statusBar->setWindowCoords(0, 734, graphic->getWidth(), graphic->getHeight());
    statusBar->setGraphic(graphic);
    registerElement(GUI_ID_STATUS_BAR, statusBar);
    
    GuiPushButton* musicPushButton = new GuiPushButton();
    musicPushButton->setGraphic(new Graphic("data/img/gui/button-music.png"));
    musicPushButton->setGraphicPressed(new Graphic("data/img/gui/button-music-pressed.png"));
    musicPushButton->setWindowCoords(785, 690, 64, 64);
    musicPushButton->setOnClickFunction([this]() {
        bool musicEnabled = ((GuiPushButton*) findElement(GUI_ID_MUSIC_PUSH_BUTTON))->isActive();
        
        if (musicEnabled) {
            soundMgr->enableMusic();
        } else {
            soundMgr->disableMusic();
        }
    });
    registerElement(GUI_ID_MUSIC_PUSH_BUTTON, musicPushButton);
    
    GuiPushButton* addBuildingPushButton = new GuiPushButton();
    addBuildingPushButton->setGraphic(new Graphic("data/img/gui/button-add-building.png"));
    addBuildingPushButton->setGraphicPressed(new Graphic("data/img/gui/button-add-building-pressed.png"));
    addBuildingPushButton->setWindowCoords(790, 320, 52, 64);
    addBuildingPushButton->setOnClickFunction([this]() {
        bool buttonActive = ((GuiPushButton*) findElement(GUI_ID_ADD_BUILDING_PUSH_BUTTON))->isActive();
        
        if (buttonActive) {
            game->startAddingStructure(StructureType::MARKETPLACE);
        } else {
            game->endAddingStructure();
        }
    });
    registerElement(GUI_ID_ADD_BUILDING_PUSH_BUTTON, addBuildingPushButton);
    
    GuiPushButton* panelSwitchPushButtonBuild = new GuiPushButton();
    panelSwitchPushButtonBuild->setGraphic(new Graphic("data/img/gui/button-build.png"));
    panelSwitchPushButtonBuild->setGraphicPressed(new Graphic("data/img/gui/button-build-pressed.png"));
    panelSwitchPushButtonBuild->setWindowCoords(790, 235, 48, 64);
    panelSwitchPushButtonBuild->setOnClickFunction([this]() {
        ((GuiPushButton*) findElement(GUI_ID_PANEL_SWITCH_PUSH_BUTTON_2))->setActive(false);
        ((GuiPushButton*) findElement(GUI_ID_PANEL_SWITCH_PUSH_BUTTON_3))->setActive(false);
        ((GuiPushButton*) findElement(GUI_ID_PANEL_SWITCH_PUSH_BUTTON_4))->setActive(false);
        std::cout << "panel: build" << std::endl;
    });
    registerElement(GUI_ID_PANEL_SWITCH_PUSH_BUTTON_BUILD, panelSwitchPushButtonBuild);
    
    GuiPushButton* panelSwitchPushButton2 = new GuiPushButton();
    panelSwitchPushButton2->setGraphic(new Graphic("data/img/gui/button-dummy.png"));
    panelSwitchPushButton2->setGraphicPressed(new Graphic("data/img/gui/button-dummy-pressed.png"));
    panelSwitchPushButton2->setWindowCoords(845, 235, 48, 64);
    panelSwitchPushButton2->setOnClickFunction([this]() {
        ((GuiPushButton*) findElement(GUI_ID_PANEL_SWITCH_PUSH_BUTTON_BUILD))->setActive(false);
        ((GuiPushButton*) findElement(GUI_ID_PANEL_SWITCH_PUSH_BUTTON_3))->setActive(false);
        ((GuiPushButton*) findElement(GUI_ID_PANEL_SWITCH_PUSH_BUTTON_4))->setActive(false);
        std::cout << "panel: dummy2" << std::endl;
    });
    registerElement(GUI_ID_PANEL_SWITCH_PUSH_BUTTON_2, panelSwitchPushButton2);
    
    GuiPushButton* panelSwitchPushButton3 = new GuiPushButton();
    panelSwitchPushButton3->setGraphic(new Graphic("data/img/gui/button-dummy.png"));
    panelSwitchPushButton3->setGraphicPressed(new Graphic("data/img/gui/button-dummy-pressed.png"));
    panelSwitchPushButton3->setWindowCoords(900, 235, 48, 64);
    panelSwitchPushButton3->setOnClickFunction([this]() {
        ((GuiPushButton*) findElement(GUI_ID_PANEL_SWITCH_PUSH_BUTTON_BUILD))->setActive(false);
        ((GuiPushButton*) findElement(GUI_ID_PANEL_SWITCH_PUSH_BUTTON_2))->setActive(false);
        ((GuiPushButton*) findElement(GUI_ID_PANEL_SWITCH_PUSH_BUTTON_4))->setActive(false);
        std::cout << "panel: dummy3" << std::endl;
    });
    registerElement(GUI_ID_PANEL_SWITCH_PUSH_BUTTON_3, panelSwitchPushButton3);
    
    GuiPushButton* panelSwitchPushButton4 = new GuiPushButton();
    panelSwitchPushButton4->setGraphic(new Graphic("data/img/gui/button-dummy.png"));
    panelSwitchPushButton4->setGraphicPressed(new Graphic("data/img/gui/button-dummy-pressed.png"));
    panelSwitchPushButton4->setWindowCoords(955, 235, 48, 64);
    panelSwitchPushButton4->setOnClickFunction([this]() {
        ((GuiPushButton*) findElement(GUI_ID_PANEL_SWITCH_PUSH_BUTTON_BUILD))->setActive(false);
        ((GuiPushButton*) findElement(GUI_ID_PANEL_SWITCH_PUSH_BUTTON_2))->setActive(false);
        ((GuiPushButton*) findElement(GUI_ID_PANEL_SWITCH_PUSH_BUTTON_3))->setActive(false);
        std::cout << "panel: dummy4" << std::endl;
    });
    registerElement(GUI_ID_PANEL_SWITCH_PUSH_BUTTON_4, panelSwitchPushButton4);
    
    // Testzeugs
    graphic = new Graphic("data/img/gui/testbutton.png");
    GuiButton* testButton = new GuiButton();
    testButton->setWindowCoords(795, 390, graphic->getWidth(), graphic->getHeight());
    testButton->setGraphic(graphic);
    testButton->setGraphicPressed(new Graphic("data/img/gui/testbutton-pressed.png"));
    testButton->setOnClickFunction([]() {
        std::cout << "Click1" << std::endl;
    });
    testButton->setVisible(false);
    registerElement(GUI_ID_TEST_BUTTON1, testButton);
    
    graphic = new Graphic("data/img/gui/testbutton.png");
    GuiButton* testButton2 = new GuiButton();
    testButton2->setWindowCoords(875, 390, graphic->getWidth(), graphic->getHeight());
    testButton2->setGraphic(graphic);
    testButton2->setGraphicPressed(new Graphic("data/img/gui/testbutton-pressed.png"));
    testButton2->setOnClickFunction([]() {
        std::cout << "Click2" << std::endl;
    });
    testButton2->setVisible(false);
    registerElement(GUI_ID_TEST_BUTTON2, testButton2);
    
    graphic = new Graphic("data/img/gui/testbutton.png");
    GuiPushButton* testPushButton = new GuiPushButton();
    testPushButton->setWindowCoords(955, 390, graphic->getWidth(), graphic->getHeight());
    testPushButton->setGraphic(graphic);
    testPushButton->setGraphicPressed(new Graphic("data/img/gui/testbutton-pressed.png"));
    testPushButton->setOnClickFunction([this]() {
        GuiPushButton* testPushButton = (GuiPushButton*) findElement(GUI_ID_TEST_PUSH_BUTTON);
        ((GuiPushButton*) findElement(GUI_ID_TEST_BUTTON1))->setVisible(testPushButton->isActive());
        ((GuiPushButton*) findElement(GUI_ID_TEST_BUTTON2))->setVisible(testPushButton->isActive());
        std::cout << "Click im PushButton: " << (testPushButton->isActive() ? "active" : "inactive") << std::endl;
    });
    registerElement(GUI_ID_TEST_PUSH_BUTTON, testPushButton);
}

GuiMgr::~GuiMgr() {
    for (auto iter = identifierMap.cbegin(); iter != identifierMap.cend(); iter++) {
		GuiBase* guiElement = iter->second;
        delete guiElement;
    }
    identifierMap.clear();
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
        
        if (!guiElement->isVisible()) {
            continue;
        }
        
        guiElement->render(renderer);
    }
}

void GuiMgr::onEvent(SDL_Event& event) {
    // TODO Event besser queuen und nicht sofort abarbeiten
    
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
        } else if (event.key.keysym.scancode == SDL_SCANCODE_UP) {
            map->scroll(0, -16);
        } else if (event.key.keysym.scancode == SDL_SCANCODE_DOWN) {
            map->scroll(0, 16);
        } else if (event.key.keysym.scancode == SDL_SCANCODE_LEFT) {
            map->scroll(-16, 0);
        } else if (event.key.keysym.scancode == SDL_SCANCODE_RIGHT) {
            map->scroll(16, 0);
        } else if (event.key.keysym.scancode == SDL_SCANCODE_F2) {
            map->setScreenZoom(4);
        } else if (event.key.keysym.scancode == SDL_SCANCODE_F3) {
            map->setScreenZoom(2);
        } else if (event.key.keysym.scancode == SDL_SCANCODE_F4) {
            map->setScreenZoom(1);
        }
        
        // Debug-Zwecke
        if (event.key.keysym.scancode == SDL_SCANCODE_1) {
            game->startAddingStructure(StructureType::CHAPEL);
        } else if (event.key.keysym.scancode == SDL_SCANCODE_2) {
            game->startAddingStructure(StructureType::WEAPONSMITH);
        } else if (event.key.keysym.scancode == SDL_SCANCODE_3) {
            game->startAddingStructure(StructureType::SIGNALFIRE);
        } else if (event.key.keysym.scancode == SDL_SCANCODE_4) {
            game->startAddingStructure(StructureType::HERBARY);
        } else if (event.key.keysym.scancode == SDL_SCANCODE_5) {
            game->startAddingStructure(StructureType::BRICKYARD);
        } else if (event.key.keysym.scancode == SDL_SCANCODE_6) {
            game->startAddingStructure(StructureType::BRICKYARD2);
        } else if (event.key.keysym.scancode == SDL_SCANCODE_7) {
            game->startAddingStructure(StructureType::OFFICE);
        } else if (event.key.keysym.scancode == SDL_SCANCODE_8) {
            game->startAddingStructure(StructureType::WAY_E);
        } else if (event.key.keysym.scancode == SDL_SCANCODE_9) {
            game->startAddingStructure(StructureType::WAY_SW_NE);
        } else if (event.key.keysym.scancode == SDL_SCANCODE_0) {
            game->startAddingStructure(StructureType::FORESTERS);
        } else if (event.key.keysym.scancode == SDL_SCANCODE_DELETE) {
            map->deleteSelectedObject();
        }
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
        
        if (!guiElement->isVisible()) {
            continue;
        }
        
        guiElement->onEvent(event);
    }
}
