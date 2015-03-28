#ifndef _HARVESTABLE_H
#define _HARVESTABLE_H

#include <cassert>
#include "map/MapObject.h"

/**
 * @brief Map-Objekt, das eine Landschafts-Kachel darstellt, die abgeerntet werden kann, z.&nbsp;B. Wald oder Getreide.
 * Diese Kacheln sind 1x1 groß.
 */
class Harvestable : public MapObjectFixed {

private:
    // TODO Unterscheidung der verschiedenen Landschaften. Aktuell benutzen wir nur Wald und auch immer nur eine Kachel

    // TODO das wird man vermutlich in MapObjectFixed verlagern können. Schließlich sollten auch Inseln drehbar sein
    /**
     * @brief Ausrichtung der Kachel
     */
    FourthDirection view;

    /**
     * @brief Alter der Landschaft. Dies gibt an, welche Grafik (wie hoch ist das Zeugs gewachsen?) wir verwenden und
     * ob geerntet werden kann.
     */
    double age;

public:
    Harvestable() {
        mapWidth = 1;
        mapHeight = 1;
        age = 0;
    }

    virtual ~Harvestable() {}


    virtual void setMapWidth(int mapWidth) override {
        // Darf nicht aufgerufen werden. Kachelgröße ist IMMER 1x1!
        assert(false);
    }

    virtual void setMapHeight(int mapHeight) override {
        // Darf nicht aufgerufen werden. Kachelgröße ist IMMER 1x1!
        assert(false);
    }

    const FourthDirection& getView() const {
        return view;
    }

    void setView(FourthDirection view) {
        this->view = view;
    }

    double getAge() const {
        return age;
    }

    void setAge(double age) {
        this->age = age;
    }

    /**
     * @brief Gibt an, ob das Feld ausgewachsen ist und abgeerntet werden kann.
     * @return `true`, wenn geerntet werden kann, sonst `false`
     */
    bool isHarvestReady() const {
        return (age >= 4.0f);
    }

};

#endif