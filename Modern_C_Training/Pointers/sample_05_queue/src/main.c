#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  const char *label; // queue label
  size_t capacity;   // monitors queue size
  size_t length;     // current quantity of items
  size_t begin;      // initial position index
  size_t end;        // final position index
  double items[];    // flexible array member
} Queue;

constexpr size_t INITIAL_CAPACITY = 5; // Test

Queue *initialize(const char *label);
void destroy(Queue *q);
static bool redim(Queue **qp, size_t new_capacity);

void enqueue(Queue **q, double value);
bool dequeue(Queue *q, double *value);
void show(Queue *q);

int main(void) {
  char label[] = "Queue";
  Queue *q = initialize(label);
  if (q == nullptr) {
    return EXIT_FAILURE;
  }

  // Enqueue elements
  double values[] = {7.5, -9.0, 20.2, 5.8, 10.05, -1.7};
  for (int i = 0; i < 6; ++i) {
    enqueue(&q, values[i]);
    printf("Enqueue: %.2f (%lu items)\n", values[i], q->length);
  }

  show(q);

  // Dequeue front elements
  double value = 0;
  size_t limit = q->length - 1;
  for (size_t i = 0; i < limit; ++i) {
    if (dequeue(q, &value)) {
      printf("Dequeue: %.2f (%lu items)\n", value, q->length);
    }
  }

  show(q);

  // Free
  destroy(q);

  return EXIT_SUCCESS;
}

Queue *initialize(const char *label) {
  Queue *q;
  size_t capacity = INITIAL_CAPACITY;

  q = malloc(sizeof(Queue) + capacity * sizeof(double));
  if (q == nullptr) {
    return nullptr;
  }

  q->label = nullptr;
  if (label != nullptr && strlen(label) > 0) {
    q->label = strdup(label);
  }

  q->capacity = capacity;
  q->length = 0;
  q->begin = 0;
  q->end = 0;

  return q;
}

void destroy(Queue *q) {
  if (q == nullptr)
    return;
  free((char *)q->label);
  free(q);
}

void enqueue(Queue **qp, double value) {
  if (qp == nullptr || *qp == nullptr) {
    return;
  }

  Queue *q = *qp;
  if (q->capacity == 0) {
    return;
  }

  if (q->end == q->capacity) {
    if (!redim(qp, 2 * q->capacity))
      return;
    q = *qp;
  }

  q->items[q->end] = value;
  q->end++;

  q->length = (q->end >= q->begin) ? q->end - q->begin : 0;
}

bool dequeue(Queue *q, double *value) {
  if (q == nullptr)
    return false;

  if (q->begin >= q->end) {
    q->length = 0;
    return false;
  }

  *value = q->items[q->begin++];
  q->length = q->end - q->begin;

  return true;
}

void show(Queue *q) {
  if (q == nullptr) {
    return;
  }

  if (q->label != nullptr && strlen(q->label) > 0)
    printf("%s\n", q->label);

  printf("[");
  for (size_t i = q->begin; i != q->end; ++i) {
    printf("%.2f"
           "%s",
           q->items[i], i < q->end - 1 ? " " : "");
  }
  printf("]\n");
  printf("%ld items\n", q->length);
}

static bool redim(Queue **qp, size_t new_capacity) {
  if (qp == nullptr || *qp == nullptr) {
    return false;
  }

  Queue *tmp = realloc(*qp, sizeof(Queue) + new_capacity * sizeof(double));
  if (tmp == nullptr) {
    return false;
  }

  tmp->capacity = new_capacity;
  *qp = tmp; // update the caller's pointer

  return true;
}
