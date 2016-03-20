#include <list>
#include <gtest/gtest.h>
#include "utils/RandomEngine.h"


TEST(RandomEngineTest, getBoolByProbability) {
    RealRandomEngine randomEngine;

    constexpr unsigned long TRIES = 1000000;
    unsigned long falseCount = 0;
    unsigned long trueCount = 0;

    for (unsigned long i = 0; i < TRIES; i++) {
        if (randomEngine.getBoolByProbability(0.25)) {
            trueCount++;
        } else {
            falseCount++;
        }
    }

    // Eine gewisse Unsicherheit is erlaubt, der Zufall wird immer nicht genau die Grenze treffen (Gesetz der großen Zahlen)
    constexpr double UNCERTAINTY = 1.01; // 1%

    ASSERT_LT(trueCount, (unsigned long) (250000 * UNCERTAINTY));
    ASSERT_LT(falseCount, (unsigned long) (750000 * UNCERTAINTY));
}

TEST(RandomEngineTest, getRandomListEntry) {
    RealRandomEngine randomEngine;

    std::list<int> list{1, 2, 3, 4, 5, 6, 7, 8, 9, 0};

    constexpr unsigned long TRIES = 1000000;
    unsigned long counts[10] = {0};

    for (unsigned long i = 0; i < TRIES; i++) {
        int randomListEntry = randomEngine.getRandomListEntry(list);
        counts[randomListEntry]++;
    }

    // Eine gewisse Unsicherheit is erlaubt
    constexpr double UNCERTAINTY = 1.01; // 1%

    for (int i = 0; i < 10; i++) {
        ASSERT_LT(counts[i], (unsigned long) (100000 * UNCERTAINTY));
    }
}