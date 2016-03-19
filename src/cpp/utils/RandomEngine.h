#ifndef _RANDOM_GENERATOR_H
#define _RANDOM_GENERATOR_H

#include <random>

/**
 * @brief Interface für Zufallszahlengenerator-Engines
 */
class IRandomEngine {

public:
    virtual ~IRandomEngine() {}

    // Zu definiertende Operationen:
    // @see http://www.cplusplus.com/reference/random/default_random_engine/

    typedef unsigned long result_type;

    virtual result_type operator()() = 0;
    virtual result_type min() = 0;
    virtual result_type max() = 0;

    /**
     * @brief Liefert zufällig einen bool-Wert zurück, der mit einer bestimmten Wahrscheinlich `true` ist.
     * @param probability Wahrscheinlichkeit (Bereich 0 bis 1), mit der `true` zurückkommt
     * @return `true` zu einer Wahrscheinlichkeit von `probability`,
     *         `false` zu einer Wahrscheinlichkeit von `1 - probability`,
     */
    bool getBoolByProbability(double probability) {
        std::uniform_real_distribution<double> distribution(0, 1);
        double randomValue = distribution(*this);
        return (randomValue <= probability);
    }
};


/**
 * @brief echte Zufallsgenerator-Engine
 */
class RealRandomEngine : public IRandomEngine {

private:
    /**
     * @brief Zufallsengine
     */
    std::default_random_engine generator;

public:
    virtual result_type operator()() {
        return generator();
    }

    virtual result_type min() {
        return generator.min();
    }

    virtual result_type max() {
        return generator.max();
    }
};


#ifdef IN_TESTS
    /**
     * @brief Zufallsgenerator-Engine, die zum Testen benutzt wird.
     */
    class TestRandomEngine : public IRandomEngine {
    private:
        /**
         * @brief Zufallsengine
         */
        std::linear_congruential_engine<unsigned long, 9997, 4711, 99999989> generator; // 99999989 ist prim

    public:
        virtual result_type operator()() {
            return generator();
        }

        virtual result_type min() {
            return generator.min();
        }

        virtual result_type max() {
            return generator.max();
        }
    };
#endif

#endif