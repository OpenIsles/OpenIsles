#include <SDL.h>
#include "graphics/mgr/IFontMgr.h"
#include "gui/components/GuiLuaConsole.h"
#include "utils/Color.h"
#include "utils/Lua.h"

static Color colorBackground = Color(0, 0, 0, 128);
static Color colorHistoryCommand = Color(128, 128, 128, 255);
static Color colorCurrentCommand = Color(192, 192, 192, 255);

// so ausrichten, dass es unten an den FPS-Counter anschließt
static constexpr int yBottom = 680;

static constexpr int maxHistorySize = 10;
static constexpr int yHeight = (29 + maxHistorySize * 16);


GuiLuaConsole::GuiLuaConsole(const Context& context) : GuiBase(context) {
    lua = createNewLuaState(context);
    setCoords(10, yBottom - yHeight, 740, yHeight);
}

GuiLuaConsole::~GuiLuaConsole() {
    lua_close(lua);
}

void GuiLuaConsole::renderElement(IRenderer* renderer) {
    int windowX, windowY;
    getWindowCoords(windowX, windowY);

    // Hintergrund
    renderer->setDrawColor(colorBackground);
    renderer->fillRect({windowX, windowY, width, height});

    // History
    int y = yBottom - 45;
    for (const std::string& command : history) {
        context.fontMgr->renderText(renderer, command, 15, y,
                                &colorHistoryCommand, nullptr, "DroidSans.ttf", 13, RENDERTEXT_HALIGN_LEFT);
        y -= 16;
    }

    // aktueller Befehl
    renderer->setDrawColor(colorCurrentCommand);
    renderer->fillRect({windowX + 5, yBottom - 25, width - 10, 1});

    context.fontMgr->renderText(renderer, currentInput + "_", 15, yBottom - 20,
                                &colorCurrentCommand, nullptr, "DroidSans.ttf", 14, RENDERTEXT_HALIGN_LEFT);
}

bool GuiLuaConsole::onEventElement(SDL_Event& event) {
    // Uns interessieren nur Tastendrücke
    if (event.type != SDL_KEYDOWN && event.type != SDL_TEXTINPUT) {
        return true;
    }

    // Enter aktiviert die Konsole bzw. führt den Befehl aus.
    if (event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_RETURN) {
        if (visible) {
            executeCommand();
        } else {
            setVisible(true);
            SDL_StartTextInput();
        }
        return false;
    }

    // Andere Tasten gehen nur an die Konsole, wenn sie sichtbar ist
    if (!visible) {
        return true;
    }

    // Backspace
    if (event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_BACKSPACE) {
        if (!currentInput.empty()) {
            currentInput.pop_back();
        }
        return false;
    }

    // Text zur Zeile hinzufügen
    if (event.type == SDL_TEXTINPUT) {
        currentInput.append(event.text.text);
        return false;
    }

    // Alle Events als erledigt markieren, auch wenn wir nichts gemacht haben. Das ist wichtig, weil wir
    // z.B. für ein "i" ein SDL_TEXTINPUT-Event verarbeiten. Das zugehörige SDL_KEYDOWN-Event nutzen wir nicht.
    // Würden wir es aber nicht als erledigt markieren, so würde der GuiMgr es verwenden, um das Panel in
    // die Info-Anzeige zu schalten. Das soll nicht passieren, während die Konsole offen ist.
    return false;
}

void GuiLuaConsole::executeCommand() {
    // Leerer String? Versteckt die Konsole wieder
    if (currentInput.empty()) {
        setVisible(false);
        SDL_StopTextInput();
        return;
    }

    // Befehl ausführen
    luaL_dostring(lua, currentInput.c_str());

    // In die History aufnehmen
    history.push_front(currentInput);
    if (history.size() > maxHistorySize) {
        history.pop_back();
    }

    currentInput.clear();
}
