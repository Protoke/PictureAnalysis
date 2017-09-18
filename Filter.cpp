//
// Created by protoke on 18/09/17.
//

#include "Filter.h"

Filter::Filter() {}

Filter::Filter(const Mat& kernel) {
    kernel.copyTo(kernel_);
}

Mat Filter::apply(const Mat& img) {
    return Mat();
}

static Mat Filter::horizontalGradient(int sizeX = 3, int sizeY = 3) {
    return Mat();
}

static Mat Filter::verticalGradient(int sizeX = 3, int sizeY = 3) {
    return Mat();
}