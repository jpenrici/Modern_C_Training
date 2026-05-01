#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    char* data;
    size_t length;
    size_t capacity;
} String;

String* init(size_t length);
String* create(const char* string);
String* copy(const String* string, size_t start, size_t end);
String* invert(const String* string);

void destroy(String* string);
void show(const String* string);

int main()
{
    auto str = create("String");
    show(str);

    auto str1 = invert(str);
    show(str1);

    destroy(str);
    destroy(str1);

    return 0;
}

void show(const String* string)
{
    if (!string)
        return;

    printf("Data    : '%s'\n", string->data);
    printf("Length  : %zu\n", string->length);
    printf("Capacity: %zu\n", string->capacity);
    printf("---------------------------\n");
}

String* init(size_t length)
{
    String* str = malloc(sizeof(String));
    if (str == nullptr)
        return nullptr;

    str->data = malloc(length + 1);
    if (str->data == nullptr) {
        free(str);
        return nullptr;
    }

    str->data[0] = '\0';
    str->length = 0;
    str->capacity = length + 1;

    return str;
}

String* create(const char* string)
{
    if (!string)
        return nullptr;

    size_t len = 0;
    while (string[len] != '\0')
        len++;

    String* str = init(len);
    if (!str)
        return nullptr;

    for (size_t i = 0; i < len; i++) {
        str->data[i] = string[i];
    }

    str->data[len] = '\0';
    str->length = len;

    return str;
}

String* copy(const String* string, size_t start, size_t end)
{
    if (!string || start > end || end > string->length) {
        return nullptr;
    }

    size_t new_len = end - start;
    String* str_cpy = init(new_len);
    if (!str_cpy)
        return nullptr;

    for (size_t i = 0; i < new_len; i++) {
        str_cpy->data[i] = string->data[start + i];
    }

    str_cpy->data[new_len] = '\0';
    str_cpy->length = new_len;

    return str_cpy;
}

String* invert(const String* string)
{
    if (!string)
        return nullptr;

    String* inverted = init(string->length);
    if (!inverted)
        return nullptr;

    for (size_t i = 0; i < string->length; i++) {
        inverted->data[i] = string->data[string->length - 1 - i];
    }

    inverted->data[string->length] = '\0';
    inverted->length = string->length;

    return inverted;
}

void destroy(String* string)
{
    if (string) {
        free(string->data);
        free(string);
    }
}
