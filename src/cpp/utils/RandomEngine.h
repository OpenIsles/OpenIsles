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

    virtual unsigned long operator()() = 0;
    virtual unsigned long min() = 0;
    virtual unsigned long max() = 0;
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
    virtual unsigned long operator()() {
        return generator();
    }

    virtual unsigned long min() {
        return generator.min();
    }

    virtual unsigned long max() {
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
        virtual unsigned long operator()() {
            unsigned long returnValue = nextNumber;
            nextNumber = (nextNumber + 1) % 10000000;
            return returnValue;
        }

        virtual unsigned long min() {
            return 0;
        }

        virtual unsigned long max() {
            return 9999999;
        }
    };
#endif

#endif