

#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <QGraphicsView>
#include "utilities.h"


//using namespace cv;


class ScanNode;
//! [0]
class GraphWidget : public QGraphicsView
{
    Q_OBJECT

public:
    GraphWidget(QWidget* parent = nullptr);

    void itemMoved();
    void setSelectedNode(ScanNode* node);
    void setPicture(QImage& pixmap);
    QList<ScanNode*> nodeList;
    void handleEdges();
    void warpPerspective(QLabel* item);

public slots:
    void zoomIn();
    void zoomOut();

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void timerEvent(QTimerEvent* event) override;
#if QT_CONFIG(wheelevent)
    void wheelEvent(QWheelEvent* event) override;
#endif
    void drawBackground(QPainter* painter, const QRectF& rect) override;

    void scaleView(qreal scaleFactor);

private:
    int timerId = 0;
    ScanNode* selectedNode;
    QGraphicsScene* _scene;
    QGraphicsPixmapItem* pixmapItem;
    QImage _image;
    cv::Mat* warpedImage;
    
};
//! [0]

#endif // GRAPHWIDGET_H