// **********************************************************
//   File: Point.h
//   Description: Definition of class Point
//
//   Author: Carlos Moreno
// **********************************************************

#ifndef __POINT_H__
#define __POINT_H__

class Segment;
class Triangle;
class Polygon;

class Point {
public:
    Point() : x(0), y(0) {};
    // Constructor
    explicit Point(double _x, double _y) :
            x(_x), y(_y) {
    }

    // Geometric and miscelaneous operations
    void draw(int radius = 0) const;
    // Platform dependent - implemented for Win32

    bool is_on(const Segment &) const;

    bool is_inside(const Triangle &) const;
    bool is_inside(const Polygon &) const;

    // Arithmetic operators
    Point operator+(const Point & p) const;
    Point operator-(const Point & p) const;
    Point operator*(double r) const;
    double operator*(const Point & p) const; // Scalar product
    Point operator/(double r) const;

    // Comparison operators
    bool operator==(const Point & p) const;
    bool operator!=(const Point & p) const;

    bool operator<(const Point & p) const;
    bool operator>(const Point & p) const;

    // get utility functions
    double get_x() const {
        return x;
    }

    double get_y() const {
        return y;
    }

//private: modified to public variable by C. Pham to make is easier.
    double x, y;
};

inline Point operator*(double x, const Point & p) {
    return p * x; // Invoke member-function
}

const Point origin(0, 0);

#endif
