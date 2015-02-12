#include <gtest/gtest.h>
#include "map/coords/MapCoords.h"


TEST(MapCoords, ConstructorAndGetter) {
    MapCoords zeroCoords;
    ASSERT_EQ(0, zeroCoords.x());
    ASSERT_EQ(0, zeroCoords.y());

    MapCoords coords(34, -2);
    ASSERT_EQ(34, coords.x());
    ASSERT_EQ(-2, coords.y());
}


TEST(MapCoords, AddSubXY) {
    MapCoords coord(5, 5);

    coord.addX(3);
    ASSERT_TRUE(coord == MapCoords(8, 5));

    coord.addY(1);
    ASSERT_TRUE(coord == MapCoords(8, 6));

    coord.subX(5);
    ASSERT_TRUE(coord == MapCoords(3, 6));

    coord.subY(7);
    ASSERT_TRUE(coord == MapCoords(3, -1));
}


TEST(MapCoords, SetMapCoords) {
    DoubleMapCoords doubleCoords = DoubleMapCoords(1.5, 2.3);
    doubleCoords.setMapCoords(DoubleMapCoords(-2.4, 7.8));

    ASSERT_TRUE(doubleCoords == DoubleMapCoords(-2.4, 7.8));
}


TEST(MapCoords, Equals) {
    MapCoords a(1, 2);
    MapCoords b(2, 1);
    MapCoords c(3, 3);
    MapCoords d(3, 3);

    ASSERT_TRUE(a != b);
    ASSERT_FALSE(a == b);

    ASSERT_TRUE(a != c);
    ASSERT_FALSE(a == c);

    ASSERT_TRUE(b != c);
    ASSERT_FALSE(b == c);

    ASSERT_FALSE(d != d);
    ASSERT_TRUE(d == d);
}


TEST(MapCoords, Cast) {
    MapCoords intCoords(1, 2);
    DoubleMapCoords doubleCoords = intCoords;

    ASSERT_TRUE(doubleCoords == DoubleMapCoords(1.0, 2.0));

    /* Die folgende Zeile sollte Compile Error generieren:
     *   MapCoords intCoords2 = doubleCoords;
     */
}


TEST(MapCoords, LessThan) {
    using std::operator<;

    MapCoords a(1, 2);
    MapCoords b(2, 1);
    MapCoords c(3, 3);
    MapCoords d(3, 3);
    MapCoords e(4, 3);

    ASSERT_TRUE(b < a);
    ASSERT_TRUE(a < c);
    ASSERT_TRUE(d < e);
    ASSERT_TRUE(b < e);

    ASSERT_FALSE(a < b);
    ASSERT_FALSE(c < c);
    ASSERT_FALSE(c < d);
    ASSERT_FALSE(e < d);
}


TEST(MapCoords, Template) {
    MapCoords intCoords(1.1, 2.2);
    DoubleMapCoords doubleCoords(1.1, 2.2);

    ASSERT_EQ(1, intCoords.x());
    ASSERT_EQ(2, intCoords.y());
    ASSERT_EQ(1.1, doubleCoords.x());
    ASSERT_EQ(2.2, doubleCoords.y());

    /* Die folgenden Ausdrücke sollten Compile Errors generieren:
     *   intCoords == doubleCoords
     *   intCoords + doubleCoords
     */
}