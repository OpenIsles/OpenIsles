#include "Context.h"
#include "defines.h"
#include "map/Structure.h"
#include "gui/Identifiers.h"
#include "gui/components/GuiPushButton.h"
#include "gui/components/GuiPanelHeader.h"
#include "gui/panel-widgets/GuiOptionsMenuWidget.h"
#include "sound/sdl/SDLSoundMgr.h"


GuiOptionsMenuWidget::GuiOptionsMenuWidget(const Context& context) : GuiPanelWidget(context) {
    // TODO höchstwahrscheinlich sind die GUI-Komponenten lost-memory, da sie am Ende nicht zerstört werden
    // Idee untersuchen, dass ~GuiMgr() von jeder Komponente rekursiv die Kinder beseitigt.

    const char* statusBarTextTurnMusicOn = _("Turn music on");
    const char* statusBarTextTurnMusicOff = _("Turn music off");

    GuiPanelHeader* panelHeader = new GuiPanelHeader(context);
    panelHeader->setPosition(0, 0);
    panelHeader->setText(_("Options"));
    panelHeader->setIconGraphic(nullptr);
    addChildElement(panelHeader);

    GuiPushButton* musicPushButton = new GuiPushButton(context);
    musicPushButton->setGraphic(context.graphicsMgr->getGraphicSet("button-music")->getStatic()->getGraphic());
    musicPushButton->setGraphicPressed(context.graphicsMgr->getGraphicSet("button-music-pressed")->getStatic()->getGraphic());
    musicPushButton->setCoords(7, 345, 64, 64);
    musicPushButton->setStatusBarText(statusBarTextTurnMusicOn);
    musicPushButton->setOnClickFunction([musicPushButton, context, statusBarTextTurnMusicOff, statusBarTextTurnMusicOn]() {
        bool musicEnabled = musicPushButton->isActive();

        if (musicEnabled) {
            context.soundMgr->enableMusic();
            musicPushButton->setStatusBarText(statusBarTextTurnMusicOff);
        } else {
            context.soundMgr->disableMusic();
            musicPushButton->setStatusBarText(statusBarTextTurnMusicOn);
        }
    });
    context.guiMgr->registerElement(GUI_ID_MUSIC_PUSH_BUTTON, musicPushButton);
    addChildElement(musicPushButton);
}

void GuiOptionsMenuWidget::renderElement(IRenderer* renderer) {
}
