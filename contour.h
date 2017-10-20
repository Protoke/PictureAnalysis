//
// Created by ad on 15/10/17.
//

#ifndef PICTUREANALYSIS_CONTOUR_H
#define PICTUREANALYSIS_CONTOUR_H

#include <vector>
#include <opencv2/opencv.hpp>

using namespace cv;

struct ContourNode {
    Point2i p;
    ContourNode* src;
    std::set<ContourNode*> childrens;
    float weight;
    float heuristic;
} ;

class Contour {

public:

    Mat _mat;

    Contour(const Mat& affine, Mat& orientation, Mat& magnitude, Mat& thresh, unsigned int distance_max = 10);
    void chaining();
    bool isStart(int y, int x);
    std::list<Point2i> follow(int i, int j);
    bool isDone(Point2i point);
    Mat draw_chains();
    bool contains(std::vector<Point2i> extremes, Point2i point);
    void deleteContourNodes(ContourNode* n);
    void closeContours(std::vector<Point2i> extreme_chains, unsigned int distance_max);
    bool isContour(Point2i p);

private:

    std::vector< std::list<Point2i> > _chains;
    Mat* _orientation;
    Mat* _magnitude;
    Mat* _thresh; // image seuillee
    std::vector<Point2i> _done; // liste de points traites
    Mat _closedContours;

};


#endif //PICTUREANALYSIS_CONTOUR_H
