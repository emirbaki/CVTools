#include "edge.h"
#include "scannode.h"

#include <QPainter>
#include <QtMath>

//! [0]
Edge::Edge(ScanNode* sourceNode, ScanNode* destNode)
    : source(sourceNode), dest(destNode)
{
    setAcceptedMouseButtons(Qt::NoButton);
    source->addEdge(this);
    dest->addEdge(this);
    adjust();
}
//! [0]

//! [1]
ScanNode* Edge::sourceNode() const
{
    return source;
}

ScanNode* Edge::destNode() const
{
    return dest;
}
//! [1]

//! [2]
void Edge::adjust()
{
    if (!source || !dest)
        return;

    QLineF line(mapFromItem(source, 0, 0), mapFromItem(dest, 0, 0));
    qreal length = line.length();

    prepareGeometryChange();

    if (length > qreal(20.)) {
        QPointF edgeOffset((line.dx() * 10) / length, (line.dy() * 10) / length);
        sourcePoint = line.p1() + edgeOffset;
        destPoint = line.p2() - edgeOffset;
    }
    else {
        sourcePoint = destPoint = line.p1();
    }
}
//! [2]

//! [3]
QRectF Edge::boundingRect() const
{
    if (!source || !dest)
        return QRectF();

    qreal penWidth = 1;
    qreal extra = (penWidth + arrowSize) / 2.0;

    return QRectF(sourcePoint, QSizeF(destPoint.x() - sourcePoint.x(),
        destPoint.y() - sourcePoint.y()))
        .normalized()
        .adjusted(-extra, -extra, extra, extra);
}
//! [3]

//! [4]
void Edge::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    if (!source || !dest)
        return;

    QLineF line(sourcePoint, destPoint);
    if (qFuzzyCompare(line.length(), qreal(0.)))
        return;
    //! [4]

    //! [5]
        // Draw the line itself
    painter->setPen(QPen(Qt::black, 2, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin));
    painter->drawLine(line);
    //! [5]

    //! [6]
        // Draw the arrows
    double angle = std::atan2(-line.dy(), line.dx());


    painter->setBrush(Qt::black);
}