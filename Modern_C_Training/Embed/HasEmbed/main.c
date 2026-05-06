// Reference: https://cppreference.com/c/preprocessor/embed

#include <stdint.h>
#include <stdio.h>

void dump(const uint8_t arr[], size_t size)
{
    for (size_t i = 0; i != size; ++i)
        printf("%02X%c", arr[i], (i + 1) % 16 ? ' ' : '\n');
    puts("");
}

int main()
{

#if __has_embed("image.ppm")
    const uint8_t image_data[] = {
#embed "image.ppm"
    };
    printf("Success: Image loaded with %zu bytes.\n", sizeof(image_data));
#else
    const uint8_t image_data[] = { 0 };
    puts("Warning: image.ppm missing. Data initialized to zero.");
#endif

    puts("image_data[]:");
    dump(image_data, sizeof image_data);

#if __has_embed("message.txt")
    const char message[]
        = {
#embed "message.txt" if_empty('M', 'i', 's', 's', 'i', 'n', 'g')
              , '\0' // null terminator
          };
    printf("Message loaded: '%s'\n", message);
#else
    const char message[] = { 'M', 's', 'g', '!', '\0' };
    puts("Warning: message.txt not found. Using fallback.");
    printf("Message: '%s'\n", message);
#endif

    // message[]:
    // 4D 69 73 73 69 6E 67 00
    //  M  i  s  s  i  n  g \0
    puts("message[]:");
    dump((const uint8_t*)message, sizeof message);

    return 0;
}

// gcc -std=c23 -o test main.c
