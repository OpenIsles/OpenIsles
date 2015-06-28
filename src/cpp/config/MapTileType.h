#ifndef _MAP_TILE_TYPE_H
#define _MAP_TILE_TYPE_H

typedef unsigned int MapTileTypeInt;

/**
 * @brief Konfiguration für einen Kachel
 * 
 * Verfügbare Typen:
 *  ╔═══════════════╦════════════════════════╦════════════════╦══════════════════════════════════════════════════════════╗
 *  ║     Type      ║      Bezeichnung       ║ In Minimap als ║                    Infos zur Bebauung                    ║
 *  ╠═══════════════╬════════════════════════╬════════════════╬══════════════════════════════════════════════════════════╣
 *  ║ OCEAN         ║ Ozean                  ║ Ozean          ║ nichts                                                   ║
 *  ║ FISH_GROUNDS  ║ Fischgründe            ║ Ozean          ║ nichts, kann befischt werden                             ║
 *  ║ SHALLOW_WATER ║ seichtes Wasser        ║ Ozean          ║ nichts                                                   ║
 *  ║ SHORE_OCEAN   ║ Ufer (auf Wasserseite) ║ Insel          ║ ausschließlich Anlegestellen, keine anderen Hafenanlagen ║
 *  ║ SHORE_GRASS   ║ Ufer (auf Inselseite)  ║ Insel          ║ alle Hafenanlagen                                        ║
 *  ║ RIVER         ║ Fluss                  ║ Insel          ║ nur Brücken                                              ║
 *  ║ MOUNTAIN      ║ Berg                   ║ Insel          ║ nur Bergbau                                              ║
 *  ║ GRASS         ║ Grass                  ║ Insel          ║ regulär                                                  ║
 *  ╚═══════════════╩════════════════════════╩════════════════╩══════════════════════════════════════════════════════════╝
 */
enum class MapTileType : MapTileTypeInt {
    OCEAN         = (1 << 0),
    FISH_GROUNDS  = (1 << 1),
    SHALLOW_WATER = (1 << 2),
    SHORE_OCEAN   = (1 << 3),
    SHORE_GRASS   = (1 << 4),
    RIVER         = (1 << 5),
    MOUNTAIN      = (1 << 6),
    GRASS         = (1 << 7)
};

#endif