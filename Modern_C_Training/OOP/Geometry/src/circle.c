#include "circle.h"

#include <stdio.h>
#include <stdlib.h>

#define PI 3.14159265358979323846

static double circle_get_area(const Shape* self)
{
    const Circle* circle = (const Circle*)self;
    return PI * circle->radius * circle->radius;
}

static void circle_print(const Shape* self)
{
    const Circle* circle = (const Circle*)self;
    printf("Radius: %.2f\n", circle->radius);
}

static const ShapeVTable circle_vtable = {
    .get_area = circle_get_area,
    .print = circle_print
};

Circle* new_Circle(double radius)
{
    Circle* c = malloc(sizeof(*c));
    if (!c)
        return nullptr;
    c->base.vtable = &circle_vtable;
    c->base.label = "Circle";
    c->radius = radius;
    return c;
}
