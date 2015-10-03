#!/bin/bash

# Führt cloc aus, um die Quellcodezeilen zu zählen.
# Dieses Script muss aus dem Projekt-Rootverzeichnis aufgerufen werden.

cloc --exclude-dir=.idea,doc,doxygen,src/cpp/rapidxml .