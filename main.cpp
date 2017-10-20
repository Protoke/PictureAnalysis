#include <iostream>
#include <opencv2/opencv.hpp>

#include "threshold.h"
#include "gradient.h"
#include "contour.h"
#include <string.h>

using namespace cv;

Mat mat2gray(const Mat& src){
    Mat dst;
    normalize(src, dst, 0.0, 255.0, NORM_MINMAX, CV_8U);
    return dst;
}

void computeContours(const Mat& image, int blurSize,
                     float thresholdHigh, float thresoldLow, int thresholdSize,
                     int refiningSize,
                     bool showIntermediate = false){

    GaussianBlur(image, image, Size(blurSize, blurSize), 0, 0);
    if(showIntermediate)
        imshow("Blur", image);

    // calcul gradients, magnitude, orientation de l'image
//    Gradient gradient(image, 3, Gradient::PREWITT, Gradient::E | Gradient::S | Gradient::SW | Gradient::SE);
    Gradient gradient(image, 3, Gradient::PREWITT, Gradient::E | Gradient::N);

    // affichage des gradients
//    for(unsigned int i = 0;i < gradient._gradients.size();i++) {
//        std::string title = "G" + std::to_string(i);
//        imshow(title, gradient._gradients[i]);
//    }

    // affichage magnitude et orientation
    if(showIntermediate) {
        imshow("Magnitude", mat2gray(gradient._magnitude));
        imshow("Orientation", mat2gray(gradient._orientation));
        imshow("Orientation Map", gradient._orientation_map);
    }

    Mat seuil = hysteresisThreshold(gradient._magnitude, thresholdHigh, thresoldLow, thresholdSize);
    if(showIntermediate)
        imshow("Seuillée", seuil);

    Mat affine;
    refineContour(gradient._magnitude, gradient._orientation, seuil, affine, refiningSize);
    if(showIntermediate)
        imshow("Affinage", affine);

    Contour contour(affine, gradient._orientation, gradient._magnitude, seuil, 10);
    Mat chains = contour.draw_chains();
    imshow("Chains", chains);
}


int main() {
    // load the original picture in cv mat
    Mat image;
    image = imread("../data/Lenna.png", CV_LOAD_IMAGE_COLOR);
    //image = imread("../data/image_simple_test.png", CV_LOAD_IMAGE_COLOR);
    if (!image.data) {
        std::cout << "Error loading picture" << std::endl;
        return -1;
    }
    imshow("Source", image);

    computeContours(image, 3, 100, 75, 3, 3, true);
//    computeContours(image, 3, 100, 75, 3, 3, true);


    waitKey(0);


    return EXIT_SUCCESS;
}