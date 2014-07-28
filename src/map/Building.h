#ifndef _BUILDING_H
#define _BUILDING_H

#include "MapObject.h"

/**
 * @brief MapObject, das ein Gebäude darstellt
 */
class Building : public MapObject {

private:
	/**
	 * @brief Objekt-Typ, d.h. Index in GraphicsMgr#objects
	 * TODO kann man sicher noch besser benennen. Vielleicht Referenz auf die Grafik?
	 */
	unsigned char object;

public:
	unsigned char getObject() const {
		return object;
	}

	void setObject(unsigned char object) {
		this->object = object;
	}

	/**
	 * @brief Callback, der sich um einen Mausklick auf das Gebäude kümmert
	 * @param mouseXInBuilding X-Koordinate innerhalb des Gebäude, wo geklickt wurde
	 * @param mouseYInBuilding Y-Koordinate innerhalb des Gebäude, wo geklickt wurde
	 */
	void onClick(int mouseXInBuilding, int mouseYInBuilding);
};

#endif
