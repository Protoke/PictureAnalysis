//
// Created by protoke on 18/09/17.
//

#include <iostream>
#include <assert.h>

#include "filter.h"

Filter::Filter() {}

Filter::Filter(const Mat& kernel) {
    kernel.copyTo(kernel_);
}

Mat Filter::apply(const Mat& img) {
    assert(img.channels() == 3 || img.channels() == 1);

    int XRadius = (kernel_.rows-1)/2;
    int YRadius = (kernel_.cols-1)/2;
    Mat result = Mat(img.rows, img.cols, (img.channels() == 3)?CV_32FC3:CV_32F, (img.channels() == 3)?Scalar(0, 0, 0) : Scalar(0));
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

            for(int k = 0; k < kernel_.rows; ++k){
                for (int l = 0; l < kernel_.cols; ++l) {
                    if(img.channels() == 3){
                        Vec3f colorf = Vec3f(
                                (float)roi.at<Vec3b>(k, l)[0],
                                (float)roi.at<Vec3b>(k, l)[1],
                                (float)roi.at<Vec3b>(k, l)[2]);
                        result.at<Vec3f>(j, i) += colorf * kernel_.at<float>(k, l);
                    }
                    else
                        result.at<float>(j, i) += (float)roi.at<u_int8_t>(k, l) * kernel_.at<float>(k, l);
                }
            }
        }
    }
    return result;
}

Mat Filter::horizontalGradient(int sizeX, int sizeY, type t) {
    // sizeX and sizeY must be even
    assert(sizeX%2 == 1 && sizeY%2 == 1);

    Mat_<int> mat;
    Mat_<int> matRes;
    switch(t) {
        case PREWITT : {
            for(int i = 0;i < sizeY / 2;i++) {
                for(int j = 0;j < sizeX;j++) {
                    mat.push_back(-1);
                }
            }
            for(int j = 0;j < sizeX;j++) {
                mat.push_back(0);
            }
            for(int i = sizeY / 2 + 1;i < sizeY;i++) {
                for(int j = 0;j < sizeX;j++) {
                    mat.push_back(1);
                }
            }
            matRes = mat.reshape(sizeX, sizeY);
            return matRes;
        }
        case SOBEL : {
            for(int i = 0;i < sizeY / 2;i++) {
                for(int j = 0;j < sizeX / 2;j++) {
                    mat.push_back(-1);
                }
                mat.push_back(-2);
                for(int j = sizeX / 2 + 1;j < sizeX;j++) {
                    mat.push_back(-1);
                }
            }
            for(int j = 0;j < sizeX;j++) {
                mat.push_back(0);
            }
            for(int i = sizeY / 2 + 1;i < sizeY;i++) {
                for(int j = 0;j < sizeX / 2;j++) {
                    mat.push_back(1);
                }
                mat.push_back(2);
                for(int j = sizeX / 2 + 1;j < sizeX;j++) {
                    mat.push_back(1);
                }
            }
            matRes = mat.reshape(sizeX, sizeY);
            return matRes;
        }
        case KIRSCH : {
            for(int i = 0;i < sizeY / 2 + 1;i++) {
                for(int j = 0;j < sizeX;j++) {
                    mat.push_back(-3);
                }
            }
            for(int i = sizeY / 2 + 1;i < sizeY;i++) {
                for(int j = 0;j < sizeX;j++) {
                    mat.push_back(5);
                }
            }
            mat.at<int>(sizeY/2 + 1 + sizeX/2 + 1) = 0;
            matRes = mat.reshape(sizeX, sizeY);
            return matRes;
        }
        default : return Mat::zeros(sizeY, sizeY, CV_32FC3);
    }
}

Mat Filter::verticalGradient(int sizeX, int sizeY, type t) {
    Mat mat = horizontalGradient(sizeX, sizeY, t);
    mat = mat.t();
    return mat;
}