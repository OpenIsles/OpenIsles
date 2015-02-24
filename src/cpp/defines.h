#ifndef _DEFINES_H
#define _DEFINES_H

// Bestimmte Methoden sind nur dann virtuell, wenn wir die Tests kompilieren. Wir sparen uns für die richtige
// Anwendung den vtable-Overhead :-)
#ifdef DEFINE_VIRTUAL_ONLY_IN_TESTS
#define VIRTUAL_ONLY_IN_TESTS virtual
#else
#define VIRTUAL_ONLY_IN_TESTS
#endif

#endif