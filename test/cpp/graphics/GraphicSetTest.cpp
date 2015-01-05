#include <gtest/gtest.h>
#include "graphics/graphic/GraphicSet.h"

class GraphicSetTest : public testing::Test {

protected:
    GraphicSetKey keyEmpty = GraphicSetKey("", "");
    GraphicSetKey keyFooBar = GraphicSetKey("foo", "bar");
    GraphicSetKey keyFooBar2 = GraphicSetKey("foo", "bar");
    GraphicSetKey keyBarFoo = GraphicSetKey("bar", "foo");
    GraphicSetKey keyUppercase = GraphicSetKey("BAR", "FOO");
};


TEST_F(GraphicSetTest, GraphicSetKeyHasher) {
    GraphicSetKeyHasher graphicSetKeyHasher;

    std::size_t hashEmpty = graphicSetKeyHasher(keyEmpty);
    std::size_t hashFooBar = graphicSetKeyHasher(keyFooBar);
    std::size_t hashFooBar2 = graphicSetKeyHasher(keyFooBar2);
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
    ASSERT_TRUE(graphicSetKeyEqualer(keyFooBar, keyFooBar));
    ASSERT_TRUE(graphicSetKeyEqualer(keyBarFoo, keyBarFoo));
    ASSERT_TRUE(graphicSetKeyEqualer(keyUppercase, keyUppercase));

    ASSERT_FALSE(graphicSetKeyEqualer(keyFooBar, keyEmpty));
    ASSERT_FALSE(graphicSetKeyEqualer(keyBarFoo, keyFooBar));
    ASSERT_FALSE(graphicSetKeyEqualer(keyBarFoo, keyUppercase));

    ASSERT_TRUE(graphicSetKeyEqualer(keyFooBar, keyFooBar2));
}