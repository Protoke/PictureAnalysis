#include <iostream>
#include <opencv2/opencv.hpp>
#include <SDL2/SDL.h>
#include "window.h"

#include "filter.h"

using namespace cv;


int main() {

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

    cv::Mat img = imread("../data/Lenna.png");
    Filter f(cv::Mat::ones(cv::Size(3, 3), CV_32F));
    Mat result = f.apply(img);

    imshow("Result", result);
    waitKey(0);

    return EXIT_SUCCESS;
}