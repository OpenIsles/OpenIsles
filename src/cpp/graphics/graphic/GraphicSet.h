#ifndef _GRAPHIC_SET_H
#define _GRAPHIC_SET_H

#include <string>
#include <unordered_map>
#include "graphics/graphic/Animation.h"
#include "graphics/graphic/IGraphic.h"


/**
 * @brief Map-Key für GraphicSet bestehend aus Zustand und Ansicht
 */
struct GraphicSetKey {

    /**
     * @brief Zustand. Wird z.&nbsp;B. benutzt, um separate Animationen eines Marktkarrens für Zustand
     * "Karren mit Waren voll" und "Karren leer" zu haben.
     *
     * Ist kein Zustand zugeordnet, ist ein Leerstring gesetzt.
     */
    const std::string state;

    /**
     * @brief Ansicht. Wird benutzt, um z.&nbsp;B. ein Gebäude von mehreren Seiten (Nord, Ost, Süd, West) zu zeigen.
     *
     * Ist keine Ansicht zugeordnet, ist ein Leerstring gesetzt.
     */
    const std::string view;

    /**
     * @brief Erzeugt einen Key aus Zustand und Ansicht.
     * @param state Zustand (Leerstring, wenn nicht zutreffend)
     * @param view Ansicht (Leerstring, wenn nicht zutreffend)
     */
    GraphicSetKey(std::string& state, std::string& view) : state(state), view(view) {
    }

};

/**
 * @brief Funktionsobjekt um Hashwerte für das Verwenden von GraphicSetKey in einer STL-Map zu generieren.
 */
struct GraphicSetKeyHasher {
    /**
     * @brief Generiert den Hashwert für einen Map-Key
     * @param key Map-Key
     * @return Hashwert
     */
    std::size_t operator() (const GraphicSetKey& key) const {
        std::hash<std::string> strHash;
        return strHash(key.state) ^ strHash(key.view);
    }
};

/**
 * @brief Funktionsobjekt um zwei GraphicSetKey in einer STL-Map vergleichen zu können
 */
struct GraphicSetKeyEqualer {
    /**
     * @brief Prüft, ob zwei Map-Keys identisch sind
     * @param key1 erster Key
     * @param key2 zweiter Key
     * @return `true`, wenn die Keys identisch sind, sonst `false`
     */
    bool operator() (const GraphicSetKey& key1, const GraphicSetKey& key2) const {
        return ((key1.state == key2.state) && (key1.view == key2.view));
    }
};


/**
 * @brief Grafik-Set. Dies ist eine Sammlung von Animationen, die sich in Zustand und Ansicht unterscheiden können.
 * Wir verwenden das z.&nbsp;B. für
 *  - Gebäude mit 4 Ansichten und 2 Zuständen (Einzelgrafik "idle", Animation "Gebäude arbeitet")
 *  - GUI-Element mit Zuständen: Button normal, Button gedrückt
 *  - Gelände-Kachel mit 4 Ansichten
 *  - Fischerboot mit 8 Ansichten und mehrere Zuständen
 *
 * Nicht jede Animation hat einen Zustand und/oder Ansicht:
 *  - z.&nbsp;B. Gelände-Kacheln haben nur verschiedene Ansichten, keine Zustände
 *  - z.&nbsp;B. GUI-Elemente haben nur verschiedene Zustände, keine Ansichten
 *
 * Als "statisch" (= zustandslos) werden Animationen bezeichnet, die weder Ansicht noch Zustand haben.
 *
 * Der Map-Key muss eindeutig sein, d.&nbsp;h. dieselbe add-Methode darf nicht mit denselben Parametern (`animation`
 * ausgenommen) mehrfach aufgerufen werden.
 *
 * Es ist zu beachten, dass ein Grafik-Set grundsätzlich aus Animationen besteht. Eine Animation selber kann aber
 * wiederum nur aus *einer* Einzelgrafik bestehen.
 */
class GraphicSet : private std::unordered_map<GraphicSetKey, Animation*, GraphicSetKeyHasher, GraphicSetKeyEqualer> {

public:
    /**
     * @brief Destruktur. Entlädt alle Animationen.
     */
    ~GraphicSet() {
        for (auto iter = cbegin(); iter != cend(); iter++) {
            Animation* animation = iter->second;
            delete animation;
        }
        clear();
    }

    /**
     * @brief Fügt eine statische Animation hinzu
     * @param animation Animation
     */
    void addStatic(Animation* animation) {
        addByStateAndView("", "", animation);
    }

    /**
     * @brief Fügt eine Animation mit einem bestimmten Zustand hinzu
     * @param state Zustand
     * @param animation Animation
     */
    void addByState(std::string state, Animation* animation) {
        addByStateAndView(state, "", animation);
    }

    /**
     * @brief Fügt eine Animation mit einer bestimmten Ansicht hinzu
     * @param view Ansicht
     * @param animation Animation
     */
    void addByView(std::string view, Animation* animation) {
        addByStateAndView("", view, animation);
    }

    /**
     * @brief Fügt eine Animation mit einem bestimmten Zustand und einer bestimmten Ansicht hinzu
     * @param state Zustand (Leerstring, wenn nicht zutreffend)
     * @param view Ansicht (Leerstring, wenn nicht zutreffend)
     * @param animation Animation
     */
    void addByStateAndView(std::string state, std::string view, Animation* animation) {
        (*this)[GraphicSetKey(state, view)] = animation;
    }

    /**
     * @brief Liefert die statische Animation zurück
     * @return Animation
     */
    const Animation* getStatic() const {
        return getByStateAndView("", "");
    }

    /**
     * @brief Liefert eine Animation mit einem bestimmten Zustand zurück
     * @param state Zustand
     * @return Animation
     */
    const Animation* getByState(std::string state) const {
        return getByStateAndView(state, "");
    }

    /**
     * @brief Liefert eine Animation mit einer bestimmten Ansicht zurück
     * @param view Ansicht
     * @return Animation
     */
    const Animation* getByView(std::string view) const {
        return getByStateAndView("", view);
    }

    /**
     * @brief Liefert eine Animation mit einem bestimmten Zustand und einer bestimmten Ansicht zurück
     * @param state Zustand (Leerstring, wenn nicht zutreffend)
     * @param view Ansicht (Leerstring, wenn nicht zutreffend)
     * @return Animation
     */
    const Animation* getByStateAndView(std::string state, std::string view) const {
        return this->at(GraphicSetKey(state, view));
    }

};

#endif
