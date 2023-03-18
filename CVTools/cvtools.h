#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_qtcvtools.h"
#include <QtWidgets>

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>

#include <stdio.h>
#include "graphwidget.h"

using namespace cv;

class CVToolsWindow : public QMainWindow
{
    Q_OBJECT

public:
    CVToolsWindow(QWidget *parent = nullptr);
    bool loadFile(const QString&);
    ~CVToolsWindow();

private slots:
    void open();
    void saveAs();
    void print();
    void copy();
    void paste();
    void zoomIn();
    void zoomOut();
    void normalSize();
    void fitToWindow();
    void about();

private:
    Ui::CVToolsClass ui;

    void setMaskView();
    void setHSVView();
    void setNormalView();
    
    void initializeViews();

    void changeToMaskView();
    void changeToHSVView();
    void changeToNormalView();

    void updateMaskHSV();

    void updateViewType(const QPushButton& button);
    void connectEvents();
    void updateActions();
    bool saveFile(const QString& fileName);
    void setImage(const QImage& newImage);
    void scaleImage(double factor);
    void adjustScrollBar(QScrollBar* scrollBar, double factor);

    void setHminSlider();
    void setHmaxSlider();
    void setSminSlider();
    void setSmaxSlider();
    void setVminSlider();
    void setVmaxSlider();

    void addNode();

    void scanImage();

    QTimer *timer = new QTimer(this);
    QImage image;
    Mat normalImage, imageHSV, mask;
    GraphWidget* graphWidget;
    double scaleFactor = 1;

    int hmin = 0, hmax = 0, smin = 0, smax = 0, vmin = 0,vmax = 0;

#if defined(QT_PRINTSUPPORT_LIB) && QT_CONFIG(printer)
    QPrinter printer;
#endif
};
