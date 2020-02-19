import QtQuick 2.13
import QtQuick.Window 2.13
import Paint 1.0


Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Pavement 1.1")

    Item {
        objectName: "item"
        id: myItem
        anchors.fill: parent
        focus:true
        signal tabPressed()
        signal enterPressed()


        Keys.onTabPressed: {
            tabPressed()

        }

        Keys.onReturnPressed: {
            enterPressed()
        }

        Item {
            objectName: "map"
            id:map
            anchors.fill:parent

        }



        MouseArea {
            objectName: "mouseArea"
            anchors.fill: parent
            signal mouseTransform(int x,int y)
            hoverEnabled: true
            onMouseXChanged: {
                mouseTransform(mouseX,mouseY)
            }
            onMouseYChanged: {
                mouseTransform(mouseX,mouseY)
            }
        }




    }

}
