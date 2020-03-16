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

        Item {
            id: layer

            objectName: "layer"
            scale:1
            x:0
            y:0
            width:win.width
            height:win.height
            Item{
                x: 0
                y: 0
                width: 200
                height : 200
                PaintStructural {
                    anchors.fill: parent
                    p1: Qt.point(180,180)
                    id: line
                    objectName: "line1"
                    color: "grey"
                    lineWidth: 1
                    TextInput{
                        objectName:"debug"
                        anchors.fill:parent
                    }
                }
                PaintStructural {
                    anchors.fill: parent
                    p1: Qt.point(150,180)
                    id: line2
                    objectName: "line2"
                    color: "grey"
                    lineWidth: 1
                }
                PaintStructural {
                    anchors.fill: parent
                    p1: Qt.point(180,150)
                    id: line3
                    objectName: "line3"
                    color: "grey"
                    lineWidth: 1
                }
                PaintStructural {
                    anchors.fill: parent
                    p1: Qt.point(180,170)
                    id: line4
                    objectName: "line4"
                    color: "grey"
                    lineWidth: 1
                }
                PaintStructural {
                    anchors.fill: parent
                    p1: Qt.point(180,30)
                    id: line5
                    objectName: "line5"
                    color: "grey"
                    lineWidth: 1
                }

            }
        }
    }

}
