//
// Created by ad on 15/10/17.
//

#ifndef PICTUREANALYSIS_CONTOUR_H
#define PICTUREANALYSIS_CONTOUR_H

#include <vector>
#include <opencv2/opencv.hpp>
#include <set>
#include <list>
#include <queue>

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

    Contour(const Mat& affine, Mat& orientation, Mat& magnitude);
    void chaining();
    ContourNode* buildGraph(Point2i start, Point2i end);
    bool isStart(int y, int x);
    std::list<Point2i> follow(int i, int j);
    bool isDone(Point2i point);
    Mat draw_chains();
    bool contains(std::vector<Point2i> extremes, Point2i point);
    std::list<Point2i> searchFasterPath(ContourNode* start, Point2i end);
    static int compareContourNode(ContourNode* n1, ContourNode* n2);
    bool equal(ContourNode* n1, ContourNode* n2);
    float distance(Point2i p1, Point2i p2);
    void deleteContourNodes(ContourNode* n);
    // TODO fonction free pour tous les pointeurs

private:

    std::vector< std::list<Point2i> > _chains;
    Mat* _orientation;
    Mat* _magnitude;
    // liste de points traites
    std::vector<Point2i> _done;

};


#endif //PICTUREANALYSIS_CONTOUR_H
