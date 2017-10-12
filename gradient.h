//
// Created by ad on 10/10/17.
//

#ifndef PICTUREANALYSIS_GRADIENT_H
#define PICTUREANALYSIS_GRADIENT_H

#include <opencv2/opencv.hpp>
#include <array>

using namespace cv;

class Gradient {

private:
    unsigned int _rows;
    unsigned int _cols;

    void magnitude();
    void orientation();
    void orientation_map();
    void refineContour(int range);

public :
    enum flags {
        N   =   0x01,
        NE  =   0x02,
        E   =   0x04,
        SE  =   0x08,
        S   =   0x10,
        SW  =   0x20,
        W   =   0x40,
        NW  =   0x80
    };
    enum type {
        PREWITT,
        SOBEL,
        KIRSCH
    };

    std::vector<Mat> _gradients;
    Mat _magnitude;
    Mat _orientation;
    Mat _orientation_map;


    Gradient(const Mat& img, bool display);
    Gradient(const Mat& img, int size = 3, type t = PREWITT, int flags = S | E);

    float getWeightedAngle(Mat& mag, Mat& ori);

    Mat horizontalTopGradient(int size = 3, type t = PREWITT);
    Mat verticalLeftGradient(int size = 3, type t = PREWITT);
    Mat horizontalBottomGradient(int size = 3, type t = PREWITT);
    Mat verticalRightGradient(int size = 3, type t = PREWITT);
    Mat diagonalTopLeftGradient(int size = 3, type t = PREWITT);
    Mat diagonalTopRightGradient(int size = 3, type t = PREWITT);
    Mat diagonalBottomLeftGradient(int size = 3, type t = PREWITT);
    Mat diagonalBottomRightGradient(int size = 3, type t = PREWITT);
    std::array<Mat, 8> boussoleGradient(int size = 3, type t = PREWITT);

};


#endif //PICTUREANALYSIS_GRADIENT_H
