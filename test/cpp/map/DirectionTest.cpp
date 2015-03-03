#include <gtest/gtest.h>
#include "map/Direction.h"


TEST(DirectionTest, fromString) {
    EighthDirection viewS = Direction::fromString("south");
    EighthDirection viewSE = Direction::fromString("southeast");
    EighthDirection viewE = Direction::fromString("east");
    EighthDirection viewNE = Direction::fromString("northeast");
    EighthDirection viewN = Direction::fromString("north");
    EighthDirection viewNW = Direction::fromString("northwest");
    EighthDirection viewW = Direction::fromString("west");
    EighthDirection viewSW = Direction::fromString("southwest");

    ASSERT_EQ(Direction::SOUTH, viewS);
    ASSERT_EQ(Direction::SOUTHEAST, viewSE);
    ASSERT_EQ(Direction::EAST, viewE);
    ASSERT_EQ(Direction::NORTHEAST, viewNE);
    ASSERT_EQ(Direction::NORTH, viewN);
    ASSERT_EQ(Direction::NORTHWEST, viewNW);
    ASSERT_EQ(Direction::WEST, viewW);
    ASSERT_EQ(Direction::SOUTHWEST, viewSW);
}


TEST(DirectionTest, rotate90DegreesClockwise) {
    FourthDirection view = Direction::SOUTH;
    ASSERT_EQ(Direction::SOUTH, view);

    Direction::rotate90DegreesClockwise(view);
    ASSERT_EQ(Direction::EAST, view);

    Direction::rotate90DegreesClockwise(view);
    ASSERT_EQ(Direction::NORTH, view);

    Direction::rotate90DegreesClockwise(view);
    ASSERT_EQ(Direction::WEST, view);

    Direction::rotate90DegreesClockwise(view);
    ASSERT_EQ(Direction::SOUTH, view);
}


TEST(DirectionTest, Decrement) {
    FourthDirection view = Direction::SOUTH;
    ASSERT_EQ(Direction::SOUTH, view);

    Direction::rotate90DegreesCounterclockwise(view);
    ASSERT_EQ(Direction::WEST, view);

    Direction::rotate90DegreesCounterclockwise(view);
    ASSERT_EQ(Direction::NORTH, view);

    Direction::rotate90DegreesCounterclockwise(view);
    ASSERT_EQ(Direction::EAST, view);

    Direction::rotate90DegreesCounterclockwise(view);
    ASSERT_EQ(Direction::SOUTH, view);
}


TEST(DirectionTest, add90DegreeOffset) {
    FourthDirection view = Direction::SOUTH;
    FourthDirection viewPlus1 = Direction::add90DegreeOffset(view, 1);
    FourthDirection viewPlus2 = Direction::add90DegreeOffset(view, 2);
    FourthDirection viewPlus3 = Direction::add90DegreeOffset(view, 3);
    FourthDirection viewPlus4 = Direction::add90DegreeOffset(view, 4);

    ASSERT_EQ(Direction::SOUTH, view);
    ASSERT_EQ(Direction::EAST, viewPlus1);
    ASSERT_EQ(Direction::NORTH, viewPlus2);
    ASSERT_EQ(Direction::WEST, viewPlus3);
    ASSERT_EQ(Direction::SOUTH, viewPlus4);

    view = Direction::NORTH;
    viewPlus1 = Direction::add90DegreeOffset(view, 1);
    viewPlus2 = Direction::add90DegreeOffset(view, 2);
    viewPlus3 = Direction::add90DegreeOffset(view, 3);
    viewPlus4 = Direction::add90DegreeOffset(view, 4);

    ASSERT_EQ(Direction::NORTH, view);
    ASSERT_EQ(Direction::WEST, viewPlus1);
    ASSERT_EQ(Direction::SOUTH, viewPlus2);
    ASSERT_EQ(Direction::EAST, viewPlus3);
    ASSERT_EQ(Direction::NORTH, viewPlus4);

    view = Direction::add90DegreeOffset(view, 96);
    ASSERT_EQ(Direction::NORTH, view);
}


TEST(DirectionTest, addDirections) {
    EighthDirection view = Direction::EAST;
    EighthDirection viewPlus0 = Direction::addDirections(view, Direction::SOUTH);
    EighthDirection viewPlus1 = Direction::addDirections(view, Direction::EAST);
    EighthDirection viewPlus2 = Direction::addDirections(view, Direction::NORTH);
    EighthDirection viewPlus3 = Direction::addDirections(view, Direction::WEST);

    ASSERT_EQ(Direction::EAST, viewPlus0);
    ASSERT_EQ(Direction::NORTH, viewPlus1);
    ASSERT_EQ(Direction::WEST, viewPlus2);
    ASSERT_EQ(Direction::SOUTH, viewPlus3);
}
