#include <cassert>
#include "config/ConfigMgr.h"
#include "game/Game.h"
#include "graphics/mgr/IFontMgr.h"
#include "gui/panel-widgets/GuiSelectedHouseBuildingWidget.h"
#include "map/Map.h"
#include "utils/StringFormat.h"

static Color colorWhite = Color(255, 255, 255, 255);
static Color colorBlack = Color(0, 0, 0, 255);

static struct {
    const char* filename;
    std::string statusBarText;
} populationSatisfactionGui[4] = {
    { "good.png"   , "Zuwächse in dieser Bevölkerungsgruppe" },
    { "neutral.png", "Keine Zuwächse in dieser Bevölkerungsgruppe" },
    { "bad.png"    , "Leichte Rückgänge in dieser Bevölkerungsgruppe" },
    { "worst.png"  , "Starke Rückgänge in dieser Bevölkerungsgruppe" }
};


GuiSelectedHouseBuildingWidget::GuiSelectedHouseBuildingWidget(const Context* const context) :
    GuiPanelWidget(context), populationSatisfaction(context), inhabitants(context), populationTier(context) {

    // Grafiken laden
    IRenderer* const renderer = context->graphicsMgr->getRenderer();
    for (int i = 0; i < 4; i++) {
        std::string filename =
            std::string("data/img/gui/population-satisfaction/") + populationSatisfactionGui[i].filename;
        populationSatisfactionGraphics[i] = new SDLGraphic(renderer, filename.c_str());
    }

    // Erst die Grafik...
    populationSatisfaction.setCoords(35, 45, 91, 102);
    addChildElement(&populationSatisfaction);

    // ...dann die Texte drüberliegen
    inhabitants.setCoords(0, 85, 50, 14);
    inhabitants.setColor(&colorWhite);
    inhabitants.setShadowColor(&colorBlack);
    inhabitants.setFontName("DroidSans-Bold.ttf");
    inhabitants.setFontSize(14);
    inhabitants.setAlign(RENDERTEXT_HALIGN_RIGHT | RENDERTEXT_VALIGN_MIDDLE);
    inhabitants.setStatusBarText("Anzahl dieser Bevölkerungsgruppe");
    addChildElement(&inhabitants);

    populationTier.setCoords(115, 85, 120, 14);
    populationTier.setColor(&colorWhite);
    populationTier.setShadowColor(&colorBlack);
    populationTier.setFontName("DroidSans-Bold.ttf");
    populationTier.setFontSize(14);
    populationTier.setAlign(RENDERTEXT_HALIGN_LEFT | RENDERTEXT_VALIGN_MIDDLE);
    populationTier.setStatusBarText("Bezeichnung für diese Bevölkerung");
    addChildElement(&populationTier);
}

GuiSelectedHouseBuildingWidget::~GuiSelectedHouseBuildingWidget() {
    for (int i = 0; i < 4; i++) {
        delete populationSatisfactionGraphics[i];
    }
}

void GuiSelectedHouseBuildingWidget::renderElement(IRenderer* renderer) {
    const Building* selectedBuilding =
        reinterpret_cast<const Building*>(context->game->getMap()->getSelectedMapObject());
    assert((selectedBuilding != nullptr) && (selectedBuilding->isHouse()));

    int windowX, windowY;
    getWindowCoords(windowX, windowY);

    const Colony* colony = selectedBuilding->getColony();

    // Name der Siedlung
    context->guiMgr->drawPanelHeader(windowX, windowY, colony->name, nullptr);
}

void GuiSelectedHouseBuildingWidget::onSelectedMapBuildingChanged(const Building* newSelectedBuilding) {
    assert((newSelectedBuilding != nullptr) && (newSelectedBuilding->isHouse()));

    int populationSatisfactionIndex = 0;
    populationSatisfaction.setGraphic(populationSatisfactionGraphics[populationSatisfactionIndex]);
    populationSatisfaction.setStatusBarText(populationSatisfactionGui[populationSatisfactionIndex].statusBarText);

    inhabitants.setText(toString(4711));
    populationTier.setText("Foobaranten");

    // TODO echte Daten rein
}