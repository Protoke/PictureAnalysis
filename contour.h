//
// Created by ad on 15/10/17.
//

#ifndef PICTUREANALYSIS_CONTOUR_H
#define PICTUREANALYSIS_CONTOUR_H

#include "graphe.h"

class Contour {

public:
    Mat _mat;


    Contour(const Mat& affine, Mat& orientation);
    void chaining();
    bool isStart(int y, int x);
    std::list<Point2i> follow(int i, int j);
    bool isDone(Point2i point);

private:
    std::vector< std::list<Point2i> > _chains;
    Mat* _orientation;
    // liste de points traites
    std::vector<Point2i> _done;

};


#endif //PICTUREANALYSIS_CONTOUR_H
