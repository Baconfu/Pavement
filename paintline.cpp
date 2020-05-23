#include "paintline.h"




PaintLine::PaintLine(QQuickItem * parent):
    QQuickItem (parent)
{
    setFlag(ItemHasContents,true);
}

PaintLine::~PaintLine()
{

}

QSGNode * PaintLine::updatePaintNode(QSGNode * oldNode,UpdatePaintNodeData *)
{

    QSGGeometryNode *node = nullptr;
    QSGGeometry * geometry = nullptr;

    if(!oldNode){

        node = new QSGGeometryNode;
        geometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(),2);
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
        geometry->allocate(2);
        node->setMaterial(material);
    }

    QPointF origin = p0();
    QPointF destination = p1();


    QSGGeometry::Point2D * vertices = geometry->vertexDataAsPoint2D();


    vertices[0].set(float(origin.x()),float(origin.y()));
    vertices[1].set(float(destination.x()),float(destination.y()));

    node->markDirty(QSGNode::DirtyGeometry);
    return node;
}
void PaintLine::setLineWidth(int n)
{
    if(m_lineWidth != n){
        m_lineWidth = n;
        emit lineWidthChanged();
        update();
    }
}

