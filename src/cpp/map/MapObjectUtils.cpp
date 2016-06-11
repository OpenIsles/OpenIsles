#include <cassert>
#include "economics/Carrier.h"
#include "map/Harvestable.h"
#include "map/MapObjectUtils.h"
#include "map/Ship.h"
#include "map/Street.h"
#include "graphics/graphic/GraphicSet.h"
#include "graphics/graphic/GraphicSetKeyState.h"


const Animation* MapObjectUtils::getAnimation(const MapObject& mapObject, const FourthDirection& screenView) {
    const MapObjectType* mapObjectType = mapObject.getMapObjectType();
    const GraphicSet* graphicSet = mapObjectType->graphicSet;

    if (mapObjectType->type == MapObjectTypeClass::HARVESTABLE) {
        const Harvestable& harvestable = dynamic_cast<const Harvestable&>(mapObject);

        GraphicSetKeyState state = (GraphicSetKeyState) (GraphicSetKeyState::GROWTH0 + int(harvestable.getAge()));
        assert (state >= GraphicSetKeyState::GROWTH0 && state <= GraphicSetKeyState::GROWTH6);

        EighthDirection view = Direction::addDirections(harvestable.getView(), screenView);

        return graphicSet->getByStateAndView(state, view);
    }
    else if (mapObjectType->type == MapObjectTypeClass::STREET) {
        const Street& street = dynamic_cast<const Street&>(mapObject);
        const GraphicSetKeyState& state = street.getStateToRender();
        EighthDirection view = Direction::addDirections(street.getView(), screenView);

        return graphicSet->getByStateAndView(state, view);
    }
    else if (mapObjectType->type == MapObjectTypeClass::STRUCTURE ||
             mapObjectType->type == MapObjectTypeClass::BUILDING) {

        const MapObjectFixed& mapObjectFixed = dynamic_cast<const MapObjectFixed&>(mapObject);
        EighthDirection view = Direction::addDirections(mapObjectFixed.getView(), screenView);

        return graphicSet->getByView(view);
    }
    else if (mapObjectType->type == MapObjectTypeClass::CARRIER) {
        const Carrier& carrier = dynamic_cast<const Carrier&>(mapObject);

        // TODO Carrier können später MEHRERE Animationen haben ("stehen und ernten", "laufen").

        // Übersetzung von "Laufrichtung" + "aktuelle Ansicht" in korrekte Animation
        unsigned char animViewIndex = (unsigned char) ((10 - carrier.getCurrentMovingDirection() + screenView) % 8);

        return carrier.getAnimations()[animViewIndex];
    }
    else if (mapObjectType->type == MapObjectTypeClass::SHIP) {
        const Ship& ship = dynamic_cast<const Ship&>(mapObject);

        // TODO Schiffe können später MEHRERE Animationen haben ("vor Anker", "segeln").

        // Übersetzung von "Laufrichtung" + "aktuelle Ansicht" in korrekte Animation
        unsigned char animViewIndex = (unsigned char) ((10 - ship.getCurrentMovingDirection() + screenView) % 8);

        return ship.getMapObjectType()->graphicSet->getByView((GraphicSetKeyView) animViewIndex); // TODO verbessern später ;)
    }

    assert(false);
    return nullptr;
}

const IGraphic* MapObjectUtils::getGraphic(const MapObject& mapObject, const FourthDirection& screenView) {
    const Animation* animation = getAnimation(mapObject, screenView);
    return animation->getFrame((unsigned int) mapObject.animationFrame);
}

const IGraphic* MapObjectUtils::getGraphic(const MapObjectType* mapObjectType, const EighthDirection& view) {
    const GraphicSet* graphicSet = mapObjectType->graphicSet;

    const Animation* animation = nullptr;
    if (mapObjectType->type == MapObjectTypeClass::HARVESTABLE) {
        // Harvestable? ausgewachsenen Zustand nehmen
        unsigned char maxAge = mapObjectType->maxAge;
        GraphicSetKeyState graphicSetKeyStateFullgrown = (GraphicSetKeyState) (GraphicSetKeyState::GROWTH0 + maxAge);

        animation = graphicSet->getByStateAndView(graphicSetKeyStateFullgrown, view);
    }
    else if (mapObjectType->type == MapObjectTypeClass::STREET) {
        // für Straßen immer denselben State nehmen. Rotation macht die View.
        const GraphicSetKeyState& state = GraphicSetKeyState::STRAIGHT0;

        animation = graphicSet->getByStateAndView(state, view);
    }
    else if (mapObjectType->type == MapObjectTypeClass::STRUCTURE ||
             mapObjectType->type == MapObjectTypeClass::BUILDING) {

        animation = graphicSet->getByView(view);
    } else {
        assert(false); // TODO ggf. später für SHIP (wenn in Editor); CARRIER wird man hier wohl nie brauchen
    }

    return animation->getFrame(0);
}

void MapObjectUtils::animateObject(MapObject& mapObject, const Animation* animation, const Context& context) {
    mapObject.animationFrame +=
        (double) mapObject.getTicksSinceLastUpdate(context) / (double) TICKS_PER_SECOND * animation->getFps();

    while (mapObject.animationFrame >= animation->getFramesCount()) {
        mapObject.animationFrame -= animation->getFramesCount();
    }
}