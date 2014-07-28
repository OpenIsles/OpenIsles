#include "Gui.h"

Gui::Gui() {
	panel = new Graphic("data/img/gui/panel.png");
	statusBar = new Graphic("data/img/gui/statusbar.png");
}

Gui::~Gui() {
	delete panel;
	panel = nullptr;

	delete statusBar;
	statusBar = nullptr;
}

void Gui::renderGui(SDL_Renderer* renderer) {
	// TODO GUI-Positionen zentral über Konfiguration steuern. (siehe Map.cpp#mapClipRect)

	SDL_Rect rectDestination = { 768, 0, panel->getWidth(), panel->getHeight() };
	SDL_RenderCopy(renderer, panel->getTexture(), nullptr, &rectDestination);

	rectDestination = { 0, 734, statusBar->getWidth(), statusBar->getHeight() };
	SDL_RenderCopy(renderer, statusBar->getTexture(), nullptr, &rectDestination);
}
