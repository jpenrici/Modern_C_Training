#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  char marker;       // single character
  const char *label; // pointer to a string
  uint8_t str_size;  // FAM capacity
  char str_array[];  // FAM (Flexible Array Member)
} FlexText;

typedef struct {
  uint8_t u8;   // 0 to 255
  uint16_t u16; // 0 to 65535
  uint32_t u32; // 0 to ~4 billion
  uint64_t u64; // 0 to 18 quintillion
  float f32;    // ~7 significant decimal digits
  double f64;   // ~15 significant decimal digits
} NumPack;

typedef struct {
  FlexText *text; // dynamically allocated, becouse of the FAM inside FlexText
  NumPack numbers;
} Record;

#define BINARY_FILE "record.bin"

FlexText *flexText_create(char marker, const char *label, const char *str_array,
                          uint8_t str_size);
void flexText_print(const FlexText *ft);

void numPack_print(const NumPack *np);

void record_print(const char *label, const Record *rec);
bool record_save(Record rc);
bool record_load(Record *rc);
bool record_load_embed(Record *rc);
void record_free(Record *rc);

int main(void) {
  const char sample[] = "META_v1";

  FlexText *text =
      flexText_create('R',          // marker: 'R' for Record
                      "Hello, C23", // label pointed to from outside
                      sample,       // FAM content
                      (uint8_t)sizeof(sample) - 1 // FAM size, excluded '\0'
      );
  if (text == nullptr) {
    fputs("Error: could not allocate FlexText.\n", stderr);
    return EXIT_FAILURE;
  }

  NumPack nums = {.u8 = 255,
                  .u16 = 65535,
                  .u32 = 0xAABBCCDD,
                  .u64 = UINT64_C(0xAAAABBBBCCCCDDDD),
                  .f32 = 0.12345f,
                  .f64 = 0.123456789012345};

  Record original = {.text = text, .numbers = nums};

  if (!record_save(original))
    return EXIT_FAILURE;
  puts("");

  Record from_classic = {0};
  if (record_load(&from_classic)) {
    record_print("Loaded via fread() - classic", &from_classic);
    record_free(&from_classic);
  }

  Record from_embed = {0};
  if (record_load_embed(&from_embed)) {
    record_print("Loaded via #embed - C23", &from_embed);
    record_free(&from_embed);
  }

  return EXIT_SUCCESS;
}

FlexText *flexText_create(char marker, const char *label, const char *str_array,
                          uint8_t str_size) { /* TO DO */  return nullptr; }
void flexText_print(const FlexText *ft) { /* TO DO */ }
void numPack_print(const NumPack *np) { /* TO DO */ }
void record_print(const char *label, const Record *rec) { /* TO DO */ }
bool record_save(Record rc) { /* TO DO */ return false; }
bool record_load(Record *rc) { /* TO DO */ return false; }
bool record_load_embed(Record *rc) { /* TO DO */ return false; }
void record_free(Record *rc) { /* TO DO */ }
