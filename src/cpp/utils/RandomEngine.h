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
        assert((probability >= 0) && (probability <= 1));

        result_type minValue = min();
        result_type maxValue = max();
        result_type randomValue = operator()();

        result_type threshold = result_type(double(minValue) + double(maxValue - minValue) * probability);
        return (randomValue <= threshold);
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
     * Die Engine ist nicht wirklich zufällig, sondern zählt einfach nur immer um eins nach oben
     */
    class TestRandomEngine : public IRandomEngine {
    private:
        /**
         * @brief nächste "Zufalls"zahl, die zurückgeliefert werden wird
         */
        unsigned long nextNumber;

    public:
        virtual result_type operator()() {
            result_type returnValue = nextNumber;
            nextNumber = (nextNumber + 1) % 10000000;
            return returnValue;
        }

        virtual result_type min() {
            return 0;
        }

        virtual result_type max() {
            return 9999999;
        }
    };
#endif

#endif