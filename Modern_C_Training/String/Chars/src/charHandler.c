#include "charHandler.h"

#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

StrObj* Str_New(size_t initial_cap)
{
    StrObj* s = malloc(sizeof(StrObj));
    if (s == nullptr)
        return nullptr;

    s->data = malloc(initial_cap + 1);
    if (s->data == nullptr) {
        free(s);
        return nullptr;
    }

    s->data[0] = '\0';
    s->length = 0;
    s->capacity = initial_cap;
    return s;
}

StrObj* Str_Create(const char* str)
{
    if (str == nullptr)
        return nullptr;

    size_t len = 0;
    while (str[len] != '\0')
        len++;

    StrObj* s = Str_New(len);
    if (s == nullptr)
        return nullptr;

    for (size_t i = 0; i < len; i++)
        s->data[i] = str[i];

    s->data[len] = '\0';
    s->length = len;

    return s;
}

bool Str_Init(StrObj* self, size_t initial_cap)
{
    if (self == nullptr)
        return false;

    self->data = malloc(initial_cap + 1);
    if (self->data == nullptr)
        return false;

    self->data[0] = '\0';
    self->length = 0;
    self->capacity = initial_cap;
    return true;
}

void Str_Cleanup(StrObj* self)
{
    if (self != nullptr && self->data != nullptr) {
        free(self->data);
        self->data = nullptr;
        self->length = 0;
        self->capacity = 0;
    }
}

void Str_Delete(StrObj** self)
{
    if (self != nullptr && *self != nullptr) {
        Str_Cleanup(*self);
        free(*self);
        *self = nullptr;
    }
}

bool Str_EnsureCapacity(StrObj* self, size_t needed)
{
    if (self->capacity >= needed)
        return true;

    size_t new_cap = self->capacity * 2;
    if (new_cap < needed)
        new_cap = needed;

    char* new_ptr = realloc(self->data, new_cap + 1);
    if (new_ptr == nullptr)
        return false;

    self->data = new_ptr;
    self->capacity = new_cap;
    return true;
}

bool Str_Append(StrObj* self, const char* suffix)
{
    if (self == nullptr || suffix == nullptr)
        return false;

    size_t s_len = 0;
    while (suffix[s_len])
        s_len++;

    if (!Str_EnsureCapacity(self, self->length + s_len))
        return false;

    for (size_t i = 0; i < s_len; i++) {
        self->data[self->length + i] = suffix[i];
    }

    self->length += s_len;
    self->data[self->length] = '\0';
    return true;
}

void Str_Invert(StrObj* self)
{
    if (self == nullptr || self->length < 2)
        return;
    for (size_t i = 0, j = self->length - 1; i < j; i++, j--) {
        char tmp = self->data[i];
        self->data[i] = self->data[j];
        self->data[j] = tmp;
    }
}

void Str_RemoveChar(StrObj* self, char target)
{
    if (self == nullptr || self->length == 0)
        return;

    size_t write_idx = 0;
    for (size_t read_idx = 0; read_idx < self->length; read_idx++) {
        if (self->data[read_idx] != target) {
            self->data[write_idx++] = self->data[read_idx];
        }
    }
    self->length = write_idx;
    self->data[write_idx] = '\0';
}

void Str_TrimLeft(StrObj* self)
{
    if (self == nullptr || self->length == 0)
        return;

    size_t start = 0;
    while (start < self->length && self->data[start] == ' ')
        start++;

    if (start > 0) {
        for (size_t i = 0; i < self->length - start; i++) {
            self->data[i] = self->data[start + i];
        }
        self->length -= start;
        self->data[self->length] = '\0';
    }
}

void Str_TrimRight(StrObj* self)
{
    if (self == nullptr || self->length == 0)
        return;

    while (self->length > 0 && self->data[self->length - 1] == ' ') {
        self->length--;
    }
    self->data[self->length] = '\0';
}

StrObj* Str_Substring(const StrObj* self, size_t start, size_t count)
{
    if (self == nullptr || start >= self->length)
        return nullptr;
    if (start + count > self->length)
        count = self->length - start;

    StrObj* sub = Str_New(count);
    if (sub == nullptr)
        return nullptr;

    for (size_t i = 0; i < count; i++) {
        sub->data[i] = self->data[start + i];
    }
    sub->data[count] = '\0';
    sub->length = count;
    return sub;
}

StrObj* Str_Copy(const StrObj* self)
{
    if (self == nullptr)
        return nullptr;
    return Str_Substring(self, 0, self->length);
}

bool Str_Format(StrObj* self, const char* format, ...)
{
    if (self == nullptr || format == nullptr)
        return false;

    va_list args;
    va_start(args, format);

    // 1. Dry run: find out how much space we need
    va_list tmp;
    va_copy(tmp, args);
    int needed = vsnprintf(nullptr, 0, format, tmp);
    va_end(tmp);

    if (needed < 0) {
        va_end(args);
        return false;
    }

    // 2. Ensure capacity (Memory Safety)
    if (!Str_EnsureCapacity(self, (size_t)needed)) {
        va_end(args);
        return false;
    }

    // 3. Actual formatting
    vsnprintf(self->data, self->capacity + 1, format, args);
    self->length = (size_t)needed;

    va_end(args);

    return true;
}

StrObj** Str_Split(const StrObj* self, char delimiter, size_t* out_count)
{
    if (self == nullptr || self->length == 0)
        return nullptr;

    // 1. Count delimiters to determine array size
    size_t count = 1;
    for (size_t i = 0; i < self->length; i++) {
        if (self->data[i] == delimiter)
            count++;
    }

    // 2. Allocate the array of pointers (plus one for a NULL terminator)
    StrObj** result = malloc((count + 1) * sizeof(StrObj*));
    if (result == nullptr)
        return nullptr;

    if (count == 1) {
        result[0] = Str_Substring(self, 0, self->length);
        result[1] = nullptr; // Sentinel value (NULL terminator)
        if (out_count)
            *out_count = 1;
        return result;
    }

    size_t start = 0;
    size_t current_part = 0;

    for (size_t i = 0; i <= self->length; i++) {
        // Look for delimiter OR end of string
        if (self->data[i] == delimiter || i == self->length) {
            size_t part_len = i - start;

            // Reuse your Str_Substring logic!
            result[current_part] = Str_Substring(self, start, part_len);

            current_part++;
            start = i + 1;
        }
    }

    result[count] = nullptr; // Sentinel value (NULL terminator)
    if (out_count)
        *out_count = count;

    return result;
}

void Str_Show(const StrObj* str)
{
    if (str == nullptr)
        return;

    printf("  KEY          | VALUE\n");
    printf("---------------+----------------------------------------------------------------\n");
    printf("  Data         | %s\n", str->data);
    printf("  Length       | %zu\n", str->length);
    printf("  Capacity     | %zu\n", str->capacity);
    printf("  Struct Addr  | %p\n", (void*)str);
    printf("  Buffer Addr  | %p\n", (void*)str->data);

    if (str->length > 0) {
        char first_char = str->data[0];
        char* first_addr = &str->data[0];
        char last_char = str->data[str->length - 1];
        char* last_addr = &str->data[str->length - 1];

        if (isprint((unsigned char)first_char))
            printf("  First Char   | '%c' (at %p)\n", first_char, (void*)first_addr);
        else
            printf("  First Char   | NON-PRINTABLE (0x%02X) at %p\n", (unsigned char)first_char, (void*)first_addr);

        if (isprint((unsigned char)last_char))
            printf("  Last Char    | '%c' (at %p)\n", last_char, (void*)last_addr);
        else
            printf("  Last Char    | NON-PRINTABLE (0x%02X) at %p\n", (unsigned char)last_char, (void*)last_addr);
    }
}
