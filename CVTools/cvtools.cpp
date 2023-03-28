#include "cvtools.h"
#include <QtWidgets/QMainWindow>
#include <QFileDialog>

#include <qmetaobject.h>
#include <qstandardpaths.h>
#include <qimagereader.h>
#include <qimagewriter.h>
#include <qstatusbar.h>
#include <qcolorspace.h>
#include <QtWidgets/qmessagebox.h>
#include <iostream>


#include <stdio.h>



CVToolsWindow::CVToolsWindow(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    ui.imageLabel->setBackgroundRole(QPalette::Base);
    ui.imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    ui.imageLabel->setScaledContents(true);

    ui.scrollArea->setBackgroundRole(QPalette::Dark);
    setNormalView();
    /*ui.scrollArea->setWidget(ui.imageLabel);
    ui.scrollArea->setVisible(false);
    setCentralWidget(ui.scrollArea);*/

    ui.hminSlider->setRange(0, 255);
    ui.hmaxSlider->setRange(0, 255);
    ui.sminSlider->setRange(0, 255);
    ui.smaxSlider->setRange(0, 255);
    ui.vminSlider->setRange(0, 255);
    ui.vmaxSlider->setRange(0, 255);

    graphWidget = new GraphWidget;
    graphWidget->setParent(ui.scanningTab);
    statusBar()->setEnabled(true);
    
    objDetector = new ObjectDetector;
    objDetector->ui = &ui;

    connectEvents();
}


CVToolsWindow::~CVToolsWindow()
{
    delete objDetector;
}

static void initializeImageFileDialog(QFileDialog& dialog, QFileDialog::AcceptMode acceptMode)
{
    static bool firstDialog = true;

    if (firstDialog) {
        firstDialog = false;
        const QStringList picturesLocations = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
        dialog.setDirectory(picturesLocations.isEmpty() ? QDir::currentPath() : picturesLocations.last());
    }

    QStringList mimeTypeFilters;
    const QByteArrayList supportedMimeTypes = acceptMode == QFileDialog::AcceptOpen
        ? QImageReader::supportedMimeTypes() : QImageWriter::supportedMimeTypes();
    for (const QByteArray& mimeTypeName : supportedMimeTypes)
        mimeTypeFilters.append(mimeTypeName);
    mimeTypeFilters.sort();
    dialog.setMimeTypeFilters(mimeTypeFilters);
    dialog.selectMimeTypeFilter("image/jpeg");
    dialog.setAcceptMode(acceptMode);
    if (acceptMode == QFileDialog::AcceptSave)
        dialog.setDefaultSuffix("jpg");
}
static void initializeClassifierFileDialog(QFileDialog& dialog, QFileDialog::AcceptMode acceptMode)
{
    static bool firstDialog = true;

    if (firstDialog) {
        firstDialog = false;
        const QStringList desktop= QStandardPaths::standardLocations(QStandardPaths::DesktopLocation);
        dialog.setDirectory(desktop.isEmpty() ? QDir::currentPath() : desktop.last());
    }
    dialog.setNameFilter("XML (*.xml)");
    dialog.setFileMode(QFileDialog::AnyFile);
}

bool CVToolsWindow::loadFile(const QString& fileName)
{
    QImageReader reader(fileName);
    reader.setAutoTransform(true);
    const QImage newImage = reader.read();
    if (newImage.isNull()) {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
            tr("Cannot load %1: %2")
            .arg(QDir::toNativeSeparators(fileName), reader.errorString()));
        return false;
    }
    if (ui.maskingTab->isVisible()) {
        setImage(newImage);
    }
    else if (ui.scanningTab->isVisible()) {
        QImage instance = newImage.copy();
        graphWidget->setPicture(instance);
    }
    else if (ui.objectDetectorTab->isVisible()) {
        ui.objDetector_output->setPixmap(QPixmap::fromImage(newImage));
        QImage instance = newImage.copy();
        objDetector->initMat(instance);
    }

    setWindowFilePath(fileName);

    const QString description = image.colorSpace().isValid()
        ? image.colorSpace().description() : tr("unknown");
    const QString message = tr("Opened \"%1\", %2x%3, Depth: %4 (%5)")
        .arg(QDir::toNativeSeparators(fileName)).arg(image.width()).arg(image.height())
        .arg(image.depth()).arg(description);
    statusBar()->showMessage(message);
    return true;
}

bool CVToolsWindow::loadClassifierFile(const QString& fileName)
{
    objDetector->addNewClassifier(fileName);
   /* QImageReader reader(fileName);
    reader.setAutoTransform(true);
    const QImage newImage = reader.read();
    if (newImage.isNull()) {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
            tr("Cannot load %1: %2")
            .arg(QDir::toNativeSeparators(fileName), reader.errorString()));
        return false;
    }*/

    setWindowFilePath(fileName);

    const QString description = image.colorSpace().isValid()
        ? image.colorSpace().description() : tr("unknown");
    const QString message = tr("Opened \"%1\", %2x%3, Depth: %4 (%5)")
        .arg(QDir::toNativeSeparators(fileName)).arg(image.width()).arg(image.height())
        .arg(image.depth()).arg(description);
    statusBar()->showMessage(message);
    return true;
}

void CVToolsWindow::saveAs()
{
    QFileDialog dialog(this, tr("Save File As"));
    initializeImageFileDialog(dialog, QFileDialog::AcceptSave);

    while (dialog.exec() == QDialog::Accepted && !saveFile(dialog.selectedFiles().constFirst())) {}
}

void CVToolsWindow::print()
{
    Q_ASSERT(!ui.imageLabel->pixmap(Qt::ReturnByValue).isNull());
#if defined(QT_PRINTSUPPORT_LIB) && QT_CONFIG(printdialog)
    //! [6] //! [7]
    QPrintDialog dialog(&printer, this);
    //! [7] //! [8]
    if (dialog.exec()) {
        QPainter painter(&printer);
        QPixmap pixmap = imageLabel->pixmap(Qt::ReturnByValue);
        QRect rect = painter.viewport();
        QSize size = pixmap.size();
        size.scale(rect.size(), Qt::KeepAspectRatio);
        painter.setViewport(rect.x(), rect.y(), size.width(), size.height());
        painter.setWindow(pixmap.rect());
        painter.drawPixmap(0, 0, pixmap);
    }
#endif
}

void CVToolsWindow::copy()
{
#ifndef QT_NO_CLIPBOARD
    QGuiApplication::clipboard()->setImage(image);
#endif // !QT_NO_CLIPBOARD
}



void CVToolsWindow::paste()
{
#ifndef QT_NO_CLIPBOARD
    const QImage newImage = CVToolsUtility::clipboardImage();
    if (newImage.isNull()) {
        statusBar()->showMessage(tr("No image in clipboard"));
    }
    else {
        setImage(newImage);
        setWindowFilePath(QString());
        const QString message = tr("Obtained image from clipboard, %1x%2, Depth: %3")
            .arg(newImage.width()).arg(newImage.height()).arg(newImage.depth());
        statusBar()->showMessage(message);
    }
#endif // !QT_NO_CLIPBOARD
}

void CVToolsWindow::zoomIn()
{
    scaleImage(1.25);
}

void CVToolsWindow::zoomOut()
{
    scaleImage(0.8);
}

void CVToolsWindow::normalSize()
{
    ui.imageLabel->adjustSize();
    scaleFactor = 1.0;
}

void CVToolsWindow::fitToWindow()
{
    bool fitToWindow = ui.actionFit_the_Window->isChecked();
    ui.scrollArea->setWidgetResizable(fitToWindow);
    if (!fitToWindow)
        normalSize();
    updateActions();
}

void CVToolsWindow::about()
{
    QMessageBox::about(this, tr("About Image Viewer"),
        tr("<p>The <b>Image Viewer</b> example shows how to combine QLabel "
            "and QScrollArea to display an image. QLabel is typically used "
            "for displaying a text, but it can also display an image. "
            "QScrollArea provides a scrolling view around another widget. "
            "If the child widget exceeds the size of the frame, QScrollArea "
            "automatically provides scroll bars. </p><p>The example "
            "demonstrates how QLabel's ability to scale its contents "
            "(QLabel::scaledContents), and QScrollArea's ability to "
            "automatically resize its contents "
            "(QScrollArea::widgetResizable), can be used to implement "
            "zooming and scaling features. </p><p>In addition the example "
            "shows how to use QPainter to print an image.</p>"));
}

void CVToolsWindow::openClassifierFile()
{
    QFileDialog dialog(this, tr("Select a Classifier File"));
    initializeClassifierFileDialog(dialog, QFileDialog::AcceptOpen);

    while (dialog.exec() == QDialog::Accepted && !loadClassifierFile(dialog.selectedFiles().constFirst())) {}
}

void CVToolsWindow::scanImageWithClassifiers()
{
    objDetector->scanImageWithClassifiers();
}

void CVToolsWindow::open()
{
    QFileDialog dialog(this, tr("Open File"));
    initializeImageFileDialog(dialog, QFileDialog::AcceptOpen);

    while (dialog.exec() == QDialog::Accepted && !loadFile(dialog.selectedFiles().constFirst())) {}
}

void CVToolsWindow::connectEvents()
{

    connect(ui.openToolBox, &QToolButton::released, this, &CVToolsWindow::open);
    connect(ui.openToolBox_scan, &QToolButton::released, this, &CVToolsWindow::open);
    connect(ui.openToolBox_objectDetector, &QToolButton::released, this, &CVToolsWindow::open);

    connect(ui.addCascade_btn, &QToolButton::released, this, &CVToolsWindow::addClassifier);
    connect(ui.removeCascade_btn, &QToolButton::released, this, &CVToolsWindow::removeClassifier);
    connect(ui.startDetection_btn, &QToolButton::released, this, &CVToolsWindow::scanImageWithClassifiers);

    connect(ui.saveOutput_btn, &QToolButton::released, this, &CVToolsWindow::saveAs);
    connect(ui.addPointer_btn, &QToolButton::released, this, &CVToolsWindow::addNode);
    connect(ui.scanPermit_btn, &QToolButton::released, this, &CVToolsWindow::warpImage);

    connect(ui.maskViewButton, &QToolButton::released, this, &CVToolsWindow::setMaskView);
    connect(ui.hsvViewButton, &QToolButton::released, this, &CVToolsWindow::setHSVView);
    connect(ui.normalViewButton, &QToolButton::released, this, &CVToolsWindow::setNormalView);

    connect(ui.hminSlider, &QSlider::valueChanged, this, &CVToolsWindow::setHminSlider);
    connect(ui.hmaxSlider, &QSlider::valueChanged, this, &CVToolsWindow::setHmaxSlider);
    connect(ui.sminSlider, &QSlider::valueChanged, this, &CVToolsWindow::setSminSlider);
    connect(ui.smaxSlider, &QSlider::valueChanged, this, &CVToolsWindow::setSmaxSlider);
    connect(ui.vminSlider, &QSlider::valueChanged, this, &CVToolsWindow::setVminSlider);
    connect(ui.vmaxSlider, &QSlider::valueChanged, this, &CVToolsWindow::setVmaxSlider);

    connect(ui.actionOpen, &QAction::triggered, this, &CVToolsWindow::open);
    connect(ui.actionSave_as, &QAction::triggered, this, &CVToolsWindow::saveAs);
    connect(ui.actionPrint, &QAction::triggered, this, &CVToolsWindow::print);
    connect(ui.actionExit, &QAction::triggered, this, &QWidget::close);


    connect(ui.actionCopy, &QAction::triggered, this, &CVToolsWindow::copy);


    connect(ui.actionZoom_In, &QAction::triggered, this, &CVToolsWindow::zoomIn);
    connect(ui.actionZoomOut, &QAction::triggered, this, &CVToolsWindow::zoomOut);
    connect(ui.actionNormal_Size, &QAction::triggered, this, &CVToolsWindow::normalSize);
    connect(ui.actionFit_the_Window, &QAction::triggered, this, &CVToolsWindow::fitToWindow);

    connect(ui.actionAbout_Us, &QAction::triggered, this, &CVToolsWindow::about);


    ui.actionOpen->setShortcut(QKeySequence::Open);

    ui.actionAbout_Us->setShortcut(QKeySequence::Open);

    ui.actionSave_as->setEnabled(false);

    ui.actionPrint->setShortcut(QKeySequence::Print);
    ui.actionPrint->setEnabled(false);

    ui.actionExit->setShortcut(tr("Ctrl+Q"));

    ui.actionCopy->setShortcut(QKeySequence::Copy);
    ui.actionCopy->setEnabled(false);

    QAction* pasteAct = ui.menuEdit->addAction(tr("&Paste"), this, &CVToolsWindow::paste);
    pasteAct->setShortcut(QKeySequence::Paste);


    ui.actionZoom_In->setShortcut(QKeySequence::ZoomIn);
    ui.actionZoom_In->setEnabled(false);

    ui.actionZoomOut->setShortcut(QKeySequence::ZoomOut);
    ui.actionZoomOut->setEnabled(false);

    ui.actionNormal_Size->setShortcut(tr("Ctrl+S"));
    ui.actionNormal_Size->setEnabled(false);

    ui.actionFit_the_Window->setEnabled(false);
    ui.actionFit_the_Window->setCheckable(true);
    ui.actionFit_the_Window->setShortcut(tr("Ctrl+F"));


    ui.menuHelp->addAction(tr("About &Qt"), this, &QApplication::aboutQt);
}

void CVToolsWindow::updateActions()
{
    ui.actionSave_as->setEnabled(!image.isNull());
    ui.actionCopy->setEnabled(!image.isNull());
    ui.actionZoom_In->setEnabled(!ui.actionFit_the_Window->isChecked());
    ui.actionZoomOut->setEnabled(!ui.actionFit_the_Window->isChecked());
    ui.actionNormal_Size->setEnabled(!ui.actionFit_the_Window->isChecked());
}

bool CVToolsWindow::saveFile(const QString& fileName)
{
    QImageWriter writer(fileName);

    if (!writer.write(image)) {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
            tr("Cannot write %1: %2")
            .arg(QDir::toNativeSeparators(fileName), writer.errorString()));
        return false;
    }
    const QString message = tr("Wrote \"%1\"").arg(QDir::toNativeSeparators(fileName));
    statusBar()->showMessage(message);
    return true;
}


void CVToolsWindow::setImage(const QImage& newImage)
{
    image = newImage;
    if (image.colorSpace().isValid())
        image.convertToColorSpace(QColorSpace::SRgb);
    ui.imageLabel->setPixmap(QPixmap::fromImage(image));
    //! [4]
    scaleFactor = 1.0;

    ui.scrollArea->setVisible(true);
    ui.actionPrint->setEnabled(true);
    ui.actionFit_the_Window->setEnabled(true);
    updateActions();

    if (!ui.actionFit_the_Window->isChecked())
        ui.imageLabel->adjustSize();

    normalImage = CVToolsUtility::qimage2mat(image);
    initializeViews();
}

void CVToolsWindow::scaleImage(double factor)
{
    scaleFactor *= factor;
    ui.imageLabel->resize(scaleFactor * ui.imageLabel->pixmap(Qt::ReturnByValue).size());

    adjustScrollBar(ui.scrollArea->horizontalScrollBar(), factor);
    adjustScrollBar(ui.scrollArea->verticalScrollBar(), factor);

    ui.actionZoom_In->setEnabled(scaleFactor < 3.0);
    ui.actionZoomOut->setEnabled(scaleFactor > 0.333);
}

void CVToolsWindow::adjustScrollBar(QScrollBar* scrollBar, double factor)
{
    scrollBar->setValue(int(factor * scrollBar->value()
        + ((factor - 1) * scrollBar->pageStep() / 2)));
}

void CVToolsWindow::setMaskView() {
    ui.maskViewButton->setChecked(true);
    ui.hsvViewButton->setChecked(false);
    ui.normalViewButton->setChecked(false);
    updateViewType(*ui.maskViewButton);
}
void CVToolsWindow::setHSVView() {
    ui.maskViewButton->setChecked(false);
    ui.hsvViewButton->setChecked(true);
    ui.normalViewButton->setChecked(false);
    updateViewType(*ui.hsvViewButton);

}
void CVToolsWindow::setNormalView() {
    ui.maskViewButton->setChecked(false);
    ui.hsvViewButton->setChecked(false);
    ui.normalViewButton->setChecked(true);
    updateViewType(*ui.normalViewButton);

}

void CVToolsWindow::updateViewType(const QPushButton &button) {
    
    if (button.objectName() == ui.maskViewButton->objectName()) {
        changeToMaskView();
    }
    else if (button.objectName() == ui.hsvViewButton->objectName()) {
        changeToHSVView();
    }
    else {
        changeToNormalView();
    }
}

void CVToolsWindow::initializeViews() 
{
    cvtColor(normalImage, imageHSV, COLOR_RGB2HSV);
    cvtColor(normalImage, mask, COLOR_RGB2HSV);
    updateMaskHSV();
}
void CVToolsWindow::changeToMaskView() 
{
    /*Scalar minLevels(hmin, smin, vmin);
    Scalar maxLevels(hmax, smax, vmax);
    inRange(imageHSV, minLevels, maxLevels, mask);*/

    ui.imageLabel->setPixmap(CVToolsUtility::pixmapFromMat(mask));
    //cv::waitKey(10);
}

void CVToolsWindow::changeToHSVView() 
{


    ui.imageLabel->setPixmap(CVToolsUtility::pixmapFromMat(imageHSV));
}

void CVToolsWindow::changeToNormalView()
{
    if (image.colorSpace().isValid())
        image.convertToColorSpace(QColorSpace::SRgb);
    ui.imageLabel->setPixmap(QPixmap::fromImage(image));
}


void CVToolsWindow::updateMaskHSV() {
    Scalar minLevels(hmin, smin, vmin);
    Scalar maxLevels(hmax, smax, vmax);
    inRange(imageHSV, minLevels, maxLevels, mask);

    ui.imageLabel->setPixmap(CVToolsUtility::pixmapFromMat(mask));
    waitKey(0);
}

void CVToolsWindow::setHminSlider() {
    hmin = ui.hminSlider->value();
    ui.hminValue->setText(QString::number(hmin));
    if(ui.maskViewButton->isChecked())updateMaskHSV();
}
void CVToolsWindow::setHmaxSlider() {
    hmax = ui.hmaxSlider->value();
    ui.hmaxValue->setText(QString::number(hmax));
    if (ui.maskViewButton->isChecked())updateMaskHSV();

}
void CVToolsWindow::setSminSlider() {
    smin = ui.sminSlider->value();
    ui.sminValue->setText(QString::number(smin));
    if (ui.maskViewButton->isChecked())updateMaskHSV();

}
void CVToolsWindow::setSmaxSlider() {
    smax = ui.smaxSlider->value();
    ui.smaxValue->setText(QString::number(smax));
    if (ui.maskViewButton->isChecked())updateMaskHSV();

}
void CVToolsWindow::setVminSlider() {
    vmin = ui.vminSlider->value();
    ui.vminValue->setText(QString::number(vmin));
    if (ui.maskViewButton->isChecked())updateMaskHSV();

}
void CVToolsWindow::setVmaxSlider() {
    vmax = ui.vmaxSlider->value();
    ui.vmaxValue->setText(QString::number(vmax));
    if (ui.maskViewButton->isChecked())updateMaskHSV();

}

void CVToolsWindow::addNode()
{
    if (graphWidget->nodeList.count() == 4) return;
    ScanNode* newNode = new ScanNode(graphWidget);
    graphWidget->setSelectedNode(newNode);
    graphWidget->nodeList.push_back(newNode);
    graphWidget->scene()->addItem(newNode);
    newNode->setIndex(graphWidget->nodeList.count());
    newNode->setPos(0, 0);

    graphWidget->handleEdges();
}

void CVToolsWindow::warpImage()
{
    graphWidget->warpPerspective(ui.outputImage_scn);
}

void CVToolsWindow::addClassifier()
{
    openClassifierFile();

}

void CVToolsWindow::removeClassifier()
{
    objDetector->removeClassifier();
}
