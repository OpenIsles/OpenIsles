# Target: update-i18n-files
#
# Erzeugt die Übersetzungsdateien bzw. aktualisiert sie


find_package(Gettext REQUIRED)

# xgettext finden
find_program(XGETTEXT_EXECUTABLE NAMES xgettext)
if(NOT XGETTEXT_EXECUTABLE)
    message(FATAL_ERROR "xgettext was not found. It is needed to generate the .pot file.")
endif(NOT XGETTEXT_EXECUTABLE)

# msginit finden
find_program(MSGINIT_EXECUTABLE NAMES msginit)
if(NOT MSGINIT_EXECUTABLE)
    message(FATAL_ERROR "msginit was not found. It is needed to generate the .po files.")
endif(NOT MSGINIT_EXECUTABLE)

# Quelldateien zusammensuchen
foreach(source_file IN LISTS
        COMMON_SOURCE_FILES
        GUI_SOURCE_FILES
        GRAPHICS_SOURCE_FILES
        SOUND_SOURCE_FILES)

    SET(I18N_SOURCE_FILES ${I18N_SOURCE_FILES} ${source_file})
endforeach()

SET(I18N_LANGS
    en
    de
)
SET(SOURCE_PO_DIR ${CMAKE_SOURCE_DIR}/src/po)

# .pot-Datei erzeugen
add_custom_command(OUTPUT ${SOURCE_PO_DIR}/OpenIsles.pot
    COMMAND
        ${XGETTEXT_EXECUTABLE}
        --directory=${CMAKE_SOURCE_DIR}
        --c++
        --from-code=UTF-8
        --add-location=full
        --sort-by-file
        --package-name=${PROJECT_NAME}
        --package-version=${PROJECT_VERSION}
        --msgid-bugs-address=http://www.openisles.org
        --copyright-holder=theHacker
        -o ${SOURCE_PO_DIR}/OpenIsles.pot
        -k_
        ${I18N_SOURCE_FILES}
    VERBATIM
)

# .po-Dateien anlegen, falls noch nicht vorhanden
foreach(lang IN LISTS I18N_LANGS)
    add_custom_command(OUTPUT ${SOURCE_PO_DIR}/${lang}.po
        COMMAND
            ${MSGINIT_EXECUTABLE}
            -i ${SOURCE_PO_DIR}/OpenIsles.pot
            -o ${SOURCE_PO_DIR}/${lang}.po
            -l ${lang}
            --no-translator
        DEPENDS
            ${SOURCE_PO_DIR}/OpenIsles.pot
        VERBATIM
    )
endforeach(lang IN LISTS I18N_LANGS)

# .mo-Dateien kompilieren
foreach(lang IN LISTS I18N_LANGS)
    set(MO_FILE ${CMAKE_BINARY_DIR}/data/locale/${lang}/LC_MESSAGES/OpenIsles.mo)

    add_custom_command(OUTPUT ${MO_FILE}
        COMMAND
            ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/data/locale/${lang}/LC_MESSAGES
        COMMAND
            ${GETTEXT_MSGFMT_EXECUTABLE}
            ${SOURCE_PO_DIR}/${lang}.po
            -o ${MO_FILE}
        DEPENDS
            ${SOURCE_PO_DIR}/${lang}.po
        VERBATIM
    )

    SET(GENERATED_MO_FILES ${GENERATED_MO_FILES} ${MO_FILE})
endforeach(lang IN LISTS I18N_LANGS)

add_custom_target(update-i18n-files
    DEPENDS ${GENERATED_MO_FILES}
)
