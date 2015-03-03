#include <list>
#include <gtest/gtest.h>
#include "utils/MapCoordsIterator.h"


TEST(MapCoordsIteratorTest, IterateSouth) {
    std::list<MapCoords> mapCoordsFromIterator;
    MapCoordsIterator mapCoordsIterator(MapCoords(5, 2), MapCoords(7, 4), Direction::SOUTH);
    mapCoordsIterator.iterate([&mapCoordsFromIterator] (const MapCoords& mapCoords) {
        mapCoordsFromIterator.push_back(mapCoords);
    });

    ASSERT_TRUE(mapCoordsFromIterator.front() == MapCoords(5, 2));
    mapCoordsFromIterator.pop_front();
    ASSERT_TRUE(mapCoordsFromIterator.front() == MapCoords(6, 2));
    mapCoordsFromIterator.pop_front();
    ASSERT_TRUE(mapCoordsFromIterator.front() == MapCoords(7, 2));
    mapCoordsFromIterator.pop_front();
    ASSERT_TRUE(mapCoordsFromIterator.front() == MapCoords(5, 3));
    mapCoordsFromIterator.pop_front();
    ASSERT_TRUE(mapCoordsFromIterator.front() == MapCoords(6, 3));
    mapCoordsFromIterator.pop_front();
    ASSERT_TRUE(mapCoordsFromIterator.front() == MapCoords(7, 3));
    mapCoordsFromIterator.pop_front();
    ASSERT_TRUE(mapCoordsFromIterator.front() == MapCoords(5, 4));
    mapCoordsFromIterator.pop_front();
    ASSERT_TRUE(mapCoordsFromIterator.front() == MapCoords(6, 4));
    mapCoordsFromIterator.pop_front();
    ASSERT_TRUE(mapCoordsFromIterator.front() == MapCoords(7, 4));
    mapCoordsFromIterator.pop_front();

    ASSERT_TRUE(mapCoordsFromIterator.empty());
}


TEST(MapCoordsIteratorTest, IterateWest) {
    std::list<MapCoords> mapCoordsFromIterator;
    MapCoordsIterator mapCoordsIterator(MapCoords(2, 1), MapCoords(5, 3), Direction::WEST);
    mapCoordsIterator.iterate([&mapCoordsFromIterator] (const MapCoords& mapCoords) {
        mapCoordsFromIterator.push_back(mapCoords);
    });

    ASSERT_TRUE(mapCoordsFromIterator.front() == MapCoords(5, 1));
    mapCoordsFromIterator.pop_front();
    ASSERT_TRUE(mapCoordsFromIterator.front() == MapCoords(5, 2));
    mapCoordsFromIterator.pop_front();
    ASSERT_TRUE(mapCoordsFromIterator.front() == MapCoords(5, 3));
    mapCoordsFromIterator.pop_front();
    ASSERT_TRUE(mapCoordsFromIterator.front() == MapCoords(4, 1));
    mapCoordsFromIterator.pop_front();
    ASSERT_TRUE(mapCoordsFromIterator.front() == MapCoords(4, 2));
    mapCoordsFromIterator.pop_front();
    ASSERT_TRUE(mapCoordsFromIterator.front() == MapCoords(4, 3));
    mapCoordsFromIterator.pop_front();
    ASSERT_TRUE(mapCoordsFromIterator.front() == MapCoords(3, 1));
    mapCoordsFromIterator.pop_front();
    ASSERT_TRUE(mapCoordsFromIterator.front() == MapCoords(3, 2));
    mapCoordsFromIterator.pop_front();
    ASSERT_TRUE(mapCoordsFromIterator.front() == MapCoords(3, 3));
    mapCoordsFromIterator.pop_front();
    ASSERT_TRUE(mapCoordsFromIterator.front() == MapCoords(2, 1));
    mapCoordsFromIterator.pop_front();
    ASSERT_TRUE(mapCoordsFromIterator.front() == MapCoords(2, 2));
    mapCoordsFromIterator.pop_front();
    ASSERT_TRUE(mapCoordsFromIterator.front() == MapCoords(2, 3));
    mapCoordsFromIterator.pop_front();

    ASSERT_TRUE(mapCoordsFromIterator.empty());
}


TEST(MapCoordsIteratorTest, IterateNorth) {
    std::list<MapCoords> mapCoordsFromIterator;
    MapCoordsIterator mapCoordsIterator(MapCoords(1, 1), MapCoords(3, 3), Direction::NORTH);
    mapCoordsIterator.iterate([&mapCoordsFromIterator] (const MapCoords& mapCoords) {
        mapCoordsFromIterator.push_back(mapCoords);
    });

    ASSERT_TRUE(mapCoordsFromIterator.front() == MapCoords(3, 3));
    mapCoordsFromIterator.pop_front();
    ASSERT_TRUE(mapCoordsFromIterator.front() == MapCoords(2, 3));
    mapCoordsFromIterator.pop_front();
    ASSERT_TRUE(mapCoordsFromIterator.front() == MapCoords(1, 3));
    mapCoordsFromIterator.pop_front();
    ASSERT_TRUE(mapCoordsFromIterator.front() == MapCoords(3, 2));
    mapCoordsFromIterator.pop_front();
    ASSERT_TRUE(mapCoordsFromIterator.front() == MapCoords(2, 2));
    mapCoordsFromIterator.pop_front();
    ASSERT_TRUE(mapCoordsFromIterator.front() == MapCoords(1, 2));
    mapCoordsFromIterator.pop_front();
    ASSERT_TRUE(mapCoordsFromIterator.front() == MapCoords(3, 1));
    mapCoordsFromIterator.pop_front();
    ASSERT_TRUE(mapCoordsFromIterator.front() == MapCoords(2, 1));
    mapCoordsFromIterator.pop_front();
    ASSERT_TRUE(mapCoordsFromIterator.front() == MapCoords(1, 1));
    mapCoordsFromIterator.pop_front();

    ASSERT_TRUE(mapCoordsFromIterator.empty());
}


TEST(MapCoordsIteratorTest, IterateEast) {
    std::list<MapCoords> mapCoordsFromIterator;
    MapCoordsIterator mapCoordsIterator(MapCoords(85, -9), MapCoords(83, -10), Direction::EAST);
    mapCoordsIterator.iterate([&mapCoordsFromIterator] (const MapCoords& mapCoords) {
        mapCoordsFromIterator.push_back(mapCoords);
    });

    ASSERT_TRUE(mapCoordsFromIterator.front() == MapCoords(83, -9));
    mapCoordsFromIterator.pop_front();
    ASSERT_TRUE(mapCoordsFromIterator.front() == MapCoords(83, -10));
    mapCoordsFromIterator.pop_front();
    ASSERT_TRUE(mapCoordsFromIterator.front() == MapCoords(84, -9));
    mapCoordsFromIterator.pop_front();
    ASSERT_TRUE(mapCoordsFromIterator.front() == MapCoords(84, -10));
    mapCoordsFromIterator.pop_front();
    ASSERT_TRUE(mapCoordsFromIterator.front() == MapCoords(85, -9));
    mapCoordsFromIterator.pop_front();
    ASSERT_TRUE(mapCoordsFromIterator.front() == MapCoords(85, -10));
    mapCoordsFromIterator.pop_front();

    ASSERT_TRUE(mapCoordsFromIterator.empty());
}