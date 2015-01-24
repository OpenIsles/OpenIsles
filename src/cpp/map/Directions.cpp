#include "map/Directions.h"

const std::string FourDirectionsView::viewNames[] = {
    "south",
    "east",
    "north",
    "west"
};

const FourDirectionsView FourDirectionsView::ALL_VIEWS[] = {
    FourDirectionsView("south"),
    FourDirectionsView("east"),
    FourDirectionsView("north"),
    FourDirectionsView("west")
};