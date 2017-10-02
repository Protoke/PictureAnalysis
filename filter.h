//
// Created by protoke on 18/09/17.
//

#ifndef PICTUREANALYSIS_FILTER_H
#define PICTUREANALYSIS_FILTER_H

#include <opencv2/opencv.hpp>

using namespace cv;

enum type {PREWITT, SOBEL, KIRSCH};

class Filter {
    Mat kernel_;

public:
    Filter();
    Filter(const Mat& kernel);

    Mat apply(const Mat& img);

    static Mat horizontalGradient(int sizeX = 3, int sizeY = 3, type t = PREWITT);
    static Mat verticalGradient(int sizeX = 3, int sizeY = 3, type t = PREWITT);
};


#endif //PICTUREANALYSIS_FILTER_H
