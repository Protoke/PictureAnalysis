//
// Created by ad on 10/10/17.
//

#ifndef PICTUREANALYSIS_GRADIENT_H
#define PICTUREANALYSIS_GRADIENT_H

#include <opencv2/opencv.hpp>
#include <array>

using namespace cv;

enum type {PREWITT, SOBEL, KIRSCH};

class Gradient {

public :
    Gradient(const Mat& image);

    static float GetWeightedAngle(Mat& mag, Mat& ang);
    static Mat mat2gray(const cv::Mat& src);
    static Mat orientationMap(const cv::Mat& mag, const cv::Mat& ori, double thresh = 1.0);

    static Mat horizontalTopGradient(int size = 3, type t = PREWITT);
    static Mat verticalLeftGradient(int size = 3, type t = PREWITT);
    static Mat horizontalBottomGradient(int size = 3, type t = PREWITT);
    static Mat verticalRightGradient(int size = 3, type t = PREWITT);
    static Mat diagonalTopLeftGradient(int size = 3, type t = PREWITT);
    static Mat diagonalTopRightGradient(int size = 3, type t = PREWITT);
    static Mat diagonalBottomLeftGradient(int size = 3, type t = PREWITT);
    static Mat diagonalBottomRightGradient(int size = 3, type t = PREWITT);
    static std::array<Mat, 8> boussoleGradient(int size = 3, type t = PREWITT);

private :
    // TODO : stocker l'orientation etc dans des champs

};


#endif //PICTUREANALYSIS_GRADIENT_H
