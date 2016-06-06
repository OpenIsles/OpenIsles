#ifndef _STREET_H
#define _STREET_H

#include <bitset>
#include "global.h"
#include "graphics/graphic/GraphicSetKeyState.h"
#include "map/Structure.h"


/**
 * @brief Bitmaske, die angibt, welche Grafik verwendet wird = von welchen Seiten andere Straßen anschließen
 *
 * Aufbau ist:
 * <pre>
 * ╔════════════════════════════════╗
 * ║              Bits              ║
 * ╠═══════╦════════╦═══════╦═══════╣
 * ║  3    ║   2    ║   1   ║   0   ║
 * ║ oben  ║ rechts ║ unten ║ links ║
 * ║ NORTH ║  EAST  ║ SOUTH ║ WEST  ║
 * ╚═══════╩════════╩═══════╩═══════╝
 * </pre>
 */
class StreetConnections : public std::bitset<4> {

public:

    // Bit-Indexe der vier Richtungen, von denen andere Straßen anschließen können
    static constexpr unsigned char BIT_POS_NORTH = 3;
    static constexpr unsigned char BIT_POS_EAST  = 2;
    static constexpr unsigned char BIT_POS_SOUTH = 1;
    static constexpr unsigned char BIT_POS_WEST  = 0;
    
    // Bitmasken der vier Richtungen, von denen andere Straßen anschließen können
    static constexpr unsigned char BIT_MASK_NORTH = 1 << BIT_POS_NORTH;
    static constexpr unsigned char BIT_MASK_EAST  = 1 << BIT_POS_EAST;
    static constexpr unsigned char BIT_MASK_SOUTH = 1 << BIT_POS_SOUTH;
    static constexpr unsigned char BIT_MASK_WEST  = 1 << BIT_POS_WEST;

public:
    StreetConnections() : bitset(0) {
    }

    StreetConnections(const std::string& bitString) : bitset(bitString) {
        assert(to_ulong() >= 0 && to_ulong() < 16);
    }

#ifdef IN_TESTS
    /**
     * @brief Cast-Operator auf `unsigned char`.
     * @return Bitwert als `unsigned char`
     */
    operator unsigned char() const {
        return (unsigned char) to_ulong();
    }
#endif
};

/**
 * @brief Straße
 *
 * Das ist eine Struktur, die verschiedene Ausprägungen haben kann, welche Grafik gezeigt wird, basierend
 * darauf, von welchen Seiten weitere Straßen anschließen.
 *
 * <b>Wichtig:</b> Straßen sind immer süd-ausgerichtet. Es gibt nur eine einzige Ausnahme zu dieser Regel.
 * Eine Straße, die von KEINER anderen Straße umgeben ist, wird mittels `view` gedreht. Sobald allerdings
 * eine andere Straße anschließt, wird die view auf `SOUTH` gesetzt.
 */
class Street : public Structure {

public:
    /**
     * @brief Bitmaske, die angibt, welche Grafik verwendet wird = von welchen Seiten andere Straßen anschließen
     */
    StreetConnections streetConnections;

public:
    Street() {
    }

    virtual ~Street() {
    }

    virtual bool updateObject(const Context& context) override {
        return true;
    }

    /**
     * @brief Ermittelt den Zustand, den wir rendern müssen, basierend auf `streetConnections`
     * @return Zustand
     */
    const GraphicSetKeyState& getStateToRender() const {
        unsigned char bitMask = (unsigned char) streetConnections.to_ulong();
        assert(bitMask >= 0 && bitMask < 16);

        static GraphicSetKeyState bitMaskToState[16] = {
            GraphicSetKeyState::STRAIGHT0,
            GraphicSetKeyState::STRAIGHT0,
            GraphicSetKeyState::STRAIGHT90,
            GraphicSetKeyState::CURVE180,
            GraphicSetKeyState::STRAIGHT0,
            GraphicSetKeyState::STRAIGHT0,
            GraphicSetKeyState::CURVE90,
            GraphicSetKeyState::TEE270,
            GraphicSetKeyState::STRAIGHT90,
            GraphicSetKeyState::CURVE270,
            GraphicSetKeyState::STRAIGHT90,
            GraphicSetKeyState::TEE0,
            GraphicSetKeyState::CURVE0,
            GraphicSetKeyState::TEE90,
            GraphicSetKeyState::TEE180,
            GraphicSetKeyState::CROSS
        };
        return bitMaskToState[bitMask];
    }
};

#endif
