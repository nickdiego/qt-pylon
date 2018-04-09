import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.0
import QtQuick.Layouts 1.0

ToolBar {
  Material.foreground: "white"

  RowLayout {
    spacing: 20
    anchors.fill: parent

    ToolButton {
      contentItem: Image {
        fillMode: Image.Pad
        horizontalAlignment: Image.AlignHCenter
        verticalAlignment: Image.AlignVCenter
        source: "qrc:/images/drawer.png"
      }
      onClicked: drawer.open()
    }

    Label {
      id: titleLabel
      text: "Qt Pylon Demo"
      font.pixelSize: 20
      elide: Label.ElideRight
      horizontalAlignment: Qt.AlignHCenter
      verticalAlignment: Qt.AlignVCenter
      Layout.fillWidth: true
    }

    ToolButton {
      contentItem: Image {
        fillMode: Image.Pad
        horizontalAlignment: Image.AlignHCenter
        verticalAlignment: Image.AlignVCenter
        source: "qrc:/images/menu.png"
      }
      onClicked: optionsMenu.open()

      Menu {
        id: optionsMenu
        x: parent.width - width
        transformOrigin: Menu.TopRight

        MenuItem {
          text: "Settings"
          onTriggered: settingsPopup.open()
        }
        MenuItem {
          text: "About"
          onTriggered: aboutDialog.open()
        }
      }
    }
  }
}

