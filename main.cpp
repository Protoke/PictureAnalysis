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
                     unsigned int distanceMax,
                     bool showIntermediate){

    // calcul gradients, magnitude, orientation de l'image
    Gradient gradient(image, 3, filterType, gradientType);

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

    Contour contour(affine, gradient._orientation, gradient._magnitude, seuil, distanceMax);
    Mat chains = contour.draw_chains();
    imshow("Chaînes", chains);

    Mat closedContours = contour.getFinalContours();

    result = closedContours;
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

    cout << "Entrez la distance maximale pour refermer les contours : ";
    unsigned int distanceMax;
    cin >> distanceMax;

    cout << "Voulez-vous afficher les images intermédiaires? (0 = non, 1 = oui) : ";
    int showIntermediate;
    cin >> showIntermediate;

    imshow("Source", image);

    Mat result;
    computeContours(image, result, filterType, gradientFlags,
                    thresholdMax, thresholdMin, thresholdSize, refiningSize, distanceMax, showIntermediate);

    imshow("Resultat", result);
    waitKey(0);



    return EXIT_SUCCESS;
}