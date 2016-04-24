#include "global.h"
#include "graphics/mgr/IFontMgr.h"
#include "gui/components/GuiFoodSupplyElement.h"
#include "utils/Color.h"
#include "utils/Rect.h"


GuiFoodSupplyElement::GuiFoodSupplyElement(const Context& context) : GuiStaticTextElement(context) {
    // Größe ist fix
    width = 210;
    height = 20;

    setText(_("Food"));
    setColor(&Color::lightBrown);
    setShadowColor(&Color::black);
    setFontName("DroidSans-Bold.ttf");
    setFontSize(14);
    setAlign(RENDERTEXT_HALIGN_LEFT | RENDERTEXT_VALIGN_MIDDLE);
    setStatusBarText(_("Food supply (State equal over all tiers)"));
}

GuiFoodSupplyElement::~GuiFoodSupplyElement() {
}

void GuiFoodSupplyElement::renderElement(IRenderer* renderer) {
    GuiStaticTextElement::renderElement(renderer);

    int windowX, windowY;
    getWindowCoords(windowX, windowY);

    // Balken
    const Rect foodSupplyRect(windowX + 100, windowY, 110, 20);
    renderer->setDrawColor(Color::black);
    renderer->drawRect(foodSupplyRect);

    Rect foodSupplyFilledRect(windowX + 101, windowY + 1, int(108.0 * foodSupply), 18);
    renderer->setDrawColor(Color::red);
    renderer->fillRect(foodSupplyFilledRect);

    renderer->setDrawColor(Color::black);
    int x = windowX + 175;
    renderer->drawLine(x, windowY + 1, x, windowY + 19);

    // TODO ggf. später mit Grafiken machen. Anno hat 2x2 Grafiken: das Rechteck und den Separator, je einmal horizontal und vertikal
}
