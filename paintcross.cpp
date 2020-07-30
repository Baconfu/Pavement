#include "paintcross.h"



PaintCross::PaintCross(QQuickItem * parent):
    QQuickItem (parent)
{
    setFlag(ItemHasContents,true);
}

PaintCross::~PaintCross()
{

}

QSGNode * PaintCross::updatePaintNode(QSGNode * oldNode,UpdatePaintNodeData *)
{

    QSGGeometryNode *node = nullptr;
    QSGGeometry * geometry = nullptr;

    if(!oldNode){

        node = new QSGGeometryNode;
        geometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(),4);
        geometry->setLineWidth(m_lineWidth);
        geometry->setDrawingMode(QSGGeometry::DrawLines);
        node->setGeometry(geometry);
        node->setFlag(QSGNode::OwnsGeometry);
        QSGFlatColorMaterial *material = new QSGFlatColorMaterial;
        material->setColor(m_color);

        node->setMaterial(material);
        node->setFlag(QSGNode::OwnsMaterial);
    } else{
        QSGFlatColorMaterial *material = new QSGFlatColorMaterial;
        material->setColor(m_color);

        node = static_cast<QSGGeometryNode *>(oldNode);
        geometry = node->geometry();
        geometry->setLineWidth(m_lineWidth);
        geometry->allocate(4);
        node->setMaterial(material);
    }

    QSGGeometry::Point2D * vertices = geometry->vertexDataAsPoint2D();


    vertices[0].set(0,0);
    vertices[1].set(int(size().width()),int(size().height()));
    vertices[2].set(0,int(size().height()));
    vertices[3].set(int(size().width()),0);

    node->markDirty(QSGNode::DirtyGeometry);
    return node;
}
void PaintCross::setLineWidth(int n)
{
    if(m_lineWidth != n){
        m_lineWidth = n;
        emit lineWidthChanged();
        update();
    }
}

