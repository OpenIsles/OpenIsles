#include "map/Directions.h"

constexpr char FourDirectionsView::SOUTH;
constexpr char FourDirectionsView::EAST;
constexpr char FourDirectionsView::NORTH;
constexpr char FourDirectionsView::WEST;

const FourDirectionsView FourDirectionsView::ALL_VIEWS[] = {
    FourDirectionsView(SOUTH),
    FourDirectionsView(EAST),
    FourDirectionsView(NORTH),
    FourDirectionsView(WEST)
};


constexpr char EightDirectionsView::SOUTH;
constexpr char EightDirectionsView::SOUTHEAST;
constexpr char EightDirectionsView::EAST;
constexpr char EightDirectionsView::NORTHEAST;
constexpr char EightDirectionsView::NORTH;
constexpr char EightDirectionsView::NORTHWEST;
constexpr char EightDirectionsView::WEST;
constexpr char EightDirectionsView::SOUTHWEST;

const EightDirectionsView EightDirectionsView::ALL_VIEWS[] = {
    EightDirectionsView(SOUTH),
    EightDirectionsView(SOUTHEAST),
    EightDirectionsView(EAST),
    EightDirectionsView(NORTHEAST),
    EightDirectionsView(NORTH),
    EightDirectionsView(NORTHWEST),
    EightDirectionsView(WEST),
    EightDirectionsView(SOUTHWEST)
};