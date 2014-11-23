#ifndef _GUI_MGR_H
#define _GUI_MGR_H

#include <SDL.h>
#include <map>
#include "gui/panel-widgets/GuiBuildMenuWidget.h"

class GuiBase;
class GuiButton;
class GuiStaticElement;
class GuiPushButton;
class GuiPanelWidget;
class MapObject;
class SDL_Renderer;

enum BuildingGroup : unsigned char;
enum StructureType : unsigned char;


/**
* Enum für 4 Buttons, um die Panels umzuschalten
*/
typedef
enum PanelButton : unsigned char {

    /**
     * @brief Baumenü
     */
    ADD_BUILDING = 0,

    /**
     * @brief Kampfmenü
     */
    MILITARY = 1,

    /**
     * @brief Info-Panel
     */
    INFO = 2,

    /**
     * @brief Einstellungs-Panel
     */
    OPTIONS = 3

} PanelButton;


/**
 * @brief Struktur für den Zustand des Panels auf der rechten Seite
 */
typedef
struct PanelState {
    // 4 Buttons oben /////////////////////////////////////////////////////////////////////////////

    /**
     * @brief ausgewählter Button (4 Buttons, die die Panels umschalten)
     */
    PanelButton selectedPanelButton;

    // Panel Baumenü //////////////////////////////////////////////////////////////////////////////

    /**
     * @brief zuletzt/aktuell gewählte Kategorie im Baumenü
     */
    BuildingGroup selectedBuildingGroup;

    /**
     * @brief zuletzt/aktuell gewählte Struktur im Baumenü
     */
    StructureType addingStructure;

    /**
     * @brief Flag, was angibt, ob das Popup-Menü mit den einzelnen Gebäuden aufgeklappt ist
     */
    bool buildingMenuOpen;

    // gezeigtes Panel ////////////////////////////////////////////////////////////////////////////

    /**
     * @brief Aktives GUI-Panel-Widget, was gerade gezeigt wird. Es kann immer nur maximal ein solches Widget
     * sichtbar sein. Ist gar nix gezeigt, so kann hier `nullptr` gesetzt werden.
     */
    GuiPanelWidget* activeGuiPanelWidget;

} PanelState;


/**
 * GUI-Manager. Verwaltet alle GUI-Elemente.
 */
class GuiMgr {

    // Die Widgets dürfen den inneren Zustand des GuiMgrs manipulieren
    friend class GuiBuildMenuWidget;
    friend class GuiOptionsMenuWidget;

private:
	/**
     * @brief Fenster-Koordinaten, an dem der letzte Click gestartet wurde.
     * Gültig, bis die Maustaste losgelassen wurde.
     */
    int startClickX, startClickY;
    
    /**
     * @brief Map, die alle registrierten Gui-Elemente enthält und durch ihre Identifier referenziert.
     */
    std::map<int, GuiBase*> identifierMap;

    /**
     * @brief Zustand des Panels auf der rechten Seite
     */
    PanelState panelState;

public:
	GuiMgr();
	~GuiMgr();

    /**
     * @brief Legt alle GUI-Elemente an. Diese Methode muss direkt nach dem Konstrutor aufgerufen werden.
     * Der Konstruktor selber kann dies nicht erledigt, da die erzeugten GUI-Komponenten auf den guiMgr zugreifen.
     * D.h. der muss zu diesem Zeitpunkt bereits gesetzt sein.
     */
    void initGui();
    
    void getStartClickCoords(int& startClickX, int& startClickY) {
        startClickX = this->startClickX;
        startClickY = this->startClickY;
    }

	/**
	 * @brief Zeichnet die Benutzeroberfläche
	 */
	void render(SDL_Renderer* renderer);
    
    /**
     * @brief Callback, der ein Event handelt
     * @param event SDL-Event
     */
    void onEvent(SDL_Event& event);

    /**
     * @brief Callback, der sich drum kümmert, wenn auf der Karte ein anderes MapObject ausgewählt wurde.
     * @param newSelectedMapObject das neu gewählte MapObject oder `nullptr`, wenn abgewählt wurde
     */
    void onSelectedMapObjectChanged(MapObject* newSelectedMapObject);

    /**
     * @brief Liefert den aktuellen Zustand des Panels zurück
     * @return Zustand des Panels
     */
    PanelState const& getPanelState() const {
        return panelState;
    }

private:
    /**
     * @brief Legt alle Panel-Widgets an
     */
    void initPanelWidgets();

    /**
     * @brief Registriert ein neues GUI-Element
     * @param identifier eindeutiger Identifier, um das Element mit diesem anzusprechen
     * @param guiElement Referenz zum GUI-Element
     */
    void registerElement(int identifier, GuiBase* guiElement);
    
    /**
     * @brief Liefert ein bestimmtes GUI-Element zurück.
     * @param identifier Identifier des Elements
     * @return Referenz zum GUI-Element
     */
    GuiBase* findElement(int identifier);

    /**
     * @brief Aktualisiert die Sichtbarkeit der GUI-Komponenten des Panels ausgehend vom Stand der aktuellen
     * Variablen #panelState.
     */
    void updateGuiFromPanelState();
};

#endif
