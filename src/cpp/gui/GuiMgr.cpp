#include "GuiMgr.h"
#include "GuiStaticElement.h"
#include "Graphic.h"
#include "GuiButton.h"

GuiMgr::GuiMgr() {
	panel = new GuiStaticElement();
    panel->setWindowCoords(768, 0);
    panel->setGraphic(new Graphic("data/img/gui/panel.png"));
    
	statusBar = new GuiStaticElement();
    statusBar->setWindowCoords(0, 734);
    statusBar->setGraphic(new Graphic("data/img/gui/statusbar.png"));
    
    testButton = new GuiButton();
    testButton->setWindowCoords(795, 25);
    testButton->setGraphic(new Graphic("data/img/gui/testbutton.png"));
    testButton->setGraphicPressed(new Graphic("data/img/gui/testbutton-pressed.png"));
}

GuiMgr::~GuiMgr() {
    delete panel->getGraphic();
	delete panel;
	panel = nullptr;

    delete statusBar->getGraphic();
	delete statusBar;
	statusBar = nullptr;
    
    delete testButton->getGraphic();
    delete testButton->getGraphicPressed();
	delete testButton;
	testButton = nullptr;
}

void GuiMgr::render(SDL_Renderer* renderer) {
	panel->render(renderer);
    statusBar->render(renderer);
    testButton->render(renderer);
}

void GuiMgr::onEvent(SDL_Event& event) {
    panel->onEvent(event);
    statusBar->onEvent(event);
    testButton->onEvent(event);
}