#include <clocale>
#include "i18n/LanguageMgr.h"


void LanguageMgr::initFromEnvironment() {
#ifdef WINDOWS
    const char* newLocale = setlocale(LC_ALL, "");
#else
    const char* newLocale = std::setlocale(LC_ALL, "");
#endif
    if (newLocale != nullptr) {
        Log::info(_("Using locale '%s'."), newLocale);
    }

    bindtextdomain("OpenIsles", "data/locale"); // TODO CMake ${PROJECT_NAME} und eine Variable für das Locale-Dir nutzen
    bind_textdomain_codeset("OpenIsles", "UTF-8");
    textdomain("OpenIsles");
}

std::string LanguageMgr::translate(const std::string& phrase) {
    return translate(phrase.c_str());
}

std::string LanguageMgr::translate(const char* phrase) {
    // std::string(const char*) kopiert das Ergebnis von gettext() ab, sodass wir garantiert keine bösen Zeiger kriegen,
    // auch wenn gettext() nix findet und wieder den internen Zeiger von phrase zurückliefert.

    return gettext(phrase);
}

std::string LanguageMgr::translatePlural(const std::string& phraseSingular, const std::string& phrasePlural,
                                         unsigned long n) {

    return translatePlural(phraseSingular.c_str(), phrasePlural.c_str(), n);
}

std::string LanguageMgr::translatePlural(const char* phraseSingular, const char* phrasePlural,
                                         unsigned long n) {

    // std::string(const char*) kopiert das Ergebnis von ngettext() ab, sodass wir garantiert keine bösen Zeiger kriegen,
    // auch wenn ngettext() nix findet und wieder den internen Zeiger von phrase zurückliefert.

    return ngettext(phraseSingular, phrasePlural, n);
}