import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2

MenuBar
{
    //------------
    //--- File ---
    //------------
    Menu {
        title: qsTr("&File")
        MenuItem {
            text: qsTr("&Open")
            onTriggered: console.log("Open action triggered");
        }
        MenuItem {
            text: qsTr("&Open")
        }
        MenuItem {
            text: qsTr("&Save")
        }
        MenuItem {
            text: qsTr("Save &As")
        }

        MenuSeparator{}

        MenuItem {
            text: qsTr("Export PNG")
        }

        MenuSeparator{}

        MenuItem {
            text: qsTr("E&xit")
            onTriggered: Qt.quit();
        }
    }


    //------------
    //--- Edit ---
    //------------
    Menu {
        title: qsTr("&Edit")
        MenuItem {
            text: qsTr("&Undo")
        }
        MenuItem {
            text: qsTr("&Redo")
        }

        MenuSeparator{}

        MenuItem {
            text: qsTr("Cu&t")
        }
        MenuItem {
            text: qsTr("&Copy")
        }
        MenuItem {
            text: qsTr("&Paste")
        }
    }


    //------------
    //--- View ---
    //------------
    Menu {
        title: qsTr("&View")
        MenuItem {
            text: qsTr("&Zoom")
        }
    }


    //------------
    //--- Map ---
    //------------
    Menu {
        title: qsTr("&Map")

        MenuItem {
            text: qsTr("&Resize")
        }

        MenuSeparator{}

        MenuItem {
            text: qsTr("&Properties")
        }
    }

}
