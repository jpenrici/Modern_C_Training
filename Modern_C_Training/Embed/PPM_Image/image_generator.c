#include <stdbool.h>
#include <stdio.h>
#include <string.h>

bool image_generator(const char* name, const unsigned width, const unsigned height)
{
    if (name == nullptr) {
        printf("Invalid filename!");
        return false;
    }
    if (width == 0 || height == 0) {
        printf("Invalid dimension!");
        return false;
    }

    const char* extension = ".ppm";

    size_t buffer_size = strlen(name) + strlen(extension) + 1;
    char buffer[buffer_size];

    snprintf(buffer, buffer_size, "%s%s", name, extension);

    FILE* f = fopen(buffer, "wb");
    if (f == nullptr) {
        perror("Error opening file");
        return false;
    }

    // P6: PPM binary header
    fprintf(f, "P6\n%d %d\n255\n", width, height);

    for (unsigned y = 0; y < height; y++) {
        for (unsigned x = 0; x < width; x++) {
            // 4. Casting e lógica de cores
            unsigned char pixel[3] = {
                (unsigned char)(x % 256),
                (unsigned char)(y % 256),
                128
            };
            fwrite(pixel, 1, 3, f);
        }
    }
    fclose(f);

    printf("Image created: %s\n", buffer);

    return true;
}

int main(void)
{
    if (!image_generator("image", 400, 400)) {
        return 1;
    }
    return 0;
}

// cc -std=c23 -o image_generator image_generator.c && ./image_generator
