//
// Created by ad on 15/10/17.
//

#ifndef PICTUREANALYSIS_GRAPHE_H
#define PICTUREANALYSIS_GRAPHE_H


#include <vector>
#include "gradient.h"

class Graphe {

    struct Node {
        unsigned int x;
        unsigned int y;
        std::vector<Node*> childrens;
    };

private:
    Node _start;
    Mat* _img;
    Mat* _orientation;

public :

    // img : image binaire
    Graphe(Mat orientation, Mat img);

    void buildGraph(Node& n);

    void display(Node* n);

    void remove(Node* n);

};


#endif //PICTUREANALYSIS_GRAPHE_H
