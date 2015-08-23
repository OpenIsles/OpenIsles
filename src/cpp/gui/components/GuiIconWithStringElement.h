#ifndef _GUI_ICON_WITH_STRING_ELEMENT_H
#define _GUI_ICON_WITH_STRING_ELEMENT_H

#include <string>
#include "graphics/graphic/IGraphic.h"
#include "graphics/renderer/IRenderer.h"
#include "gui/components/GuiStaticElement.h"
#include "Context.h"


/**
 * @brief Hilfs-Komponente, die ein Symbol und einen String daneben anzeigt.
 * Wir verwenden sie für die Baumaterialen, Münzkosten und (TODO) Betriebskosten
 *
 * Als Symbol wird GuiStaticElement#graphic verwendet.
 */
class GuiIconWithStringElement : public GuiStaticElement {

private:
    /**
     * @brief String, der ausgegeben werden soll
     */
    std::string string;

public:
    GuiIconWithStringElement(const Context* const context) : GuiStaticElement(context) {
    }
    virtual ~GuiIconWithStringElement() override {}

    void setString(const std::string& string) {
        this->string = string;
    }

    virtual void renderElement(IRenderer* renderer) override;
};

#endif