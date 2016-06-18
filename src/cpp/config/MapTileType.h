#ifndef _MAP_TILE_TYPE_H
#define _MAP_TILE_TYPE_H

typedef unsigned int MapTileTypeInt;

/**
 * @brief Konfiguration für einen Kachel
 * 
 * Verfügbare Typen:
 *  ╔═══════════════╦═════════════════════════════════╦════════════╦═══════════════════════════════════════════════════════════╦═══════════════╗
 *  ║     Type      ║           Bezeichnung           ║ In Minimap ║                    Infos zur Bebauung                     ║ durch Schiffe ║
 *  ║               ║                                 ║     als    ║                                                           ║   befahrbar   ║
 *  ╠═══════════════╬═════════════════════════════════╬════════════╬═══════════════════════════════════════════════════════════╬═══════════════╣
 *  ║ OCEAN         ║ Ozean                           ║ Ozean      ║ nichts                                                    ║ ja            ║
 *  ║ FISH_GROUNDS  ║ Fischgründe                     ║ Ozean      ║ nichts, kann befischt werden                              ║ ja            ║
 *  ║ SHALLOW_WATER ║ seichtes Wasser                 ║ Ozean      ║ nichts                                                    ║ ja            ║
 *  ║ SHORE_WATER   ║ Küste (seichtes Wasser zu Sand) ║ Ozean      ║ nichts                                                    ║ nein          ║
 *  ║ SHORE_SAND    ║ flache Küste (Sand)             ║ Insel      ║ ausschließlich Anlegestellen, keine anderen Hafenanlagen  ║ nein          ║
 *  ║ SHORE_GRASS   ║ Küste (Hang Sand zu Graß)       ║ Insel      ║ alle Hafenanlagen                                         ║ nein          ║
 *  ║ RIVER         ║ Fluss                           ║ Insel      ║ nur Brücken                                               ║ nein          ║
 *  ║ MOUNTAIN      ║ Berg                            ║ Insel      ║ nur Bergbau                                               ║ nein          ║
 *  ║ GRASS         ║ Graß                            ║ Insel      ║ regulär                                                   ║ nein          ║
 *  ╚═══════════════╩═════════════════════════════════╩════════════╩═══════════════════════════════════════════════════════════╩═══════════════╝
 */
enum class MapTileType : MapTileTypeInt {
    OCEAN         = (1 << 0),
    FISH_GROUNDS  = (1 << 1),
    SHALLOW_WATER = (1 << 2),
    SHORE_WATER   = (1 << 3),
    SHORE_SAND    = (1 << 4),
    SHORE_GRASS   = (1 << 5),
    RIVER         = (1 << 6),
    MOUNTAIN      = (1 << 7),
    GRASS         = (1 << 8)
};

#endif