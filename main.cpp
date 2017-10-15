#include <iostream>
#include <opencv2/opencv.hpp>

#include "threshold.h"
#include "gradient.h"
#include "graphe.h"
#include "contour.h"
#include <string.h>

using namespace cv;

Mat mat2gray(const Mat& src){
    Mat dst;
    normalize(src, dst, 0.0, 255.0, NORM_MINMAX, CV_8U);
    return dst;
}


int main() {
    bool testContour = true;
    if(testContour) {
        uchar img[] =  {  0,   0,   0,  0,  0,
                        255, 255, 255,  0, 255,
                          0,   0,   0,  0,  0 };
        float ori[] =  {0.0,   0.0,  0.0,  0.0,  0.0,
                        90.0, 90.0, 90.0,  0.0,  90.0,
                        0.0,   0.0,  0.0,  0.0,  0.0 };
        Mat affine(3, 5, CV_8U, img);
        Mat orientation(3, 5, CV_32F, ori);
        Contour contour(affine, orientation);

    }else {
        // load the original picture in cv mat
        Mat image;
        //image = imread("../data/Lenna.png", CV_LOAD_IMAGE_COLOR);
        image = imread("../data/image_simple_test.png", CV_LOAD_IMAGE_COLOR);
        if(! image.data) {
            std::cout << "Error loading picture" << std::endl;
            return -1;
        }
        imshow("Source", image);

        // calcul gradients, magnitude, orientation de l'image
        Gradient gradient(image, 3, Gradient::PREWITT, Gradient::E | Gradient::N);// | Gradient::SW | Gradient::SE);

        // affichage des gradients
//    for(unsigned int i = 0;i < gradient._gradients.size();i++) {
//        std::string title = "G" + std::to_string(i);
//        imshow(title, gradient._gradients[i]);
//    }

        // affichage magnitude et orientation
        imshow("Magnitude", mat2gray(gradient._magnitude));
        imshow("Orientation", mat2gray(gradient._orientation));
        imshow("Orientation Map", gradient._orientation_map);
        Mat seuil = hysteresisThreshold(gradient._magnitude, 50, 25, 3);
        imshow("Seuillage", seuil);
        Mat affine;
        refineContour(gradient._magnitude, gradient._orientation, seuil, affine, 5);
        imshow("Affinage", affine);
    }



    waitKey(0);


    return EXIT_SUCCESS;
}