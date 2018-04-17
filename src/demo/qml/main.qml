import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.0
import QtQuick.Layouts 1.0
import QtPositioning 5.2
import "./widgets"

ApplicationWindow {
    id: app
    visible: true
    title: qsTr("Pylon Prototype")
    property string statusText

    width: 900
    height: 600

    header: MainToolBar {}

    Drawer {
        id: drawer
        width: Math.min(app.width, app.height) / 3
        height: app.height

        ListView {
            id: listView
            currentIndex: -1
            anchors.fill: parent

            delegate: ItemDelegate {
                width: parent.width
                text: model.title
                highlighted: ListView.isCurrentItem
                onClicked: {
                    if (listView.currentIndex != index) {
                        listView.currentIndex = index
                        console.log("partiu " + model.title + "?");
                        //viewStack.push(model.source)
                    }
                    drawer.close()
                }
            }

            model: ListModel {
                ListElement { title: "Histórico"; source: "qrc:/pages/HistoryPage.qml" }
                ListElement { title: "Help"; source: "qrc:/pages/HelpPage.qml" }
            }

            ScrollIndicator.vertical: ScrollIndicator { }
        }
    }

    StackView {
        id: viewStack
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        state: "home"
        onStateChanged: {
            console.log("state changed: ", controller.currentState);
        }
        states: [
            State {
                name: "home"
                StateChangeScript {
                    script: viewStack.push(startPage)
                }
            }
        ]
        Connections {
            target: app
            onClosing: {
                console.log("closiiing...");
                if (viewStack.depth > 1) {
                    viewStack.pop();
                    close.accepted = false;
                } else {
                    close.accepted = true;
                }
            }
        }
    }

    Component {
        id: startPage
        StartPage { }
    }
}

