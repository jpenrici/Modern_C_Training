#include <stdio.h>
#include <stdlib.h>

// Global variable: lives in the Data/BSS segment.
// Persists for the entire lifetime of the program.
int a = -10;

// Global static variable: scope is restricted to this file only.
static int file_static_a = -50;

void f1()
{
    // Local variable shadowing the global 'a'
    int a = 100;
    printf("Func 1 (Local)       : %d [%p]\n", a, (void*)&a);
}

// Demonstrating pointer behavior, stack addresses, and pass-by-reference
void f2(int* n)
{
    *n = 1000; // Modifies the value of the variable pointed to (local 'a' from main)

    printf("Func 2 (Value of *n) : %d\n", *n);

    // This prints the address STORED in n (which is the address of 'a' from main)
    printf("Func 2 (Address held by n -> points to Main's local): [%p]\n", (void*)n);

    // This prints the address OF the pointer variable 'n' itself on f2's stack frame
    printf("Func 2 (Address of pointer n itself on f2's stack) : [%p]\n", (void*)&n);
}

void f_static()
{
    // Local static variable: scoped to this function, but stored in the Data segment.
    // It retains its value between function calls and is initialized only once.
    static int static_counter = 0;
    static_counter++;
    printf("Static Func (Local static) : %d [%p]\n", static_counter, (void*)&static_counter);
}

void f_heap()
{
    // Dynamic allocation: allocates memory on the Heap segment
    int* heap_ptr = (int*)malloc(sizeof(int));
    if (heap_ptr == nullptr) {
        printf("Memory allocation failed\n");
        return;
    }
    *heap_ptr = 500;

    // heap_ptr itself is a local variable stored on the Stack
    printf("Heap Func (Pointer heap_ptr on Stack) : [%p]\n", (void*)&heap_ptr);
    // The memory heap_ptr points to is located on the Heap
    printf("Heap Func (Allocated memory on Heap)  : %d [%p]\n", *heap_ptr, (void*)heap_ptr);

    free(heap_ptr); // Always free heap memory to prevent leaks
}

int main()
{
    // 1. Global Variables
    printf("--- Global & File Static Scope ---\n");
    printf("Global 'a'                 : %d [%p]\n", a, (void*)&a);
    printf("File Static 'file_static_a': %d [%p]\n", file_static_a, (void*)&file_static_a);
    printf("\n");

    // 2. Local Scope & Shadowing
    printf("--- Local & Block Scopes (Stack) ---\n");
    int a = 10; // Shadows global 'a'
    printf("Local 'a' (Main Stack)     : %d [%p]\n", a, (void*)&a);

    {
        // Block 1: No local variable defined here. Uses Main's local 'a'.
        printf("Block 1 (No new var)       : %d [%p]\n", a, (void*)&a);
    }

    {
        // Block 2: New variable 'a' defined. Shadows Main's local 'a'.
        int a = 20;
        printf("Block 2 (Shadow var)       : %d [%p]\n", a, (void*)&a);
    }

    {
        // Block 3: Block 2's 'a' is dead. Back to Main's local 'a'.
        printf("Block 3 (Back to local)    : %d [%p]\n", a, (void*)&a);
    }

    printf("After Blocks (Main Local)  : %d [%p]\n", a, (void*)&a);
    printf("\n");

    // 3. Function Calls & Stack Frames
    printf("--- Function Scopes ---\n");
    f1();
    printf("\n");

    // 4. Pass by Reference / Pointer Inspection
    printf("--- Pass by Reference Inspection ---\n");
    printf("Main Local 'a' before f2   : %d [%p]\n", a, (void*)&a);
    f2(&a); // Passing the memory address of main's local 'a'
    printf("Main Local 'a' after f2    : %d [%p]\n", a, (void*)&a);
    printf("\n");

    // 5. Static Lifetimes (Data Segment vs Stack Segment)
    printf("--- Static Lifetime Scope ---\n");
    f_static();
    f_static(); // Calling again to show value retention and identical address
    printf("\n");

    // 6. Dynamic Memory (Heap Segment)
    printf("--- Heap Scope ---\n");
    f_heap();
    printf("\n");

    return 0;
}
