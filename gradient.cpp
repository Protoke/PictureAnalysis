//
// Created by ad on 10/10/17.
//

#include "gradient.h"
#include "filter.h"

#include <assert.h>

Gradient::Gradient(const Mat& img, int size, type t, int flags) {
    _rows = img.rows;
    _cols = img.cols;
    _magnitude = Mat::zeros(_rows, _cols, CV_32F);
    _orientation = Mat::zeros(_rows, _cols, CV_32F);
    _orientation_map = Mat::zeros(_orientation.size(), CV_8UC3);
    _orientation_lines = Mat::zeros(_rows,_cols,CV_8UC1);

    // convert rgb to gray and type CV_8UC1
    Mat image;
    cvtColor(img, image, COLOR_RGB2GRAY);
    image.convertTo(image, CV_8UC1);

    // init des differents gradients de l'image
    std::array<Mat, 8> gradients = boussoleGradient(size, t);

    Filter filter;
    if(flags & N) {
        filter = Filter( gradients[4] );
        _gradients.emplace_back(filter.apply(image));
    }
    if(flags & NE) {
        filter = Filter( gradients[7] );
        _gradients.emplace_back(filter.apply(image));
    }
    if(flags & E) {
        filter = Filter( gradients[1] );
        _gradients.emplace_back(filter.apply(image));
    }
    if(flags & SE) {
        filter = Filter( gradients[3] );
        _gradients.emplace_back(filter.apply(image));
    }
    if(flags & S) {
        filter = Filter( gradients[0] );
        _gradients.emplace_back(filter.apply(image));
    }
    if(flags & SW) {
        filter = Filter( gradients[2] );
        _gradients.emplace_back(filter.apply(image));
    }
    if(flags & W) {
        filter = Filter( gradients[5] );
        _gradients.emplace_back(filter.apply(image));
    }
    if(flags & NW) {
        filter = Filter( gradients[6] );
        _gradients.emplace_back(filter.apply(image));
    }

    magnitude();

    orientation();

    orientation_map();

    orientation_lines();
}

void Gradient::magnitude() {
    if(_gradients.size() == 2) {
        // CAS BIDIRECTIONNEL
        // calculate magnitude : sqrt(X^2 + Y^2 + ...)
        Mat m0_2, m1_2;
        multiply(_gradients[0], _gradients[0], m0_2);
        multiply(_gradients[1], _gradients[1], m1_2);
        add(m0_2, m1_2, _magnitude);
        sqrt(_magnitude, _magnitude);
    }else if(_gradients.size() > 2){
        // CAS MULTIDIRECTIONNEL
        _magnitude = Mat::zeros(_rows, _cols, CV_32F);
        for(unsigned int i = 0;i < _rows;i++) {
            for(unsigned int j = 0;j < _cols;j++) {
                // gradient max = magnitude
                unsigned int id_gradient_max = 0;
                float max = _gradients[0].at<float>(i,j);
                for(unsigned int k = 1;k < _gradients.size();k++) {
                    if(_gradients[k].at<float>(i,j) > max) {
                        max = _gradients[k].at<float>(i,j);
                        id_gradient_max = k;
                    }

                }
                _magnitude.at<float>(i,j) = _gradients[id_gradient_max].at<float>(i,j);
            }
        }
    }
}

void Gradient::orientation() {
    if(_gradients.size() == 2) {
        // CAS BIDIRECTIONNEL
        _orientation = Mat::zeros(_rows, _cols, CV_32F);
        for(unsigned int i = 0;i < _rows;i++) {
            for(unsigned int j = 0;j < _cols;j++) {
                _orientation.at<float>(i,j) = fastAtan2(_gradients[1].at<float>(i,j), _gradients[0].at<float>(i,j)) * 180/M_PI; // angle in degrees
            }
        }
    }else if(_gradients.size() > 2) {
        // CAS MULTIDIRECTIONNEL
        _orientation = Mat::zeros(_rows, _cols, CV_32F);
        for(unsigned int i = 0;i < _rows;i++) {
            for(unsigned int j = 0;j < _cols;j++) {
                //_orientation.at<float>(i,j) = () * 180/M_PI; // angle in degrees
            }
        }
    }
    normalize(_orientation, _orientation, 0, 360, NORM_MINMAX);
}



void Gradient::orientation_map(double thresh){
    Vec3b red(0, 0, 255);
    Vec3b cyan(255, 255, 0);
    Vec3b green(0, 255, 0);
    Vec3b yellow(0, 255, 255);
    for(int i = 0; i < _rows*_cols; i++)
    {
        float* magPixel = reinterpret_cast<float*>(_magnitude.data + i*sizeof(float));
        if(*magPixel > thresh)
        {
            float* oriPixel = reinterpret_cast<float*>(_orientation.data + i*sizeof(float));
            Vec3b* mapPixel = reinterpret_cast<Vec3b*>(_orientation_map.data + i*3*sizeof(char));
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
}

void Gradient::orientation_lines(unsigned int thresh) {
    int blockSize = _cols/thresh-1;
    float r = blockSize;

    for(int i = 0;i < _rows-blockSize;i += blockSize) {
        for(int j = 0 ;j < _cols-blockSize;j += blockSize) {
            Mat m = _magnitude(Rect(j,i,blockSize,blockSize));
            Mat o = _orientation(Rect(j,i,blockSize,blockSize));
            float angle = getWeightedAngle(m,o);

            if(angle != -1) {
                float dx = r*cos(angle);
                float dy = r*sin(angle);
                int x = j;
                int y = i;

                cv::line(_orientation_lines,cv::Point(x,y),cv::Point(x+dx,y+dy),Scalar::all(255),1,CV_AA);
            }
        }
    }
}

float Gradient::getWeightedAngle(Mat& mag, Mat& ori)
{
    float res = 0;
    float n = 0;
    for (int i = 0;i < mag.rows;++i){
        for (int j = 0;j< mag.cols;++j){
            res += ori.at<float>(i,j)*mag.at<float>(i,j);
            n += mag.at<float>(i,j);
        }
    }
    if(n != 0)
        res/=n;
    else
        res = -1;
    return res;
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

