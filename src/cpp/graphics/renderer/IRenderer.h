#ifndef _I_RENDERER_H
#define _I_RENDERER_H

#include "utils/Color.h"
#include "utils/Rect.h"


/**
 * @brief Interface für den Renderer, der alles übernimmt, was mit Zeichnen zu tun hat.
 * Damit wird es uns möglich, die Anwendung auch ohne SDL für die Tests zu kompilieren.
 */
class IRenderer {

public:
    static const int BLENDMODE_NONE  = (1 << 0);
    static const int BLENDMODE_BLEND = (1 << 1);
    static const int BLENDMODE_ADD   = (1 << 2);
    static const int BLENDMODE_MOD   = (1 << 3);

public:
    virtual ~IRenderer() {}

    /**
     * @brief Liefert die Breite des Fensters zurück.
     * @return Fensterbreite in Pixeln
     */
    virtual const int getWindowWidth() const = 0;

    /**
     * @brief Liefert die Höhe des Fensters zurück.
     * @return Fensterhöhe in Pixeln
     */
    virtual const int getWindowHeight() const = 0;

    /**
     * @brief Zeigt das Fenster an und bringt es in den Vordergrund.
     * Initial wird das Fenster nicht gezeigt.
     */
    virtual void showWindow() = 0;

    /**
     * @brief Setzt das Clipping-Rechteck, in welchem der Renderer zeichnet.
     * @param rect neues Clipping-Rechteck oder `nullptr`, um Clipping zu deaktivieren
     */
    virtual void setClipRect(const Rect* const rect) = 0;

    /**
     * @brief Setzt die Zeichenfarbe des Renderers
     * @param color Farbe
     */
    virtual void setDrawColor(const Color& color) = 0;

    /**
     * @brief Setzt den Überblendmodus für das Zeichnen des Renderers
     * @param blendMode eine der BLENDMODE_*-Konstanten
     */
    virtual void setDrawBlendMode(int blendMode) = 0;

    /**
     * @brief Zeichnet eine Linie
     * @param x1 Start-X-Koordinate
     * @param y1 Start-Y-Koordinate
     * @param x2 End-X-Koordinate
     * @param y2 End-Y-Koordinate
     */
    virtual void drawLine(int x1, int y1, int x2, int y2) = 0;

    /**
     * @brief Zeichnet ein Rechteck
     * @param rect Rechteck
     */
    virtual void drawRect(const Rect& rect) = 0;

    /**
     * @brief Zeichnet ein ausgemaltes Rechteck
     * @param rect Rechteck
     */
    virtual void fillRect(const Rect& rect) = 0;

    /**
     * @brief Kontrolliert, wie sich neu angelegte Texturen verwalten, wenn sie skaliert werden.
     * Siehe SDL_HINT_RENDER_SCALE_QUALITY
     *
     * @param scaleQuality "0"/"nearest", "1"/"linear", "2"/"best"
     */
    virtual void setHintRenderScaleQuality(const char* scaleQuality) = 0;

    /**
     * @brief Bereitet das Zeichnen eines neuen Frames vor: Der Renderer erhält als Ziel den Backbuffer,
     * und löscht den Hintergrund.
     */
    virtual void startFrame() = 0;

    /**
     * @brief Beendet das Zeichnen eines Frames vor: Der Renderer kopiert den Backbuffer in das sichtbare Fenster.
     */
    virtual void endFrame() = 0;

};

#endif
