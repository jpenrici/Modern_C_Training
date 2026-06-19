#pragma once

#include "shape.h"

typedef struct {
    Shape base;
    double width;
    double height;
} Rectangle;

Rectangle* new_Rectangle(double width, double height);
