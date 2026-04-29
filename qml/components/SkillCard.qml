import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root
    property var skill
    property bool expanded: false

    Layout.fillWidth: true
    implicitHeight: content.implicitHeight + 20
    radius: 8
    color: skillMouse.containsMouse ? "#f4f8fa" : "#ffffff"
    border.color: "#d9e1ea"

    Behavior on color { ColorAnimation { duration: 120 } }

    ColumnLayout {
        id: content
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: 10
        spacing: 8

        RowLayout {
            Layout.fillWidth: true
            spacing: 8

            Rectangle {
                Layout.preferredWidth: 58
                Layout.preferredHeight: 26
                radius: 6
                color: "#f1f5f8"

                Text {
                    anchors.centerIn: parent
                    text: skill.level > 0 ? skill.level + "级" : skill.group
                    color: "#526071"
                    font.pixelSize: 11
                    font.bold: true
                    elide: Text.ElideRight
                    maximumLineCount: 1
                }
            }

            Text {
                text: skill.name
                color: "#111827"
                font.pixelSize: 14
                font.bold: true
                elide: Text.ElideRight
                Layout.fillWidth: true
                maximumLineCount: 1
            }

            Rectangle {
                visible: skill.attributeLabel !== ""
                Layout.preferredHeight: 26
                Layout.preferredWidth: attrText.implicitWidth + 16
                radius: 6
                color: skill.attributeColor

                Text {
                    id: attrText
                    anchors.centerIn: parent
                    text: skill.attributeLabel
                    color: skill.attributeTextColor
                    font.pixelSize: 12
                    font.bold: true
                }
            }

            Text {
                text: "耗 " + skill.energy + " / 威 " + skill.power
                color: "#697386"
                font.pixelSize: 12
            }

            Text {
                text: root.expanded ? "收起" : "详情"
                color: "#2f6f8f"
                font.pixelSize: 12
                font.bold: true
            }
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 8
            visible: root.expanded

            Text {
                Layout.fillWidth: true
                text: skill.effect
                color: "#334155"
                font.pixelSize: 13
                wrapMode: Text.WordWrap
                lineHeight: 1.18
            }
        }
    }

    MouseArea {
        id: skillMouse
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
        onClicked: root.expanded = !root.expanded
    }
}
