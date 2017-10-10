//
// Created by protoke on 09/10/17.
//

#include "threshold.h"

Mat threshold(const Mat& src, double thresholdValue){
    // Split of the image channels into many gray-scale images
    Mat channels[src.channels()];
    split(src, channels);

    // Threshold each channel by the threshold value
    Mat thresholds[src.channels()];
    for (int c = 0; c < src.channels(); ++c) {
        thresholds[c] = (channels[c] >= thresholdValue);
    }

    // Merge every threshold image into a multi-channel image to return
    Mat threshold;
    merge(thresholds, src.channels(), threshold);

    return threshold;
}

Mat globalThreshold(const Mat& src){
    // Split of the image channels into many gray-scale images
    Mat channels[src.channels()];
    split(src, channels);

    // Compute the mean of each channels of the image
    Scalar meanVals = mean(src);

    // Threshold each channel by its mean
    Mat thresholds[src.channels()];
    for (int c = 0; c < src.channels(); ++c) {
        thresholds[c] = (channels[c] >= meanVals[c]);
    }

    // Merge every threshold image into a multi-channel image to return
    Mat threshold;
    merge(thresholds, src.channels(), threshold);

    return threshold;
}

Mat localThreshold(const Mat& src, int size){
    assert(size%2 == 1);

    int radius = (size-1)/2;
    Mat threshold = Mat(src.rows, src.cols, CV_8UC(src.channels()), Scalar::all(0.0));
    cv::Range ranges[2];

    // Apply a global threshold on a local patch of the image
    // A point is above the final thresold if it is above one of the local thresolds on it
    for (int i = 0; i < src.rows; ++i) {
        for (int j = 0; j < src.cols; ++j) {
            ranges[0] = cv::Range(max(0, i - radius), min(src.rows, i + radius + 1));
            ranges[1] = cv::Range(max(0, j - radius), min(src.cols, j + radius + 1));

            Mat lThres = globalThreshold(src(ranges));
            int iThres = min(i, radius);
            int jThres = min(j, radius);
            Range a = Range(i, i+1);
            Range b = Range(j, j+1);
            Range c = Range(iThres, iThres+1);
            Range d = Range(jThres, jThres+1);
            threshold(a, b) |= lThres(c, d);
        }
    }

    return threshold;
}

Mat hysteresisThreshold(const Mat& src, double highThreshold, double lowThreshold, int size){
    // First threshold, trusted points
    Mat thresholdH = threshold(src, highThreshold);
    // Second threshold, points to validate
    Mat thresholdL = threshold(src, lowThreshold);

    // Init final threshold channels by the trusted values
    Mat threshold = thresholdH.clone();
    // Split the threshold to work channel per channel
    Mat tChannels[src.channels()];
    split(threshold, tChannels);

    // Test every points to validate
    int radius = (size-1)/2;
    cv::Range ranges[2];
    for (int i = 0; i < src.rows; ++i) {
        for (int j = 0; j < src.cols; ++j) {
            if(thresholdL.at<uchar>(i,j) != 0){
                ranges[0] = cv::Range(max(0, i - radius), min(src.rows, i + radius + 1));
                ranges[1] = cv::Range(max(0, j - radius), min(src.cols, j + radius + 1));

                // Sum of the high tresholding in a patch around the tested point
                Scalar res = sum(thresholdH(ranges));

                for (int c = 0; c < src.channels(); ++c) {
                    if(res[c] > 0)
                        tChannels[c].at<uchar>(i,j) |= 255;
                }
            }
        }
    }

    // Merge channels to return
    merge(tChannels, src.channels(), threshold);

    return threshold;
}