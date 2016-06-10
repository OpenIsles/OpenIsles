#include <cassert>
#include "map/Harvestable.h"
#include "map/MapObjectUtils.h"
#include "map/Street.h"
#include "graphics/graphic/GraphicSet.h"
#include "graphics/graphic/GraphicSetKeyState.h"


const Animation* MapObjectUtils::getAnimation(const MapObject& mapObject, const FourthDirection& view) {
    const MapObjectType* mapObjectType = mapObject.getMapObjectType();
    const GraphicSet* graphicSet = mapObjectType->graphicSet;

    if (mapObjectType->type == MapObjectTypeClass::HARVESTABLE) {
        const Harvestable& harvestable = dynamic_cast<const Harvestable&>(mapObject);

        GraphicSetKeyState state = (GraphicSetKeyState) (GraphicSetKeyState::GROWTH0 + int(harvestable.getAge()));
        assert (state >= GraphicSetKeyState::GROWTH0 && state <= GraphicSetKeyState::GROWTH6);

        return graphicSet->getByStateAndView(state, view);
    }
    else if (mapObjectType->type == MapObjectTypeClass::STREET) {
        const Street& street = dynamic_cast<const Street&>(mapObject);
        const GraphicSetKeyState& state = street.getStateToRender();

        return graphicSet->getByStateAndView(state, view);
    }
    else if (mapObjectType->type == MapObjectTypeClass::STRUCTURE ||
             mapObjectType->type == MapObjectTypeClass::BUILDING) {

        return graphicSet->getByView(view);
    }

    assert(false);
    return nullptr;
}

const IGraphic* MapObjectUtils::getGraphic(const MapObject& mapObject, const FourthDirection& view) {
    const Animation* animation = getAnimation(mapObject, view);
    return animation->getFrame(int(mapObject.animationFrame));
}

const IGraphic* MapObjectUtils::getGraphic(const MapObjectType* mapObjectType, const FourthDirection& view) {
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
        assert(false);
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