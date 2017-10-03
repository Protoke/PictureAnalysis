//
// Created by protoke on 18/09/17.
//

#include <iostream>

#include "filter.h"

Filter::Filter() {}

Filter::Filter(const Mat& kernel) {
    kernel.copyTo(kernel_);
}

Mat Filter::apply(const Mat& img) {
    assert(img.channels() == 3 || img.channels() == 1);

    int XRadius = (kernel_.rows-1)/2;
    int YRadius = (kernel_.cols-1)/2;
    Mat result = Mat(img.rows, img.cols, (img.channels() == 3)?CV_32FC3:CV_32F, (img.channels() == 3)?Scalar(0, 0, 0) : Scalar(0.0));
    Mat largeImg = Mat(img.rows + XRadius*2, img.cols + YRadius*2, img.type());

    // Insert the image into a bigger one to take care of border's ROI's problems
    cv::Range ranges[2];
    ranges[0] = cv::Range(XRadius, largeImg.rows - XRadius);
    ranges[1] = cv::Range(YRadius, largeImg.cols - YRadius);
    img.copyTo(largeImg(ranges));

    // Convolve
    for (int i = 0; i < result.rows; ++i) {
        for (int j = 0; j < result.cols; ++j) {
            // Extract the patch for the convolution
            Point p1 = Point(i, j);
            Point p2 = Point(i + XRadius*2, j + YRadius*2);
            Rect rectRoi = Rect(p1, p2);
            Mat roi = largeImg(rectRoi);
            Mat roiConverted;
            roi.convertTo(roiConverted, (img.channels() == 3)?CV_32FC3:CV_32F);

            for(int k = 0; k < kernel_.rows; ++k){
                for (int l = 0; l < kernel_.cols; ++l) {
                    if(img.channels() == 3)
                        result.at<Vec3f>(j, i) += roiConverted.at<Vec3f>(k, l) * kernel_.at<float>(k, l);
                    else
                        result.at<float>(j, i) += roi.at<float>(k, l) * kernel_.at<float>(k, l);
                }
            }
        }
    }

    Mat resultConverted;
    result.convertTo(resultConverted, (img.channels() == 3)?CV_8UC3:CV_8U);

    return resultConverted;
}

Mat Filter::horizontalGradient(int sizeX, int sizeY) {
    return Mat();
}

Mat Filter::verticalGradient(int sizeX, int sizeY) {
    return Mat();
}