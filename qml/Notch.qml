import QtQuick
import QtQuick.Window
import QtQuick.Controls
import BoringNotch
import "components"

// A single notch overlay. One instance is created per target screen by the
// C++ NotchManager, which assigns it to its screen and configures the
// layer-shell surface before showing it (hence visible:false here). Collapsed
// it is a slim pill hugging the top edge; on hover it expands to the music view.
Window {
    id: root

    // Dimensions come from settings (defaults mirror the macOS original:
    // a real MacBook notch is ~185 wide x 32 tall; open panel 640 x 190).
    readonly property int collapsedWidth: Config.closedNotchWidth
    readonly property int collapsedHeight: Config.closedNotchHeight
    readonly property int expandedWidth: Config.openNotchWidth
    readonly property int expandedHeight: Config.openNotchHeight

    // HoverHandler (not a MouseArea) so that hovering child controls doesn't
    // steal the hover and collapse us: handlers report hover independently,
    // so the root stays hovered while the cursor is anywhere inside the window.
    property bool expanded: notchHover.hovered

    width: expanded ? expandedWidth : collapsedWidth
    height: expanded ? expandedHeight : collapsedHeight
    visible: false   // shown by NotchManager once placed on its screen
    color: "transparent"
    flags: Qt.FramelessWindowHint

    Behavior on width  { NumberAnimation { duration: 250; easing.type: Easing.OutCubic } }
    Behavior on height { NumberAnimation { duration: 250; easing.type: Easing.OutCubic } }

    Rectangle {
        id: shell
        anchors.fill: parent
        // Solid black normally; translucent dark "glass" when liquid glass is on
        // (the desktop behind is blurred by KWin — see NotchWindow::setGlass).
        // Fill opacity is configurable (Config.glassOpacity, percent).
        color: Config.liquidGlass ? Qt.rgba(0.04, 0.04, 0.06, Config.glassOpacity / 100)
                                  : "#000000"
        // Square top (flush with screen edge), rounded bottom — notch silhouette.
        // Bottom radius matches the macOS original: 14 closed, 24 open.
        topLeftRadius: 0
        topRightRadius: 0
        bottomLeftRadius: root.expanded ? Config.openCornerRadius : Config.closedCornerRadius
        bottomRightRadius: root.expanded ? Config.openCornerRadius : Config.closedCornerRadius
        Behavior on bottomLeftRadius  { NumberAnimation { duration: 250; easing.type: Easing.OutCubic } }
        Behavior on bottomRightRadius { NumberAnimation { duration: 250; easing.type: Easing.OutCubic } }

        HoverHandler {
            id: notchHover
        }

        // Right-click anywhere on the notch -> context menu (like macOS).
        TapHandler {
            acceptedButtons: Qt.RightButton
            onTapped: contextMenu.popup()
        }

        Menu {
            id: contextMenu
            MenuItem { text: qsTr("Settings…"); onTriggered: App.requestSettings() }
            MenuSeparator {}
            MenuItem { text: qsTr("Quit"); onTriggered: App.quit() }
        }

        // Settings gear, shown in the expanded notch when enabled.
        Text {
            anchors.top: parent.top
            anchors.right: parent.right
            anchors.topMargin: 8
            anchors.rightMargin: 12
            visible: root.expanded && Config.settingsIconInNotch
            text: "⚙"
            font.pixelSize: 16
            color: gearHover.hovered ? "white" : "#888888"
            HoverHandler { id: gearHover; cursorShape: Qt.PointingHandCursor }
            TapHandler { onTapped: App.requestSettings() }
        }

        // Collapsed content: macOS-style live activity — album art on the left,
        // an audio visualizer on the right, flanking the (virtual) notch centre.
        Item {
            anchors.fill: parent
            visible: !root.expanded
            opacity: visible ? 1 : 0
            Behavior on opacity { NumberAnimation { duration: 150 } }

            readonly property bool hasPlayer: Mpris.hasPlayer && Mpris.active

            // Album art (left)
            Rectangle {
                id: closedArt
                anchors.left: parent.left
                anchors.leftMargin: 10
                anchors.verticalCenter: parent.verticalCenter
                width: Math.max(0, parent.height - 12)
                height: width
                radius: 5
                color: "#1a1a1a"
                clip: true
                visible: parent.hasPlayer

                Image {
                    anchors.fill: parent
                    fillMode: Image.PreserveAspectCrop
                    asynchronous: true
                    cache: true
                    source: Mpris.active ? (Mpris.active.artUrl || "") : ""
                    visible: status === Image.Ready
                }
                Text {
                    anchors.centerIn: parent
                    visible: !Mpris.active || !Mpris.active.artUrl
                    text: "♪"; color: "#555"; font.pixelSize: parent.height * 0.5
                }
            }

            // Visualizer (right)
            Equalizer {
                anchors.right: parent.right
                anchors.rightMargin: 12
                anchors.verticalCenter: parent.verticalCenter
                height: Math.max(6, parent.height * 0.5)
                visible: parent.hasPlayer
                active: parent.hasPlayer && Mpris.active.isPlaying
            }

            // Idle fallback when nothing is playing.
            Text {
                anchors.centerIn: parent
                visible: !parent.hasPlayer
                color: "#dddddd"
                font.pixelSize: 12
                text: "boring.notch"
            }
        }

        // Expanded content: full music view.
        MusicView {
            anchors.fill: parent
            anchors.margins: 14
            visible: root.expanded
            opacity: visible ? 1 : 0
            Behavior on opacity { NumberAnimation { duration: 200 } }
        }

        // Liquid-glass styling on top (sheen / specular / rim). Drawn last so the
        // rim sits above content; only visible when the option is enabled.
        GlassOverlay {
            anchors.fill: parent
            visible: Config.liquidGlass
            bottomRadius: root.expanded ? Config.openCornerRadius : Config.closedCornerRadius
            sheen: Config.glassSheen / 100
            rim: Config.glassRim / 100
        }
    }
}
