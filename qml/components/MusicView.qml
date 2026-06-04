import QtQuick
import QtQuick.Layouts

// Now-playing card bound to the active MPRIS player. Album art + title/artist +
// transport controls. Equivalent of the macOS NotchHomeView music section.
Item {
    id: card

    readonly property var player: Mpris.active

    RowLayout {
        anchors.fill: parent
        spacing: 14

        // --- Album art --------------------------------------------------------
        Rectangle {
            Layout.preferredWidth: 90
            Layout.preferredHeight: 90
            Layout.alignment: Qt.AlignVCenter
            radius: 10
            color: "#1a1a1a"
            clip: true

            Image {
                anchors.fill: parent
                fillMode: Image.PreserveAspectCrop
                asynchronous: true
                cache: true
                // MPRIS artUrl can be file:// or http(s)://; Image handles both.
                source: card.player && card.player.artUrl ? card.player.artUrl : ""
                visible: status === Image.Ready
            }
            Text {
                anchors.centerIn: parent
                visible: !card.player || !card.player.artUrl
                text: "♪"; color: "#555"; font.pixelSize: 34
            }
        }

        // --- Track info + controls -------------------------------------------
        ColumnLayout {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignVCenter
            spacing: 6

            MarqueeText {
                Layout.fillWidth: true
                text: card.player ? card.player.title : "Nothing playing"
                color: "white"
                font.pixelSize: 16
                font.bold: true
            }
            Text {
                Layout.fillWidth: true
                text: card.player ? card.player.artist : ""
                color: "#aaaaaa"
                font.pixelSize: 13
                elide: Text.ElideRight
            }

            Item { Layout.fillHeight: true }

            RowLayout {
                Layout.alignment: Qt.AlignHCenter
                spacing: 26

                TransportButton { glyph: "⏮"; onClicked: card.player && card.player.previous() }
                TransportButton {
                    glyph: card.player && card.player.isPlaying ? "⏸" : "▶"
                    big: true
                    onClicked: card.player && card.player.playPause()
                }
                TransportButton { glyph: "⏭"; onClicked: card.player && card.player.next() }
            }
        }
    }

    // Inline transport button to keep the file self-contained for the MVP.
    // Uses pointer handlers (not a MouseArea) so it doesn't steal hover from the
    // root HoverHandler that keeps the notch expanded. The Item gives a generous
    // square hit area around the small glyph.
    component TransportButton: Item {
        id: btn
        property string glyph
        property bool big: false
        signal clicked

        implicitWidth: big ? 40 : 34
        implicitHeight: big ? 40 : 34
        opacity: card.player ? 1.0 : 0.3

        Text {
            anchors.centerIn: parent
            text: btn.glyph
            color: hover.hovered ? "white" : "#cccccc"
            font.pixelSize: btn.big ? 26 : 20
        }

        HoverHandler { id: hover; cursorShape: Qt.PointingHandCursor }
        TapHandler { onTapped: btn.clicked() }
    }
}
