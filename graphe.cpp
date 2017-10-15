//
// Created by ad on 15/10/17.
//

#include "graphe.h"

Graphe::Graphe(Mat orientation, Mat img) {
    _orientation = &orientation;
    _img = &img;
    bool contour_found = false;
    unsigned int i = 0;
    unsigned int j = 0;
    while(!contour_found && i < img.rows) {
        while(!contour_found && j < img.cols) {
            if(img.at<uchar>(i,j) > 0) {
                contour_found = true;
                _start.y = i;
                _start.x = j;
            }
            j++;
        }
        i++;
    }

    buildGraph(_start);

    free(_orientation);
    free(_img);
    remove(&_start);
}

void Graphe::buildGraph(Node& n) {
    if(n.y > 0 && n.y < _img->rows-1 && n.x > 0 && n.x < _img->cols-1) {

        // parcours des voisins directs de n
        for(unsigned int i = n.y - 1;i <= n.y + 1;i++) {
            for(unsigned int j = n.x - 1;j <= n.x + 1;j++) {
                // si c'est un voisin et non n
                if(!(i==1 && j==1)) {
                    // si l'orientation est a +/- 45 degres egale a l'orientation de n
                    if( abs( _orientation->at<float>(i,j) - _orientation->at<float>(n.y,n.x) ) < (float)(M_PI/2.0) ) {
                        // alors c'est un fils de n
                        Node child;
                        child.y = i;
                        child.x = j;
                        // on construit le sous graphe du fils
                        buildGraph(child);
                        // on l'ajoute aux enfant du noeud courant
                        n.childrens.emplace_back(&child);
                    }
                }
            }
        }

    }
}

void Graphe::display(Node* n) {
    std::cout << n->y << "  " << n->x << std::endl;
    for(Node* child : n->childrens) {
        display(child);
    }
}

void Graphe::remove(Node* n) {
    for(Node* child : n->childrens) {
        remove(child);
    }
    free(n);
}