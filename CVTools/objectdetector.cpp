#include "objectdetector.h"
#include "utilities.h"
#include <iostream>


ObjectDetector::ObjectDetector()
{
	
}

ObjectDetector::~ObjectDetector()
{

}

void ObjectDetector::detectFromClassifier(cv::CascadeClassifier& cascade, double scale)
{
    vector<Rect> targets;
    Mat gray, smallImg;

    cvtColor(img, gray, cv::COLOR_RGB2GRAY); // Convert to Gray Scale
    double fx = 1 / scale;

    // Resize the Grayscale Image 
    resize(gray, smallImg, cv::Size(), fx, fx, cv::INTER_LINEAR);
    equalizeHist(smallImg, smallImg);

    // Detect targets of different sizes using cascade classifier 
    cascade.detectMultiScale(smallImg, targets, 1.1,
        2, 0 | cv::CASCADE_SCALE_IMAGE, cv::Size(30, 30));

    // Draw circles around the targets
    for (size_t i = 0; i < targets.size(); i++)
    {
        Rect r = targets[i];
        Mat smallImgROI;
        vector<Rect> nestedObjects;
        Point center;
        Scalar maviRenk = Scalar(255, 0, 0); // Color for Drawing tool
        Scalar kirmiziRenk = Scalar(0, 0, 255);
        int radius;

        double aspect_ratio = (double)r.width / r.height;
        if (0.75 < aspect_ratio && aspect_ratio < 1.3)
        {
            center.x = cvRound((r.x + r.width * 0.5) * scale);
            center.y = cvRound((r.y + r.height * 0.5) * scale);
            radius = cvRound((r.width + r.height) * 0.25 * scale);
            rectangle(img, r, kirmiziRenk, 2);
            //circle(img, center, radius, maviRenk, 3, 8, 0);
        }
        else
            rectangle(img, cv::Point(cvRound(r.x * scale), cvRound(r.y * scale)),
                cv::Point(cvRound((r.x + r.width - 1) * scale),
                    cvRound((r.y + r.height - 1) * scale)), maviRenk, 3, 8, 0);
    }

}

void ObjectDetector::addNewClassifier(const QString& path)
{
	for (int i = 0; i < classifierList.size(); i++) {
		if (classifierList[i] == path) {
			return;
		}
	}
	classifierList.push_back(path);
	new QListWidgetItem(path,ui->listWidget);
}

void ObjectDetector::removeClassifier()
{
	auto item = ui->listWidget->currentItem();


	for (int i = 0; i < classifierList.size(); i++) {
		if (classifierList[i] == item->text()) {
			classifierList.removeAt(i);
		}
	}

	delete item;
}

void ObjectDetector::scanImageWithClassifiers()
{
    for (size_t i = 0; i < classifierList.count(); i++)
    {
        CascadeClassifier cascade;
        cascade.load(classifierList[i].toStdString());
        detectFromClassifier(cascade, 1);
    }
    ui->objDetector_output->setPixmap(CVToolsUtility::pixmapFromMat(img.clone()));

}

void ObjectDetector::initMat(const QImage& qimage)
{
    img = CVToolsUtility::qimage_to_mat(qimage);
    std::cout << "aaaaa";

}
