//
// Created by protoke on 18/09/17.
//

#include <iostream>
#include <assert.h>

#include "filter.h"

Filter::Filter() {}

Filter::Filter(const Mat& kernel) {
    kernel_ = kernel.clone();
    kernel_.convertTo(kernel_, CV_32F);

    absSum = 0.0;
    for (int i = 0; i < kernel_.rows; ++i) {
        for (int j = 0; j < kernel_.cols; ++j) {
            float a = kernel.at<float>(i,j);
            absSum += a;
        }
    }
}

Mat Filter::apply(const Mat& img) {
    assert(img.channels() == 3 || img.channels() == 1);

    int XRadius = (kernel_.rows-1)/2;
    int YRadius = (kernel_.cols-1)/2;
    Mat result = Mat(img.rows, img.cols, (img.channels() == 3)?CV_32FC3:CV_32F, (img.channels() == 3)?Scalar(0, 0, 0) : Scalar(0));

    // Insert the image into a bigger one to take care of border's ROI's problems
    Mat largeImg = extendPicture(img, XRadius);

    // Convolve
    for (int i = 0; i < result.rows; ++i) {
        for (int j = 0; j < result.cols; ++j) {
            // Extract the patch for the convolution
            Mat roi = largeImg(Range(i, i + XRadius*2 + 1), Range(j, j + YRadius*2 + 1));

            // Apply the patch
            Vec3f colorf;
            for(int k = 0; k < kernel_.rows; ++k){
                for (int l = 0; l < kernel_.cols; ++l) {
                    if(img.channels() == 3){
                        colorf = Vec3f((float)roi.at<Vec3b>(k, l)[0], (float)roi.at<Vec3b>(k, l)[1], (float)roi.at<Vec3b>(k, l)[2]);
                        result.at<Vec3f>(i, j) += colorf * kernel_.at<float>(k, l);
                    }
                    else
                        result.at<float>(i, j) += (float)roi.at<uchar>(k, l) * kernel_.at<float>(k, l);
                }
            }
        }
    }
    return result;
}
