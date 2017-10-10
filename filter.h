//
// Created by protoke on 18/09/17.
//

#ifndef PICTUREANALYSIS_FILTER_H
#define PICTUREANALYSIS_FILTER_H

#include <opencv2/opencv.hpp>

using namespace cv;

class Filter {
    Mat kernel_;

public:

    Filter();
    Filter(const Mat& kernel);

    Mat apply(const Mat& img);
};


#endif //PICTUREANALYSIS_FILTER_H
