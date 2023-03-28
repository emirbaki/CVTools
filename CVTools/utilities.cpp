#pragma once

#include "utilities.h"


QImage CVToolsUtility::imageFromMat(Mat const& src) {
    //Q_ASSERT(src.type() == CV_8UC3);
    //cv::Mat* mat = new cv::Mat(src.rows, src.cols, src.type());
    //cvtColor(src, *mat, COLOR_BGR2RGB);
    /*return QImage((uchar*)mat->data, mat->cols, mat->rows, mat->step,
        QImage::Format_RGB888, &matDeleter, mat);*/
    if (src.channels() == 3) {
        QImage a((uchar*)src.data, src.cols, src.rows, src.step,
            QImage::Format_RGB888);
        a.rgbSwap();
        return a;
    }
    else if (src.channels() == 4) {
        QImage a((uchar*)src.data, src.cols, src.rows, src.step,
            QImage::Format_RGB32);
        a.rgbSwap();
        return a;
    }
    else if (src.channels() == 1) {
        return QImage((uchar*)src.data, src.cols, src.rows, src.step,
            QImage::Format_Indexed8);

    }
}

Scalar CVToolsUtility::randomScalar() {
    static RNG rng(12345);
    return Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
}

QPixmap CVToolsUtility::pixmapFromMat(const Mat& src) {
    QImage image(imageFromMat(src));
    auto a = src.channels();


    return QPixmap::fromImage(image);
}

#ifndef QT_NO_CLIPBOARD
QImage CVToolsUtility::clipboardImage()
{
    if (const QMimeData* mimeData = QGuiApplication::clipboard()->mimeData()) {
        if (mimeData->hasImage()) {
            const QImage image = qvariant_cast<QImage>(mimeData->imageData());
            if (!image.isNull())
                return image;
        }
    }
    return QImage();
}
#endif // !QT_NO_CLIPBOARD

void CVToolsUtility::matDeleter(void* mat) { delete static_cast<Mat*>(mat); }

Mat CVToolsUtility::qimage2mat(const QImage& qimage) {
    Mat mat = Mat(qimage.height(), qimage.width(), CV_8UC4, (uchar*)qimage.bits(), qimage.bytesPerLine());
    Mat mat2 = Mat(mat.rows, mat.cols, CV_8UC3);
    int from_to[] = { 0,0,  1,1,  2,2 };
    cv::mixChannels(&mat, 1, &mat2, 1, from_to, 3);
    return mat2;
};


Mat CVToolsUtility::qimage_to_mat(const QImage& qimage) {
    Mat mat;

    switch (qimage.format()) {
    case QImage::Format_RGB888:
        mat = Mat(qimage.height(), qimage.width(), CV_8UC3, const_cast<uchar*>(qimage.bits()), qimage.bytesPerLine());
        cvtColor(mat, mat, COLOR_RGB2BGR);
        break;
    case QImage::Format_RGBA8888:
        mat = Mat(qimage.height(), qimage.width(), CV_8UC4, const_cast<uchar*>(qimage.bits()), qimage.bytesPerLine());
        cvtColor(mat, mat, COLOR_RGBA2BGRA);
        break;
    case QImage::Format_RGB32:
        mat = Mat(qimage.height(), qimage.width(), CV_8UC4, const_cast<uchar*>(qimage.bits()), qimage.bytesPerLine());
        cvtColor(mat, mat, COLOR_RGBA2BGRA);
        break;
    case QImage::Format_Grayscale8:
        mat = Mat(qimage.height(), qimage.width(), CV_8UC1, const_cast<uchar*>(qimage.bits()), qimage.bytesPerLine());
        break;
    default:
        throw std::runtime_error("Unsupported image format");
    }
    //return mat.clone();
    return mat.clone();
}