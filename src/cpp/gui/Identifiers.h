/**
 * In dieser Datei sind alle guiIdentifiers gelistet
 */
#ifndef _GUI_IDENTIFIERS_H
#define _GUI_IDENTIFIERS_H


#define GUI_ID_MAP 1
#define GUI_ID_PANEL 2
#define GUI_ID_MINIMAP 3
#define GUI_ID_STATUS_BAR 4

#define GUI_ID_MUSIC_PUSH_BUTTON 101

// Gebäude bauen ///////////////////////////////////////////////////////////////////////////////////////////////////////

// Button pro Gruppe (+ Offset)
#define GUI_ID_ADD_BUILDING_PUSH_BUTTON_BASE 110

// Grid mit den verschiedenen Knöpfen  (+ Offset)
#define GUI_ID_ADD_BUILDING_GRID_BASE 120

// Grid-Knöpfe  (+ Offset)
#define GUI_ID_ADD_BUILDING_GRID_BUTTON_BASE 130

#define GUI_ID_PANEL_SWITCH_PUSH_BUTTON_BASE 201
#define GUI_ID_PANEL_SWITCH_PUSH_BUTTON_BUILD GUI_ID_PANEL_SWITCH_PUSH_BUTTON_BASE
#define GUI_ID_PANEL_SWITCH_PUSH_BUTTON_2 (GUI_ID_PANEL_SWITCH_PUSH_BUTTON_BASE+1)
#define GUI_ID_PANEL_SWITCH_PUSH_BUTTON_3 (GUI_ID_PANEL_SWITCH_PUSH_BUTTON_BASE+2)
#define GUI_ID_PANEL_SWITCH_PUSH_BUTTON_4 (GUI_ID_PANEL_SWITCH_PUSH_BUTTON_BASE+3)

// Weitere Elemente
#define GUI_ID_ADD_BUILDING_WIDGET 300

// Panel-Widgets
#define GUI_ID_PANEL_WIDGET_START 401                    // Marker für das erste Panel-Widget

#define GUI_ID_SELECTED_BUILDING_PANEL_WIDGET 401        // ausgewähltes Gebäude (Infos über Produktion und intere Lagerbestände des Gebäudes)
#define GUI_ID_COLONY_GOODS_PANEL_WIDGET 402             // Kolonie-Warenübersicht
#define GUI_ID_BUILD_MENU_PANEL_WIDGET 403               // Baumenü
#define GUI_ID_MILITARY_PANEL_WIDGET 404                 // Kampfmenü
#define GUI_ID_PLAYER_STATUS_PANEL_WIDGET 405            // Spielerstatus (Infomenü)
#define GUI_ID_OPTIONS_MENU_PANEL_WIDGET 406             // Optionen-Menü

#define GUI_ID_PANEL_WIDGET_END 406                      // Marker für das letzte Panel-Widget

#endif
