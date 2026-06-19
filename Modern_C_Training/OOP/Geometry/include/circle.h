#pragma once

#include "shape.h"

typedef struct {
    Shape base;
    double radius;
} Circle;

Circle* new_Circle(double radius);
