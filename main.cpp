#include <iostream>
#include <opencv2/opencv.hpp>

#include "filter.h"
#include "threshold.h"
#include "gradient.h"
#include "graphe.h"
#include <list>
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
        imshow("Affinée", affine);

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

    return EXIT_SUCCESS;
}