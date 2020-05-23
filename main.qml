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
            z:1
            objectName: "mouseArea"
            anchors.fill: parent
            signal mouseTransform(int x,int y,int offsetX,int offsetY)
            signal mouseClicked(int x,int y);
            signal mouseDoubleClicked(int x,int y)
            signal mousePressed(int x,int y)
            signal mouseHeld()

            signal mouseReleased();
            hoverEnabled: true
            onMouseXChanged: {
                mouseTransform(mouseX,mouseY,layer.x,layer.y)
            }
            onMouseYChanged: {
                mouseTransform(mouseX,mouseY,layer.x,layer.y)
            }
            onClicked: {
                mouseClicked(mouseX - layer.x, mouseY - layer.y)
            }
            onPressed: {
                mousePressed(mouseX - layer.x, mouseY - layer.y)
            }

            onReleased: {
                mouseReleased()
            }
            pressAndHoldInterval: 200
            onPressAndHold: {
                mouseHeld()
            }

            onDoubleClicked: {

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
