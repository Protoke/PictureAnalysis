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

    Mat& apply(const Mat& img);

    static Mat horizontalGradient(int sizeX = 3, int sizeY = 3);
    static Mat verticalGradient(int sizeX = 3, int sizeY = 3);
};


#endif //PICTUREANALYSIS_FILTER_H
