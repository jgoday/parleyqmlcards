project(parleyqmlcards)

find_package(KDE4 REQUIRED)
include (KDE4Defaults)

include_directories (${KDE4_INCLUDES}
                     ${KDE4WORKSPACE_INCLUDE_DIR})

set(parleyqmlcards_SRCS
    parleyqmlcards.cpp
)

kde4_add_ui_files(parleyqmlcards_SRCS
    config.ui
)

kde4_add_plugin(plasma_applet_parleyqmlcards ${parleyqmlcards_SRCS})
target_link_libraries(plasma_applet_parleyqmlcards
    ${KDE4_KDEUI_LIBS}
    ${KDE4_KIO_LIBS}
    ${KDE4_PLASMA_LIBS}
    ${QT_QTDECLARATIVE_LIBRARY})

install(TARGETS plasma_applet_parleyqmlcards DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES parleyqmlcards.desktop
        DESTINATION ${SERVICES_INSTALL_DIR})

install(
	DIRECTORY package/
	DESTINATION ${DATA_INSTALL_DIR}/plasma/packages/parleyqmlcards)
