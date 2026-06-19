#pragma once

typedef struct Shape Shape;

typedef struct {
    double (*get_area)(const Shape* self);
    void (*print)(const Shape* self);
} ShapeVTable;

struct Shape {
    const ShapeVTable* vtable;
    const char* label;
};

void print_Shape(const Shape* self);
