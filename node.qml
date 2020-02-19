import QtQuick 2.0
import Paint 1.0
import QtQuick.Controls 2.0

Item {
    id: container;
    objectName: "container"
    property string name
    property string type
    property int absX
    property int absY

    z:0

    property bool highlighted: false

    signal typeAccepted(string s)

    onAbsXChanged: {
        x = absX
    }
    onAbsYChanged: {
        y = absY
    }
    onHighlightedChanged: {
        if(highlighted){
            highlight.visible = true
        }
        else{
            highlight.visible = false
        }
    }

    x:0
    y:0
    width:10
    height: nameContainer.height + typeInput.height
    Rectangle{
        id: highlight
        visible: false
        anchors.fill: parent
        radius: 3
        color: "grey"
        z:1
        opacity:0.2

    }

    Rectangle {
        id:nameContainer
        color:"white"
        x:0
        y:0
        width: 10
        height: 20
        TextInput {
            id: textInput
            x:5
            objectName: "textInput"
            onContentWidthChanged: {
                if(contentWidth<5){
                    container.width = 10
                    nameContainer.width = 10
                }else{
                    container.width = contentWidth + 10
                    nameContainer.width = contentWidth + 10
                }
            }

        }
        PaintNode {
            mode: "standard"
            anchors.fill: parent
            lineWidth: 2
            color: "grey"

        }
    }

    Rectangle{
        objectName: "typeNameContainer"
        x:container.width / 2 - typeInput.contentWidth / 2
        y:nameContainer.height
        width:typeInput.contentWidth
        height:typeInput.contentHeight
        z:-2
        color: "white"
        TextInput {
            id: typeInput
            objectName: "typeName"
            anchors.fill:parent
            font.pointSize: 9
            text: ""
            font.italic: false

            property bool italic: false
            onItalicChanged: {
                typeInput.font.italic = italic
            }

            onAccepted: {
                typeAccepted(typeInput.text)
            }

        }

    }



}
