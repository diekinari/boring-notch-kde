import QtQuick
import QtQuick.Window
import BoringNotch
import "components"

// The notch overlay. Collapsed it is a slim pill hugging the top edge; on hover
// (or while music is playing) it expands to reveal the music view. Mirrors the
// open/close behaviour of the macOS BoringNotchWindow, minus the physical notch.
Window {
    id: root

    readonly property int collapsedWidth: 220
    readonly property int collapsedHeight: 32
    readonly property int expandedWidth: 420
    readonly property int expandedHeight: 160

    property bool expanded: hoverArea.containsMouse

    width: expanded ? expandedWidth : collapsedWidth
    height: expanded ? expandedHeight : collapsedHeight
    visible: true
    color: "transparent"
    flags: Qt.FramelessWindowHint

    Behavior on width  { NumberAnimation { duration: 250; easing.type: Easing.OutCubic } }
    Behavior on height { NumberAnimation { duration: 250; easing.type: Easing.OutCubic } }

    Rectangle {
        id: shell
        anchors.fill: parent
        color: "#000000"
        // Square top (flush with screen edge), rounded bottom — notch silhouette.
        topLeftRadius: 0
        topRightRadius: 0
        bottomLeftRadius: 18
        bottomRightRadius: 18

        MouseArea {
            id: hoverArea
            anchors.fill: parent
            hoverEnabled: true
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
