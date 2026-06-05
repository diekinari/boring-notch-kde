import QtQuick

// "Liquid glass" styling drawn on top of the (translucent, KWin-blurred) notch:
//   - a soft top sheen,
//   - a bright rim along the edges (fake edge-lensing).
// Both follow the notch's rounded-bottom shape so nothing pokes past the
// corners. Strengths are configurable. Real background refraction isn't possible
// on Wayland (no readback of the compositor), so the rim approximates the look.
Item {
    id: glass
    property int bottomRadius: 14
    property real sheen: 0.12   // 0..1 top highlight strength
    property real rim: 0.20     // 0..1 edge highlight strength

    // Top-down sheen, clipped to the rounded notch shape via matching radii.
    Rectangle {
        anchors.fill: parent
        topLeftRadius: 0
        topRightRadius: 0
        bottomLeftRadius: glass.bottomRadius
        bottomRightRadius: glass.bottomRadius
        gradient: Gradient {
            GradientStop { position: 0.0; color: Qt.rgba(1, 1, 1, glass.sheen) }
            GradientStop { position: 0.45; color: "transparent" }
        }
    }

    // Bright rim along the rounded edge.
    Rectangle {
        anchors.fill: parent
        color: "transparent"
        topLeftRadius: 0
        topRightRadius: 0
        bottomLeftRadius: glass.bottomRadius
        bottomRightRadius: glass.bottomRadius
        border.width: 1
        border.color: Qt.rgba(1, 1, 1, glass.rim)
    }
}
