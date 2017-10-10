//
// Created by protoke on 18/09/17.
//

#ifndef PICTUREANALYSIS_FILTER_H
#define PICTUREANALYSIS_FILTER_H

#include <opencv2/opencv.hpp>

using namespace cv;

class Filter {
    Mat kernel_;
    float absSum;

public:

    Filter();
    Filter(const Mat& kernel);

    Mat apply(const Mat& img);
};

Mat extendPicture(const Mat& src, int padding);

#endif //PICTUREANALYSIS_FILTER_H
