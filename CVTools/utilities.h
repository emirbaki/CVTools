#pragma once
#ifndef CVTOOLSUTILITIES
#define CVTOOLSUTILITIES

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/imgcodecs.hpp>

#include <QtWidgets>

using namespace cv;
using namespace std;

class CVToolsUtility {

public:
	static QImage imageFromMat(Mat const& src);
	static Scalar randomScalar();
	static QPixmap pixmapFromMat(const cv::Mat& src);
#ifndef QT_NO_CLIPBOARD
	static QImage clipboardImage();
#endif
	static void matDeleter(void* mat);
	static Mat qimage2mat(const QImage& qimage);
	static Mat qimage_to_mat(const QImage& qimage);
};

#endif