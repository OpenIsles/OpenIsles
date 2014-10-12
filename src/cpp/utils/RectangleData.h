#ifndef _RECTANGLE_DATA_H
#define	_RECTANGLE_DATA_H

/**
 * @brief Hilfsklasse, die Daten innerhalb eines Rechtecks mit definierter Breite und Höhe hält.
 */
template <typename T>
struct RectangleData {
    /**
     * Breite des Rechtecks
     */
    int width;
    
    /**
     * Höhe des Rechtecks
     */
    int height;
    
    /**
     * Nutzlast
     */
    T* data;
    
    RectangleData(int width, int height) : width(width), height(height) {
        data = new T[width * height];
    }
    
    ~RectangleData() {
        delete[] data;
    }
    
    /**
     * @brief Helper-Methode, um einen bestimmten Eintrag aus dem Array zu lesen.
     * Diese Methode kann sicher benutzt werden, um mit beliebigen Koordinaten zuzugreifen. Befinden sich die
     * Koordinaten außerhalb des Rechtecks, wird ein bestimmter Wert zurückgeliefert.
     * @param x X-Koordinate im Array
     * @param y Y-Koordinate im Array
     * @param outOfArrayValue Wert, der zurückgegeben wird, wenn die Koordinate außerhalb des Rechtecks liegt.
     * @return Wert
     */
    T getData(int x, int y, T outOfArrayValue) const {
        if (x < 0 || y < 0 || x >= width || y >= height) {
            return outOfArrayValue;
        }
        
        return data[y * width + x];
    }

    /**
    * @brief Helper-Methode, um einen bestimmten Eintrag im Array zu setzen.
    * Diese Methode kann sicher benutzt werden, um mit beliebigen Koordinaten zuzugreifen. Befinden sich die
    * Koordinaten außerhalb des Rechtecks, ist dies eine No-Op.
    * @param x X-Koordinate im Array
    * @param y Y-Koordinate im Array
    * @param value Wert, der gesetzt werden soll
    */
    void setData(int x, int y, T value) {
        if (x < 0 || y < 0 || x >= width || y >= height) {
            return;
        }

        data[y * width + x] = value;
    }
    
};

#endif