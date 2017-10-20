#include <iostream>
#include <opencv2/opencv.hpp>

#include "threshold.h"
#include "gradient.h"
#include "contour.h"
#include <string.h>

using namespace cv;
using namespace std;

Mat mat2gray(const Mat& src){
    Mat dst;
    normalize(src, dst, 0.0, 255.0, NORM_MINMAX, CV_8U);
    return dst;
}

void computeContours(const Mat& image, Mat& result,
                     Gradient::type filterType, int gradientType,
                     float thresholdHigh, float thresoldLow, int thresholdSize,
                     int refiningSize,
                     bool showIntermediate){

    // calcul gradients, magnitude, orientation de l'image
    Gradient gradient(image, 3, filterType, gradientType);

    // affichage des gradients
//    for(unsigned int i = 0;i < gradient._gradients.size();i++) {
//        std::string title = "G" + std::to_string(i);
//        imshow(title, gradient._gradients[i]);
//    }

    // affichage magnitude et orientation
    if(showIntermediate) {
        imshow("Magnitude", mat2gray(gradient._magnitude));
        imshow("Orientation", mat2gray(gradient._orientation));
//        imshow("Orientation Map", gradient._orientation_map);
    }

    Mat seuil = hysteresisThreshold(gradient._magnitude, thresholdHigh, thresoldLow, thresholdSize);
    if(showIntermediate)
        imshow("Seuillée", seuil);

    Mat affine;
    refineContour(gradient._magnitude, gradient._orientation, seuil, affine, refiningSize);
    if(showIntermediate)
        imshow("Affinage", affine);

//    Contour contour(affine, gradient._orientation, gradient._magnitude);
//
//
//    Size size(affine.cols * 1.5, affine.rows * 1.5);
//    Mat scale0;
//    resize(affine, scale0, size);
//
//    Mat chains = contour.draw_chains();
//
//    size = Size(chains.cols * 1.5, chains.rows * 1.5);
//    Mat scale1;
//    resize(chains, scale1, size);
//
//    imshow("Affine scale", scale0);
//    imshow("Chains scale", scale1);
//        imshow("Affinée", affine);

    result = affine;
}

int main() {

    cout << "Entrez le chemin de l'image : " << endl;
    string path;
    getline(cin, path);
    Mat image;
    image = imread(path, CV_LOAD_IMAGE_COLOR);
    if(! image.data) {
        std::cout << "Erreur de chargement d'image" << std::endl;
        return -1;
    }
    imshow("Source", image);

    cout << "Entrez le type du filtre : " << endl;
    cout << "(1 : Prewitt, 2 : SOBEL, 3 : KIRSCH)" << endl;
    int in;
    Gradient::type filterType;
    cin >> in;
    switch(in){
        case 1:
            filterType = Gradient::PREWITT;
            break;
        case 2:
            filterType = Gradient::SOBEL;
            break;
        case 3:
            filterType = Gradient::KIRSCH;
            break;
        default:
            cout << "Valeur incorrecte" << endl;
            return -1;
    }

    cout << "Entrez le type de gradient : " << endl;
    cout << "(1 : Bidirectionnel, 2 : Multidirectionnel)" << endl;
    int gradientFlags;
    cin >> in;
    switch(in){
        case 1:
            gradientFlags = Gradient::N | Gradient::E;
            break;
        case 2:
            gradientFlags = Gradient::NW | Gradient::N | Gradient::NE | Gradient::E;
            break;
        default:
            cout << "Valeur incorrecte" << endl;
            return -1;
    }

    cout << "Entrez la valeur haute de seuillage : ";
    int thresholdMax;
    cin >> thresholdMax;

    cout << "Entrez la valeur basse du seuillage : ";
    int thresholdMin;
    cin >> thresholdMin;

    cout << "Entrez la largeur du filtre de seuillage : ";
    int thresholdSize;
    cin >> thresholdSize;

    cout << "Entrez la largeur du filtre d'affinage : ";
    int refiningSize;
    cin >> refiningSize;

    cout << "Voulez-vous afficher les images intermédiaires? (0 = non, 1 = oui) : ";
    int showIntermediate;
    cin >> showIntermediate;

    Mat result;
    computeContours(image, result, filterType, gradientFlags,
                    thresholdMax, thresholdMin, thresholdSize, refiningSize, showIntermediate);


    /*// load the original picture in cv mat
    Mat image = imread("../data/Lenna.png", CV_LOAD_IMAGE_COLOR);
//    image = imread("../data/image_simple_test.png", CV_LOAD_IMAGE_COLOR);
    if(! image.data) {
        std::cout << "Error loading picture" << std::endl;
        return -1;
    }
    imshow("Source", image);

    Mat result;
    computeContours(image, result, Gradient::PREWITT, Gradient::N | Gradient::E, 50, 25, 3, 3, 1);*/

    imshow("Resultat", result);
    waitKey(0);


    // ANAIS
    bool testContour = true;
    if(testContour) {
        uchar img[] =  {  0,     0,   0,  0,  0,
                          0,   255, 255,  0, 255,
                          255,   0,   0,  0,  0 };
        float ori[] =  {0.0,   0.0,  0.0,  0.0,  0.0,
                        0.0,   0.0,  0.0,  0.0,  0.0,
                        0.0,   0.0,  0.0,  0.0,  0.0 };
        float mag[] = {  0.0,   0.0,    10.0,  0.0,  0.0,
                         10.0,  10.0,   10.0,  10.0,  10.0,
                         10.0,   0.0,    10.0,  0.0,  0.0 };
        Mat affine(3, 5, CV_8U, img);
        Mat orientation(3, 5, CV_32F, ori);
        Mat magnitude(3, 5, CV_32F, mag);
        Contour contour(affine, orientation, magnitude);
        Mat chains = contour.draw_chains();

        Mat scale(chains.rows * 30, chains.cols * 30, CV_8UC3);
        for(unsigned int i = 0;i < scale.rows;i++) {
            for(unsigned int j = 0;j < scale.cols;j++) {
                scale.at<Vec3b>(i,j) = chains.at<Vec3b>(i/30, j/30);
            }
        }

        imshow("Affine", affine);
        imshow("orientation", orientation);
        imshow("Magnitude", magnitude);
        imshow("Chains", scale);
        waitKey(0);

    }else {
        // load the original picture in cv mat
        Mat image;
//    image = imread("../data/Lenna.png", CV_LOAD_IMAGE_COLOR);
        image = imread("../data/image_simple_test.png", CV_LOAD_IMAGE_COLOR);
        if (!image.data) {
            std::cout << "Error loading picture" << std::endl;
            return -1;
        }
        imshow("Source", image);

        computeContours(image, 3, 50, 25, 3, 3, true);
//    computeContours(image, 3, 100, 75, 3, 3, true);


        waitKey(0);
    }


    return EXIT_SUCCESS;
}