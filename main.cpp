#include <iostream>
#include <opencv2/opencv.hpp>

#include "filter.h"

int main() {
    cv::Mat img = imread("../data/horizontal.png");
    Filter f(cv::Mat::zeros(cv::Size(3, 3), CV_32F));

    f.apply(img);

    return 0;
}