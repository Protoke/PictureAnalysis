#include <iostream>
#include <opencv2/opencv.hpp>

#include "filter.h"

int main() {
    cv::Mat img = imread("../data/Lenna.png");
    Filter f(cv::Mat::ones(cv::Size(3, 3), CV_32F));
    Mat result = f.apply(img);

    imshow("Result", result);
    waitKey(0);

    return 0;
}