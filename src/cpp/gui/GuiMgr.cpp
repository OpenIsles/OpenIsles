#include "gui/GuiButton.h"
#include "gui/GuiMgr.h"
#include "gui/GuiStaticElement.h"
#include "map/Map.h"
#include "sound/SoundMgr.h"
#include "Graphic.h"
#include "GuiPushButton.h"

extern bool quitGame;
extern Map* map;
extern SoundMgr* soundMgr;


GuiMgr::GuiMgr() {
    Graphic* graphic = new Graphic("data/img/gui/panel.png");
	panel = new GuiStaticElement();
    panel->setWindowCoords(768, 0, graphic->getWidth(), graphic->getHeight());
    panel->setGraphic(graphic);
    
    graphic = new Graphic("data/img/gui/statusbar.png");
	statusBar = new GuiStaticElement();
    statusBar->setWindowCoords(0, 734, graphic->getWidth(), graphic->getHeight());
    statusBar->setGraphic(graphic);
    
    musicPushButton = new GuiPushButton();
    musicPushButton->setGraphic(new Graphic("data/img/gui/button-music.png"));
    musicPushButton->setGraphicPressed(new Graphic("data/img/gui/button-music-pressed.png"));
    musicPushButton->setWindowCoords(785, 690, 64, 64);
    musicPushButton->setOnClickFunction([this]() {
        bool musicEnabled = this->musicPushButton->isActive();
        
        if (musicEnabled) {
            soundMgr->enableMusic();
        } else  {
            soundMgr->disableMusic();
        }
    });
    
    panelSwitchPushButtonBuild = new GuiPushButton();
    panelSwitchPushButtonBuild->setGraphic(new Graphic("data/img/gui/button-build.png"));
    panelSwitchPushButtonBuild->setGraphicPressed(new Graphic("data/img/gui/button-build-pressed.png"));
    panelSwitchPushButtonBuild->setWindowCoords(790, 235, 48, 64);
    panelSwitchPushButtonBuild->setOnClickFunction([this]() {
        this->panelSwitchPushButton2->setActive(false);
        this->panelSwitchPushButton3->setActive(false);
        this->panelSwitchPushButton4->setActive(false);
        std::cout << "panel: build" << std::endl;
    });
    
    panelSwitchPushButton2 = new GuiPushButton();
    panelSwitchPushButton2->setGraphic(new Graphic("data/img/gui/button-dummy.png"));
    panelSwitchPushButton2->setGraphicPressed(new Graphic("data/img/gui/button-dummy-pressed.png"));
    panelSwitchPushButton2->setWindowCoords(845, 235, 48, 64);
    panelSwitchPushButton2->setOnClickFunction([this]() {
        this->panelSwitchPushButtonBuild->setActive(false);
        this->panelSwitchPushButton3->setActive(false);
        this->panelSwitchPushButton4->setActive(false);
        std::cout << "panel: dummy2" << std::endl;
    });
    
    panelSwitchPushButton3 = new GuiPushButton();
    panelSwitchPushButton3->setGraphic(new Graphic("data/img/gui/button-dummy.png"));
    panelSwitchPushButton3->setGraphicPressed(new Graphic("data/img/gui/button-dummy-pressed.png"));
    panelSwitchPushButton3->setWindowCoords(900, 235, 48, 64);
    panelSwitchPushButton3->setOnClickFunction([this]() {
        this->panelSwitchPushButtonBuild->setActive(false);
        this->panelSwitchPushButton2->setActive(false);
        this->panelSwitchPushButton4->setActive(false);
        std::cout << "panel: dummy3" << std::endl;
    });
    
    panelSwitchPushButton4 = new GuiPushButton();
    panelSwitchPushButton4->setGraphic(new Graphic("data/img/gui/button-dummy.png"));
    panelSwitchPushButton4->setGraphicPressed(new Graphic("data/img/gui/button-dummy-pressed.png"));
    panelSwitchPushButton4->setWindowCoords(955, 235, 48, 64);
    panelSwitchPushButton4->setOnClickFunction([this]() {
        this->panelSwitchPushButtonBuild->setActive(false);
        this->panelSwitchPushButton2->setActive(false);
        this->panelSwitchPushButton3->setActive(false);
        std::cout << "panel: dummy4" << std::endl;
    });
    
    // Testzeugs
    graphic = new Graphic("data/img/gui/testbutton.png");
    testButton = new GuiButton();
    testButton->setWindowCoords(795, 390, graphic->getWidth(), graphic->getHeight());
    testButton->setGraphic(graphic);
    testButton->setGraphicPressed(new Graphic("data/img/gui/testbutton-pressed.png"));
    testButton->setOnClickFunction([]() {
        std::cout << "Click" << std::endl;
    });
    
    graphic = new Graphic("data/img/gui/testbutton.png");
    testPushButton = new GuiPushButton();
    testPushButton->setWindowCoords(875, 390, graphic->getWidth(), graphic->getHeight());
    testPushButton->setGraphic(graphic);
    testPushButton->setGraphicPressed(new Graphic("data/img/gui/testbutton-pressed.png"));
    testPushButton->setOnClickFunction([this]() {
        std::cout << "Click im PushButton: " << (this->testPushButton->isActive() ? "active" : "inactive") << std::endl;
    });
}

GuiMgr::~GuiMgr() {
    // TODO alle GUI-Element durchiterieren
    
    delete panel->getGraphic();
	delete panel;
	panel = nullptr;

    delete statusBar->getGraphic();
	delete statusBar;
	statusBar = nullptr;
    
    delete musicPushButton->getGraphic();
    delete musicPushButton->getGraphicPressed();
	delete musicPushButton;
    musicPushButton = nullptr;
    
    delete panelSwitchPushButtonBuild->getGraphic();
    delete panelSwitchPushButtonBuild->getGraphicPressed();
	delete panelSwitchPushButtonBuild;
    panelSwitchPushButtonBuild = nullptr;
    
    delete panelSwitchPushButton2->getGraphic();
    delete panelSwitchPushButton2->getGraphicPressed();
	delete panelSwitchPushButton2;
    panelSwitchPushButton2 = nullptr;
    
    delete panelSwitchPushButton3->getGraphic();
    delete panelSwitchPushButton3->getGraphicPressed();
	delete panelSwitchPushButton3;
    panelSwitchPushButton3 = nullptr;
    
    delete panelSwitchPushButton4->getGraphic();
    delete panelSwitchPushButton4->getGraphicPressed();
	delete panelSwitchPushButton4;
    panelSwitchPushButton4 = nullptr;
    
    // Testzeugs
    delete testButton->getGraphic();
    delete testButton->getGraphicPressed();
	delete testButton;
	testButton = nullptr;
    
    delete testPushButton->getGraphic();
    delete testPushButton->getGraphicPressed();
	delete testPushButton;
	testPushButton = nullptr;
}

void GuiMgr::render(SDL_Renderer* renderer) {
    // TODO alle GUI-Element durchiterieren
    
	panel->render(renderer);
    statusBar->render(renderer);
    musicPushButton->render(renderer);
    
    panelSwitchPushButtonBuild->render(renderer);
    panelSwitchPushButton2->render(renderer);
    panelSwitchPushButton3->render(renderer);
    panelSwitchPushButton4->render(renderer);
    
    // Testzeugs
    testButton->render(renderer);
    testPushButton->render(renderer);
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
        // Rechtsklick deselektiert auf der Karte
        else if (event.button.button == SDL_BUTTON_RIGHT) {
            map->setSelectedMapObject(nullptr);
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
    
    // TODO alle GUI-Element durchiterieren
    panel->onEvent(event);
    statusBar->onEvent(event);
    musicPushButton->onEvent(event);
    
    panelSwitchPushButtonBuild->onEvent(event);
    panelSwitchPushButton2->onEvent(event);
    panelSwitchPushButton3->onEvent(event);
    panelSwitchPushButton4->onEvent(event);
    
    // Testzeugs
    testButton->onEvent(event);
    testPushButton->onEvent(event);
}