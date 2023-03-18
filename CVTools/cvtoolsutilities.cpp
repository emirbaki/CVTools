//#include "cvtools.h"
//#include <opencv2/core.hpp>
//#include <opencv2/videoio.hpp>
//#include <opencv2/highgui.hpp>
//#include <opencv2/imgproc.hpp>
//#include <opencv2/objdetect.hpp>
//
//
//#include <QtWidgets>
//using namespace cv;
//
//Mat qimage2mat(const QImage& qimage) {
//    Mat mat = Mat(qimage.height(), qimage.width(), CV_8UC4, (uchar*)qimage.bits(), qimage.bytesPerLine());
//    Mat mat2 = Mat(mat.rows, mat.cols, CV_8UC3);
//    int from_to[] = { 0,0,  1,1,  2,2 };
//    cv::mixChannels(&mat, 1, &mat2, 1, from_to, 3);
//    return mat2;
//};