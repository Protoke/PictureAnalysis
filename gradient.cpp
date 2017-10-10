//
// Created by ad on 10/10/17.
//

#include "gradient.h"
#include "filter.h"

#include <assert.h>

// TODO nettoyer constructeur
Gradient::Gradient(const Mat& img) {
    Mat image;
    // convert rgb to gray and type CV_8UC1
    cvtColor(img, image, COLOR_RGB2GRAY);
    image.convertTo(image, CV_8UC1);

    Mat X, Y;
    Filter fi;
    // TODO remplacer par apply de la classe Filter
    filter2D(image, Y, CV_32F , horizontalTopGradient(3, SOBEL));
    filter2D(image, X, CV_32F , verticalLeftGradient(3, SOBEL));

    // calculate magnitude
    Mat mag; // magnitude = sqrt(X^2 + Y^2);
    Mat X_2; multiply(X, X, X_2);
    Mat Y_2; multiply(Y, Y, Y_2);
    add(X_2, Y_2, mag);
    sqrt(mag, mag);

    // calculate orientation
    Mat orientation(mag.rows, mag.cols, CV_32F);
    for(unsigned int i = 0;i < orientation.rows;i++) {
        for(unsigned int j = 0;j < orientation.cols;j++) {
            orientation.at<float>(i,j) = fastAtan2(Y.at<float>(i,j), X.at<float>(i,j)) * 180/M_PI; // angle in degrees
        }
    }
    normalize(orientation, orientation, 0, 360, NORM_MINMAX);

    // color a map in function of the orientation
    Mat oriMap = orientationMap(mag, orientation, 1.0);

    normalize(mag,mag,0,1,cv::NORM_MINMAX);
    Mat angRes = Mat::zeros(image.rows,image.cols,CV_8UC1);
    int blockSize = image.cols/50-1;
    float r = blockSize;

    for (int i=0;i< image.rows-blockSize;i+= blockSize)
    {
        for (int j=0;j< image.cols-blockSize;j+= blockSize)
        {
            Mat m = mag(Rect(j,i,blockSize,blockSize));
            Mat o = orientation(Rect(j,i,blockSize,blockSize));
            float a = GetWeightedAngle(m,o);

            if(a != -1) {
                float dx=r*cos(a);
                float dy=r*sin(a);
                int x=j;
                int y=i;

                cv::line(angRes,cv::Point(x,y),cv::Point(x+dx,y+dy),Scalar::all(255),1,CV_AA);
            }

        }
    }


    imshow("original", image);
    imshow("X", mat2gray(X));
    imshow("y", mat2gray(Y));
    imshow("magnitude", mat2gray(mag));
    imshow("orientation", mat2gray(orientation));
    imshow("orientation map", oriMap);
    imshow("orientation lines", angRes);

    waitKey(0);
}

float Gradient::GetWeightedAngle(Mat& mag, Mat& ang)
{
    float res = 0;
    float n = 0;
    for (int i = 0;i< mag.rows;++i){
        for (int j = 0;j< mag.cols;++j){
            res += ang.at<float>(i,j)*mag.at<float>(i,j);
            n += mag.at<float>(i,j);
        }
    }
    if(n != 0)
        res/=n;
    else
        res = -1;
    return res;
}

Mat Gradient::mat2gray(const cv::Mat& src){
    Mat dst;
    normalize(src, dst, 0.0, 255.0, cv::NORM_MINMAX, CV_8U);
    return dst;
}

Mat Gradient::orientationMap(const cv::Mat& mag, const cv::Mat& ori, double thresh){
    Mat oriMap = Mat::zeros(ori.size(), CV_8UC3);
    Vec3b red(0, 0, 255);
    Vec3b cyan(255, 255, 0);
    Vec3b green(0, 255, 0);
    Vec3b yellow(0, 255, 255);
    for(int i = 0; i < mag.rows*mag.cols; i++)
    {
        float* magPixel = reinterpret_cast<float*>(mag.data + i*sizeof(float));
        if(*magPixel > thresh)
        {
            float* oriPixel = reinterpret_cast<float*>(ori.data + i*sizeof(float));
            Vec3b* mapPixel = reinterpret_cast<Vec3b*>(oriMap.data + i*3*sizeof(char));
            if(*oriPixel < 90.0)
                *mapPixel = red;
            else if(*oriPixel >= 90.0 && *oriPixel < 180.0)
                *mapPixel = cyan;
            else if(*oriPixel >= 180.0 && *oriPixel < 270.0)
                *mapPixel = green;
            else if(*oriPixel >= 270.0 && *oriPixel < 360.0)
                *mapPixel = yellow;
        }
    }

    return oriMap;
}

Mat Gradient::horizontalTopGradient(int size, type t) {
    // size must be even
    assert(size%2 == 1);

    Mat_<int> mat;
    Mat_<int> matRes;
    switch(t) {
        case PREWITT : {
            for(int i = 0;i < size / 2;i++) {
                for(int j = 0;j < size;j++) {
                    mat.push_back(-1);
                }
            }
            for(int j = 0;j < size;j++) {
                mat.push_back(0);
            }
            for(int i = size / 2 + 1;i < size;i++) {
                for(int j = 0;j < size;j++) {
                    mat.push_back(1);
                }
            }
            matRes = mat.reshape(size, size);
            return matRes;
        }
        case SOBEL : {
            for(int i = 0;i < size / 2;i++) {
                for(int j = 0;j < size / 2;j++) {
                    mat.push_back(-1);
                }
                mat.push_back(-2);
                for(int j = size / 2 + 1;j < size;j++) {
                    mat.push_back(-1);
                }
            }
            for(int j = 0;j < size;j++) {
                mat.push_back(0);
            }
            for(int i = size / 2 + 1;i < size;i++) {
                for(int j = 0;j < size / 2;j++) {
                    mat.push_back(1);
                }
                mat.push_back(2);
                for(int j = size / 2 + 1;j < size;j++) {
                    mat.push_back(1);
                }
            }
            matRes = mat.reshape(size, size);
            return matRes;
        }
        case KIRSCH : {
            for(int i = 0;i < size / 2 + 1;i++) {
                for(int j = 0;j < size;j++) {
                    mat.push_back(-3);
                }
            }
            for(int i = size / 2 + 1;i < size;i++) {
                for(int j = 0;j < size;j++) {
                    mat.push_back(5);
                }
            }
            matRes = mat.reshape(size, size);
            matRes.at<int>(size/2, size/2) = 0;
            return matRes;
        }
        default : return Mat::zeros(size, size, CV_32F);
    }
}

Mat Gradient::verticalLeftGradient(int size, type t) {
    Mat mat = horizontalTopGradient(size, t);
    mat = mat.t();
    return mat;
}


Mat Gradient::horizontalBottomGradient(int size, type t) {
    // size must be even
    assert(size%2 == 1);

    if(t == KIRSCH) {
        Mat mat = Mat::zeros(size, size, CV_32S);
        for(int i = 0;i < size / 2;i++) {
            for(int j = 0;j < size;j++) {
                mat.at<int>(i,j) = 5;
            }
        }
        for(int i = size / 2;i < size;i++) {
            for(int j = 0;j < size;j++) {
                mat.at<int>(i,j) = -3;
            }
        }
        mat.at<int>(size/2, size/2) = 0;
        return mat;
    }else {
        return -horizontalTopGradient(size, t);
    }
}

Mat Gradient::verticalRightGradient(int size, type t) {
    // size must be even
    assert(size%2 == 1);

    if(t == KIRSCH) {
        Mat mat = Mat::zeros(size, size, CV_32S);
        for(int i = 0;i < size;i++) {
            for (int j = 0; j < size / 2; j++) {
                mat.at<int>(i, j) = 5;
            }
            for (int j = size / 2; j < size; j++) {
                mat.at<int>(i, j) = -3;
            }
            mat.at<int>(size / 2, size / 2) = 0;
        }
        return mat;
    }else {
        return -verticalLeftGradient(size, t);
    }
}

Mat Gradient::diagonalTopLeftGradient(int size, type t) {
    // size must be even
    assert(size%2 == 1);

    Mat mat = Mat::zeros(size, size, CV_32S);
    switch(t) {
        case PREWITT : {
            for(int i = 0;i < size;i++) {
                for(int j = 0;j < size - 1 - i;j++) {
                    mat.at<int>(i,j) = -1;
                }

                for(int j = size - i;j < size;j++) {
                    mat.at<int>(i,j) = 1;
                }
            }
            return mat;
        }
        case SOBEL : {
            for(int i = 0;i < size;i++) {
                for(int j = 0;j < size - 1 - i;j++) {
                    mat.at<int>(i,j) = -1;
                }

                for(int j = size - i;j < size;j++) {
                    mat.at<int>(i,j) = 1;
                }
            }
            for(int i = 0;i < size/2;i++) {
                mat.at<int>(i, i) = -2;
            }
            for(int i = size/2 + 1;i < size;i++) {
                mat.at<int>(i, i) = 2;
            }
            return mat;
        }
        case KIRSCH : {
            for(int i = 0;i < size;i++) {
                for(int j = 0;j < size - i;j++) {
                    mat.at<int>(i,j) =  -3;
                }
                for(int j = size - i;j < size;j++) {
                    mat.at<int>(i,j) = 5;
                }
                mat.at<int>(size/2, size/2) = 0;
            }
            return mat;
        }
        default : return Mat::zeros(size, size, CV_32FC3);
    }
}

Mat Gradient::diagonalTopRightGradient(int size, type t) {
    // size must be even
    assert(size%2 == 1);

    Mat mat = Mat::zeros(size, size, CV_32S);
    switch(t) {
        case PREWITT : {
            for(int i = 0;i < size;i++) {
                for(int j = i + 1;j < size;j++) {
                    mat.at<int>(i,j) = -1;
                }
                for(int j = 0;j < i;j++) {
                    mat.at<int>(i,j) = 1;
                }
            }
            return mat;
        }
        case SOBEL : {
            for(int i = 0;i < size;i++) {
                for(int j = i + 1;j < size;j++) {
                    mat.at<int>(i,j) = -1;
                }
                for(int j = 0;j < i;j++) {
                    mat.at<int>(i,j) = 1;
                }
            }
            for(int i = 0;i < size/2;i++) {
                mat.at<int>(i, size - 1 - i) = -2;
            }
            for(int i = size/2 + 1;i < size;i++) {
                mat.at<int>(i, size - 1 - i) = 2;
            }
            return mat;
        }
        case KIRSCH : {
            for(int i = 0;i < size;i++) {
                for(int j = i;j < size;j++) {
                    mat.at<int>(i,j) =  -3;
                }
                for(int j = 0;j < i;j++) {
                    mat.at<int>(i,j) = 5;
                }
                mat.at<int>(size/2, size/2) = 0;
            }
            return mat;
        }
        default : return Mat::zeros(size, size, CV_32FC3);
    }
}


Mat Gradient::diagonalBottomLeftGradient(int size, type t) {
    // size must be even
    assert(size%2 == 1);

    if(t == KIRSCH) {
        Mat mat = Mat::zeros(size, size, CV_32S);
        for(int i = 0;i < size;i++) {
            for(int j = i-1;j < size;j++) {
                mat.at<int>(i,j) =  5;
            }
            for(int j = 0;j < i+1;j++) {
                mat.at<int>(i,j) = -3;
            }
            mat.at<int>(size/2, size/2) = 0;
        }
        return mat;
    }else{
        return -diagonalTopRightGradient(size, t);
    }
}

Mat Gradient::diagonalBottomRightGradient(int size, type t) {
    // size must be even
    assert(size%2 == 1);

    Mat mat = Mat::zeros(size, size, CV_32S);
    switch(t) {
        case PREWITT : {
            for(int i = 0;i < size;i++) {
                for(int j = 0;j < size - 1 - i;j++) {
                    mat.at<int>(i,j) = 1;
                }

                for(int j = size - i;j < size;j++) {
                    mat.at<int>(i,j) = -1;
                }
            }
            return mat;
        }
        case SOBEL : {
            for(int i = 0;i < size;i++) {
                for(int j = 0;j < size - 1 - i;j++) {
                    mat.at<int>(i,j) = 1;
                }

                for(int j = size - i;j < size;j++) {
                    mat.at<int>(i,j) = -1;
                }
            }
            for(int i = 0;i < size/2;i++) {
                mat.at<int>(i, i) = 2;
            }
            for(int i = size/2 + 1;i < size;i++) {
                mat.at<int>(i, i) = -2;
            }
            return mat;
        }
        case KIRSCH : {
            for(int i = 0;i < size;i++) {
                for(int j = 0;j < size - i;j++) {
                    mat.at<int>(i,j) =  5;
                }
                for(int j = size - i;j < size;j++) {
                    mat.at<int>(i,j) = -3;
                }
                mat.at<int>(size/2, size/2) = 0;
            }
            return mat;
        }
        default : return Mat::zeros(size, size, CV_32FC3);
    }
}

std::array<Mat, 8> Gradient::boussoleGradient(int size, type t) {
    // size must be even
    assert(size%2 == 1);

    std::array<Mat, 8> gradients;
    gradients[0] = horizontalTopGradient(size, t);
    gradients[1] = verticalLeftGradient(size, t);
    gradients[2] = diagonalTopRightGradient(size, t);
    gradients[3] = diagonalTopLeftGradient(size, t);
    gradients[4] = horizontalBottomGradient(size, t);
    gradients[5] = verticalRightGradient(size, t);
    gradients[6] = diagonalBottomRightGradient(size, t);
    gradients[7] = diagonalBottomLeftGradient(size, t);

    return gradients;
}

