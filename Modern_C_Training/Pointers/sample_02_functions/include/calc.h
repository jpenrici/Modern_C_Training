#pragma once

#include <stdbool.h>

typedef enum : unsigned {
    SUM,
    MULTIPLY,
    SUBTRACT,
    DIVIDE,
    LIMIT // limit of functions operations
} OperationType;

bool calc(OperationType op, double a, double b, double *result);
