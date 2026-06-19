#include "rectangle.h"

#include <stdio.h>
#include <stdlib.h>

static double rectangle_get_area(const Shape* self)
{
    const Rectangle* rect = (const Rectangle*)self;
    return rect->width * rect->height;
}

static void rectangle_print(const Shape* self)
{
    const Rectangle* rect = (const Rectangle*)self;
    printf("Width: %.2f\nHeight: %.2f\n", rect->width, rect->height);
}

static const ShapeVTable rectangle_vtable = {
    .get_area = rectangle_get_area,
    .print = rectangle_print
};

Rectangle* new_Rectangle(double width, double height)
{
    Rectangle* r = malloc(sizeof(*r));
    if (!r)
        return nullptr;
    r->base.vtable = &rectangle_vtable;
    r->base.label = "Rectangle";
    r->width = width;
    r->height = height;
    return r;
}
