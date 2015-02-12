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


/**
 * @brief "Addiert" zwei Ansichten.
 */
FourDirectionsView operator+ (const FourDirectionsView& view1, const FourDirectionsView& view2) {
    unsigned char newViewIndex = (view1.getViewIndex() + view2.getViewIndex()) % FourDirectionsView::MAX_VIEW;

    return FourDirectionsView::ALL_VIEWS[newViewIndex];
}