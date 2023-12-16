#ifndef _POINT_H
#define _POINT_H
#include "point.h"
#endif

Point::Point(int x, int y) :
	x{x}, y{y}
{};

Point::Point(Point &p) : 
	x{p.x}, y{p.y}
{};

void Point::move(int x_offset, int y_offset){
	x += x_offset;
	y += y_offset;
}

void Point::moveTo(int x, int y){
	this->x = x;
	this->y = y;
}

