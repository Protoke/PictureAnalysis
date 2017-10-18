//
// Created by ad on 15/10/17.
//

#include "contour.h"
#include <functional>
#include <vector>

Contour::Contour(const Mat& affine, Mat& orientation, Mat& magnitude) {
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

    // on construit les graphes de possibilites pour relier chaque extremite de chaine
    std::vector<ContourNode*> graphs = std::vector<ContourNode*>(0);
    for(unsigned int i = 0;i < extreme_chains.size();i++) {
        for(unsigned int j = i+1;j < extreme_chains.size();j++) {
            ContourNode* node = buildGraph(extreme_chains[i], extreme_chains[j]);
            graphs.push_back( node );
            //std::cout << extreme_chains[i].y << " " << extreme_chains[i].x << "  LINK  " << extreme_chains[j].y << " " << extreme_chains[j].x << std::endl;
        }
    }
    unsigned int i = 0;
    for(ContourNode* n : graphs) {
        std::cout << "Noeud " << i << std::endl;
        std::cout << n->p << std::endl;
        for(ContourNode* c : n->childrens) {
            std::cout << "children " << std::endl;
            std::cout << c->p << std::endl;
        }
        i++;
    }

    for(ContourNode* n : graphs) {
            free(n);
    }

    /*Point2i p0 = _chains[0].back();
    Point2i p1 = _chains[1].front();
    ContourNode n = buildGraph(p0,p1);
    std::cout << "p0 " << p0.y << " " << p0.x << std::endl;
    std::cout << "p1 " << p1.y << " " << p1.x << std::endl;
    std::cout << (n.childrens[0])->p.y << " " << (n.childrens[0])->p.x << std::endl;*/

}

void Contour::chaining() {

    for(unsigned int i = 0;i < _mat.rows;i++) {
        for(unsigned int j = 0;j < _mat.cols;j++) {
            // si un debut de chaine est trouve, on le suit
            if(_mat.at<uchar>(i,j) > 0 && isStart(i, j)) {
                //std::cout << "Debut de chaine : y = " << i << "  x = " << j << std::endl;
                std::list<Point2i> list;
                list = follow(i,j);
                _chains.emplace_back(list);
                /*for(Point2i p : _done) {
                    std::cout << "[" << p.y << ", " << p.x << "]" << std::endl;
                }*/
                /*std::cout << "Chaine : " << std::endl;
                for(Point2i p : list) {
                    std::cout << "[" << p.y << ", " << p.x << "]" << std::endl;
                }*/
            }
        }
    }

}

ContourNode* Contour::buildGraph(Point2i start, Point2i end) {
    std::set<ContourNode*> nodesDone = std::set<ContourNode*>();
    std::set<ContourNode*> nodesToDo = std::set<ContourNode*>();

    ContourNode* nStart = new ContourNode();
    nStart->p = start;
    nStart->weight = 0.0;
    ContourNode* currentNode;

    bool end_found = false;
    float distance_start_current = 0.0;
    float distance_start_end = sqrt( (end.x - start.x)*(end.x - start.x) + (end.y - start.y)*(end.y - start.y) );
    float coef_distance = 2.0;

    nodesToDo.insert(nStart);

    while(!nodesToDo.empty() && distance_start_current <= coef_distance*distance_start_end) {
        // on met a jour le noeud courant en recuperant un noeud dans la pile de noeuds a traiter et on le supprime des noeuds a traiter
        currentNode = *nodesToDo.begin();
        nodesToDo.erase(currentNode);

        distance_start_current = sqrt( (currentNode->p.x - start.x)*(currentNode->p.x - start.x) + (currentNode->p.y - start.y)*(currentNode->p.y - start.y) );

        currentNode->childrens = std::set<ContourNode*>();

        // on parcourt les voisins, pour voir s'il y a le point final dans le voisinage
        for(unsigned int i = std::max(0, currentNode->p.y-1);i <= min(_mat.rows-1, currentNode->p.y+1);i++) {
            for (unsigned int j = std::max(0, currentNode->p.x - 1);
                 j <= min(_mat.cols - 1, currentNode->p.x + 1); j++) {
                // si ce n'est pas le point central
                // si | ori(a) - ori(b) | < PI/2 alors b est un point suivant potentiel du contour
                if (!(i == currentNode->p.y && j == currentNode->p.x) &&
                    Point2i(j,i) == end) {
                    // si c'est le point final, le point courant ne doit pas avoir d'autres fils que lui
                    ContourNode* n = new ContourNode();
                    n->p = Point2i(j, i);
                    n->weight = currentNode->weight + abs(_magnitude->at<float>(i, j));
                    currentNode->childrens.insert(n);
                    end_found = true;
                }
            }
        }

        if(!end_found) {
            // on parcourt les voisins
            for(unsigned int i = std::max(0, currentNode->p.y-1);i <= min(_mat.rows-1, currentNode->p.y+1);i++) {
                for(unsigned int j = std::max(0, currentNode->p.x-1);j <= min(_mat.cols-1, currentNode->p.x+1);j++) {
                    // si ce n'est pas le point central
                    // si la magnitude (norme du gradient) en ce point n'est pas nulle
                    // si | ori(a) - ori(b) | < PI/2 alors b est un point suivant potentiel du contour
                    if( !(i==currentNode->p.y && j==currentNode->p.x) &&
                        _magnitude->at<float>(i,j) != 0 ) {// &&
                        //(abs( fmod(_orientation->at<float>(currentNode->p.y,currentNode->p.x - _orientation->at<float>(i,j) ), 2.0*M_PI) )  < M_PI/2)) {
                        ContourNode* n = new ContourNode();
                        n->p = Point2i(j,i);
                        n->weight = currentNode->weight + abs(_magnitude->at<float>(i,j));
                        currentNode->childrens.insert(n);

                        nodesToDo.insert(n);
                    }
                }
            }
        }

        // on ajoute le noeud courant aux noeuds traites
        nodesDone.insert(currentNode);

    }


    return nStart;
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
                    //!isDone(Point2i(j,i)) &&
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
                /*float angleMax = std::max(_orientation->at<float>(currentY,currentX), _orientation->at<float>(i,j));
                float angleMin = std::min(_orientation->at<float>(currentY,currentX), _orientation->at<float>(i,j));
                float angle = angleMax - angleMin;
                angle = abs(fmod(angle + 180.0, 360.0) - 180.0);*/
                if( !(i == currentY && j == currentX) &&
                    _mat.at<uchar>(i,j) > 0 &&
                    !isDone(Point2i(j,i)) ) {//   &&
                    //angle <= M_PI/2.0 ) {
                    //( fmod(_orientation->at<float>(currentY,currentX) - _orientation->at<float>(i,j) , 2.0*M_PI) )  <= M_PI/2.0 )  {

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
    //Mat mat = Mat::zeros(_mat.rows, _mat.cols, CV_8U);
    unsigned int i = 0;
    for(std::list<Point2i> l : _chains) {
        unsigned int colorR = ((sin(i) + 1.0) / 2.0)*255;
        unsigned int colorG = ((cos(i) + 1.0) / 2.0)*255;
        unsigned int colorB = ((tan(i) + 1.0) / 2.0)*255;
        for(Point2i p : l) {
            mat.at<Vec3b>(p.y, p.x) = Vec3b(colorR, colorG, colorB);
        }
        //Point2i p = l.front();
        //mat.at<Vec3b>(p.y, p.x) = Vec3b(colorR, colorG, colorB);
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

std::vector<Point2i> Contour::searchFasterPath(ContourNode* graph, ContourNode* start, ContourNode* end) {
    std::queue<ContourNode*> closedList;
    std::priority_queue< ContourNode*, std::vector<ContourNode*>, std::function<int (ContourNode*, ContourNode*)> > openList (compareContourNode);
    openList.push(start);
    while(!openList.empty()) {
        ContourNode* n = openList.top();
        openList.pop();
        // TODO
    }

    return std::vector<Point2i>();

}

int Contour::compareContourNode(ContourNode* n1, ContourNode* n2) {
    if(n1->weight > n2->weight)
        return 1;
    if(n1->weight == n2->weight)
        return 0;
    return -1;
}

bool equal(ContourNode* n1, ContourNode* n2) {
    if(n1->p.x == n2->p.x && n1->p.y == n2->p.y)
        return true;
    return false;
}