#include <iostream>
#include <cmath>
#include <opencv2/opencv.hpp>

#include "filter.h"
#include "gradient.h"

using namespace cv;



int main() {

    // load the original picture in cv mat
    Mat image;
    image = imread("../data/image_simple_test.png", CV_LOAD_IMAGE_COLOR);
    if(! image.data) {
        std::cout << "Error loading picture" << std::endl;
        return -1;
    }

    Gradient gradient(image);

    return EXIT_SUCCESS;
}