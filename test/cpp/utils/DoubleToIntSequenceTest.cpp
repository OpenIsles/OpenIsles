#include <gtest/gtest.h>
#include "utils/DoubleToIntSequence.h"

/**
 * @brief Prüft die Integer-Zahlenreihe mit Ganzzahlen
 */
TEST(DoubleToIntSequenceTest, checkWithWholeNumbers) {
    {
        std::array<int, 6> array = DoubleToIntSequence::toIntSequence(4);
        ASSERT_EQ(4, array[0]);
        ASSERT_EQ(4, array[1]);
        ASSERT_EQ(4, array[2]);
        ASSERT_EQ(4, array[3]);
        ASSERT_EQ(4, array[4]);
        ASSERT_EQ(4, array[5]);
    }

    {
        std::array<int, 6> array = DoubleToIntSequence::toIntSequence(4711);
        ASSERT_EQ(4711, array[0]);
        ASSERT_EQ(4711, array[1]);
        ASSERT_EQ(4711, array[2]);
        ASSERT_EQ(4711, array[3]);
        ASSERT_EQ(4711, array[4]);
        ASSERT_EQ(4711, array[5]);
    }

    {
        std::array<int, 6> array = DoubleToIntSequence::toIntSequence(0);
        ASSERT_EQ(0, array[0]);
        ASSERT_EQ(0, array[1]);
        ASSERT_EQ(0, array[2]);
        ASSERT_EQ(0, array[3]);
        ASSERT_EQ(0, array[4]);
        ASSERT_EQ(0, array[5]);
    }
}

/**
 * @brief Prüft die Integer-Zahlenreihe mit Zahlen, die sich exakt abbilden lassen
 */
TEST(DoubleToIntSequenceTest, checkWithExactlySequences) {
    {
        std::array<int, 6> array = DoubleToIntSequence::toIntSequence(3.5);
        ASSERT_EQ(3, array[0]);
        ASSERT_EQ(4, array[1]);
        ASSERT_EQ(3, array[2]);
        ASSERT_EQ(4, array[3]);
        ASSERT_EQ(3, array[4]);
        ASSERT_EQ(4, array[5]);
    }

    {
        std::array<int, 6> array = DoubleToIntSequence::toIntSequence(55.833333333333333);
        ASSERT_EQ(56, array[0]);
        ASSERT_EQ(56, array[1]);
        ASSERT_EQ(56, array[2]);
        ASSERT_EQ(55, array[3]);
        ASSERT_EQ(56, array[4]);
        ASSERT_EQ(56, array[5]);
    }

    {
        std::array<int, 6> array = DoubleToIntSequence::toIntSequence(17.3333333333333333);
        ASSERT_EQ(17, array[0]);
        ASSERT_EQ(17, array[1]);
        ASSERT_EQ(18, array[2]);
        ASSERT_EQ(17, array[3]);
        ASSERT_EQ(17, array[4]);
        ASSERT_EQ(18, array[5]);
    }
}

/**
 * @brief Prüft die Integer-Zahlenreihe mit Zahlen, die sich nicht so exakt abbilden lassen
 */
TEST(DoubleToIntSequenceTest, checkWithNotSoExactlySequences) {
    {
        std::array<int, 6> array = DoubleToIntSequence::toIntSequence(3.24); // ~3.16666666...
        ASSERT_EQ(3, array[0]);
        ASSERT_EQ(3, array[1]);
        ASSERT_EQ(3, array[2]);
        ASSERT_EQ(4, array[3]);
        ASSERT_EQ(3, array[4]);
        ASSERT_EQ(3, array[5]);
    }

    {
        std::array<int, 6> array = DoubleToIntSequence::toIntSequence(3.26); // ~3.333333333...
        ASSERT_EQ(3, array[0]);
        ASSERT_EQ(3, array[1]);
        ASSERT_EQ(4, array[2]);
        ASSERT_EQ(3, array[3]);
        ASSERT_EQ(3, array[4]);
        ASSERT_EQ(4, array[5]);
    }

    {
        std::array<int, 6> array = DoubleToIntSequence::toIntSequence(1234.1234); // ~1.234,1666666...
        ASSERT_EQ(1234, array[0]);
        ASSERT_EQ(1234, array[1]);
        ASSERT_EQ(1234, array[2]);
        ASSERT_EQ(1235, array[3]);
        ASSERT_EQ(1234, array[4]);
        ASSERT_EQ(1234, array[5]);
    }

    {
        std::array<int, 6> array = DoubleToIntSequence::toIntSequence(9.98417874); // ~10
        ASSERT_EQ(10, array[0]);
        ASSERT_EQ(10, array[1]);
        ASSERT_EQ(10, array[2]);
        ASSERT_EQ(10, array[3]);
        ASSERT_EQ(10, array[4]);
        ASSERT_EQ(10, array[5]);
    }
}

/**
 * @brief Prüft, ob die Zahlen gleichmäßig verteilt sind, also
 * z.&nbsp;b: { 2, 2, 1, 2, 2, 1 } statt { 1, 1, 2, 2, 2, 2 }.
 */
TEST(DoubleToIntSequenceTest, checkThatSequenceIsSplitEqually) {
    // 1/6
    {
        std::array<int, 6> array = DoubleToIntSequence::toIntSequence(55.16666666666667);
        ASSERT_EQ(55, array[0]);
        ASSERT_EQ(55, array[1]);
        ASSERT_EQ(55, array[2]);
        ASSERT_EQ(56, array[3]);
        ASSERT_EQ(55, array[4]);
        ASSERT_EQ(55, array[5]);
    }

    // 2/6
    {
        std::array<int, 6> array = DoubleToIntSequence::toIntSequence(55.33333333333333);
        ASSERT_EQ(55, array[0]);
        ASSERT_EQ(55, array[1]);
        ASSERT_EQ(56, array[2]);
        ASSERT_EQ(55, array[3]);
        ASSERT_EQ(55, array[4]);
        ASSERT_EQ(56, array[5]);
    }

    // 3/6
    {
        std::array<int, 6> array = DoubleToIntSequence::toIntSequence(55.5);
        ASSERT_EQ(55, array[0]);
        ASSERT_EQ(56, array[1]);
        ASSERT_EQ(55, array[2]);
        ASSERT_EQ(56, array[3]);
        ASSERT_EQ(55, array[4]);
        ASSERT_EQ(56, array[5]);
    }

    // 4/6
    {
        std::array<int, 6> array = DoubleToIntSequence::toIntSequence(55.66666666666667);
        ASSERT_EQ(56, array[0]);
        ASSERT_EQ(56, array[1]);
        ASSERT_EQ(55, array[2]);
        ASSERT_EQ(56, array[3]);
        ASSERT_EQ(56, array[4]);
        ASSERT_EQ(55, array[5]);
    }

    // 5/6
    {
        std::array<int, 6> array = DoubleToIntSequence::toIntSequence(55.83333333333333);
        ASSERT_EQ(56, array[0]);
        ASSERT_EQ(56, array[1]);
        ASSERT_EQ(56, array[2]);
        ASSERT_EQ(55, array[3]);
        ASSERT_EQ(56, array[4]);
        ASSERT_EQ(56, array[5]);
    }
}