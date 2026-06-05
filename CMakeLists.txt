cmake_minimum_required(VERSION 3.20)

project(boring-notch-kde VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

# --- Dependencies -----------------------------------------------------------
# Qt6: Quick/Qml for the UI, DBus for MPRIS/UPower, Multimedia later for the
# audio visualizer & webcam.
# Widgets is pulled in for the system-tray icon (QSystemTrayIcon / QMenu).
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Qml Quick QuickControls2 DBus)

# LayerShellQt: lets us anchor the overlay to the top edge of the screen on
# Wayland (wlr-layer-shell). Ships with KDE / Plasma.
#   Arch package: layer-shell-qt
find_package(LayerShellQt REQUIRED)

# KF6 KWindowSystem: KWindowEffects::enableBlurBehind for the liquid-glass blur.
#   Arch package: kwindowsystem
find_package(KF6WindowSystem REQUIRED)

qt_standard_project_setup(REQUIRES 6.5)

# --- Target -----------------------------------------------------------------
qt_add_executable(boring-notch-kde
    src/main.cpp
    src/NotchWindow.cpp
    src/NotchWindow.h
    src/NotchManager.cpp
    src/NotchManager.h
    src/MprisManager.cpp
    src/MprisManager.h
    src/MprisPlayer.cpp
    src/MprisPlayer.h
    src/AppSettings.cpp
    src/AppSettings.h
    src/AppController.cpp
    src/AppController.h
)

# Tr.qml is a QML singleton (pragma Singleton) — mark it so the module's
# generated qmldir exposes it as a singleton type.
set_source_files_properties(qml/Tr.qml PROPERTIES QT_QML_SINGLETON_TYPE TRUE)

qt_add_qml_module(boring-notch-kde
    URI BoringNotch
    VERSION 1.0
    QML_FILES
        qml/Notch.qml
        qml/SettingsWindow.qml
        qml/Tr.qml
        qml/components/MusicView.qml
        qml/components/MarqueeText.qml
        qml/components/Equalizer.qml
)

target_link_libraries(boring-notch-kde PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::DBus
    LayerShellQt::Interface
    KF6::WindowSystem
)

include(GNUInstallDirs)

install(TARGETS boring-notch-kde RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

# Desktop entry -> shows up in the KDE application launcher.
install(FILES data/org.theboringteam.boringnotchkde.desktop
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/applications)

# App icon -> used by the launcher entry and the system-tray icon
# (Icon=boring-notch-kde resolves to this themed icon).
install(FILES data/boring-notch-kde.svg
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/scalable/apps)
