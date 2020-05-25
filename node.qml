import QtQuick 2.0
import QtQuick 2.13
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

    property bool expanded: false


    property bool highlighted: false
    property bool faded: false
    property bool ghost:false

    signal typeAccepted(string s)
    signal typePassivelyAccepted(string s)
    signal update()
    signal passivelyAccepted()


    onExpandedChanged: {
        textInput.updateInput();
        if(expanded){
            standard.visible = false
            expandedArea.visible = true
            typeNameContainer.y = expandedArea.y + expandedArea.height
            container.width = expandedArea.width
            container.height = nameContainer.height + expandedRectangle.height + typeNameContainer.height


        }
        else{
            standard.visible = true
            expandedArea.visible = false

            typeNameContainer.y = nameContainer.y + nameContainer.height
            textInput.updateInput()

            container.height = nameContainer.height + typeInput.height
        }
    }

    onHighlightedChanged: {
        if(highlighted){
            highlight.visible = true
        }
        else{
            highlight.visible = false
        }
    }
    onFadedChanged: {
        if(faded){
            opacity = 0.4
        }
        else{
            opacity = 1
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
        objectName: "nameContainer"
        color:"white"
        x:container.width/2 - width/2
        y:0
        width: 10

        height: 20

        TextInput {
            id: textInput
            x:5

            text: ""
            enabled: !ghost
            objectName: "textInput"
            activeFocusOnPress:false
            onContentWidthChanged: {
                updateInput()
            }

            Shortcut {
                sequence: "Ctrl+."

            }

            function updateInput(){
                if(contentWidth<5){
                    if(!expanded){
                        container.width = 10
                    }
                    nameContainer.width = 10
                }else{
                    if(!expanded){
                        container.width = contentWidth + 10
                    }
                    nameContainer.width = contentWidth + 10
                }
            }

            onFocusChanged: {
                if(!focus){
                    passivelyAccepted()
                }
            }

        }
        PaintNode {
            id: standard
            mode: "standard"
            anchors.fill: parent
            lineWidth: 2
            color: "grey"

        }

    }

    Rectangle{
        id: typeNameContainer
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
            activeFocusOnPress: false
            enabled: !ghost
            anchors.fill:parent
            font.pointSize: 9
            text: ""
            font.italic: false


            onFocusChanged: {
                if(focus == false){
                    typePassivelyAccepted(typeInput.text)
                }
            }

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
    Item {
        id: expandedArea
        objectName: "expandedArea"
        visible: false
        y:0
        x:0
        width: 100
        height:100
        onWidthChanged: {
            if(expanded){

                container.width = width
                typeNameContainer.x = width/2 - typeNameContainer.width/2
            }
        }
        onHeightChanged: {
            if(expanded){
                container.height = height + typeNameContainer.height
                typeNameContainer.y = height
            }
        }

        Rectangle {
            id: expandedRectangle
            objectName: "expandedRectangle"
            y:nameContainer.height
            x:0
            width:expandedArea.width
            height:expandedArea.height-y
            z:-1

            radius: 10
            border.color: "black"
            border.width: 1




        }
    }


}
