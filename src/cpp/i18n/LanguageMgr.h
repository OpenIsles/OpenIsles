#ifndef _LANGUAGE_MGR_H
#define _LANGUAGE_MGR_H

#include <string>
#include "global.h"


/**
 * Sprach-Manager. Zuständig für die Mehrsprachigkeit.
 * Diese Klasse wird nur statisch benutzt.
 */
class LanguageMgr {

public:
    /**
     * @brief Stellt gettext für die Sprache gemäß Environment-Variablen ein.
     */
    static void initFromEnvironment();

    /**
     * @brief Übersetzt eine Phrase
     * @param phrase zu übersetzende Phrase (msgid)
     * @return übersetzter String oder `phrase`, wenn keine Übersetzung gefunden wurde.
     */
    static std::string translate(const std::string& phrase);

    /**
     * @brief Übersetzt eine Phrase
     * @param phrase zu übersetzende Phrase (msgid)
     * @return übersetzter String oder `phrase`, wenn keine Übersetzung gefunden wurde.
     */
    static std::string translate(const char* phrase);

    /**
     * @brief Übersetzt eine Phrase mit möglicher Pluralform
     * @param phraseSingular zu übersetzende Phrase im Singular (msgid)
     * @param phrasePlural zu übersetzende Phrase im Plural (msgid)
     * @param n Zahl, die über Pluralform entscheidet.
     * @return übersetzter String oder `phraseSingular`, wenn keine Übersetzung gefunden wurde.
     */
    static std::string translatePlural(const std::string& phraseSingular,
                                       const std::string& phrasePlural,
                                       unsigned long n);

private:
    /**
     * @brief Übersetzt eine Phrase mit möglicher Pluralform
     * @param phraseSingular zu übersetzende Phrase im Singular (msgid)
     * @param phrasePlural zu übersetzende Phrase im Plural (msgid)
     * @param n Zahl, die über Pluralform entscheidet.
     * @return übersetzter String oder `phraseSingular`, wenn keine Übersetzung gefunden wurde.
     */
    static std::string translatePlural(const char* phraseSingular, const char* phrasePlural, unsigned long n);
};

#endif
