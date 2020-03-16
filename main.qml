import QtQuick 2.13
import QtQuick.Window 2.13
import Paint 1.0


Window {
    id: win
    visible: true
    width: 640
    height: 480
    title: qsTr("Pavement 1.1")

    Item {
        anchors.fill:parent
        objectName: "item"
        id: myItem
        signal tabPressed()
        signal enterPressed()
        focus: true
        Keys.onTabPressed: {
            myItem.tabPressed()

        }

        Keys.onReturnPressed: {
            myItem.enterPressed()
        }
        MouseArea {
            objectName: "mouseArea"
            anchors.fill: parent
            signal mouseTransform(int x,int y,int offsetX,int offsetY)
            hoverEnabled: true
            onMouseXChanged: {
                mouseTransform(mouseX,mouseY,layer.x,layer.y)
            }
            onMouseYChanged: {
                mouseTransform(mouseX,mouseY,layer.x,layer.y)
            }
        }

        TextInput{
            x:0
            y:-50
            objectName: "debug"
        }

        Item {
            id: layer

            objectName: "layer"
            scale:1
            x:0
            y:0
            width:win.width
            height:win.height

        }
    }

}
