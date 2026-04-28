#include <stdio.h>
#include <stdlib.h>

#define print(x) _Generic((x), \
    _Bool:  printf("Boolean: %s\n", (x) ? "true" : "false"), \
    int:    printf("Integer: %d\n", (x)), \
    double: printf("Double: %f\n", (x)), \
    char*:  printf("String: %s\n", (x)), \
    default: printf("Unsupported type\n") \
)

#define swap(a, b) do { \
    typeof(a) _tmp = (a); \
    (a) = (b); \
    (b) = _tmp; \
} while (0)

int main()
{
    print(42);
    print(3.14);
    print("Modern C");

    int a = 10;
    int b = 20;
    swap(a, b);
    print(a);
    print(b);

    return 0;
}
