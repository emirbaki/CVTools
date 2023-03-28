#pragma once
#include <QtWidgets>
#include <ui_qtcvtools.h>

#include <opencv2/core.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/imgproc.hpp>

using namespace std;

class ObjectDetector {

public:
	ObjectDetector();
	~ObjectDetector();

	Ui::CVToolsClass* ui;

	void addNewClassifier(const QString& path);
	void removeClassifier();
	void scanImageWithClassifiers();
	void initMat(const QImage& qimage);

private:

	cv::Mat img;

	QList<QString> classifierList;

	void detectFromClassifier(cv::CascadeClassifier& cascade, double scale);

	
};