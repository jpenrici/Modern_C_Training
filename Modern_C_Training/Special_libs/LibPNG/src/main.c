// Reference: https://en.wikipedia.org/wiki/Julia_set

#include <math.h>
#include <png.h>
#include <regex.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    uint8_t r, g, b, a;
} Color;

typedef struct {
    double real, imag;
} Complex;

typedef struct {
    unsigned width, height;
    Color pixels[]; // Flexible Array Member
} Image;

[[nodiscard]] int generate(char filename[], unsigned width, unsigned height);

static Image* image_create(unsigned width, unsigned height);
static void image_destroy(Image* image);
static bool image_set_pixel(Image* image, unsigned x, unsigned y, Color color);
static bool image_render(Image* image);
static bool image_export(const char* filename, const Image* image);

static Color color_from_iteration(unsigned iteration, unsigned max_iteration);

int main(void)
{
    return generate("example.png", 800, 800);
}

[[nodiscard]] int generate(char filename[], unsigned width, unsigned height)
{
    if (width == 0 || height == 0) {
        printf("Invalid dimensions!");
        return EXIT_FAILURE;
    }

    regex_t regex;
    const char* pattern = "^.+\\.[pP][nN][gG]$";

    int reti = regcomp(&regex, pattern, REG_EXTENDED | REG_NOSUB);

    if (reti != 0) {
        printf("Invalid filename: '%s'\n", filename);
        filename = "output.png";
        printf("Using default: '%s'\n", filename);
    }

    regfree(&regex);

    Image* image = image_create(width, height);
    if (image != nullptr) {
        if (image_render(image)) {
            if (image_export(filename, image)) {
                printf("Image generated: %s\n", filename);
                return EXIT_SUCCESS;
            }
        }
    }

    image_destroy(image);

    return EXIT_FAILURE;
}

static Image* image_create(unsigned width, unsigned height)
{
    size_t pixel_count = (size_t)width * (size_t)height;

    Image* image = malloc(sizeof(Image) + sizeof(Color) * pixel_count);

    if (image == nullptr) {
        fprintf(stderr, "Failed to allocate image memory.\n");
        return nullptr;
    }

    image->width = width;
    image->height = height;
    memset(image->pixels, 0, sizeof(Color) * pixel_count);

    return image;
}

static void image_destroy(Image* image)
{
    free(image);
}

static bool image_set_pixel(Image* image, unsigned x, unsigned y, Color color)
{
    if (image == nullptr)
        return false;

    if (x >= image->width || y >= image->height)
        return false;

    image->pixels[y * image->width + x] = color;

    return true;
}

static bool image_render(Image* image)
{
    if (image == nullptr) {
        return false;
    }

    // Render Julia Set
    const Complex c = { .real = -0.7, .imag = 0.27015 };

    const unsigned max_iteration = 300;

    for (unsigned y = 0; y < image->height; ++y) {
        for (unsigned x = 0; x < image->width; ++x) {
            double zx = 1.5 * (x - image->width / 2.0) / (0.5 * image->width);
            double zy = (y - image->height / 2.0) / (0.5 * image->height);

            unsigned iteration = 0;
            while ((zx * zx + zy * zy < 4.0) && iteration < max_iteration) {
                double xtemp = zx * zx - zy * zy + c.real;
                zy = 2.0 * zx * zy + c.imag;
                zx = xtemp;
                ++iteration;
            }

            Color color = color_from_iteration(iteration, max_iteration);
            image_set_pixel(image, x, y, color);
        }
    }

    return true;
}

static bool image_export(const char* filename, const Image* image)
{
    if (image == nullptr)
        return false;

    FILE* file = fopen(filename, "wb");
    if (file == nullptr) {
        fprintf(stderr, "Failed to open output file.\n");
        return false;
    }

    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (png == nullptr) {
        fprintf(stderr, "Failed to create PNG write struct.\n");
        fclose(file);
        return false;
    }

    png_infop info = png_create_info_struct(png);
    if (info == nullptr) {
        fprintf(stderr, "Failed to create PNG info struct.\n");
        png_destroy_write_struct(&png, nullptr);
        fclose(file);
        return false;
    }

    if (setjmp(png_jmpbuf(png))) {
        fprintf(stderr, "libpng write error.\n");
        png_destroy_write_struct(&png, &info);
        fclose(file);
        return false;
    }

    png_init_io(png, file);

    png_set_IHDR(
        png,
        info,
        image->width,
        image->height,
        8,
        PNG_COLOR_TYPE_RGBA,
        PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_DEFAULT,
        PNG_FILTER_TYPE_DEFAULT);

    png_write_info(png, info);

    png_bytep* rows = malloc(sizeof(png_bytep) * (size_t)image->height);
    if (rows == nullptr) {
        fprintf(stderr, "Failed to allocate PNG rows.\n");
        png_destroy_write_struct(&png, &info);
        fclose(file);
        return false;
    }

    for (unsigned y = 0; y < image->height; ++y) {
        rows[y] = (png_bytep)&image->pixels[y * image->width];
    }

    png_write_image(png, rows);
    png_write_end(png, nullptr);

    free(rows);
    png_destroy_write_struct(&png, &info);
    fclose(file);

    return true;
}

static Color color_from_iteration(unsigned iteration, unsigned max_iteration)
{
    if (iteration >= max_iteration)
        return (Color) { 0, 0, 0, 255 };

    double t = (double)iteration / (double)max_iteration;

    uint8_t r = (uint8_t)(9.0 * (1.0 - t) * t * t * t * 255.0);
    uint8_t g = (uint8_t)(15.0 * (1.0 - t) * (1.0 - t) * t * t * 255.0);
    uint8_t b = (uint8_t)(8.5 * (1.0 - t) * (1.0 - t) * (1.0 - t) * t * 255.0);

    return (Color) { r, g, b, 255 };
}
