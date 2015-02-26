#include "map/Directions.h"

template<>
const std::string FourDirectionsView::viewNames[] = {
    "south",
    "east",
    "north",
    "west"
};

template<>
const FourDirectionsView FourDirectionsView::ALL_VIEWS[] = {
    FourDirectionsView("south"),
    FourDirectionsView("east"),
    FourDirectionsView("north"),
    FourDirectionsView("west")
};

template<>
const std::string EightDirectionsView::viewNames[] = {
    "south",
    "southeast",
    "east",
    "northeast",
    "north",
    "northwest",
    "west",
    "southwest"
};

template<>
const EightDirectionsView EightDirectionsView::ALL_VIEWS[] = {
    EightDirectionsView("south"),
    EightDirectionsView("southeast"),
    EightDirectionsView("east"),
    EightDirectionsView("northeast"),
    EightDirectionsView("north"),
    EightDirectionsView("northwest"),
    EightDirectionsView("west"),
    EightDirectionsView("southwest")
};