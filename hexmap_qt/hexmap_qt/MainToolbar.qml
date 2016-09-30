import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3

ToolBar
{
    Row
    {
        anchors.fill: parent

        spacing: 2

        ToolButton{
            text: "asdf"
        }
        ToolButton
        {
            text: "fdsa"
        }
        ToolButton
        {
            text: "Tool3"
        }
        ToolButton
        {
            text: "Tool4"
        }
    }
}
