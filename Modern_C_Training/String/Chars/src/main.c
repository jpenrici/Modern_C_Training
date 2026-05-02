#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    char* data; // string
    size_t length; // current string size
    size_t capacity; // total space allocated in memory
} String;

String* init(size_t length);
String* create(const char* str);
String* copy(const String* str, size_t start, size_t end);
String* invert(const String* str);
String* remove_char(const String* str, size_t start, size_t end, char character);

bool redim(String* str, size_t length);

void destroy(String* str);
void show(const String* str);

int main()
{
    String* arr[] = {
        init(0), // valid string but empty
        init(10), // reserved space
        create(""), // empty string
        create("__Hello__"), // with repeated characters
    };

    show(arr[0]);

    show(arr[1]);
    redim(arr[1], 20);
    show(arr[1]);

    show(arr[2]);
    show(arr[3]);
    arr[2] = remove_char(arr[3], 0, 3, '_');
    show(arr[2]);

    for (size_t i = 0; i < 4; i++) {
        destroy(arr[i]);
    }

    return 0;
}

void show(const String* str)
{
    if (str == nullptr)
        return;

    printf("Data     : '%s'\n", str->data);
    printf("Length   : %zu\n", str->length);
    printf("Capacity : %zu\n", str->capacity);
    printf("Address  : %p (String struct)\n", (void*)str);
    printf("Address  : %p (Buffer address - data)\n", (void*)str->data);

    if (str->length > 0) {
        char first_char = str->data[0];
        char* first_addr = &str->data[0];
        char last_char = str->data[str->length - 1];
        char* last_addr = &str->data[str->length - 1];
        printf("First char : %c (Address: %p)\n", first_char, (void*)first_addr);
        printf("Last  char : %c (Address: %p)\n", last_char, (void*)last_addr);
    }

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

String* create(const char* str)
{
    if (str == nullptr)
        return nullptr;

    size_t len = 0;
    while (str[len] != '\0')
        len++;

    String* new_str = init(len);
    if (new_str == nullptr)
        return nullptr;

    for (size_t i = 0; i < len; i++)
        new_str->data[i] = str[i];

    new_str->data[len] = '\0';
    new_str->length = len;

    return new_str;
}

bool redim(String* str, size_t length)
{
    if (str == nullptr)
        return false;

    if (str->length >= length)
        return true;

    char* new_data = realloc(str->data, length + 1);
    if (new_data == nullptr)
        return false;

    str->data = new_data;
    str->length = length;
    str->capacity = length + 1;

    return true;
}

void destroy(String* str)
{
    if (str) {
        free(str->data);
        free(str);
    }
}

String* copy(const String* str, size_t start, size_t end)
{
    if (str == nullptr || start > end || end > str->length)
        return nullptr;

    size_t new_len = end - start;
    String* str_cpy = init(new_len);
    if (str_cpy == nullptr)
        return nullptr;

    for (size_t i = 0; i < new_len; i++)
        str_cpy->data[i] = str->data[start + i];

    str_cpy->data[new_len] = '\0';
    str_cpy->length = new_len;

    return str_cpy;
}

String* remove_char(const String* str, size_t start, size_t end, char character)
{
    if (str == nullptr || start > end || end >= str->length)
        return nullptr;

    if (str->length == 0)
        return nullptr;

    String* str_cpy = init(str->length);
    if (str_cpy == nullptr)
        return nullptr;

    size_t current_len = 0;
    while (start < str->length && start <= end && str->data[start] != character)
        str_cpy->data[current_len++] = str->data[start++];

    str_cpy->data[current_len] = '\0';

    String* srt_removed = copy(str_cpy, 0, current_len);
    if (srt_removed == nullptr)
        return nullptr;

    return srt_removed;
}

String* invert(const String* str)
{
    if (str == nullptr)
        return nullptr;

    String* srt_inverted = init(str->length);
    if (srt_inverted == nullptr)
        return nullptr;

    for (size_t i = 0; i < str->length; i++)
        srt_inverted->data[i] = str->data[str->length - 1 - i];

    srt_inverted->data[str->length] = '\0';
    srt_inverted->length = str->length;

    return srt_inverted;
}
