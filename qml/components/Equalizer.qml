import QtQuick

// A small animated equalizer. For now the bars are a synthetic animation that
// runs while `active` is true (placeholder for a real PipeWire/FFT spectrum
// later). When inactive the bars settle to a flat minimum.
Row {
    id: eq

    property bool active: false
    property color barColor: "#1db954"
    property int bars: 4
    property int barWidth: 3
    readonly property real minH: 3
    readonly property real maxH: height

    spacing: 2

    Repeater {
        model: eq.bars
        delegate: Rectangle {
            id: bar
            required property int index
            width: eq.barWidth
            radius: eq.barWidth / 2
            color: eq.barColor
            anchors.verticalCenter: parent.verticalCenter

            // Oscillate height via a smoothly animated phase. Each bar starts at
            // a different phase and runs at a slightly different speed, so they
            // don't move in lockstep.
            property real phase: index
            height: eq.active
                    ? eq.minH + (eq.maxH - eq.minH) * (0.5 + 0.5 * Math.sin(phase))
                    : eq.minH
            Behavior on height { enabled: !eq.active; NumberAnimation { duration: 200 } }

            NumberAnimation on phase {
                running: eq.active
                from: bar.index
                to: bar.index + 2 * Math.PI
                duration: 700 + (bar.index % 3) * 180
                loops: Animation.Infinite
            }
        }
    }
}
