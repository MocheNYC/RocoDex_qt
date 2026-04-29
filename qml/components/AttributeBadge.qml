import QtQuick

Rectangle {
    id: root
    property string label: ""
    property string badgeColor: "#e5e7eb"
    property string textColor: "#18202a"
    property bool compact: false
    property bool selected: false
    property bool clickable: true
    signal clicked()

    width: textItem.implicitWidth + (compact ? 18 : 24)
    height: compact ? 26 : 30
    radius: 6
    color: selected ? "#172033" : badgeColor
    border.width: selected ? 1 : 1
    border.color: selected ? "#172033" : Qt.darker(badgeColor, 1.08)
    opacity: enabled ? 1 : 0.55

    Behavior on color { ColorAnimation { duration: 120 } }
    Behavior on border.color { ColorAnimation { duration: 120 } }

    Text {
        id: textItem
        anchors.centerIn: parent
        text: root.label
        color: root.selected ? "#ffffff" : root.textColor
        font.pixelSize: root.compact ? 12 : 13
        font.bold: true
        elide: Text.ElideRight
        maximumLineCount: 1
    }

    MouseArea {
        anchors.fill: parent
        enabled: root.clickable
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
        onClicked: root.clicked()
    }
}
