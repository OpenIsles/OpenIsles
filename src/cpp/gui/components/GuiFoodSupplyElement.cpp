#include "graphics/mgr/IFontMgr.h"
#include "gui/components/GuiFoodSupplyElement.h"
#include "utils/Color.h"
#include "utils/Rect.h"

static Color colorBlack = Color(0, 0, 0, 255);
static Color colorLightBrown = Color(223, 216, 183, 255);
static Color colorRed = Color(255, 0, 0, 255);


GuiFoodSupplyElement::GuiFoodSupplyElement(const Context* const context) : GuiStaticTextElement(context) {
    // Größe ist fix
    width = 210;
    height = 20;

    setText("Nahrung");
    setColor(&colorLightBrown);
    setShadowColor(&colorBlack);
    setFontName("DroidSans-Bold.ttf");
    setFontSize(14);
    setAlign(RENDERTEXT_HALIGN_LEFT | RENDERTEXT_VALIGN_MIDDLE);
    setStatusBarText("Nahrungsversorgung (Status für alle Gruppen gleich)");
}

GuiFoodSupplyElement::~GuiFoodSupplyElement() {
}

void GuiFoodSupplyElement::renderElement(IRenderer* renderer) {
    GuiStaticTextElement::renderElement(renderer);

    int windowX, windowY;
    getWindowCoords(windowX, windowY);

    // Balken
    const Rect foodSupplyRect(windowX + 100, windowY, 110, 20);
    renderer->setDrawColor(colorBlack);
    renderer->drawRect(foodSupplyRect);

    Rect foodSupplyFilledRect(windowX + 101, windowY + 1, int(108.0 * foodSupply), 18);
    renderer->setDrawColor(colorRed);
    renderer->fillRect(foodSupplyFilledRect);

    renderer->setDrawColor(colorBlack);
    int x = windowX + 175;
    renderer->drawLine(x, windowY + 1, x, windowY + 19);

    // TODO ggf. später mit Grafiken machen. Anno hat 2x2 Grafiken: das Rechteck und den Separator, je einmal horizontal und vertikal
}
