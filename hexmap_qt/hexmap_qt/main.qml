import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2

import Hexmap 1.0

// TODO
// Use a 'QQuickFramebufferObject' for rendering hexmap, since the method
// used in the example uses 'QOpenGLFunctions' which is OpenGL ES 2
// I can already get my existing hexmap code to render the map to a framebuffer
// So this works out nicely
// Will need to look at QQuickFramebufferObject::Renderer and QOpenGLContext
// I think I can grab an existing openGL context, ensure it is at least 3.3
// and then use it with the framebuffer



ApplicationWindow {
    visible: true
    width: 800
    height: 500
    title: qsTr("Hexmap")

    menuBar: MainMenuBar
    {
        id: app_menu_bar
    }

    statusBar: Row
    {
        id: app_status_bar
        width: parent.width
        height: 20

        spacing: 20

        Text {
            text: "Command Log Stuff Will Go Here"
        }

        Text {
            text: "Progress Bar?"
        }
    }


    MainToolbar
    {
        id: main_toolbar

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
    }


    MainForm {
        anchors.top: main_toolbar.bottom
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right

        Item
        {
            id: left_column

            width: 48
            anchors.left:    parent.left
            anchors.top:     parent.top
            anchors.bottom:  parent.bottom

            MapToolsBar
            {
                id: map_tools
                anchors.fill: parent
            }
        }


        //Item
        HexmapView
        {
            id: hex_map_placeholder

            width: 100
            height: 100

            anchors.leftMargin: 10
            anchors.rightMargin: 10

            anchors.top:    parent.top
            anchors.bottom: parent.bottom
            anchors.left:   left_column.right
            anchors.right:  right_column.left

//            Rectangle{
//                color: "#232323"
//                border.width: 1
//                border.color: "white"

//                anchors.fill: parent
//            }


        }


        Item
        {
            id: right_column

            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter

            width: parent.width * 0.3
            height: parent.height
//            spacing: 2

            // placeholder
            Rectangle
            {
                width: parent.width
                height: parent.height * 0.7

                color: "#444444"
            }

            LayersPanel
            {
                anchors.right: parent.right
                anchors.bottom: parent.bottom

                width: parent.width;
                height: parent.height * 0.3
            }
        }

    }

    MessageDialog {
        id: messageDialog
        title: qsTr("May I have your attention, please?")

        function show(caption) {
            messageDialog.text = caption;
            messageDialog.open();
        }
    }
}
