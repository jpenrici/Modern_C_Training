#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  uint8_t *buffer;
  size_t capacity;
  size_t offset;
} Arena;

Arena arena_create(size_t capacity) {
  return (Arena){
      .buffer = malloc(capacity),
      .capacity = capacity,
      .offset = 0,
  };
}

void *arena_alloc(Arena *a, size_t size) {
  if (a->offset + size > a->capacity) {
    return nullptr;
  }

  void *ptr = &a->buffer[a->offset];
  a->offset += size;
  return ptr;
}

void arena_reset(Arena *a) { a->offset = 0; }

void arena_print(const Arena *a) {
  printf("\n=================== ARENA STATE ===================\n");
  printf("Full capacity: %zu Bytes\n", a->capacity);
  printf("Current use  : %zu Bytes (%.2f%%)\n", a->offset,
         ((double)a->offset / a->capacity) * 100);
  printf("Free space   : %zu Bytes\n", a->capacity - a->offset);

  // Visual occupancy bar
  printf("Usage Map     : [");
  size_t bar_width = 40;
  size_t occupied_bars = (a->offset * bar_width) / a->capacity;
  for (size_t i = 0; i < bar_width; i++) {
    if (i < occupied_bars)
      printf("#"); // Memory occupied
    else
      printf("."); // Free memory
  }
  printf("]\n");

  // 2. Dump of occupied memory (in 16-byte blocks)
  printf("\n--- MEMORY DUMP (Busy) ---\n");
  if (a->offset == 0) {
    printf("(Empty Arena)\n");
  } else {
    // Align the dump to show multiples of 16 bytes that cover the offset.
    size_t bytes_to_show = ((a->offset + 15) / 16) * 16;
    if (bytes_to_show > a->capacity)
      bytes_to_show = a->capacity;

    for (size_t i = 0; i < bytes_to_show; i += 16) {
      // Displays the relative address (offset) in hexadecimal.
      printf("  0x%04zX: ", i);

      // Displays the bytes in hexadecimal format.
      for (size_t j = 0; j < 16; j++) {
        if (i + j < a->offset) {
          printf("%02X ", a->buffer[i + j]);
        } else if (i + j < bytes_to_show) {
          printf(".. "); // Space allocated by the dump alignment, but free in
                         // the arena.
        } else {
          printf("   ");
        }
      }
      printf(" | ");

      // Displays the bytes in readable character format (ASCII).
      for (size_t j = 0; j < 16; j++) {
        if (i + j < a->offset) {
          uint8_t ch = a->buffer[i + j];
          // If it's a printable character, it displays the character;
          // otherwise, it displays a period.
          printf("%c", isprint(ch) ? ch : '.');
        } else {
          printf(" ");
        }
      }
      printf("\n");
    }
  }
  printf("===================================================\n\n");
}

int main(void) {
  Arena runtime_arena = arena_create(64); // bytes

  printf("--- Initial state ---");
  arena_print(&runtime_arena);

  const char *content = "Hello Arena!"; // literal
  size_t len_content = strlen(content) + 1; // 13 bytes

  char *txt = arena_alloc(&runtime_arena, sizeof(char) * len_content);
  if (txt == nullptr) {
      perror("Error: Failed to allocate memory.");
      return 1;
  }

  // memcpy(txt, content, len_content);
  snprintf(txt, len_content, "%s", content);
  arena_print(&runtime_arena);

  int *list = arena_alloc(&runtime_arena, sizeof(int) * 10); // 40 bytes
  if (txt == nullptr) {
      perror("Error: Failed to allocate memory.");
      return 1;
  }

  list[0] = 65; // 'A' in ASCII
  list[2] =
      0xDEADBEEF; // The standard hexadecimal code commonly used in debugging.
  list[9] = 66;   // 'B' in ASCII
  arena_print(&runtime_arena);

  printf("--- After reset ---");
  arena_reset(&runtime_arena);
  arena_print(&runtime_arena);

  free(runtime_arena.buffer);

  return 0;
}
