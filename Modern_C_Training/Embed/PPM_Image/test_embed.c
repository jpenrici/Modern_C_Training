#include <stdio.h>

const unsigned char image_data[] = {
#embed "./image.ppm"
};

int main()
{
    printf("Image uploaded via #embed!\n");
    printf("Data size: %zu bytes\n", sizeof(image_data));

    // Check if the first bytes match the PPM header (P6...)
    if (sizeof(image_data) > 2) {
        printf("File signature: %c%c\n", image_data[0], image_data[1]);
    }

    return 0;
}

// cc -std=c23 -o test test_embed.c && ./test
