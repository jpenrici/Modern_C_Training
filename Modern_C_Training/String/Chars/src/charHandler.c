#include "charHandler.h"

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
