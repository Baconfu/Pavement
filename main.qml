import QtQuick 2.12
import QtQuick 2.15
import QtQuick.Window 2.12
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls 1.0
import Paint 1.0


ApplicationWindow {
    id: win
    visible: true
    width: 640
    height: 480
    style: ApplicationWindowStyle {
            background: Rectangle {
                color: "#FFFFFF"
            }
        }

    title: qsTr("Pavement 1.1")

    onClosing: {
        myItem.closing()
    }

    Item {
        anchors.fill:parent
        objectName: "item"
        id: myItem
        signal tabPressed()
        signal enterPressed()
        signal closing()
        signal escapePressed()
        signal scroll(int x,int y,bool ctrl)

        focus: true
        Keys.onTabPressed: {
            myItem.tabPressed()

        }

        Keys.onReturnPressed: {
            myItem.enterPressed()
        }
        Keys.onEscapePressed: {
            myItem.escapePressed()
        }

        property bool acceptMouseEvent: true
        onAcceptMouseEventChanged: {

        }

        MouseArea {
            z:1
            id:mouseArea
            propagateComposedEvents: true
            objectName: "mouseArea"
            anchors.fill: parent
            signal mouseTransform(int x,int y,int offsetX,int offsetY)
            signal mouseClicked(int x,int y);
            signal mouseDoubleClicked(int x,int y)
            signal mousePressed(int x,int y)
            signal mouseHeld()

            signal mouseInWindowChanged(bool b)

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

                mouse.accepted = myItem.acceptMouseEvent
            }

            onReleased: {
                mouseReleased()

            }

            onContainsMouseChanged: {
                mouseInWindowChanged(containsMouse)
            }

            pressAndHoldInterval: 200
            onPressAndHold: {
                mouseHeld()
            }
            /*
            onWheel: {

                if(wheel.modifiers == Qt.ControlModifier){
                    scroll(wheel.angleDelta.x,wheel.angleDelta.y,true)
                }else{
                    scroll(wheel.angleDelta.x,wheel.angleDelta.y,false)
                }
            }*/

            onDoubleClicked: {
                console.log("hey");
            }


        }
        WheelHandler {
            id:trackpad
            acceptedDevices: PointerDevice.TouchPad
            onWheel: {
                if(wheel.modifiers === Qt.ControlModifier){
                    myItem.scroll(wheel.angleDelta.x,wheel.angleDelta.y,true)
                }else{
                    myItem.scroll(wheel.angleDelta.x,wheelAngleDelta.y,false)
                }



            }
        }
        WheelHandler {
            id:wheel
            acceptedDevices: PointerDevice.Mouse
            property int rotationOld: 0

            onWheel: {

                myItem.scroll(0,(rotation-rotationOld) * 10,true)

                rotationOld = rotation

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


            transform: Scale {

                objectName: "layerScale"
                origin.x: 0
                origin.y: 0

                property int x:0
                property int y:0
                onXChanged: {
                    origin.x = x
                }
                onYChanged: {
                    origin.y = y
                }



                xScale: 1
                yScale: 1
            }

            x:0
            y:0
            width:win.width
            height:win.height
            /*
            Rectangle{
                z:10
                objectName: "show"
                width:10
                height:10
                color:"black"
            }*/
        }
    }
}
