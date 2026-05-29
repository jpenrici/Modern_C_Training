#include <stdio.h>

typedef struct {
    union {
        // Coordinates
        struct {
            float x, y, z;
        };
        // Color
        struct {
            float r, g, b;
        };
        // Array
        float numbers[3];
    };
} Vector3D;

int main()
{

    Vector3D vec3d;

    printf("%lu bytes\n", sizeof(Vector3D)); // 12 bytes

    vec3d = (Vector3D) { .x = 10.0, .y = 20.0, .z = 5.0 };

    printf("x   = %.2f , y   = %.2f, z   = %.2f\n", vec3d.x, vec3d.y, vec3d.z);
    printf("r   = %.2f , g   = %.2f, b   = %.2f\n", vec3d.r, vec3d.g, vec3d.b);
    printf("[0] =%.2f ,  [1] = %.2f, [2] = %.2f\n", vec3d.numbers[0], vec3d.numbers[1], vec3d.numbers[2]);

    return 0;
}
