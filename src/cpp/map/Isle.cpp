#include <iostream>
#include <string.h>
#include "map/Isle.h"
#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_utils.hpp"


// TODO Fehlermanagement, wenn die Datei mal nicht so hübsch aussieht, dass alle Tags da sind
Isle::Isle(const char* filename) {
	// Datei öffnen
	rapidxml::file<> tmxFile(filename);

	rapidxml::xml_document<>* xmlDocument = new rapidxml::xml_document<>();
	xmlDocument->parse<0>(tmxFile.data());

	rapidxml::xml_node<>* mapNode = xmlDocument->first_node("map");
	int isleWidth = atoi(mapNode->first_attribute("width", 5, true)->value());
	int isleHeight = atoi(mapNode->first_attribute("height", 6, true)->value());

	rapidxml::xml_node<>* layerNode = mapNode->first_node("layer");
	rapidxml::xml_node<>* dataNode = layerNode->first_node("data");

	const char* dataEncoding = dataNode->first_attribute("encoding", 8, true)->value();
	if (strcmp(dataEncoding, "csv") != 0)
		throw new std::runtime_error("Data is not csv encoded");
    
	char* csvData = dataNode->value();
	char* csvDataPtr = csvData;

	// CSV-Daten parsen und Insel laden
	char currentValue[6];
	int currentValueSize = 0;

    // Speicher anlegen
	tiles = new RectangleData<unsigned char>(isleWidth, isleHeight);

	for (int i = 0; i < isleWidth * isleHeight; i++) {
		// ein CSV-Feld lesen, d.h. bis zum nächsten Komma oder Stringende
		currentValueSize = 0;
		while (*csvDataPtr != ',' && *csvDataPtr != '\0') {
			// Nicht-Ziffern überspringen (das sind z.B. die Newlines am Zeilenende)
			if (*csvDataPtr >= '0' && *csvDataPtr <= '9') {
				currentValue[currentValueSize++] = *csvDataPtr;
			}

			csvDataPtr++;
		}
		csvDataPtr++; // Trennzeichen skippen

		// Tile zuweisen
		currentValue[currentValueSize] = '\0';
		char tile = atoi(currentValue);
		tiles->data[i] = tile;
	}

	// XML-Document wegräumen
	delete xmlDocument;
}

Isle::~Isle() {
	delete tiles;
}