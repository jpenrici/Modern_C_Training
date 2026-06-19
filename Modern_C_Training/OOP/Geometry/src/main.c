#include "circle.h"
#include "rectangle.h"

#include <stdlib.h>

int main(void)
{
    Circle* circle = new_Circle(3.0);
    Rectangle* rectangle = new_Rectangle(4.0, 5.0);

    Shape* shapes[] = {
        (Shape*)circle,
        (Shape*)rectangle
    };

    for (size_t i = 0; i < 3; i++) {
        print_Shape(shapes[i]);
    }

    free(circle);
    free(rectangle);
}
