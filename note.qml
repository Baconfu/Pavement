import QtQuick 2.0
import QtQuick.Controls 1.0

Item{
    objectName: "container"
    id: container
    x:0
    y:0
    width:textArea.contentWidth + 20
    height:textArea.contentHeight + 10

    signal update()

    onWidthChanged: {

        update()
    }
    onHeightChanged: {

        update()
    }

    Rectangle {
        objectName: "bg"
        id: bg
        color:"white"
        anchors.fill: parent
        z:-1

        border.width: 1
        border.color: "grey"

    }
    TextArea {
        x:0
        y:0

        id: textArea
        objectName: "textArea"
        font.pointSize: 9
        wrapMode: TextEdit.NoWrap

        onImplicitWidthChanged: {

        }
        onImplicitHeightChanged: {

        }

        onContentWidthChanged: {
            implicitWidth = contentWidth + 20

        }
        onContentHeightChanged: {
            implicitHeight = contentHeight + 20
        }


        onFocusChanged: {
            if(focus){

                forceActiveFocus();
                cursorPosition = -1

            }
        }

    }

}


