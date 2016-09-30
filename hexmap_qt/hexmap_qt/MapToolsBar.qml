import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3

Rectangle
{
    color: "#4d4d4d"
    ToolBar
    {
        ColumnLayout
        {
            anchors.fill: parent
            property real button_size: parent.parent.width

            ToolButton {
                text: "Select"
                width:  parent.button_size
                height: parent.button_size
            }
            ToolButton {
                text: "Paint"
                width:  parent.button_size
                height: parent.button_size
            }
        }
    }
}
