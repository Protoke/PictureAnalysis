#include <iostream>
#include <opencv2/opencv.hpp>

#include "filter.h"
#include "threshold.h"
#include "gradient.h"
#include <string.h>

using namespace cv;

Mat mat2gray(const cv::Mat& src){
    Mat dst;
    normalize(src, dst, 0.0, 255.0, cv::NORM_MINMAX, CV_8U);
    return dst;
}

int main() {

    // load the original picture in cv mat
    Mat image;
//    image = imread("../data/Lenna.png", CV_LOAD_IMAGE_COLOR);
    image = imread("../data/image_simple_test.png", CV_LOAD_IMAGE_COLOR);
    if(! image.data) {
        std::cout << "Error loading picture" << std::endl;
        return -1;
    }
    imshow("Source", image);

    // calcul gradients, magnitude, orientation de l'image
    Gradient gradient(image, 3, Gradient::PREWITT, Gradient::E | Gradient::N);

    // affichage des gradients
//    for(unsigned int i = 0;i < gradient._gradients.size();i++) {
//        std::string title = "G" + std::to_string(i);
//        imshow(title, gradient._gradients[i]);
//    }

    // affichage magnitude et orientation
    imshow("Magnitude", mat2gray(gradient._magnitude));
    imshow("Orientation", mat2gray(gradient._orientation));
//    imshow("Orientation Map", gradient._orientation_map);
    Mat seuil = hysteresisThreshold(gradient._magnitude, 50, 25, 3);
    imshow("Seuillée", seuil);
    Mat affine;
    refineContour(gradient._magnitude, gradient._orientation, seuil, affine, 5);
    imshow("Affinage", affine);

//    cv::Mat img2 = imread("../data/Lenna.png", CV_LOAD_IMAGE_COLOR);
//    cv::Mat img2 = imread("../data/image_simple.test.png", CV_LOAD_IMAGE_COLOR);
//    cvtColor(img2, img2, cv::COLOR_RGB2GRAY);

//    imshow("Source", img2);

//    Mat filt = Mat(3, 3, CV_32F, Scalar(0.0));
//    filt.at<float>(0,0) = 1.0/9.0; filt.at<float>(0,1) = 1.0/9.0; filt.at<float>(0,2) = 1.0/9.0;
//    filt.at<float>(1,0) = 1.0/9.0; filt.at<float>(1,1) = 1.0/9.0; filt.at<float>(1,2) = 1.0/9.0;
//    filt.at<float>(2,0) = 1.0/9.0; filt.at<float>(2,1) = 1.0/9.0; filt.at<float>(2,2) = 1.0/9.0;

//    filt.at<float>(0,0) = -1; filt.at<float>(0,1) = 0; filt.at<float>(0,2) = 1;
//    filt.at<float>(1,0) = -1; filt.at<float>(1,1) = 0; filt.at<float>(1,2) = 1;
//    filt.at<float>(2,0) = -1; filt.at<float>(2,1) = 0; filt.at<float>(2,2) = 1;

//    filt.at<float>(0,0) = -1; filt.at<float>(0,1) = -1; filt.at<float>(0,2) = -1;
//    filt.at<float>(1,0) = 0; filt.at<float>(1,1) = 0; filt.at<float>(1,2) = 0;
//    filt.at<float>(2,0) = 1; filt.at<float>(2,1) = 1; filt.at<float>(2,2) = 1;

//    filt.at<float>(1,1) = 1.0;

    /*filt = Gradient::horizontalTopGradient();

    Filter f(filt);
    Mat result = f.apply(img2);
    result.convertTo(result, CV_8UC3);
    imshow("Filtre : Moi", abs(result));*/
//    Mat channels[3];
//    split(result, channels);
//    imshow("R", abs(channels[0]));
//    imshow("G", abs(channels[1]));
//    imshow("B", abs(channels[2]));

//    Mat result2;
//    filter2D(img2, result2, (img2.channels() == 3)?CV_32FC3:CV_32F , filt);
//    result2.convertTo(result2, CV_8UC3);
//    imshow("Opencv", abs(result2));

    /*Mat resG = globalThreshold(result);
    imshow("Global", resG);
    Mat resL = localThreshold(result, 9);
    imshow("Local", resL);
    Mat resH = hysteresisThreshold(result, 80, 80*0.8, 7);
    imshow("Hyseteris", resH);*/

    waitKey(0);


    return EXIT_SUCCESS;
}