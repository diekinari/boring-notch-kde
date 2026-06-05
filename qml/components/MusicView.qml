import QtQuick
import QtQuick.Layouts
import BoringNotch

// Now-playing card bound to the active MPRIS player: album art, title/artist,
// a live seek bar, transport controls, and (when several players are running)
// a switcher. Equivalent of the macOS NotchHomeView music section.
Item {
    id: card

    readonly property var player: Mpris.active
    readonly property real lengthUs: player ? player.length : 0

    // Live playback position in microseconds. Polled while playing (MPRIS has no
    // continuous position signal), and refreshed on track/player changes.
    property real positionUs: 0

    function refreshPosition() {
        card.positionUs = card.player ? card.player.position() : 0;
    }
    function fmt(us) {
        if (us <= 0) return "0:00";
        var total = Math.floor(us / 1000000);
        var m = Math.floor(total / 60);
        var s = total % 60;
        return m + ":" + (s < 10 ? "0" + s : s);
    }

    Timer {
        interval: 500; repeat: true
        running: card.player && card.player.isPlaying && !seek.dragging
        onTriggered: card.refreshPosition()
    }
    Connections {
        target: card.player
        ignoreUnknownSignals: true
        function onMetadataChanged() { card.refreshPosition() }
        function onPlaybackChanged() { card.refreshPosition() }
    }
    Connections {
        target: Mpris
        function onActiveChanged() { card.refreshPosition() }
    }

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

        // --- Track info + seek + controls ------------------------------------
        ColumnLayout {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignVCenter
            spacing: 6

            // Player switcher chips: one per running player, active highlighted.
            // Inline (not a popup) so nothing escapes the small notch surface.
            Flow {
                Layout.fillWidth: true
                visible: Mpris.playerCount > 1
                spacing: 6
                Repeater {
                    model: card.playerList
                    delegate: Rectangle {
                        id: chip
                        required property var modelData
                        radius: 9
                        height: 18
                        width: chipLabel.implicitWidth + 16
                        color: modelData.isActive ? "#1db954"
                                                  : (chipHover.hovered ? "#4a4a4a" : "#3a3a3a")
                        Text {
                            id: chipLabel
                            anchors.centerIn: parent
                            text: chip.modelData.identity
                            color: "white"
                            font.pixelSize: 10
                        }
                        HoverHandler { id: chipHover; cursorShape: Qt.PointingHandCursor }
                        TapHandler { onTapped: Mpris.activate(chip.modelData.serviceName) }
                    }
                }
            }

            MarqueeText {
                Layout.fillWidth: true
                text: card.player ? card.player.title : Tr.t("Nothing playing")
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

            // Seek bar with current / total time.
            RowLayout {
                Layout.fillWidth: true
                spacing: 8
                visible: card.lengthUs > 0
                Text {
                    text: card.fmt(seek.dragging ? seek.shown * card.lengthUs : card.positionUs)
                    color: "#9a9a9a"; font.pixelSize: 10
                }
                SeekBar {
                    id: seek
                    Layout.fillWidth: true
                    position: card.lengthUs > 0 ? card.positionUs / card.lengthUs : 0
                    onSeekRequested: function (fraction) {
                        if (card.player && card.lengthUs > 0) {
                            var us = Math.round(fraction * card.lengthUs);
                            card.player.setPosition(us);
                            card.positionUs = us;
                        }
                    }
                }
                Text {
                    text: card.fmt(card.lengthUs)
                    color: "#9a9a9a"; font.pixelSize: 10
                }
            }

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

    // Player list for the switcher chips, refreshed when players come/go or the
    // active one changes.
    property var playerList: Mpris.playerInfos()
    Connections {
        target: Mpris
        function onPlayersChanged() { card.playerList = Mpris.playerInfos() }
        function onActiveChanged() { card.playerList = Mpris.playerInfos() }
    }

    // Inline transport button. Uses pointer handlers (not a MouseArea) so it
    // doesn't steal hover from the root HoverHandler that keeps the notch open.
    // The Item gives a generous square hit area around the small glyph.
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

    // Inline seek bar. `position` (0..1) is the playback fraction from outside;
    // while dragging it shows the drag fraction instead and emits seekRequested
    // on release. Pointer handlers keep the notch from collapsing mid-drag.
    component SeekBar: Item {
        id: bar
        property real position: 0
        property bool dragging: false
        property real dragFraction: 0
        readonly property real shown: dragging ? dragFraction
                                               : Math.max(0, Math.min(1, position))
        signal seekRequested(real fraction)

        implicitHeight: 14
        Layout.preferredHeight: 14

        function fractionAt(x) {
            return Math.max(0, Math.min(1, width > 0 ? x / width : 0));
        }

        Rectangle {
            id: track
            anchors.verticalCenter: parent.verticalCenter
            width: parent.width
            height: 4
            radius: 2
            color: "#3a3a3a"

            Rectangle {
                height: parent.height
                radius: 2
                color: "#1db954"
                width: parent.width * bar.shown
            }
            // Draggable knob.
            Rectangle {
                width: 10; height: 10; radius: 5
                color: "white"
                y: (parent.height - height) / 2
                x: parent.width * bar.shown - width / 2
                visible: bar.dragging || knobHover.hovered
            }
        }

        HoverHandler { id: knobHover; cursorShape: Qt.PointingHandCursor }
        DragHandler {
            id: drag
            target: null
            onActiveChanged: {
                if (active) bar.dragging = true;
                else { bar.dragging = false; bar.seekRequested(bar.dragFraction); }
            }
            onCentroidChanged: if (active) bar.dragFraction = bar.fractionAt(centroid.position.x)
        }
        TapHandler {
            onTapped: (eventPoint) => bar.seekRequested(bar.fractionAt(eventPoint.position.x))
        }
    }
}
