#ifndef _FPS_COUNTER_H
#define _FPS_COUNTER_H

#include <SDL.h>
#include <list>

/**
 * @brief Klasse, die sich um die Zeitmessung und Berechnung der FPS kümmert. Sie speichert die letzten x Werte
 * vergangener Zeit und berechnet uns einen Durchschnitt.
 * Alle Zeitangaben sind in Millisekunden.
 *
 * Zur Berechnung muss jeweils einmal start() und zum Schluss end() aufgerufen werden, den Rest macht die Klasse.
 * Es können dann der Durchschnittswert und eine FPS-Angabe (=Inversangabe auf 1000 Millisekunden) ausgelesen werden.
 */
class PerformanceCounter {
    
private:
    /**
     * @brief Anzahl Zeiten, die für die Berechnung verwendet werden
     */
    unsigned int maxSamplesCount;
    
    /**
     * @brief Liste mit den gemessenen Zeiten
     */
    std::list<uint32_t> samples;
    
    /**
     * @brief Summe aller Elemente in der Liste. Wir wollen für die FPS-Berechnung nicht jedes Mal die Liste
     * durchloopen und die Summe bilden.
     */
    uint32_t sumSamples = 0;
    
    /**
     * @brief Zeitstempel, als start() aufgerufen wurde.
     */
    uint32_t currentStartTime;
    
public:
    /**
     * Konstruktor.
     *
     * @param maxSamplesCount Anzahl an Zeiten, die für den Durchschnitt verwendet werden sollen
     */
    PerformanceCounter(unsigned int maxSamplesCount) : maxSamplesCount(maxSamplesCount) {
    }


    /**
     * @brief Muss aufgerufen werden, wenn die Messung beginnt.
     */
    void start() {
        currentStartTime = SDL_GetTicks();
    }
    
    /**
     * @brief Muss aufgerufen werden, wenn die Messung fertig ist.
     */
    void end() {
        uint32_t currentEndTime = SDL_GetTicks();
        uint32_t newSample = currentEndTime - currentStartTime;
        
        if (samples.size() == maxSamplesCount) {
            uint32_t frontSample = samples.front();
            
            samples.pop_front();
            sumSamples -= frontSample;
        }
        
        samples.push_back(newSample);
        sumSamples += newSample;
    }


    /**
     * @brief Liefert die durchschnittliche Zeit zurück.
     * @return durchschnittliche Zeit in Millisekunden
     */
    double getMillisAvg() const {
        int countSamples = samples.size();
        if (countSamples == 0) {
            return 0.0;
        }

        return (double) sumSamples / (double) countSamples;
    }

    /**
     * @brief Liefert die Zeit basierend vom letzten Durchlauf zurück.
     * @return aktuelle Zeit in Millisekunden
     */
    double getMillisCurrent() const {
        int countSamples = samples.size();
        if (countSamples == 0) {
            return 0.0;
        }

        uint32_t latestSample = samples.back();
        return (double) latestSample;
    }


    /**
     * @brief Liefert die durchschnittliche FPS zurück.
     * @return durchschnittliche FPS
     */
    double getFpsAvg() const {
        int countSamples = samples.size();
        if (countSamples == 0) {
            return 0.0;
        }
        
        return 1000.0 / ((double) sumSamples / (double) countSamples);
    }
    
    /**
     * @brief Liefert die FPS basierend vom letzten Frame zurück.
     * @return aktuelle FPS
     */
    double getFpsCurrent() const {
        int countSamples = samples.size();
        if (countSamples == 0) {
            return 0.0;
        }

        uint32_t latestSample = samples.back();
        return 1000.0 / (double) latestSample;
    }
    
};

#endif