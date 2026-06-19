#include "shape.h"

#include <stdio.h>

void print_Shape(const Shape* self)
{
    if (!self || !self->vtable)
        return;

    printf("Shape: %s\n", self->label);
    self->vtable->print(self);
    printf("Area: %.2f\n\n", self->vtable->get_area(self));
}
