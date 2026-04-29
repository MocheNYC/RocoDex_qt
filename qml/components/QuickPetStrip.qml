import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root
    property string title: ""
    property var pets: []
    signal petClicked(string key)

    visible: pets && pets.length > 0
    Layout.fillWidth: true
    Layout.preferredHeight: visible ? 118 : 0
    radius: 8
    color: "#ffffff"
    border.color: "#cfd8e3"

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 8

        RowLayout {
            Layout.fillWidth: true
            spacing: 6

            Text {
                text: root.title
                color: "#111827"
                font.pixelSize: 14
                font.bold: true
                Layout.fillWidth: true
            }

            Text {
                text: root.pets.length + " 只"
                color: "#697386"
                font.pixelSize: 12
            }
        }

        Flickable {
            id: flick
            Layout.fillWidth: true
            Layout.fillHeight: true
            contentWidth: petRow.implicitWidth
            contentHeight: height
            boundsBehavior: Flickable.StopAtBounds
            clip: true

            Row {
                id: petRow
                height: parent.height
                spacing: 8

                Repeater {
                    model: root.pets
                    delegate: Rectangle {
                        width: 156
                        height: 70
                        radius: 7
                        color: cardMouse.containsMouse ? "#f0f6f8" : "#f7f9fb"
                        border.color: modelData.key === dex.selectedPet.key ? "#2f6f8f" : "#d9e1ea"

                        RowLayout {
                            anchors.fill: parent
                            anchors.margins: 7
                            spacing: 8

                            Rectangle {
                                Layout.preferredWidth: 48
                                Layout.preferredHeight: 48
                                radius: 7
                                color: "#ffffff"
                                border.color: "#d9e1ea"

                                Image {
                                    anchors.fill: parent
                                    anchors.margins: 4
                                    source: modelData.image
                                    fillMode: Image.PreserveAspectFit
                                    asynchronous: true
                                }
                            }

                            ColumnLayout {
                                Layout.fillWidth: true
                                spacing: 4

                                Text {
                                    Layout.fillWidth: true
                                    text: modelData.nameZh
                                    color: "#111827"
                                    font.pixelSize: 13
                                    font.bold: true
                                    elide: Text.ElideRight
                                    maximumLineCount: 1
                                }

                                Text {
                                    Layout.fillWidth: true
                                    text: "NO." + modelData.id + " · " + modelData.stage
                                    color: "#697386"
                                    font.pixelSize: 11
                                    elide: Text.ElideRight
                                    maximumLineCount: 1
                                }
                            }
                        }

                        MouseArea {
                            id: cardMouse
                            anchors.fill: parent
                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor
                            onClicked: root.petClicked(modelData.key)
                        }
                    }
                }
            }

            ScrollBar.horizontal: ScrollBar {
                policy: ScrollBar.AsNeeded
            }
        }
    }
}
