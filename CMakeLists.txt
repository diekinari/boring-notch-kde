cmake_minimum_required(VERSION 3.20)

project(boring-notch-kde VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

# --- Dependencies -----------------------------------------------------------
# Qt6: Quick/Qml for the UI, DBus for MPRIS/UPower, Multimedia later for the
# audio visualizer & webcam.
find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick QuickControls2 DBus)

# LayerShellQt: lets us anchor the overlay to the top edge of the screen on
# Wayland (wlr-layer-shell). Ships with KDE / Plasma.
#   Arch package: layer-shell-qt
find_package(LayerShellQt REQUIRED)

qt_standard_project_setup(REQUIRES 6.5)

# --- Target -----------------------------------------------------------------
qt_add_executable(boring-notch-kde
    src/main.cpp
    src/NotchWindow.cpp
    src/NotchWindow.h
    src/MprisManager.cpp
    src/MprisManager.h
    src/MprisPlayer.cpp
    src/MprisPlayer.h
)

qt_add_qml_module(boring-notch-kde
    URI BoringNotch
    VERSION 1.0
    QML_FILES
        qml/Main.qml
        qml/components/MusicView.qml
        qml/components/MarqueeText.qml
)

target_link_libraries(boring-notch-kde PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::DBus
    LayerShellQt::Interface
)

install(TARGETS boring-notch-kde RUNTIME DESTINATION bin)
install(FILES data/org.theboringteam.boringnotchkde.desktop
    DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications)
