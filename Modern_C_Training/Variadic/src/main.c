// Reference: https://cppreference.com/c/variadic

#include <stdarg.h>
#include <stdio.h>

void show(const char* fmt, ...);
void list(const char* fmt, ...);

int main()
{
    // The first string should contain the selection list.
    list("sdcffk", "Variadic", 1, 'a', 1.954, 50.5, 'b');
    show("sdcffk", "Variadic", 1, 'a', 1.954, 50.5, 'b');
    return 0;
}

void show(const char* fmt, ...)
{
    // va : variadic arguments
    va_list args;
    va_start(args, fmt);

    while (*fmt != '\0') {
        switch (*fmt) {
        // integers
        case 'd':
        case 'i': {
            // va_arg : accesses the next variadic
            printf("%d\n", va_arg(args, int));
            break;
        }
        // char
        case 'c': {
            printf("%c\n", va_arg(args, int));
            break;
        }
        // string
        case 's': {
            printf("%s\n", va_arg(args, char*));
            break;
        }
        // numbers with decimals
        case 'f': {
            printf("%f\n", va_arg(args, double));
            break;
        }
        default:
            printf("Unknow formatter: '%c'", *fmt);
        }

        fmt++;
    }

    va_end(args);
    printf("\n");
}

void list(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    printf("List: ");
    vfprintf(stdout, fmt, args);
    va_end(args);
    putchar('\n');
}
