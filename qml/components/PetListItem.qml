import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root
    property bool selected: false
    signal clicked()

    height: 106
    radius: 8
    color: selected ? "#eef6f8" : itemMouse.containsMouse ? "#f7fafc" : "#ffffff"
    border.color: selected ? "#2f6f8f" : "#d9e1ea"
    border.width: selected ? 1 : 1

    Behavior on color { ColorAnimation { duration: 120 } }
    Behavior on border.color { ColorAnimation { duration: 120 } }

    Rectangle {
        width: 4
        height: parent.height - 18
        anchors.left: parent.left
        anchors.leftMargin: 8
        anchors.verticalCenter: parent.verticalCenter
        radius: 2
        color: selected ? "#2f6f8f" : "transparent"
    }

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 18
        anchors.rightMargin: 10
        anchors.topMargin: 10
        anchors.bottomMargin: 10
        spacing: 10

        Rectangle {
            Layout.preferredWidth: 66
            Layout.preferredHeight: 66
            radius: 8
            color: "#f7f9fb"
            border.color: "#d9e1ea"

            Image {
                anchors.fill: parent
                anchors.margins: 5
                source: image
                fillMode: Image.PreserveAspectFit
                asynchronous: true
            }

            Text {
                anchors.centerIn: parent
                visible: !image
                text: nameZh ? nameZh.charAt(0) : "?"
                color: "#8a96a6"
                font.pixelSize: 20
                font.bold: true
            }
        }

        ColumnLayout {
            Layout.fillWidth: true
            spacing: 6

            RowLayout {
                Layout.fillWidth: true
                spacing: 6

                Text {
                    Layout.fillWidth: true
                    text: nameZh
                    color: "#111827"
                    font.pixelSize: 16
                    font.bold: true
                    elide: Text.ElideRight
                    maximumLineCount: 1
                }

                Text {
                    visible: favorite
                    text: "★"
                    color: "#d97706"
                    font.pixelSize: 15
                    font.bold: true
                }

                Rectangle {
                    Layout.preferredWidth: noText.implicitWidth + 14
                    Layout.preferredHeight: 24
                    radius: 5
                    color: "#f1f5f8"

                    Text {
                        id: noText
                        anchors.centerIn: parent
                        text: "NO." + petId
                        color: "#526071"
                        font.pixelSize: 11
                        font.bold: true
                    }
                }
            }

            Flow {
                Layout.fillWidth: true
                spacing: 5
                Repeater {
                    model: attributes
                    delegate: AttributeBadge {
                        label: modelData.label
                        badgeColor: modelData.color
                        textColor: modelData.textColor
                        compact: true
                        clickable: false
                    }
                }
            }

            Text {
                Layout.fillWidth: true
                text: stage + " · " + roleLabel
                color: "#697386"
                font.pixelSize: 12
                elide: Text.ElideRight
                maximumLineCount: 1
            }
        }

        ColumnLayout {
            Layout.preferredWidth: 56
            spacing: 2

            Text {
                Layout.fillWidth: true
                text: baseStats
                color: "#111827"
                font.pixelSize: 19
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
            }

            Text {
                Layout.fillWidth: true
                text: "总和"
                color: "#697386"
                font.pixelSize: 11
                horizontalAlignment: Text.AlignHCenter
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 1
                color: "#d9e1ea"
            }

            Text {
                Layout.fillWidth: true
                text: "速 " + speed
                color: "#697386"
                font.pixelSize: 11
                horizontalAlignment: Text.AlignHCenter
                elide: Text.ElideRight
            }
        }
    }

    MouseArea {
        id: itemMouse
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
        onClicked: root.clicked()
    }
}
