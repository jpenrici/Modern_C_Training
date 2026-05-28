#include <stdio.h>
#include <stdlib.h>

struct A {
    char a; // 1 byte + 3 byte padding
    int b; // 4 bytes
    float c; // 4 bytes
};

struct B {
    char a;
    int b;
    float c;
} global_B;

typedef struct C {
    char a;
    int b;
    float c;
} type_alias_C;

typedef struct {
    char a;
    int b;
    float c;
} type_alias_D;

int main(void)
{
    struct A local_a1 = { 1, 2, 3 }; // implicit conversion to float

    // global_B = { 1, 2, 3 }; // error
    // global_B = { .a = 1, .b = 2, .c = 3 }; // error
    global_B.a = 1;
    global_B.b = 2;
    global_B.c = 3;
    struct B local_b1 = global_B; // copy by value

    struct C local_c1 = { .a = 1, .b = 2, .c = 3 };
    type_alias_C local_c2 = { .c = 3, .a = 1, .b = 2 };

    type_alias_D local_d1 = { 1, 2, 3 };

    struct A a2 = local_a1; // copy by value
    a2.b = 10; // does not affect the original

    // Compound literals
    struct B* b2 = &(struct B) { .a = 10, .c = 20 };
    global_B = (struct B) { .a = 5, .b = 10, .c = -1 };

    // Pointer and malloc
    type_alias_C* c3 = malloc(sizeof(type_alias_C));
    *c3 = (type_alias_C) { .a = 100 };
    c3->b = 15;
    free(c3);

    type_alias_D* array_d2 = malloc(3 * sizeof(type_alias_D));
    array_d2[0] = (type_alias_D) { .a = 100 };
    array_d2[1] = (type_alias_D) { .a = 101 };
    array_d2[2] = (type_alias_D) { .a = 102 };
    free(array_d2);

    printf("Compiled without alerts!\n");

    return 0;
}

// gcc -std=c23 -o test main.c && ./test
