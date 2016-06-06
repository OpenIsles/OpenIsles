#ifndef _GRAPHIC_SET_KEY_STATE_H
#define _GRAPHIC_SET_KEY_STATE_H

enum GraphicSetKeyState : unsigned char {

    NONE,               ///< kein Zustand (z.&nbsp;B. für Gebäude ohne speziellen Zustand)
    WORKING,            ///< Gebäude arbeitet

    INVENTORY0,         ///< Zustand für verschieden hohe Lagerbestände innerhalb des Gebäude (Steinmetz, Förster)
    INVENTORY1,         ///< Zustand für verschieden hohe Lagerbestände innerhalb des Gebäude (Steinmetz, Förster)
    INVENTORY2,         ///< Zustand für verschieden hohe Lagerbestände innerhalb des Gebäude (Steinmetz, Förster)
    INVENTORY3,         ///< Zustand für verschieden hohe Lagerbestände innerhalb des Gebäude (Steinmetz, Förster)

    GROWTH0,            ///< Zustand für verschiedene Wachstumszustände für Harvestables
    GROWTH1,            ///< Zustand für verschiedene Wachstumszustände für Harvestables
    GROWTH2,            ///< Zustand für verschiedene Wachstumszustände für Harvestables
    GROWTH3,            ///< Zustand für verschiedene Wachstumszustände für Harvestables
    GROWTH4,            ///< Zustand für verschiedene Wachstumszustände für Harvestables
    GROWTH5,            ///< Zustand für verschiedene Wachstumszustände für Harvestables
    GROWTH6,            ///< Zustand für verschiedene Wachstumszustände für Harvestables

    WALKING,            ///< Träger, der läuft
    EATING,             ///< Träger, der isst (also Schafe/Rinder),
    WALKING_WITH_GOODS, ///< wie WALKING, allerdings ist der Träger beladen
    EATING_WITH_GOODS,  ///< wie EATING, allerdings ist der Träger beladen

    STRAIGHT0,          ///< Zustand für Straßen
    STRAIGHT90,         ///< Zustand für Straßen
    CURVE0,             ///< Zustand für Straßen
    CURVE90,            ///< Zustand für Straßen
    CURVE180,           ///< Zustand für Straßen
    CURVE270,           ///< Zustand für Straßen
    TEE0,               ///< Zustand für Straßen
    TEE90,              ///< Zustand für Straßen
    TEE180,             ///< Zustand für Straßen
    TEE270,             ///< Zustand für Straßen
    CROSS,              ///< Zustand für Straßen

    LEFT,               ///< Zustand für das GuiMapRotateWidget
    RIGHT,              ///< Zustand für das GuiMapRotateWidget
    LEFT_PRESSED,       ///< Zustand für das GuiMapRotateWidget
    RIGHT_PRESSED,      ///< Zustand für das GuiMapRotateWidget

    PLUS,               ///< Zustand für das GuiMapZoomWidget
    MINUS,              ///< Zustand für das GuiMapZoomWidget
    PLUS_PRESSED,       ///< Zustand für das GuiMapZoomWidget
    MINUS_PRESSED,      ///< Zustand für das GuiMapZoomWidget

};

#endif
