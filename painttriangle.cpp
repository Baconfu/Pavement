#include "painttriangle.h"

PaintTriangle::PaintTriangle(QQuickItem * parent):
    QQuickItem (parent),
    m_lineWidth(1)
{
    setFlag(ItemHasContents,true);
}

PaintTriangle::~PaintTriangle()
{

}

QSGNode * PaintTriangle::updatePaintNode(QSGNode * oldNode,UpdatePaintNodeData *)
{


    QSGGeometryNode *node = nullptr;
    QSGGeometry * geometry = nullptr;

    if(!oldNode){

        node = new QSGGeometryNode;
        geometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(),6);
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
        if(expandable()){
            geometry->allocate(6);
        }else{
            geometry->allocate(6);
        }
        node->setMaterial(material);
    }

    const double pi = 3.1415;
    QSizeF s = size();

    double gradient = s.height() / s.width();
    double angle = atan(gradient);


    //quartiles
    bool top = false;
    bool right = false;

    if(s.height()>0){
        top = true;
    }
    if(s.width()>0){
        right = true;
    }
    if(!right){angle += pi;}
    if(right && !top){angle += 2*pi;}

    double arm = angle + (pi/2) ;
    double arm2 = angle - (pi/2);

    QPointF origin = p0();
    QPointF destination = p1();

    double armX = cos(arm) * 5;
    double armY = sin(arm) * 5;

    double arm2X = cos(arm2) * 5;
    double arm2Y = sin(arm2) * 5;

    QPointF armP = QPointF(destination.x() - armX,destination.y() - armY);
    QPointF arm2P = QPointF(destination.x() - arm2X,destination.y() - arm2Y);

    QSGGeometry::Point2D * vertices = geometry->vertexDataAsPoint2D();


    vertices[0].set(float(origin.x()),float(origin.y()));
    vertices[1].set(float(armP.x()),float(armP.y()));
    vertices[2].set(float(armP.x()),float(armP.y()));
    vertices[3].set(float(arm2P.x()),float(arm2P.y()));
    vertices[4].set(float(origin.x()),float(origin.y()));
    vertices[5].set(float(arm2P.x()),float(arm2P.y()));


    node->markDirty(QSGNode::DirtyGeometry);

    return node;
}

void PaintTriangle::setLineWidth(int n)
{
    if(m_lineWidth != n){

        m_lineWidth = n;
        lineWidthChanged();
        update();

    }

}
