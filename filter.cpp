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
    int XRadius = (kernel_.rows-1)/2;
    int YRadius = (kernel_.cols-1)/2;
    Mat result = Mat::zeros(img.rows, img.cols, CV_32FC3);
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
//            std::cout << p1 << " ; " << p2 << std::endl;
            Rect rectRoi = Rect(p1, p2);
            Mat roi = largeImg(rectRoi);

            for(int k = 0; k < kernel_.rows; ++k){
                for (int l = 0; l < kernel_.cols; ++l) {
                    for (int m = 0; m < kernel_.channels(); ++m) {
                        result.at<CV_32F>(k, l, m) += (float)roi.at<uchar>(k, l, m) * (float)kernel_.at<uchar>(k, l, m);
                    }
                }
            }
        }
    }

    imshow("resultTemp", result);
    waitKey(0);

    return result;
}

Mat Filter::horizontalGradient(int sizeX, int sizeY) {
    return Mat();
}

Mat Filter::verticalGradient(int sizeX, int sizeY) {
    return Mat();
}