#include "map/Structure.h"
#include "gui/GuiMgr.h"
#include "gui/Identifiers.h"
#include "gui/components/GuiPushButton.h"
#include "gui/panel-widgets/GuiOptionsMenuWidget.h"
#include "sound/SoundMgr.h"


// Aus main.cpp importiert
extern GraphicsMgr* graphicsMgr;
extern GuiMgr* guiMgr;
extern SoundMgr* soundMgr;


GuiOptionsMenuWidget::GuiOptionsMenuWidget() {
    musicPushButton = new GuiPushButton();
    musicPushButton->setGraphic(new PlainGraphic("data/img/gui/button-music.png"));
    musicPushButton->setGraphicPressed(new PlainGraphic("data/img/gui/button-music-pressed.png"));
    musicPushButton->setCoords(7, 378, 64, 64);
    musicPushButton->setOnClickFunction([this]() {
        bool musicEnabled = ((GuiPushButton*) guiMgr->findElement(GUI_ID_MUSIC_PUSH_BUTTON))->isActive();

        if (musicEnabled) {
            soundMgr->enableMusic();
        } else {
            soundMgr->disableMusic();
        }
    });
    guiMgr->registerElement(GUI_ID_MUSIC_PUSH_BUTTON, musicPushButton);
    addChildElement(musicPushButton);
}

GuiOptionsMenuWidget::~GuiOptionsMenuWidget() {
    // TODO Die Grafiken könnte man alle schön in den GraphicsMgr packen, dann muss man hier den Aufwand nicht betreiben
    delete musicPushButton->getGraphic();
    delete musicPushButton->getGraphicPressed();
}