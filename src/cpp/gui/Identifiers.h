/**
 * In dieser Datei sind alle guiIdentifiers gelistet
 *
 * Wichtig:
 * Da guiMgr::identifierMap eine std::map ist, bedeutet das, wir iterieren von niedriger zu hoher ID.
 * Niedrige IDs werden zuerst gezeichnet und bekommen Events zuletzt. Wir müssen also hier die Reihenfolge
 * korrekt setzen.
 */
#ifndef _GUI_IDENTIFIERS_H
#define _GUI_IDENTIFIERS_H


#define GUI_ID_MAP 1
#define GUI_ID_PANEL 2
#define GUI_ID_MINIMAP 3
#define GUI_ID_STATUS_BAR 4
#define GUI_ID_RESOURCES_BAR 5
#define GUI_ID_MAP_ROTATE_WIDGET 6
#define GUI_ID_MAP_ZOOM_WIDGET 7

// Gebäude bauen ///////////////////////////////////////////////////////////////////////////////////////////////////////

// Button pro Gruppe (+ Offset)
#define GUI_ID_ADD_BUILDING_PUSH_BUTTON_BASE 110

#define GUI_ID_PANEL_SWITCH_PUSH_BUTTON_BASE 201
#define GUI_ID_PANEL_SWITCH_PUSH_BUTTON_BUILD GUI_ID_PANEL_SWITCH_PUSH_BUTTON_BASE
#define GUI_ID_PANEL_SWITCH_PUSH_BUTTON_2 (GUI_ID_PANEL_SWITCH_PUSH_BUTTON_BASE+1)
#define GUI_ID_PANEL_SWITCH_PUSH_BUTTON_3 (GUI_ID_PANEL_SWITCH_PUSH_BUTTON_BASE+2)
#define GUI_ID_PANEL_SWITCH_PUSH_BUTTON_4 (GUI_ID_PANEL_SWITCH_PUSH_BUTTON_BASE+3)
#define GUI_ID_DEMOLISH_PUSH_BUTTON 210

#define GUI_ID_BUILD_MENU 220

// Weitere Elemente
#define GUI_ID_ADD_BUILDING_WIDGET 300

// Panel-Widgets
#define GUI_ID_PANEL_WIDGET_START 401                            // Marker für das erste Panel-Widget

#define GUI_ID_SELECTED_PRODUCTION_BUILDING_PANEL_WIDGET 401     // ausgewähltes Produktionsgebäude (Infos über
                                                                 // Produktion und intere Lagerbestände des Gebäudes)
#define GUI_ID_SELECTED_HOUSE_BUILDING_PANEL_WIDGET 402          // ausgewähltes Haus
#define GUI_ID_SELECTED_PUBLIC_BUILDING_PANEL_WIDGET 403         // ausgewähltes öffentliches Gebäude
#define GUI_ID_COLONY_GOODS_PANEL_WIDGET 404                     // Kolonie-Warenübersicht
#define GUI_ID_BUILD_MENU_PANEL_WIDGET 405                       // Baumenü
#define GUI_ID_MILITARY_PANEL_WIDGET 406                         // Kampfmenü
#define GUI_ID_PLAYER_STATUS_PANEL_WIDGET 407                    // Spielerstatus (Infomenü)
#define GUI_ID_OPTIONS_MENU_PANEL_WIDGET 408                     // Optionen-Menü

#define GUI_ID_PANEL_WIDGET_END 408                              // Marker für das letzte Panel-Widget

#endif
