#include <cassert>
#include <string>
#include "global.h"
#include "config/ConfigMgr.h"
#include "game/Game.h"
#include "graphics/mgr/IFontMgr.h"
#include "gui/panel-widgets/GuiSelectedHouseBuildingWidget.h"
#include "map/Map.h"
#include "utils/StringFormat.h"

static Color colorWhite = Color(255, 255, 255, 255);
static Color colorBlack = Color(0, 0, 0, 255);

static struct {
    const char* graphicSetName;
    const char* statusBarText;
} populationSatisfactionGui[4] = {
    { "population-satisfaction/good"   , _NOOP("Population growth in this population tier") },
    { "population-satisfaction/neutral", _NOOP("No population growth in this population tier") },
    { "population-satisfaction/bad"    , _NOOP("Slight population loss in this population tier") },
    { "population-satisfaction/worst"  , _NOOP("Severe population loss in this population tier") }
};


GuiSelectedHouseBuildingWidget::GuiSelectedHouseBuildingWidget(const Context& context) :
    GuiSelectedBuildingWidget(context), populationSatisfaction(context), inhabitants(context), populationTier(context),
    foodSupplyElement(context) {

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
    inhabitants.setStatusBarText(_("Population in this population tier"));
    addChildElement(&inhabitants);

    populationTier.setCoords(115, 85, 120, 14);
    populationTier.setColor(&colorWhite);
    populationTier.setShadowColor(&colorBlack);
    populationTier.setFontName("DroidSans-Bold.ttf");
    populationTier.setFontSize(14);
    populationTier.setAlign(RENDERTEXT_HALIGN_LEFT | RENDERTEXT_VALIGN_MIDDLE);
    populationTier.setStatusBarText(_("Population tier's name"));
    addChildElement(&populationTier);

    // Nahrungsversorgung
    foodSupplyElement.setPosition(15, 370);
    addChildElement(&foodSupplyElement);
}

GuiSelectedHouseBuildingWidget::~GuiSelectedHouseBuildingWidget() {
}

void GuiSelectedHouseBuildingWidget::renderElement(IRenderer* renderer) {
}

void GuiSelectedHouseBuildingWidget::onSelectedMapBuildingChanged(const Building* newSelectedBuilding) {
    assert(newSelectedBuilding->isHouse());

    GuiSelectedBuildingWidget::onSelectedMapBuildingChanged(newSelectedBuilding);
    
    const Colony* colony = newSelectedBuilding->getColony();
    const MapObjectType* mapObjectType = newSelectedBuilding->getMapObjectType();
    const ColonyPopulationTier& colonyPopulationTier = colony->populationTiers.at(mapObjectType->populationTier);

    // Zufriedenheit
    unsigned char populationSatisfactionIndex = colonyPopulationTier.populationSatisfaction;
    const char* graphicSetName = populationSatisfactionGui[populationSatisfactionIndex].graphicSetName;
    const IGraphic* graphic = context.graphicsMgr->getGraphicSet(graphicSetName)->getStatic()->getGraphic();
    populationSatisfaction.setGraphic(graphic);
    populationSatisfaction.setStatusBarText(_(populationSatisfactionGui[populationSatisfactionIndex].statusBarText));

    // Bevölkerungsdaten
    inhabitants.setText(toString(colonyPopulationTier.population));

    std::string populationTierTitle = _N(mapObjectType->populationTier->getTitleMsgid(false),
                                         mapObjectType->populationTier->getTitleMsgid(true),
                                         colonyPopulationTier.population);
#ifdef DEBUG
    // Im Debug-Modus zeigen wir zusätzlich noch die Einwohnerzahl im Gebäude selber an
    std::string debugTitle = "(" + toString(newSelectedBuilding->inhabitants) + ") " + populationTierTitle;
    populationTier.setText(debugTitle);
#else
    populationTier.setText(populationTierTitle);
#endif

    // TODO Steuersatz

    // verlangte Güter
    // TODO untersuchen, ob es nicht performanter ist, immer 6(? aus der Config ausrechnen) Elemente anzulegen und ein-/auszublenden bzw. abzuändern
    for (GuiGoodElement* goodElement : needsGoods) {
        removeChildElement(goodElement);
        delete goodElement;
    }
    needsGoods.clear();

    // TODO wirklich nur anzeigen, wenn Bedarf nicht erfüllt wird
    // TOOD teilweise-gedeckten Bedarf verarbeiten
    int posIndex = 0;
    for (const NeededGood& neededGood : mapObjectType->populationTier->needsGoods) {
        GuiGoodElement* goodElement = new GuiGoodElement(context);

        std::string neededGoodText = string_sprintf(
            _("The demand for %s is not being fulfilled"), _(neededGood.good->getTitleMsgid()).c_str());

        goodElement->setPosition(25 + (posIndex % 3) * 60, 220 + (posIndex / 3) * 60);
        goodElement->setStatusBarText(neededGoodText);
        goodElement->setGood(neededGood.good);
        goodElement->setDisplayValue(false);
        goodElement->setDisplayBar(false);

        needsGoods.push_back(goodElement);
        addChildElement(goodElement);

        posIndex++;
    }

    // TODO verlangte öffentliche Gebäude

    // Nahrungsversorgung
    double foodSupply = 1.0; // TODO im Moment können wir den Fall "verhungern" noch nicht handlen, drum is die Versorgung immer 100%
    foodSupplyElement.setFoodSupply(foodSupply);
}