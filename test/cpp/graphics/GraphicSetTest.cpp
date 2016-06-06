#include <gtest/gtest.h>
#include "graphics/graphic/GraphicSet.h"
#include "graphics/graphic/GraphicSetKeyState.h"

class GraphicSetTest : public testing::Test {

protected:
    GraphicSetKey keyWalkingNorth = GraphicSetKey(GraphicSetKeyState::WALKING, Direction::NORTH);
    GraphicSetKey keyWalkingNorth2 = GraphicSetKey(GraphicSetKeyState::WALKING, Direction::NORTH);
    GraphicSetKey keyEatingEast = GraphicSetKey(GraphicSetKeyState::EATING, Direction::EAST);
};


TEST_F(GraphicSetTest, GraphicSetKeyHasher) {
    GraphicSetKeyHasher graphicSetKeyHasher;

    std::size_t hashWalkingNorth = graphicSetKeyHasher(keyWalkingNorth);
    std::size_t hashWalkingNorth2 = graphicSetKeyHasher(keyWalkingNorth2);
    std::size_t hashEatingEast = graphicSetKeyHasher(keyEatingEast);

    ASSERT_NE(hashWalkingNorth, hashEatingEast);
    ASSERT_EQ(hashWalkingNorth, hashWalkingNorth2);
}

TEST_F(GraphicSetTest, GraphicSetKeyEqualer) {
    GraphicSetKeyEqualer graphicSetKeyEqualer;

    ASSERT_TRUE(graphicSetKeyEqualer(keyWalkingNorth, keyWalkingNorth));
    ASSERT_TRUE(graphicSetKeyEqualer(keyEatingEast, keyEatingEast));

    ASSERT_FALSE(graphicSetKeyEqualer(keyEatingEast, keyWalkingNorth));

    ASSERT_TRUE(graphicSetKeyEqualer(keyWalkingNorth, keyWalkingNorth2));
}