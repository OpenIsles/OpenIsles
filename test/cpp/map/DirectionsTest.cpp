#include <gtest/gtest.h>
#include "map/Directions.h"


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