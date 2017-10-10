//
// Created by protoke on 09/10/17.
//

#ifndef PICTUREANALYSIS_THRESHOLD_H
#define PICTUREANALYSIS_THRESHOLD_H

#include <opencv2/opencv.hpp>

using namespace cv;

Mat threshold(const Mat& src, double thresholdValue);
Mat globalThreshold(const Mat& src);
Mat localThreshold(const Mat& src, int size);
Mat hysteresisThreshold(const Mat& src, double highThreshold, double lowThreshold, int size);

#endif //PICTUREANALYSIS_THRESHOLD_H
