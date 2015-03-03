#include <gtest/gtest.h>
#include "graphics/graphic/GraphicSet.h"

class GraphicSetTest : public testing::Test {

protected:
    GraphicSetKey keyEmpty = GraphicSetKey("", Direction::NONE);
    GraphicSetKey keyFooNorth = GraphicSetKey("foo", Direction::NORTH);
    GraphicSetKey keyFooNorth2 = GraphicSetKey("foo", Direction::NORTH);
    GraphicSetKey keyBarFoo = GraphicSetKey("bar", Direction::EAST);
    GraphicSetKey keyUppercase = GraphicSetKey("BAR", Direction::EAST);
};


TEST_F(GraphicSetTest, GraphicSetKeyHasher) {
    GraphicSetKeyHasher graphicSetKeyHasher;

    std::size_t hashEmpty = graphicSetKeyHasher(keyEmpty);
    std::size_t hashFooBar = graphicSetKeyHasher(keyFooNorth);
    std::size_t hashFooBar2 = graphicSetKeyHasher(keyFooNorth2);
    std::size_t hashBarFoo = graphicSetKeyHasher(keyBarFoo);
    std::size_t hashUppercase = graphicSetKeyHasher(keyUppercase);

    ASSERT_NE(hashEmpty, hashFooBar);
    ASSERT_NE(hashEmpty, hashBarFoo);
    ASSERT_NE(hashEmpty, hashUppercase);
    ASSERT_NE(hashFooBar, hashBarFoo);
    ASSERT_NE(hashFooBar, hashUppercase);
    ASSERT_NE(hashBarFoo, hashUppercase);

    ASSERT_EQ(hashFooBar, hashFooBar2);
}

TEST_F(GraphicSetTest, GraphicSetKeyEqualer) {
    GraphicSetKeyEqualer graphicSetKeyEqualer;

    ASSERT_TRUE(graphicSetKeyEqualer(keyEmpty, keyEmpty));
    ASSERT_TRUE(graphicSetKeyEqualer(keyFooNorth, keyFooNorth));
    ASSERT_TRUE(graphicSetKeyEqualer(keyBarFoo, keyBarFoo));
    ASSERT_TRUE(graphicSetKeyEqualer(keyUppercase, keyUppercase));

    ASSERT_FALSE(graphicSetKeyEqualer(keyFooNorth, keyEmpty));
    ASSERT_FALSE(graphicSetKeyEqualer(keyBarFoo, keyFooNorth));
    ASSERT_FALSE(graphicSetKeyEqualer(keyBarFoo, keyUppercase));

    ASSERT_TRUE(graphicSetKeyEqualer(keyFooNorth, keyFooNorth2));
}