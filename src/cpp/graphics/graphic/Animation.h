#ifndef _ANIMATION_H
#define _ANIMATION_H

#include <array>
#include <cstdio>
#include <stdexcept>
#include <vector>
#include "defines.h"
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
class Animation : private std::vector<const IGraphic*> {

private:
    /**
     * @brief Anzahl Frames in der Animation.
     *
     * Dies entspricht `size()` des Vektors. Wir merken uns den Wert aber selber,
     * um den Container nicht fragen zu können.
     */
    unsigned int framesCount;

public:
    /**
     * @brief Legt eine neue Animation an.
     * @param framesCount Anzahl der Frames, den die Animation hat.
     */
    Animation(int framesCount) : std::vector<const IGraphic*>(framesCount), framesCount(framesCount) {
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
        for (auto iter = cbegin(); iter != cend(); iter++) {
            const IGraphic* graphic = *iter;
            delete graphic;
        }
        clear();
    }

    /**
     * @brief Fügt die Grafik für einen Frame hinzu.
     * Da die Zahl der Frames innerhalb einer Animation immer konstant ist, muss diese Methode einmalig
     * #framesCount-mal, nämlich 1x für jeden Frame nach der Instanziierung aufgerufen werden.
     *
     * @param frameIndex Index des Frames im Bereich 0 bis `framesCount-1`
     * @param graphic Zeiger auf die Einzelgrafik
     */
    void addFrame(int frameIndex, const IGraphic* graphic) {
        this->at(frameIndex) = graphic;
    }

    /**
     * @brief Liefert die Einzelgrafik zurück.
     * Diese Methode darf nur aufgerufen werden, wenn die Animation nur aus exakt 1 Frame besteht.
     *
     * @return Zeiger auf die Einzelgrafik
     */
    const IGraphic* getGraphic() const {
        if (framesCount != 1) {
            std::fputs(_("Animation does not have only frame\n"), stderr);
            throw std::runtime_error("Animation does not have only frame");
        }

        return front();
    }

    /**
     * @brief Liefert die Einzelgrafik für einen Frame zurück.
     *
     * @param frameIndex Index des Frames im Bereich 0 bis `framesCount-1`
     * @return Zeiger auf die Einzelgrafik des Frames
     */
    const IGraphic* getFrame(unsigned int frameIndex) const {
        if (frameIndex >= framesCount) {
            std::fputs(_("Animation does not have this many frames\n"), stderr);
            throw std::runtime_error("Animation does not have this many frames");
        }

        return at(frameIndex);
    }

    /**
     * @brief Liefert die Anzahl der Frames in der Animation zurück
     * @return Anzahl Frames in der Animation
     */
    unsigned int getFramesCount() const {
        return framesCount;
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