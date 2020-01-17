// **********************************************************
//   File: Polygon.h
//   Description: Definition of class Polygon and classes 
//                Polygon::iterator and const_iterator 
//                (both implemented as inlines)
//
//   Author: Carlos Moreno
// **********************************************************

#ifndef __POLYGON_H__
#define __POLYGON_H__

#include "Point.h"
#include <list>



const bool draw_vertices = true;
const bool no_draw_vertices = false;

class Polygon {
    public:
        // Geometric and miscelaneous operations
        void draw(bool = no_draw_vertices, int radius = 2) const;

        int orientation() const;
        bool contains(const Point &) const;

        Polygon convex_hull() const;

        // Vertices manipulation
        class iterator;
        class const_iterator;

        iterator begin();
        const_iterator begin() const;
        void push_back(const Point &);
        void push_front(const Point &);
        void insert(const iterator &, const Point &);
        iterator remove(const iterator &);

        std::list<Point> vertices;
    private:

// ********************************************************
//          Definition of class Polygon::iterator
// ********************************************************

    public:
        class iterator {
            public:
                iterator(const std::list<Point>::iterator & _i = std::list<Point>::iterator(),
                        const std::list<Point>::iterator & _last = std::list<Point>::iterator()) :
                        i(_i), last(_last) {
                }

                std::list<Point>::iterator get_i() const {
                    return i;
                }

                std::list<Point>::iterator get_last() const {
                    return last;
                }

                const iterator & operator++() {
                    if (++i == last) {
                        i++;
                    }
                    return *this;
                }

                const iterator operator++(int) {
                    iterator original = *this;

                    if (++i == last) {
                        i++;
                    }
                    return original;
                }

                const iterator operator+(int n) const {
                    iterator result = *this;

                    for (int i = 0; i < n; i++) {
                        result++;
                    }

                    return result;
                }

                const iterator & operator--() {
                    if (--i == last) {
                        i--;
                    }
                    return *this;
                }

                const iterator operator--(int) {
                    iterator original = *this;

                    if (--i == last) {
                        i--;
                    }
                    return original;
                }

                const iterator operator-(int n) const {
                    iterator result = *this;

                    for (int i = 0; i < n; i++) {
                        result--;
                    }

                    return result;
                }

                Point & operator*() const {
                    return *i;
                }

                bool operator==(const iterator & other) const {
                    return i == other.i && last == other.last;
                }

                bool operator!=(const iterator & other) const {
                    return !(*this == other);
                }

            private:
                std::list<Point>::iterator i;
                std::list<Point>::iterator last;
        };

// ********************************************************
//       Definition of class Polygon::const_iterator
// ********************************************************

        class const_iterator {
            public:
                const_iterator(const std::list<Point>::const_iterator & _i = std::list<Point>::const_iterator(),
                        const std::list<Point>::const_iterator & _last = std::list<Point>::const_iterator()) :
                        i(_i), last(_last) {
                }

                // conversion constructor (from iterator to const_iterator)
                const_iterator(const iterator & it) :
                        i(it.get_i()), last(it.get_last()) {
                }
                // Use conversion from list::iter to const_iter

                std::list<Point>::const_iterator get_i() const {
                    return i;
                }

                const const_iterator & operator++() {
                    if (++i == last) {
                        i++;
                    }
                    return *this;
                }

                const const_iterator operator++(int) {
                    const_iterator original = *this;

                    if (++i == last) {
                        i++;
                    }
                    return original;
                }

                const const_iterator operator+(int n) const {
                    const_iterator result = *this;

                    for (int i = 0; i < n; i++) {
                        result++;
                    }

                    return result;
                }

                const const_iterator & operator--() {
                    if (--i == last) {
                        i--;
                    }
                    return *this;
                }

                const const_iterator operator--(int) {
                    const_iterator original = *this;

                    if (--i == last) {
                        i--;
                    }
                    return original;
                }

                const const_iterator operator-(int n) const {
                    const_iterator result = *this;

                    for (int i = 0; i < n; i++) {
                        result--;
                    }

                    return result;
                }

                const Point & operator*() const {
                    return *i;
                }

                bool operator==(const const_iterator & other) const {
                    return i == other.i && last == other.last;
                }

                bool operator!=(const const_iterator & other) const {
                    return !(*this == other);
                }

            private:
                std::list<Point>::const_iterator i;
                std::list<Point>::const_iterator last;
        };

};

#endif
