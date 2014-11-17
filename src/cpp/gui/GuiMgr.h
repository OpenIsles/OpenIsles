#ifndef _GUI_MGR_H
#define _GUI_MGR_H

#include <map>
#include "SDL.h"

class GuiBase;
class GuiButton;
class GuiStaticElement;
class GuiPushButton;
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
    /**
     * @brief ausgewählter Button (4 Buttons, die die Panels umschalten)
     */
    PanelButton selectedPanelButton;

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

} PanelState;


/**
 * GUI-Manager. Verwaltet alle GUI-Elemente.
 */
class GuiMgr {

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
    
    void getStartClickCoords(int& startClickX, int& startClickY) {
        startClickX = this->startClickX;
        startClickY = this->startClickY;
    }

	/**
	 * @brief Zeichnet die Benutzeroberfläche
	 */
	void render(SDL_Renderer* renderer);
    
    /**
     * Callback, der ein Event handelt
     * @param event SDL-Event
     */
    void onEvent(SDL_Event& event);

    /**
     * @brief Liefert den aktuellen Zustand des Panels zurück
     * @return Zustand des Panels
     */
    PanelState const& getPanelState() const {
        return panelState;
    }

private:
    /**
     * @brief Initialisiert die GUI für den ersten Tab ("Gebäude bauen") im Panel. Wird vom Konstruktor aufgerufen.
     */
    void initGuiForPanelAddBuilding();

    /**
     * @brief Initialisiert die GUI für den dritten Tab ("Info") im Panel. Wird vom Konstruktor aufgerufen.
     */
    void initGuiForPanelInfo();

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
