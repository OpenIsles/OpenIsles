#include <gtest/gtest.h>
#include "map/Directions.h"


TEST(FourDirectionsViewTest, CharConstructor) {
    FourDirectionsView inRange(2);
    ASSERT_EQ(2, inRange.getViewIndex());

    FourDirectionsView modulus(4+4+3);
    ASSERT_EQ(3, modulus.getViewIndex());

    FourDirectionsView negativeModulus(1-4);
    ASSERT_EQ(1, negativeModulus.getViewIndex());
}


TEST(FourDirectionsViewTest, ViewNameConstructor) {
    FourDirectionsView viewS("south");
    FourDirectionsView viewE("east");
    FourDirectionsView viewN("north");
    FourDirectionsView viewW("west");

    ASSERT_EQ(FourDirectionsView::SOUTH, viewS.getViewIndex());
    ASSERT_EQ(FourDirectionsView::EAST, viewE.getViewIndex());
    ASSERT_EQ(FourDirectionsView::NORTH, viewN.getViewIndex());
    ASSERT_EQ(FourDirectionsView::WEST, viewW.getViewIndex());
}


TEST(FourDirectionsViewTest, Increment) {
    FourDirectionsView view(FourDirectionsView::SOUTH);
    ASSERT_EQ(FourDirectionsView::SOUTH, view.getViewIndex());

    view++;
    ASSERT_EQ(FourDirectionsView::EAST, view.getViewIndex());

    view++;
    ASSERT_EQ(FourDirectionsView::NORTH, view.getViewIndex());

    view++;
    ASSERT_EQ(FourDirectionsView::WEST, view.getViewIndex());

    view++;
    ASSERT_EQ(FourDirectionsView::SOUTH, view.getViewIndex());

    // mit Zuweisung testen

    FourDirectionsView view2 = view++;
    ASSERT_EQ(FourDirectionsView::SOUTH, view2.getViewIndex());
    ASSERT_EQ(FourDirectionsView::EAST, view.getViewIndex());
}


TEST(FourDirectionsViewTest, Decrement) {
    FourDirectionsView view(FourDirectionsView::SOUTH);
    ASSERT_EQ(FourDirectionsView::SOUTH, view.getViewIndex());

    view--;
    ASSERT_EQ(FourDirectionsView::WEST, view.getViewIndex());

    view--;
    ASSERT_EQ(FourDirectionsView::NORTH, view.getViewIndex());

    view--;
    ASSERT_EQ(FourDirectionsView::EAST, view.getViewIndex());

    view--;
    ASSERT_EQ(FourDirectionsView::SOUTH, view.getViewIndex());

    // mit Zuweisung testen

    FourDirectionsView view2 = view--;
    ASSERT_EQ(FourDirectionsView::SOUTH, view2.getViewIndex());
    ASSERT_EQ(FourDirectionsView::WEST, view.getViewIndex());
}


TEST(FourDirectionsViewTest, AdditionNumber) {
    FourDirectionsView view(FourDirectionsView::SOUTH);
    FourDirectionsView viewPlus1 = view + 1;
    FourDirectionsView viewPlus2 = view + 2;
    FourDirectionsView viewPlus3 = view + 3;
    FourDirectionsView viewPlus4 = view + 4;

    ASSERT_EQ(FourDirectionsView::SOUTH, view.getViewIndex());
    ASSERT_EQ(FourDirectionsView::EAST, viewPlus1.getViewIndex());
    ASSERT_EQ(FourDirectionsView::NORTH, viewPlus2.getViewIndex());
    ASSERT_EQ(FourDirectionsView::WEST, viewPlus3.getViewIndex());
    ASSERT_EQ(FourDirectionsView::SOUTH, viewPlus4.getViewIndex());

    view = FourDirectionsView(FourDirectionsView::NORTH);
    viewPlus1 = view + 1;
    viewPlus2 = view + 2;
    viewPlus3 = view + 3;
    viewPlus4 = view + 4;

    ASSERT_EQ(FourDirectionsView::NORTH, view.getViewIndex());
    ASSERT_EQ(FourDirectionsView::WEST, viewPlus1.getViewIndex());
    ASSERT_EQ(FourDirectionsView::SOUTH, viewPlus2.getViewIndex());
    ASSERT_EQ(FourDirectionsView::EAST, viewPlus3.getViewIndex());
    ASSERT_EQ(FourDirectionsView::NORTH, viewPlus4.getViewIndex());

    view = view + 4828;
    ASSERT_EQ(FourDirectionsView::NORTH, view.getViewIndex());

    // mit Invalid darf man nicht rechnen

    FourDirectionsView invalid;

}


TEST(FourDirectionsViewTest, AdditionView) {
    FourDirectionsView view(FourDirectionsView::EAST);
    FourDirectionsView viewPlus0 = view + FourDirectionsView(FourDirectionsView::SOUTH);
    FourDirectionsView viewPlus1 = view + FourDirectionsView(FourDirectionsView::EAST);
    FourDirectionsView viewPlus2 = view + FourDirectionsView(FourDirectionsView::NORTH);
    FourDirectionsView viewPlus3 = view + FourDirectionsView(FourDirectionsView::WEST);

    ASSERT_EQ(FourDirectionsView::EAST, viewPlus0.getViewIndex());
    ASSERT_EQ(FourDirectionsView::NORTH, viewPlus1.getViewIndex());
    ASSERT_EQ(FourDirectionsView::WEST, viewPlus2.getViewIndex());
    ASSERT_EQ(FourDirectionsView::SOUTH, viewPlus3.getViewIndex());
}


TEST(FourDirectionsViewTest, Invalid) {
    // Invalid darf man nicht direkt via -1 konstruieren

    FourDirectionsView invalidShouldBeConstructedViaChar(-1);
    ASSERT_NE(-1, invalidShouldBeConstructedViaChar.getViewIndex());
    ASSERT_EQ(3, invalidShouldBeConstructedViaChar.getViewIndex());

    // mit Invalid darf man nicht rechnen

    FourDirectionsView invalid;
    ASSERT_THROW(invalid--, std::runtime_error);
    ASSERT_THROW(invalid++, std::runtime_error);
    ASSERT_THROW(invalid + 1, std::runtime_error);
    ASSERT_THROW(invalid + FourDirectionsView(FourDirectionsView::SOUTH), std::runtime_error);
    ASSERT_THROW(FourDirectionsView(FourDirectionsView::SOUTH) + invalid, std::runtime_error);

    // equals

    FourDirectionsView invalid2;
    ASSERT_EQ(invalid, invalid2);
}


TEST(FourDirectionsViewTest, Equals) {
    FourDirectionsView viewS(FourDirectionsView::SOUTH);
    FourDirectionsView viewE(FourDirectionsView::EAST);
    FourDirectionsView viewN(FourDirectionsView::NORTH);
    FourDirectionsView viewW(FourDirectionsView::WEST);

    ASSERT_TRUE(viewS == FourDirectionsView::SOUTH);
    ASSERT_TRUE(viewE == FourDirectionsView::EAST);
    ASSERT_TRUE(viewN == FourDirectionsView::NORTH);
    ASSERT_TRUE(viewW == FourDirectionsView::WEST);

    ASSERT_FALSE(viewS == FourDirectionsView::EAST);
    ASSERT_FALSE(viewS == FourDirectionsView::NORTH);
    ASSERT_FALSE(viewS == FourDirectionsView::WEST);
}