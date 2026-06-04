import QtQuick

// Horizontally scrolling text for long titles, like the macOS MarqueeTextView.
// Scrolls only when the text overflows the available width.
Item {
    id: marquee
    property alias text: label.text
    property alias color: label.color
    property alias font: label.font

    clip: true
    implicitHeight: label.implicitHeight

    readonly property bool overflowing: label.implicitWidth > width

    Text {
        id: label
        y: 0
        elide: Text.ElideNone

        x: marquee.overflowing ? scroll.x : 0

        SequentialAnimation {
            id: scroll
            property real x: 0
            running: marquee.overflowing
            loops: Animation.Infinite
            PauseAnimation { duration: 1200 }
            NumberAnimation {
                target: scroll; property: "x"
                from: 0; to: -(label.implicitWidth - marquee.width)
                duration: Math.max(1, (label.implicitWidth - marquee.width)) * 30
                easing.type: Easing.Linear
            }
            PauseAnimation { duration: 1200 }
            NumberAnimation {
                target: scroll; property: "x"
                to: 0; duration: 600; easing.type: Easing.InOutQuad
            }
        }
    }
}
