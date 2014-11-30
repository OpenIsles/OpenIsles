#ifndef _FPS_COUNTER_H
#define	_FPS_COUNTER_H

#include <SDL.h>
#include <list>

/**
 * @brief Klasse, die sich um die Berechnung der FPS kümmert. Sie speichert die letzten x Werte und berechnet uns
 * einen Durchschnitt. Wir rufen Methoden auf, wenn wir ein Frame starten und wenn wir fertig sind, den Rest macht
 * die Klasse.
 */
class FpsCounter {
    
private:
    /**
     * @brief Anzahl Zeiten, die für die Berechnung verwendet werden
     */
    unsigned int maxSamplesCount;
    
    /**
     * @brief Liste mit den gemessenen Zeiten
     */
    std::list<Uint32> samples;
    
    /**
     * @brief Summe aller Element in der Liste. Wir wollen für die FPS-Berechnung nicht jedes Mal die Liste
     * durchloopen und die Summe bilden.
     */
    Uint32 sumSamples = 0;
    
    /**
     * @brief Zeitstempel bei Start des Frames
     */
    Uint32 currentFrameStartTime;
    
public:
    /**
     * Konstruktor.
     * @param maxSamplesCount Anzahl an Zeiten, die für den Durchschnitt verwendet werden sollen
     */
    FpsCounter(unsigned int maxSamplesCount) : maxSamplesCount(maxSamplesCount) {
    }
    
    /**
     * @brief Muss aufgerufen werden, wenn wir mit dem Frame anfangen
     */
    void startFrame() {
        currentFrameStartTime = SDL_GetTicks();
    }
    
    /**
     * @brief Muss aufgerufen werden, wenn wir mit dem Frame fertig sind.
     */
    void endFrame() {
        Uint32 currentFrameEndTime = SDL_GetTicks();
        Uint32 newSample = currentFrameEndTime - currentFrameStartTime;
        
        if (samples.size() == maxSamplesCount) {
            Uint32 frontSample = samples.front();
            
            samples.pop_front();
            sumSamples -= frontSample;
        }
        
        samples.push_back(newSample);
        sumSamples += newSample;
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
        
        Uint32 latestSample = samples.back();
        return 1000.0 / (double) latestSample;
    }
    
};

#endif