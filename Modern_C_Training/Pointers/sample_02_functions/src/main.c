#include "calc.h"
#include <stdio.h>

static void print_result(const char *label, bool ok, double result)
{
    if (ok) {
        printf("%s: %.2f\n", label, result);
    }
    else {
        printf("%s: error\n", label);
    }
}

int main()
{
    double a = 10, b = 5;
    double result;
    bool ok;

    ok = calc(SUM, a, b, &result);
    print_result("Sum", ok, result);

    ok = calc(MULTIPLY, a, b, &result);
    print_result("Multiply",  ok, result);

    ok = calc(SUBTRACT, a, b, &result);
    print_result("Subtract",  ok, result);

    ok = calc(DIVIDE, a, b, &result);
    print_result("Divide",  ok, result);

    ok = calc(DIVIDE, a, 0.0, &result);
    print_result("Divide by zero",  ok, result);

    ok = calc((OperationType)99, a, b, &result);
    print_result("Invalid",  ok, result);

    ok = calc((OperationType)0, 10, 10, &result);
    print_result("Sum",  ok, result);

    return 0;
}
