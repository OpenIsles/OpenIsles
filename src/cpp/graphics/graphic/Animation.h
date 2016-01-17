#ifndef _ANIMATION_H
#define _ANIMATION_H

#include <array>
#include <stdexcept>
#include <vector>
#include "global.h"
#include "graphics/graphic/IGraphic.h"

/**
 * @brief Eine Animation ist eine Zusammenreihung von Einzelgrafiken.
 *
 * Nicht jede "Animation" muss auch wirklich mehrere Frames enthalten. Statische Grafiken sind ebenso möglich.
 * Hierzu gibt es die Methode getGraphic().
 *
 * Die Animation enthält immer eine fixe Zahl von Bildern, die beim Instanziieren über den Konstruktor bekannt
 * gemacht werden muss. Mittels addFrame() werden die Frame-Einzelgrafiken dann gesetzt.
 */
class Animation {

private:
    /**
     * @brief Vektor mit den Frames
     */
    std::vector<const IGraphic*> frames;

public:
    /**
     * @brief Legt eine neue Animation an.
     * @param framesCount Anzahl der Frames, den die Animation hat.
     */
    Animation(std::size_t framesCount) : frames(framesCount) {
    }

    /**
     * @brief Legt eine neue Animation mit nur einem Frame an.
     * @param graphic Einzelgrafik
     */
    Animation(const IGraphic* graphic) : Animation(1) {
        addFrame(0, graphic);
    }

    /**
     * @brief Destruktur. Entlädt alle Grafiken.
     */
    ~Animation() {
        for (auto iter = frames.cbegin(); iter != frames.cend(); iter++) {
            const IGraphic* graphic = *iter;
            delete graphic;
        }
        frames.clear();
    }

    /**
     * @brief Fügt die Grafik für einen Frame hinzu.
     * Da die Zahl der Frames innerhalb einer Animation immer konstant ist, muss diese Methode einmalig
     * #framesCount-mal, nämlich 1x für jeden Frame nach der Instanziierung aufgerufen werden.
     *
     * @param frameIndex Index des Frames im Bereich 0 bis `framesCount-1`
     * @param graphic Zeiger auf die Einzelgrafik
     */
    void addFrame(std::size_t frameIndex, const IGraphic* graphic) {
        frames.at(frameIndex) = graphic;
    }

    /**
     * @brief Liefert die Einzelgrafik zurück.
     * Diese Methode darf nur aufgerufen werden, wenn die Animation nur aus exakt 1 Frame besteht.
     *
     * @return Zeiger auf die Einzelgrafik
     */
    const IGraphic* getGraphic() const {
        if (frames.size() != 1) {
            Log::error(_("Animation does not have only frame."));
            throw std::runtime_error("Animation does not have only frame");
        }

        return frames.front();
    }

    /**
     * @brief Liefert die Einzelgrafik für einen Frame zurück.
     *
     * @param frameIndex Index des Frames im Bereich 0 bis `framesCount-1`
     * @return Zeiger auf die Einzelgrafik des Frames
     */
    const IGraphic* getFrame(std::size_t frameIndex) const {
        if (frameIndex >= frames.size()) {
            Log::error(_("Animation does not have this many frames."));
            throw std::runtime_error("Animation does not have this many frames");
        }

        return frames.at(frameIndex);
    }

    /**
     * @brief Liefert die Anzahl der Frames in der Animation zurück
     * @return Anzahl Frames in der Animation
     */
    std::size_t getFramesCount() const {
        return frames.size();
    }

    /**
     * @brief Liefert die Abspielgeschwindigkeit der Animation zurück.
     * @return Abspielgeschwindigkeit der Animation in Frames per Second. Aktuell wird hier fix 25 zurückgeliefert.
     */
    double getFps() const {
        return 25.0;
    }
};

/**
 * @brief Abkürzung, um eine Animation aus 8 verschiedenen Richtungen zu definieren.
 * Array-Index ist hierbei EighthDirection
 */
using EightDirectionsAnimation = std::array<const Animation*, 8>;

#endif