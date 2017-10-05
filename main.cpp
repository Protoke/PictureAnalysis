#include <iostream>
#include <opencv2/opencv.hpp>
#include <SDL2/SDL.h>
#include "window.h"

#include "filter.h"

using namespace cv;


int main() {

    Filter fi;
    //fi.horizontalGradient(3,3, PREWITT);
    fi.verticalGradient(3,3, SOBEL);

    Window window("Picture Analysis", 800, 600);

    // load the original picture in cv mat
    Mat img;
    img = imread("../data/horizontal.png", CV_LOAD_IMAGE_COLOR);

    if(! img.data) {
        std::cout << "Error loading picture" << std::endl;
        return -1;
    }

    // load the cv mat in the window
    window.fill_texture_from_CVmat(img);

    // main loop
    while(window.events()) {
        // key events
        if(window.key_state(SDLK_ESCAPE))
            break;

        // display
        window.render();
    }

    // exit
    window.close();

    cv::Mat img2 = imread("../data/Lenna.png");
    cvtColor(img2, img2, cv::COLOR_RGB2GRAY);

    Mat filt = Mat(3, 3, CV_32F, Scalar(0.0));
//    filt.at<float>(0,0) = 1.0/9.0; filt.at<float>(0,0) = 1.0/9.0; filt.at<float>(0,0) = 1.0/9.0;
//    filt.at<float>(1,0) = 1.0/9.0; filt.at<float>(0,0) = 1.0/9.0; filt.at<float>(0,0) = 1.0/9.0;
//    filt.at<float>(2,0) = 1.0/9.0; filt.at<float>(0,0) = 1.0/9.0; filt.at<float>(0,0) = 1.0/9.0;
    filt.at<float>(1,1) = 1.0;

    Filter f(filt);
    Mat result = f.apply(img2);
    Mat resultConverted;
    result.convertTo(resultConverted, CV_8U);
    imshow("Result1", resultConverted);
    waitKey(0);

    Mat result2;
    filter2D(img2, result2, -1 , filt);
    imshow("Result2", result2);
    waitKey(0);

    return EXIT_SUCCESS;
}