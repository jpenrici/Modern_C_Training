#pragma once

#include <stddef.h> // size_t

/**
 * Modern C23 String Manipulation Study
 * Focus: Memory safety, Stack vs Heap, and pointer arithmetic.
 */

typedef struct {
    char* data; // The actual character buffer
    size_t length; // Number of characters (excluding \0)
    size_t capacity; // Total bytes allocated for data
} StrObj;

// --- HEAP ALLOCATORS (The "Factory" approach) ---

/**
 * Allocates a StrObj on the HEAP and its data buffer.
 * Use this when the lifecycle of the string is dynamic.
 */
StrObj* Str_New(size_t initial_cap);

/**
 * Allocates a StrObj object in the HEAP and initializes it with a string.
 */
StrObj* Str_Create(const char* string);

// --- STACK/REF INITIALIZERS ---

/**
 * Initializes a StrObj that ALREADY exists (likely on the STACK).
 * Allocates only the internal data buffer.
 */
bool Str_Init(StrObj* self, size_t initial_cap);

// --- DESTRUCTORS ---

/**
 * Cleans up the internal buffer only. Useful for Stack-allocated StrObjs.
 */
void Str_Cleanup(StrObj* self);

/**
 * Cleans up the buffer AND frees the struct itself. For Heap-allocated StrObjs.
 * Sets the pointer to nullptr to prevent Use-After-Free.
 */
void Str_Delete(StrObj** self);

// --- CORE MANIPULATION FUNCTIONS ---

/**
 * Ensures the buffer has enough capacity. Centralizes memory growth.
 */
bool Str_EnsureCapacity(StrObj* self, size_t needed);

/**
 * Appends a raw C-string to our StrObj.
 */
bool Str_Append(StrObj* self, const char* suffix);

/**
 * Reverses the string in-place.
 */
void Str_Invert(StrObj* self);

/**
 * Removes all occurrences of a specific character.
 * Used as a base for Trimming.
 */
void Str_RemoveChar(StrObj* self, char target);

/**
 * Trim Left: Removes leading spaces.
 */
void Str_TrimLeft(StrObj* self);

/**
 * Trim Right: Simply moves the null terminator.
 */
void Str_TrimRight(StrObj* self);

/**
 * Returns a new Heap-allocated StrObj containing a portion of the original.
 */
StrObj* Str_Substring(const StrObj* self, size_t start, size_t count);

/**
 * Creates a complete deep copy of the string on the Heap.
 * Reuses Substring logic: from index 0 to the full length.
 */
StrObj* Str_Copy(const StrObj* self);

/**
 * Formats a string into the StrObj buffer using printf-style tokens.
 * It automatically handles memory expansion.
 */
bool Str_Format(StrObj* self, const char* format, ...);

/**
 * Splits a string into an array of StrObj pointers based on a delimiter.
 * The resulting array is NULL-terminated for easy iteration.
 * Returns: StrObj** (an array of pointers)
 */
StrObj** Str_Split(const StrObj* self, char delimiter, size_t* out_count);

/**
 * Table Visualizer
 */
void Str_Show(const StrObj* str);
