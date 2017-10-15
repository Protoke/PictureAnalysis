//
// Created by ad on 15/10/17.
//

#include "contour.h"

Contour::Contour(const Mat& affine, Mat& orientation) {
    // copie de l'image affinee
    _mat = Mat::zeros(affine.rows, affine.cols, CV_8U);
    for(unsigned int i = 0;i < affine.rows;i++) {
        for(unsigned int j = 0;j < affine.cols;j++) {
            _mat.at<uchar>(i,j) = affine.at<uchar>(i,j); // copie
        }
    }
    // sauvegarde de l'orientation
    _orientation = &orientation;

    chaining();

    //free(_orientation);
}

void Contour::chaining() {

    for(unsigned int i = 0;i < _mat.rows;i++) {
        for(unsigned int j = 0;j < _mat.cols;j++) {
            // si un debut de chaine est trouve, on le suit
            if(_mat.at<uchar>(i,j) > 0 && isStart(i, j)) {
                //std::cout << "Debut de chaine : y = " << i << "  x = " << j << std::endl;
                std::list<Point2i> list;
                list = follow(i,j);
                /*std::cout << "Chaine : " << std::endl;
                for(Point2i p : list) {
                    std::cout << "[" << p.y << ", " << p.x << "]" << std::endl;
                }*/
            }
        }
    }

}

bool Contour::isStart(int y, int x) {
    if( !isDone(Point2i(x, y)) ) {
        // compteur de voisins
        unsigned int cpt = 0;
        // parcours des voisins
        for(unsigned int i = std::max(0, y-1);i <= min(_mat.rows-1, y+1);i++) {
            for(unsigned int j = std::max(0, x-1);j <= min(_mat.cols-1, x+1);j++) {
                // si ce n'est pas le point central
                // si le point n'a pas ete deja traite
                // s'il y a plus qu'un voisin contour, ce n'est pas un debut de chaine
                if( !(i==y && j==x) &&
                    //!isDone(Point2i(j, i)) &&
                    _mat.at<uchar>(i,j) > 0 ) {
                    cpt++;
                    if(cpt > 1)
                        return false;
                }
            }
        }
        // s'il y a moins de 2 voisins,alors c'est un debut de chaine
        return true;
    }
    return false;
}

std::list<Point2i> Contour::follow(int y, int x) {
    // init point courant
    int currentX = x;
    int currentY = y;
    // ajout debut du contour dans la liste
    std::list<Point2i> list;
    list.emplace_back(Point2i(x,y));


    // parcours des voisins tant qu'un point suivant du contour n'a pas ete trouve
    bool next = true;
    while(next) {
        next = false;
        unsigned int i = std::max(0, currentY-1);
        unsigned int j = std::max(0, currentX-1);
        while(!next && i <= min(_mat.rows-1, currentY+1)) {
            j = std::max(0, currentX-1);
            while(!next && j <= min(_mat.cols-1, currentX+1)) {

                // si ce n'est pas le point courant qui est examine
                // si le point n'a pas encore ete traite
                // si | ori(a) - ori(b) | < PI/2 alors b est le point suivant du contour
                if( !(i == currentY && j == currentX) &&
                    !isDone(Point2i(j,i)) &&
                    (abs( _orientation->at<float>(currentY,currentX) - _orientation->at<float>(i,j)) < M_PI/2) ) {

                    // on ajoute le point dans la liste
                    list.emplace_back( Point2i(j,i) );
                    // on ajoute le point courant au set de points traites
                    _done.emplace_back( Point2i(currentX, currentY) );
                    // on met a jour le point courant
                    currentX = j;
                    currentY = i;
                    next = true;

                }

                j++;
            }
            i++;
        }
    }
    // ajout du point courant dans la liste des points traites
    _done.emplace_back( Point2i(currentX, currentY) );

    return list;
}

bool Contour::isDone(Point2i point) {
    for(Point2i p : _done) {
        if(p.x == point.x && p.y == point.y)
            return true;
    }
    return false;
}