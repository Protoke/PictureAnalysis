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
    _gradients_max = Mat::zeros(_rows, _cols, CV_32F);

    // convert rgb to gray and type CV_8UC1
    Mat image;
    cvtColor(img, image, COLOR_RGB2GRAY);
    image.convertTo(image, CV_8UC1);

    // init des differents gradients de l'image
    std::array<Mat,8> boussole = boussoleGradient(size, t);

    // lecture des flags, filtres choisis
    Filter filter;
    if(flags & N) {
        filter = Filter( boussole[0] );
        _gradients.emplace_back(filter.apply(image));
        _directions.emplace_back(0.0);
    }
    if(flags & NE) {
        filter = Filter( boussole[1] );
        _gradients.emplace_back(filter.apply(image));
        _directions.emplace_back(315.0);
    }
    if(flags & E) {
        filter = Filter( boussole[2] );
        _gradients.emplace_back(filter.apply(image));
        _directions.emplace_back(270.0);
    }
    if(flags & SE) {
        filter = Filter( boussole[3] );
        _gradients.emplace_back(filter.apply(image));
        _directions.emplace_back(225.0);
    }
    if(flags & S) {
        filter = Filter( boussole[4] );
        _gradients.emplace_back(filter.apply(image));
        _directions.emplace_back(180.0);
    }
    if(flags & SW) {
        filter = Filter( boussole[5] );
        _gradients.emplace_back(filter.apply(image));
        _directions.emplace_back(135.0);
    }
    if(flags & W) {
        filter = Filter( boussole[6] );
        _gradients.emplace_back(filter.apply(image));
        _directions.emplace_back(180.0);
    }
    if(flags & NW) {
        filter = Filter( boussole[7] );
        _gradients.emplace_back(filter.apply(image));
        _directions.emplace_back(45.0);
    }

    magnitude();

    orientation();

    orientation_map();
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
    }else{
        // CAS MULTIDIRECTIONNEL OU UNIDIRECTIONNEL
        _magnitude = Mat::zeros(_rows, _cols, CV_32F);
        for(unsigned int i = 0;i < _rows;i++) {
            for(unsigned int j = 0;j < _cols;j++) {
                // gradient max = magnitude
                int id_gradient_max = 0;
                float max = abs(_gradients[0].at<float>(i,j));
                for(unsigned int k = 1;k < _gradients.size();k++) {
                    if(abs(_gradients[k].at<float>(i,j)) > max) {
                        max = abs(_gradients[k].at<float>(i,j));
                        id_gradient_max = k;
                    }

                }
                // on sauvegarde le gradient max et l'id du filtre auquel il correspond
                _magnitude.at<float>(i,j) = _gradients[id_gradient_max].at<float>(i,j);
                _gradients_max.at<int>(i,j) = id_gradient_max;
            }
        }
    }
}

void Gradient::orientation() {
    if(_gradients.size() == 2) {
        // CAS BIDIRECTIONNEL
        for(unsigned int i = 0;i < _rows;i++) {
            for(unsigned int j = 0;j < _cols;j++) {
                _orientation.at<float>(i,j) = fastAtan2(_gradients[1].at<float>(i,j), _gradients[0].at<float>(i,j)); // angle in degrees
            }
        }
    }else {
        // CAS MULTIDIRECTIONNEL OU UNIDIRECTIONNEL
        for(unsigned int i = 0;i < _rows;i++) {
            for(unsigned int j = 0;j < _cols;j++) {
                // on recupere la direction du gradient max trouve
                int id_gradient_max = _gradients_max.at<int>(i,j);
                _orientation.at<float>(i,j) = (float)_directions[id_gradient_max];

                if(_magnitude.at<float>(i,j) < 0){
                    _magnitude.at<float>(i,j) = -_magnitude.at<float>(i,j);
                    _orientation.at<float>(i,j) = 360.0-_orientation.at<float>(i,j);
                }
            }
        }
    }
}



void Gradient::orientation_map(){
    for (int i = 0; i < _orientation_map.rows; ++i) {
        for (int j = 0; j < _orientation_map.cols; ++j) {
            if(_magnitude.at<float>(i,j) != 0)
                _orientation_map.at<Vec3b>(i,j) = Vec3b(abs(cos(_orientation.at<float>(i,j)*M_PI/180.0) * 255.0),
                                                        0,
                                                        abs(sin(_orientation.at<float>(i,j)*M_PI/180.0) * 255.0));
        }
    }
}

void refineContour(const Mat& magnitude, const Mat& orientation, const Mat& contours, Mat& result, int size) {
    contours.copyTo(result);
    int range = (size-1.0)/2.0;

    Mat rotation(2, 2, CV_32F);
    for (int i = 0; i < result.rows; ++i) {
        for (int j = 0; j < result.cols; ++j) {
            // MAJ de la matrice de rotation pour l'orientation du point
            float theta = orientation.at<float>(i,j) * M_PI/180.0;
            rotation.at<float>(0,0) = cos(theta); rotation.at<float>(0,1) = -sin(theta);
            rotation.at<float>(1,0) = sin(theta); rotation.at<float>(1,1) = cos(theta);

            // Calcul du vecteur direction du gradient
            Vec2f dir;
            Mat m = rotation * Mat(Vec2f(1,0), false);
            m.copyTo(cv::Mat(dir, false));
            normalize(dir, dir, 1.0, 0.0, NORM_L1);

            // Calcul des points sur la droite, et vérification de leur valeur
            float posCur = 1, negCur = -1;
            Point2f pPosCur = Vec2f(i, j) + dir * posCur;
            Point2f pNegCur = Vec2f(i, j) + dir * negCur;

//            if(result.at<uchar>(i,j) != 0)
//                std::cout << orientation.at<float>(i,j) << " " << theta << " " << dir << " POINT (" << i << ";" << j << ") : " << magnitude.at<float>(i, j) << std::endl;

            while(result.at<uchar>(i,j) != 0 &&
                    (
                            (pPosCur.x <= i+range && pPosCur.y <= j+range && pPosCur.x >= i-range && pPosCur.y >= j-range) ||
                            (pNegCur.x <= i+range && pNegCur.y <= j+range && pNegCur.x >= i-range && pNegCur.y >= j-range))
                    )
            {
//                std::cout << pPosCur << " " << pNegCur << std::endl;

                if(pPosCur.x >= 0 && pPosCur.x < result.rows && pPosCur.y >= 0 && pPosCur.y < result.cols)
                    if(magnitude.at<float>(round(pPosCur.x), round(pPosCur.y)) > magnitude.at<float>(i, j)) {
                        result.at<uchar>(i, j) = 0;
//                        std::cout << "Set 0" << std::endl;
                    }
                if(pNegCur.x >= 0 && pNegCur.x < result.rows && pNegCur.y >= 0 && pNegCur.y < result.cols)
                    if(magnitude.at<float>(round(pNegCur.x), round(pNegCur.y)) >= magnitude.at<float>(i, j)){
                        result.at<uchar>(i, j) = 0;
//                        std::cout << "Set 0" << std::endl;
                    }

                posCur += 0.5; negCur -= 0.5;
                pPosCur = Vec2f(i, j) + dir * posCur;
                pNegCur = Vec2f(i, j) + dir * negCur;
            }
        }
    }
}

Mat Gradient::southGradient(int size, type t) {
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

Mat Gradient::eastGradient(int size, type t) {
    Mat mat = southGradient(size, t);
    mat = mat.t();
    return mat;
}


Mat Gradient::northGradient(int size, type t) {
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
        return -southGradient(size, t);
    }
}

Mat Gradient::westGradient(int size, type t) {
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
        return -eastGradient(size, t);
    }
}

Mat Gradient::southEastGradient(int size, type t) {
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

Mat Gradient::southWestGradient(int size, type t) {
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


Mat Gradient::northEastGradient(int size, type t) {
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
        return -southWestGradient(size, t);
    }
}

Mat Gradient::northWestGradient(int size, type t) {
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
    gradients[0] = northGradient(size, t);
    gradients[1] = northEastGradient(size, t);
    gradients[2] = eastGradient(size, t);
    gradients[3] = southEastGradient(size, t);
    gradients[4] = southGradient(size, t);
    gradients[5] = southWestGradient(size, t);
    gradients[6] = westGradient(size, t);
    gradients[7] = northWestGradient(size, t);

    return gradients;
}

