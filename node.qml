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

    onNameChanged: {
        textInput.text = name
    }
    onTypeChanged: {
        typeInput.text = type
    }

    z:0

    property bool highlighted: false
    property bool ghost:false

    signal typeAccepted(string s)
    signal update()

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

    onWidthChanged: {
        update()
    }
    onHeightChanged: {
        update()
    }
    onXChanged: {
        update()
    }
    onYChanged:{
        update()
    }

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
            enabled: !ghost
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
            enabled: !ghost
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
    ExpandIcon {
        visible:false
        x:container.width + 2
        y:4
        width:12
        height:12

    }



}
