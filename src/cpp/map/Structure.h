#ifndef _STRUCTURE_H
#define _STRUCTURE_H

#include "MapObject.h"

/**
 * @brief MapObject, das eine Struktur darstellt. Strukturen sind Wege und Felder, also Sachen nicht klickbar sind.
 */
class Structure : public MapObject {

protected:
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

};

#endif
