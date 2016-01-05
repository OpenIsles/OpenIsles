#ifndef _GUI_ICON_WITH_STRING_ELEMENT_H
#define _GUI_ICON_WITH_STRING_ELEMENT_H

#include <string>
#include "graphics/graphic/IGraphic.h"
#include "graphics/renderer/IRenderer.h"
#include "gui/components/GuiStaticGraphicElement.h"
#include "Context.h"


/**
 * @brief Hilfs-Komponente, die ein Symbol und einen String daneben anzeigt.
 * Wir verwenden sie für die Baumaterialen, Münzkosten und Betriebskosten
 *
 * Als Symbol wird GuiStaticGraphicElement#graphic verwendet.
 */
class GuiIconWithStringElement : public GuiStaticGraphicElement {

private:
    /**
     * @brief String, der ausgegeben werden soll
     */
    std::string string;

public:
    GuiIconWithStringElement(const Context& context) : GuiStaticGraphicElement(context) {
    }
    virtual ~GuiIconWithStringElement() override {}

    void setString(const std::string& string) {
        this->string = string;
    }

    virtual void renderElement(IRenderer* renderer) override;
};

#endif