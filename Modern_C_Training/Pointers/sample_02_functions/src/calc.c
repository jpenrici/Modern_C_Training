#include "calc.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  OperationType type;
  bool (*execute)(double, double, double *);
} Operation;

static bool sum(double a, double b, double *result) {
  *result = a + b;
  return true;
}

static bool multiply(double a, double b, double *result) {
  *result = a * b;
  return true;
}

static bool subtract(double a, double b, double *result) {
  *result = a - b;
  return true;
}

static bool divide(double a, double b, double *result) {
  if (b == 0.0) {
    printf("Error: Division by zero!\n");
    return false;
  }
  *result = a / b;
  return true;
}

static bool safe_execute(const Operation registry[], OperationType op, double a,
                         double b, double *result) {
  if (op >= LIMIT) {
    printf("Error: Invalid operation!\n");
    return false;
  }

  if (registry[op].execute == nullptr) {
    printf("Error: Function not implemented!");
    return false;
  }

  return registry[op].execute(a, b, result);
}

bool calc(OperationType op, double a, double b, double *result) {

  const Operation app_operations[LIMIT] = {
      [SUM] = {.type = SUM, .execute = sum},
      [MULTIPLY] = {.type = MULTIPLY, .execute = multiply},
      [SUBTRACT] = {.type = SUBTRACT, .execute = subtract},
      [DIVIDE] = {.type = DIVIDE, .execute = divide}};

  return safe_execute(app_operations, op, a, b, result);
}
