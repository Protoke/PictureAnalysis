//
// Created by protoke on 18/09/17.
//

#include <iostream>

#include "filter.h"

Filter::Filter() {}

Filter::Filter(const Mat& kernel) {
    kernel.copyTo(kernel_);
}

Mat& Filter::apply(const Mat& img) {
    Mat result(img.rows, img.cols, CV_32F);

    for (int i = 0; i < img.rows; ++i) {
        for (int j = 0; j < img.cols; ++j) {

            int decalX = (kernel_.rows-1)/2;
            int decalY = (kernel_.cols-1)/2;

            Rect rectRoi = Rect(Point(i - decalX, i - decalY), Point(i + decalX + 1, i + decalY + 1));
            Mat roi = img(rectRoi);

            std::cout << "debug" << std::endl;
        }
    }

}

Mat Filter::horizontalGradient(int sizeX, int sizeY) {
    return Mat();
}

Mat Filter::verticalGradient(int sizeX, int sizeY) {
    return Mat();
}