/*
 * Node.h
 *
 *  Created on: Mar 15, 2012
 *      Author: bm
 */

#ifndef NODE_H
#define NODE_H

#include <iostream>

#include "Point.h"
#include "Triangle.h"

/*
 *
 */
struct Node {
    unsigned int addr;
    Point p;
    Point v;
    Point b;
    Point c;
    double alpha;
    Triangle fov;
    Node() {};
    Node(const Triangle& fov) : fov(fov) {};
};

std::ostream& operator<<(std::ostream& os, const Node& n);

#endif /* NODE_H_ */
