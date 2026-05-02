#include "charHandler.h"

#include <stdio.h>

int main()
{
    printf("--- TEST 1: HEAP ALLOCATION ---\n");
    StrObj* hStr = Str_New(10);
    Str_Append(hStr, "  Hello C23  ");
    printf("Original: [%s] (Len: %zu)\n", hStr->data, hStr->length);

    Str_TrimLeft(hStr);
    Str_TrimRight(hStr);
    printf("Trimmed : [%s]\n", hStr->data);

    Str_Invert(hStr);
    printf("Inverted: [%s]\n", hStr->data);

    // Clean up Heap Object
    Str_Delete(&hStr);

    printf("\n--- TEST 2: STACK ALLOCATION (REF) ---\n");
    StrObj sStr; // Struct is on the STACK
    Str_Init(&sStr, 32); // Only buffer is on HEAP

    Str_Append(&sStr, "Learning_C_Modern_Style");
    Str_RemoveChar(&sStr, '_');
    printf("No Underscores: %s\n", sStr.data);

    StrObj* sub = Str_Substring(&sStr, 8, 6);
    if (sub) {
        printf("Substring (8,6): %s\n", sub->data);
        Str_Delete(&sub);
    }

    Str_Cleanup(&sStr); // Free only the buffer, sStr disappears when main ends

    return 0;
}
