//
// Created by ad on 15/10/17.
//

#include "contour.h"
#include <functional>
#include <vector>

Contour::Contour(const Mat& affine, Mat& orientation, Mat& magnitude, Mat& thresh, unsigned int distance_max) {

    // copie de l'image affinee
    _mat = Mat::zeros(affine.rows, affine.cols, CV_8U);
    for(unsigned int i = 0;i < affine.rows;i++) {
        for(unsigned int j = 0;j < affine.cols;j++) {
            _mat.at<uchar>(i,j) = affine.at<uchar>(i,j); // copie
        }
    }
    // sauvegarde de l'orientation et de la magnitude
    _orientation = &orientation;
    _magnitude = &magnitude;
    _thresh = &thresh;


    // 1ere passe

    // chainage des contours existants dans l'image affinee
    // stockage dans le vector de listes de points _chains
    chaining();

    // on regroupe toutes les extremites de chaines
    std::vector<Point2i> extreme_chains;
    for(std::list<Point2i> l : _chains) {
        extreme_chains.emplace_back(l.front());
        if( !contains(extreme_chains, l.back()) ) {
            extreme_chains.emplace_back(l.back());
        }
    }

    closeContours(extreme_chains, distance_max);

    //2eme passe
    _mat = _closedContours + affine;

    chaining();
    extreme_chains = std::vector<Point2i> ();
    for(std::list<Point2i> l : _chains) {
        extreme_chains.emplace_back(l.front());
        if( !contains(extreme_chains, l.back()) ) {
            extreme_chains.emplace_back(l.back());
        }
    }

    closeContours(extreme_chains, distance_max);

    _closedContours = _closedContours + _mat;

}

void Contour::chaining() {

    for(unsigned int i = 0;i < _mat.rows;i++) {
        for(unsigned int j = 0;j < _mat.cols;j++) {
            // si un debut de chaine est trouve, on le suit
            if(_mat.at<uchar>(i,j) > 0 && isStart(i, j)) {
                std::list<Point2i> list;
                list = follow(i,j);
                _chains.emplace_back(list);
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
                // si le point appartient au contour de l'image affinee
                // s'il y a plus qu'un voisin contour, ce n'est pas un debut de chaine
                if( !(i==y && j==x) &&
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
                // si le point appartient au contour de l'image affinee
                // si le point n'a pas encore ete traite
                if( !(i == currentY && j == currentX) &&
                    _mat.at<uchar>(i,j) > 0 &&
                    !isDone(Point2i(j,i)) ) {

                    // on ajoute le point dans la liste
                    list.emplace_back( Point2i(j,i) );
                    // on ajoute le point courant au vector de points traites
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

Mat Contour::draw_chains() {
    Mat mat = Mat::zeros(_mat.rows, _mat.cols, CV_8UC3);

    unsigned int i = 0;
    for(std::list<Point2i> l : _chains) {
        unsigned int colorR = ((sin(i) + 1.0) / 2.0)*255;
        unsigned int colorG = ((cos(i) + 1.0) / 2.0)*255;
        unsigned int colorB = ((tan(i) + 1.0) / 2.0)*255;
        for(Point2i p : l) {
            mat.at<Vec3b>(p.y, p.x) = Vec3b(colorR, colorG, colorB);
        }
        i++;
    }
    return mat;
}

bool Contour::contains(std::vector<Point2i> extremes, Point2i point) {
    for(Point2i& p : extremes) {
        if(p.x == point.x && p.y == point.y)
            return true;
    }
    return false;
}

void Contour::closeContours(std::vector<Point2i> extreme_chains, unsigned int distance_max) {

    _closedContours = Mat::zeros(_mat.rows, _mat.cols, CV_8U);

    // pour chaque extremite de contour
    for(Point2i point : extreme_chains) {
        std::vector<Point2i> visited = std::vector<Point2i>();

        unsigned int distance = 0;

        Point2i p = point;
        visited.push_back(p);

        // on regarde les voisins de l'extremite concernee
        // s'ils font parti des vrais contours, on les ajoute aux points visites
        for (unsigned int i = std::max(0, p.y - 1); i <= min(_mat.rows - 1, p.y + 1); i++) {
            for (unsigned int j = std::max(0, p.x - 1); j <= min(_mat.cols - 1, p.x + 1); j++) {
                if(isContour(Point2i(j,i)))
                    visited.push_back(Point2i(j,i));
            }
        }

        Mat tmp = Mat::zeros(_mat.rows, _mat.cols, CV_8U);

        // tant que la distance max n'a pas ete depassee et qu'on n'a pas atteint un vrai contour
        do {
            float mag_max = 0.0;
            Point2i p_max = Point2i();
            // on parcourt les voisins a la recherche de la magnitude max
            for (unsigned int i = std::max(0, p.y - 1); i <= min(_mat.rows - 1, p.y + 1); i++) {
                for (unsigned int j = std::max(0, p.x - 1); j <= min(_mat.cols - 1, p.x + 1); j++) {
                    // si ce n'est pas un point deja visite
                    if (!contains(visited, Point2i(j,i))) {
                        // on met a jour le point ayant la magnitude max parmi les voisins
                        if(abs(_magnitude->at<float>(i,j)) > mag_max) {
                            mag_max = abs(_magnitude->at<float>(i,j));
                            p_max = Point2i(j,i);
                        }

                    }
                    // on ajoute le voisin comme visite
                    visited.push_back(Point2i(j,i));
                }
            }

            if(_thresh->at<uchar>(p_max.y, p_max.x) == 0) // s'il n'existe pas de contour dans l'image seuillee a cet endroit
                distance++; // on incremente la distance
            // on met a jour le point courant avec le voisin qui avait la valeur de magnitude max
            p = p_max;
            tmp.at<uchar>(p.y, p.x) = 255;
        }while(distance < distance_max && !isContour(p));

        // si on a atteint un vrai contour, ce contour cree est valide
        if(isContour(p)) {
            _closedContours = _closedContours + tmp;
        }

    }
}

bool Contour::isContour(Point2i p) {
    if(_mat.at<uchar>(p.y, p.x) != 0)
        return true;
    return false;
}

Mat Contour::getFinalContours() {
    return _closedContours;
}