#include "charHandler.h"
#include "include/charHandler.h"

#include <stdio.h>
#include <stdlib.h>

void example();

int main()
{
    example();

    return 0;
}

void example()
{
    printf("--- TEST 1: HEAP ALLOCATION ---\n");
    auto hStr = Str_New(10);
    Str_Append(hStr, "  Hello C23  ");
    printf("Original: '%s' (Len: %zu)\n", hStr->data, hStr->length);

    Str_TrimLeft(hStr);
    Str_TrimRight(hStr);
    printf("Trimmed : '%s' (Len: %zu)\n", hStr->data, hStr->length);

    Str_Invert(hStr);
    printf("Inverted: '%s' (Len: %zu)\n", hStr->data, hStr->length);

    // Clean up Heap Object
    Str_Delete(&hStr);

    printf("\n--- TEST 2: STACK ALLOCATION (REF) ---\n");
    StrObj sStr; // Struct is on the STACK
    Str_Init(&sStr, 32); // Only buffer is on HEAP

    Str_Append(&sStr, "Learning_C_Modern_Style");
    Str_RemoveChar(&sStr, '_');
    printf("No Underscores: '%s' (Len: %zu)\n", sStr.data, sStr.length);

    auto sub = Str_Substring(&sStr, 8, 6);
    if (sub != nullptr) {
        printf("Substring (8,6): '%s' (Len: %zu)\n", sub->data, sub->length);

        auto cpy = Str_Copy(sub);
        if (cpy != nullptr) {
            Str_Invert(cpy);
            printf("Reverse copy: '%s' (Len: %zu)\n", cpy->data, cpy->length);
            Str_Delete(&cpy);
        }

        Str_Delete(&sub);
    }

    printf("\n--- TEST 3: FORMAT ---\n");
    StrObj fmtStr;
    Str_Init(&fmtStr, sStr.capacity);
    if (Str_Format(&fmtStr, "No Underscores: %s\n", sStr.data))
        Str_Show(&fmtStr);

    // Free only the buffer
    Str_Cleanup(&fmtStr);
    Str_Cleanup(&sStr);

    printf("\n--- TEST 4: SPLIT ---\n");
    auto txt = "C-style string manipulation.";
    StrObj* txtStr = Str_Create(txt);

    size_t count;
    auto parts = Str_Split(txtStr, ' ', &count); // StrObj**

    if (parts != nullptr) {
        printf("Original: '%s' (Len: %zu)\n", txtStr->data, txtStr->length);
        for (size_t i = 0; i < count; i++) {
            printf("Part [%zu]: '%s' (Len: %zu)\n", i, parts[i]->data, parts[i]->length);
            Str_Delete(&parts[i]); // Clean up each object
        }
        free(parts); // Clean up the array of pointers
    }
}
