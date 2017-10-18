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
    std::set<ContourNode*> childrens;
    float weight;
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
    std::vector<Point2i> searchFasterPath(ContourNode* graph, ContourNode* start, ContourNode* end);
    static int compareContourNode(ContourNode* n1, ContourNode* n2);
    // TODO fonction free pour tous les pointeurs

private:

    std::vector< std::list<Point2i> > _chains;
    Mat* _orientation;
    Mat* _magnitude;
    // liste de points traites
    std::vector<Point2i> _done;

};


#endif //PICTUREANALYSIS_CONTOUR_H
