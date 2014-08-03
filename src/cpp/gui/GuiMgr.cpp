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
    
    // Testzeugs
    graphic = new Graphic("data/img/gui/testbutton.png");
    testButton = new GuiButton();
    testButton->setWindowCoords(795, 235, graphic->getWidth(), graphic->getHeight());
    testButton->setGraphic(graphic);
    testButton->setGraphicPressed(new Graphic("data/img/gui/testbutton-pressed.png"));
    testButton->setOnClickFunction([]() {
        std::cout << "Click" << std::endl;
    });
    
    graphic = new Graphic("data/img/gui/testbutton.png");
    testPushButton = new GuiPushButton();
    testPushButton->setWindowCoords(875, 235, graphic->getWidth(), graphic->getHeight());
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
    
    // Testzeugs
    testButton->onEvent(event);
    testPushButton->onEvent(event);
}