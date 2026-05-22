#include <stdbool.h>
#include <stdio.h>

typedef bool (*FilterPredicate)(int);
//      ^         ^             ^
//      return  function(alias) parameters

// Pointer Function
void filter_array(const int *original_array, int size, int *result,
                  int *new_size, FilterPredicate callback);

// Callbacks
bool is_even(int number);
bool is_greater_than_ten(int number);

// Auxiliary function to print arrays
void print_array(const int *array, int size);

// Test
int main(void) {
  int data[] = {2, 5, 8, 11, 14, 3, -22, 7};
  int size = sizeof(data) / sizeof(data[0]);

  int result[size]; // Array to store the output
  int new_size = 0;

  printf("Original Array: ");
  print_array(data, size);

  printf("\n--- Filtering Even Numbers ---\n");
  filter_array(data, size, result, &new_size, is_even);
  print_array(result, new_size);

  printf("\n--- Filtering Numbers Greater Than 10 ---\n");
  filter_array(data, size, result, &new_size, is_greater_than_ten);
  print_array(result, new_size);

  return 0;
}

// void filter_array(const int* original_array, int size,
//                   int* result, int* new_size,
//                   bool (*callback)(int)) {...} // without typedef
void filter_array(const int *original_array, int size, int *result,
                  int *new_size, FilterPredicate callback) {
  if (original_array == nullptr || result == nullptr || callback == nullptr) {
    *new_size = 0;
    return;
  }

  int j = 0;
  for (int i = 0; i < size; i++) {
    if (callback(original_array[i])) {
      result[j] = original_array[i];
      j++;
    }
  }
  *new_size = j;
}

// Callbacks
bool is_even(int number) { return number % 2 == 0; }
bool is_greater_than_ten(int number) { return number > 10; }

// Auxiliary function
void print_array(const int *array, int size) {

  if (array == nullptr)
    printf("Null array.");

  printf("[ ");
  for (int i = 0; i < size; i++) {
    printf("%d ", array[i]);
  }
  printf("]\n");
}
