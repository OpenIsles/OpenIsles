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
    GuiPushButton* musicPushButton = new GuiPushButton();
    musicPushButton->setGraphic(graphicsMgr->getOtherGraphic(OtherGraphic::MUSIC));
    musicPushButton->setGraphicPressed(graphicsMgr->getOtherGraphic(OtherGraphic::MUSIC_PRESSED));
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