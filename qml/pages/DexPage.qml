import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../components"

Item {
    id: root

    Rectangle {
        anchors.fill: parent
        color: "#edf1f5"
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 12

        DataManager {
            Layout.fillWidth: true
            Layout.preferredHeight: 82
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 12

            FilterRail {
                Layout.preferredWidth: 302
                Layout.fillHeight: true
            }

            Rectangle {
                Layout.preferredWidth: 418
                Layout.fillHeight: true
                radius: 8
                color: "#ffffff"
                border.color: "#cfd8e3"

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 12
                    spacing: 10

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 8

                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 2

                            Text {
                                text: "精灵列表"
                                color: "#111827"
                                font.pixelSize: 18
                                font.bold: true
                            }

                            Text {
                                text: dex.query ? "正在搜索 “" + dex.query + "”" : "按当前筛选展示"
                                color: "#697386"
                                font.pixelSize: 12
                                elide: Text.ElideRight
                                Layout.fillWidth: true
                            }
                        }

                        Rectangle {
                            Layout.preferredHeight: 30
                            Layout.preferredWidth: resultText.implicitWidth + 18
                            radius: 6
                            color: "#edf6fb"

                            Text {
                                id: resultText
                                anchors.centerIn: parent
                                text: dex.resultCount + " 条"
                                color: "#2f6f8f"
                                font.pixelSize: 12
                                font.bold: true
                            }
                        }
                    }

                    QuickPetStrip {
                        title: "收藏"
                        pets: dex.favoritePets
                        onPetClicked: (key) => dex.selectPet(key)
                    }

                    QuickPetStrip {
                        title: "最近查看"
                        pets: dex.recentPets
                        onPetClicked: (key) => dex.selectPet(key)
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 1
                        color: "#e4eaf1"
                    }

                    ListView {
                        id: listView
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        clip: true
                        spacing: 8
                        model: dex.petModel
                        boundsBehavior: Flickable.StopAtBounds
                        currentIndex: dex.selectedPet && dex.selectedPet.row !== undefined ? dex.selectedPet.row : -1

                        delegate: PetListItem {
                            width: listView.width
                            selected: dex.selectedPet.key === key
                            onClicked: dex.selectPet(key)
                        }

                        ScrollBar.vertical: ScrollBar {
                            policy: ScrollBar.AsNeeded
                        }

                        Rectangle {
                            anchors.centerIn: parent
                            width: Math.min(parent.width - 36, 280)
                            height: 130
                            radius: 8
                            color: "#f7f9fb"
                            border.color: "#d9e1ea"
                            visible: listView.count === 0

                            Column {
                                anchors.centerIn: parent
                                spacing: 8

                                Text {
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    text: "没有匹配的精灵"
                                    color: "#111827"
                                    font.pixelSize: 16
                                    font.bold: true
                                }

                                Text {
                                    width: 230
                                    text: "调整属性、阶段或关键词后再试。"
                                    color: "#697386"
                                    font.pixelSize: 12
                                    horizontalAlignment: Text.AlignHCenter
                                    wrapMode: Text.WordWrap
                                }
                            }
                        }
                    }
                }
            }

            PetDetailPage {
                Layout.fillWidth: true
                Layout.fillHeight: true
                pet: dex.selectedPet
            }
        }
    }
}
