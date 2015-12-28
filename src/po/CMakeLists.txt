# Target: update-i18n-files
#
# Erzeugt die Übersetzungsdateien bzw. aktualisiert sie


find_package(Gettext REQUIRED)

# xgettext finden
find_program(GETTEXT_XGETTEXT_EXECUTABLE NAMES xgettext)
if(NOT GETTEXT_XGETTEXT_EXECUTABLE)
    message(FATAL_ERROR "xgettext was not found. It is needed to generate the .pot file.")
endif(NOT GETTEXT_XGETTEXT_EXECUTABLE)

# msginit finden
find_program(GETTEXT_MSGINIT_EXECUTABLE NAMES msginit)
if(NOT GETTEXT_MSGINIT_EXECUTABLE)
    message(INFO "msginit was not found. It is needed to generate new .po files. "
                 "We don't use it in the Makefile but you will need it to create new languages.")
endif(NOT GETTEXT_MSGINIT_EXECUTABLE)

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
        -o ${SOURCE_PO_DIR}/OpenIsles.pot
        -k_
        ${I18N_SOURCE_FILES}
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
        VERBATIM
    )

    SET(GENERATED_PO_FILES ${GENERATED_PO_FILES} ${PO_FILE})
endforeach(lang IN LISTS I18N_LANGS)

add_custom_target(update-po-files
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
        VERBATIM
    )

    SET(GENERATED_MO_FILES ${GENERATED_MO_FILES} ${MO_FILE})
endforeach(lang IN LISTS I18N_LANGS)

add_custom_target(update-mo-files
    DEPENDS ${GENERATED_MO_FILES}
)
