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
        color: "#000000"
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

        // Collapsed content: tiny now-playing hint.
        Row {
            anchors.centerIn: parent
            spacing: 8
            visible: !root.expanded
            opacity: visible ? 1 : 0
            Behavior on opacity { NumberAnimation { duration: 150 } }

            Rectangle {
                width: 6; height: 6; radius: 3
                anchors.verticalCenter: parent.verticalCenter
                color: Mpris.hasPlayer && Mpris.active && Mpris.active.isPlaying
                       ? "#1db954" : "#555555"
            }
            Text {
                anchors.verticalCenter: parent.verticalCenter
                color: "#dddddd"
                font.pixelSize: 12
                elide: Text.ElideRight
                width: 150
                text: Mpris.hasPlayer && Mpris.active
                      ? (Mpris.active.title.length ? Mpris.active.title : Mpris.active.identity)
                      : "boring.notch"
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
    }
}
