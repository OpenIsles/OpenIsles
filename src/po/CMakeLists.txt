## Erzeugt die Übersetzungsdateien bzw. aktualisiert sie ###############################################################

find_package(Gettext REQUIRED)

# xgettext finden
find_program(GETTEXT_XGETTEXT_EXECUTABLE xgettext)
if(NOT GETTEXT_XGETTEXT_EXECUTABLE)
    message(FATAL_ERROR "xgettext was not found. It is needed to generate the .pot file.")
endif(NOT GETTEXT_XGETTEXT_EXECUTABLE)

# msginit finden
find_program(GETTEXT_MSGINIT_EXECUTABLE msginit)
if(NOT GETTEXT_MSGINIT_EXECUTABLE)
    message(INFO "msginit was not found. It is needed to generate new .po files. "
                 "We don't use it in the Makefile but you will need it to create new languages.")
endif(NOT GETTEXT_MSGINIT_EXECUTABLE)

# msgcat finden
find_program(GETTEXT_MSGCAT_EXECUTABLE msgcat)
if(NOT GETTEXT_MSGCAT_EXECUTABLE)
    message(FATAL_ERROR "msgcat was not found. It is needed to generate the .pot file.")
endif(NOT GETTEXT_MSGCAT_EXECUTABLE)

# PHP finden
find_program(PHP_EXECUTABLE php)
if(NOT PHP_EXECUTABLE)
    message(FATAL_ERROR "PHP not found. We need this script interpreter for extracting phrases "
                        "out of the data XML files.")
endif(NOT PHP_EXECUTABLE)

# Quelldateien zusammensuchen
foreach(source_file IN LISTS
        MAIN_SOURCE_FILE
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
add_custom_target(update-pot-file
    # .pot-Datei OpenIsles.sources.pot aus den Sourcen generieren
    COMMAND
        ${GETTEXT_XGETTEXT_EXECUTABLE}
        --directory=${CMAKE_SOURCE_DIR}
        --c++
        --from-code=UTF-8
        --add-location=full
        --sort-by-file
        --package-name=${PROJECT_NAME}
        --package-version=${PROJECT_VERSION}
        --msgid-bugs-address=http://www.openisles.org
        --copyright-holder=theHacker
        -o ${SOURCE_PO_DIR}/OpenIsles.sources.pot
        -k_
        -k_N:1,2
        -k_NOOP
        ${I18N_SOURCE_FILES}
    # .pot-Datei OpenIsles.data.pot aus den Data-XMLs generieren
    COMMAND
        ${PHP_EXECUTABLE}
        ${SOURCE_PO_DIR}/data-xmls-to-pot.php
        ${CMAKE_SOURCE_DIR}/data
        ${SOURCE_PO_DIR}/OpenIsles.data.pot
    # Die beiden Dateien zur Zieldatei zusammensetzen
    COMMAND
        ${GETTEXT_MSGCAT_EXECUTABLE}
        -o ${SOURCE_PO_DIR}/OpenIsles.pot
        ${SOURCE_PO_DIR}/OpenIsles.sources.pot
        ${SOURCE_PO_DIR}/OpenIsles.data.pot
    # Die beiden Dateien weglöschen
    COMMAND
        ${CMAKE_COMMAND}
        -E remove
        ${SOURCE_PO_DIR}/OpenIsles.sources.pot
        ${SOURCE_PO_DIR}/OpenIsles.data.pot
    COMMENT "Updating .pot file..."
    VERBATIM
)

foreach(lang IN LISTS I18N_LANGS)
    set(PO_FILE ${SOURCE_PO_DIR}/${lang}.po)

    # .po-Datei mit der .pot-Datei mergen, um dem geänderten Code zu entsprechen
    add_custom_command(OUTPUT ${PO_FILE}
        COMMAND
            ${GETTEXT_MSGMERGE_EXECUTABLE}
            --update
            --backup=off
            --add-location=full
            --sort-by-file
            ${PO_FILE}
            ${SOURCE_PO_DIR}/OpenIsles.pot
        DEPENDS
            update-pot-file
            ${SOURCE_PO_DIR}/OpenIsles.pot
        COMMENT "Updating .po file ${PO_FILE}..."
        VERBATIM
    )

    SET(GENERATED_PO_FILES ${GENERATED_PO_FILES} ${PO_FILE})
endforeach(lang IN LISTS I18N_LANGS)

add_custom_target(update-po-files
    COMMENT "Updating .po files..."
    DEPENDS ${GENERATED_PO_FILES}
)

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
        COMMENT "Updating .mo file ${MO_FILE}..."
        VERBATIM
    )

    SET(GENERATED_MO_FILES ${GENERATED_MO_FILES} ${MO_FILE})
endforeach(lang IN LISTS I18N_LANGS)

add_custom_target(update-mo-files ALL
    DEPENDS ${GENERATED_MO_FILES}
    COMMENT "Updating .mo files..."
)
