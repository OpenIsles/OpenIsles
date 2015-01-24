#include <gtest/gtest.h>
#include "map/Directions.h"


TEST(FourDirectionsViewTest, ViewName) {
    FourDirectionsView viewS("south");
    FourDirectionsView viewE("east");
    FourDirectionsView viewN("north");
    FourDirectionsView viewW("west");

    ASSERT_EQ(0, viewS.getViewIndex());
    ASSERT_EQ("south", viewS.getViewName());

    ASSERT_EQ(1, viewE.getViewIndex());
    ASSERT_EQ("east", viewE.getViewName());

    ASSERT_EQ(2, viewN.getViewIndex());
    ASSERT_EQ("north", viewN.getViewName());

    ASSERT_EQ(3, viewW.getViewIndex());
    ASSERT_EQ("west", viewW.getViewName());
}


TEST(FourDirectionsViewTest, Increment) {
    FourDirectionsView view;

    ASSERT_EQ(0, view.getViewIndex());
    ASSERT_EQ("south", view.getViewName());

    view++;

    ASSERT_EQ(1, view.getViewIndex());
    ASSERT_EQ("east", view.getViewName());

    view++;

    ASSERT_EQ(2, view.getViewIndex());
    ASSERT_EQ("north", view.getViewName());

    view++;

    ASSERT_EQ(3, view.getViewIndex());
    ASSERT_EQ("west", view.getViewName());

    view++;

    ASSERT_EQ(0, view.getViewIndex());
    ASSERT_EQ("south", view.getViewName());

    // mit Zuweisung testen

    FourDirectionsView view2 = view++;

    ASSERT_EQ(0, view2.getViewIndex());
    ASSERT_EQ("south", view2.getViewName());

    ASSERT_EQ(1, view.getViewIndex());
    ASSERT_EQ("east", view.getViewName());
}


TEST(FourDirectionsViewTest, Decrement) {
    FourDirectionsView view;

    ASSERT_EQ(0, view.getViewIndex());
    ASSERT_EQ("south", view.getViewName());

    view--;

    ASSERT_EQ(3, view.getViewIndex());
    ASSERT_EQ("west", view.getViewName());

    view--;

    ASSERT_EQ(2, view.getViewIndex());
    ASSERT_EQ("north", view.getViewName());

    view--;

    ASSERT_EQ(1, view.getViewIndex());
    ASSERT_EQ("east", view.getViewName());

    view--;

    ASSERT_EQ(0, view.getViewIndex());
    ASSERT_EQ("south", view.getViewName());

    // mit Zuweisung testen

    FourDirectionsView view2 = view--;

    ASSERT_EQ(0, view2.getViewIndex());
    ASSERT_EQ("south", view2.getViewName());

    ASSERT_EQ(3, view.getViewIndex());
    ASSERT_EQ("west", view.getViewName());
}


TEST(FourDirectionsViewTest, Addition) {
    FourDirectionsView view("south");
    FourDirectionsView viewPlus1 = view + 1;
    FourDirectionsView viewPlus2 = view + 2;
    FourDirectionsView viewPlus3 = view + 3;
    FourDirectionsView viewPlus4 = view + 4;

    ASSERT_EQ(0, view.getViewIndex());
    ASSERT_EQ(1, viewPlus1.getViewIndex());
    ASSERT_EQ(2, viewPlus2.getViewIndex());
    ASSERT_EQ(3, viewPlus3.getViewIndex());
    ASSERT_EQ(0, viewPlus4.getViewIndex());

    view = FourDirectionsView("north");
    viewPlus1 = view + 1;
    viewPlus2 = view + 2;
    viewPlus3 = view + 3;
    viewPlus4 = view + 4;

    ASSERT_EQ(2, view.getViewIndex());
    ASSERT_EQ(3, viewPlus1.getViewIndex());
    ASSERT_EQ(0, viewPlus2.getViewIndex());
    ASSERT_EQ(1, viewPlus3.getViewIndex());
    ASSERT_EQ(2, viewPlus4.getViewIndex());

    view = view + 4828;
    ASSERT_EQ(2, view.getViewIndex());
}