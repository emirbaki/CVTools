#pragma once
#include <QtWidgets>
#include <QGraphicsItem>
#include <QList>

class Edge;
class GraphWidget;

class ScanNode : public QGraphicsItem {

	public:
		ScanNode(GraphWidget* graphWidget);
        void addEdge(Edge* edge);
        QList<Edge*> edges() const;

        enum { Type = UserType + 1 };
        int type() const override { return Type; }

        QRectF boundingRect() const override;
        QPainterPath shape() const override;
        void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

        void setIndex(int value);

    protected:
        QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

        void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
        void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

    private:
        QList<Edge*> edgeList;
        QPointF newPos;
        GraphWidget* graph;

        int index;
        QGraphicsTextItem* textItem;
};
