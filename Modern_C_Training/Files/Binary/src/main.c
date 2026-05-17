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

#define BINARY_FILE "../resources/record.bin"

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
  if (!record_save(original)) {
    return EXIT_FAILURE;
  }
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
                          uint8_t str_size) {
  FlexText *ft = malloc(sizeof(FlexText) + str_size);
  if (ft == nullptr)
    return nullptr;

  ft->marker = marker;
  ft->label = label;
  ft->str_size = str_size;
  memcpy(ft->str_array, str_array, str_size);

  return ft;
}

void flexText_print(const FlexText *ft) {
  printf("FlexText {\n"
         " marker      = '%c'\n"
         " label       = '%s'\n"
         " string size = %u byte(s)\n"
         " string      = \"%.*s\"\n"
         "}\n",
         ft->marker, ft->label ? ft->label : "(null)", ft->str_size,
         (int)ft->str_size, ft->str_array);
}

void numPack_print(const NumPack *np) {
  printf("NumericPack {\n"
         " u8 = %u\n"
         " u16= %u\n"
         " u32= 0x%08X  (%u)\n"
         " u64= 0x%016llX  (%llu)\n"
         " f32= %.6f\n"
         " f64= %.15f\n"
         "}\n",
         np->u8, np->u16, np->u32, np->u32, (unsigned long long)np->u64,
         (unsigned long long)np->u64, (double)np->f32, np->f64);
}

void record_print(const char *label, const Record *rec) {
  printf("=== %s ===\n", label);
  flexText_print(rec->text);
  numPack_print(&rec->numbers);
  puts("");
}

/* ======================================================================
 * Binary file layout (fields written in this exact order):
 *
 *  ┌─────────────────────────────────────────────────────────────────┐
 *  │ [1 byte ] marker    — the FlexText marker char                  │
 *  │ [2 bytes] label_len — length of `label` string (without '\0')   │
 *  │ [N bytes] label     — the string characters, no null terminator │
 *  │ [1 byte ] str_size  — length of the FAM payload                 │
 *  │ [M bytes] str_array — the FAM content                           │
 *  │ [sizeof(NumPack)]   — flat dump of the whole NumericPack struct │
 *  └─────────────────────────────────────────────────────────────────┘
 * ====================================================================== */
bool record_save(Record rc) {

  FILE *file = fopen(BINARY_FILE, "wb");
  if (file == nullptr) {
    perror("save: fopen");
    return false;
  }

  FlexText *ft = rc.text;

  // Write marker (1 byte)
  if (fwrite(&ft->marker, 1, 1, file) != 1) {
    perror("save: fwrite");
    fclose(file);
    return false;
  }

  // Write label: length first (2 bytes), then the characters
  uint16_t label_len = ft->label ? (uint16_t)strlen(ft->label) : 0;
  if (fwrite(&label_len, sizeof(label_len), 1, file) != 1) {
    perror("save: fwrite");
    fclose(file);
    return false;
  }
  if (label_len && fwrite(ft->label, 1, label_len, file) != label_len) {
    perror("save: fwrite");
    fclose(file);
    return false;
  }

  // Write FM: size first (1 byte), then the bytes
  if (fwrite(&ft->str_size, 1, 1, file) != 1) {
    perror("save: fwrite");
    fclose(file);
    return false;
  }
  if (ft->str_size &&
      fwrite(&ft->str_array, 1, ft->str_size, file) != ft->str_size) {
    perror("save: fwrite");
    fclose(file);
    return false;
  }

  // Write NumPack
  if (fwrite(&rc.numbers, sizeof(NumPack), 1, file) != 1) {
    perror("save: fwrite");
    fclose(file);
    return false;
  }

  fclose(file);
  printf("[save] \"%s\" written successfully (%zu text bytes + %zu numeric "
         "bytes).\n",
         BINARY_FILE,
         (size_t)(1 + sizeof(uint16_t) + label_len + 1 + ft->str_size),
         sizeof(NumPack));

  return true;
}

/* =========================================================================
 * record_load — reads the file field by field using fread()
 *
 * Allocates FlexText dynamically; caller must release with record_free().
 * ========================================================================= */
bool record_load(Record *rc) {

  FILE *file = fopen(BINARY_FILE, "rb");
  if (file == nullptr) {
    perror("load classic: fopen");
    return false;
  }

  // Read marker
  char marker;
  if (fread(&marker, 1, 1, file) != 1) {
    perror("load_classic: fread");
    fclose(file);
    return false;
  }

  // Read label length, then allocate and fill the string
  uint16_t label_len;
  if (fread(&label_len, sizeof label_len, 1, file) != 1) {
    perror("load_classic: fread");
    fclose(file);
    return false;
  }

  char *label = nullptr;
  if (label_len) {
    label = malloc(label_len + 1); // +1 for the null terminator
    if (label == nullptr) {
      perror("load_classic: fread");
      fclose(file);
      return false;
    }
    if (fread(label, 1, label_len, file) != label_len) {
      free(label);
      perror("load_classic: fread");
      fclose(file);
      return false;
    }
    label[label_len] = '\0';
  }

  // Read FAM size, then allocate FlexText with exactly that much extra space
  uint8_t str_size;
  if (fread(&str_size, 1, 1, file) != 1) {
    free(label);
    perror("load_classic: fread");
    fclose(file);
    return false;
  }

  FlexText *ft = malloc(sizeof(FlexText) + str_size);
  if (ft == nullptr) {
    free(label);
    perror("load_classic: fread");
    fclose(file);
    return false;
  }

  ft->marker = marker;
  ft->label = label; // ownership transferred to ft
  ft->str_size = str_size;

  if (str_size && fread(ft->str_array, 1, str_size, file) != str_size) {
    free(label);
    free(ft);
    perror("load_classic: fread");
    fclose(file);
    return false;
  }

  // Read NumericPack as a flat dump
  NumPack numbers;
  if (fread(&numbers, sizeof(NumPack), 1, file) != 1) {
    free(label);
    free(ft);
    perror("load_classic: fread");
    fclose(file);
    return false;
  }

  fclose(file);
  rc->text = ft;
  rc->numbers = numbers;
  printf("[record_load] OK — field-by-field read via fread().\n");
  return true;
}

/* =============================================================================
 * record_load_embed — deserializes bytes baked into the executable at compile
 * time
 *
 * The #embed directive (C23) expands the binary file into a list of integer
 * literal constants directly in the compiled source. The result is a static
 * `unsigned char[]` array holding the exact bytes of the file.
 *
 * __has_embed() lets the preprocessor check whether the file exists before
 * trying to include it — preventing a compile error on the first build.
 *
 * Key point: because the data lives inside the executable, there is no
 * fopen() at runtime. "Reading" is simply traversing a static array.
 * ==============================================================================
 */
bool record_load_embed(Record *rc) {
#if __has_embed(BINARY_FILE)

  static const unsigned char baked_bytes[] = {
#embed BINARY_FILE
  };
  const size_t baked_size = sizeof baked_bytes;

  printf("[record_load_embed] %zu bytes baked in via #embed (compile-time).\n",
         baked_size);

  // Read cursor over the static array
  const unsigned char *cursor = baked_bytes;
  const unsigned char *end = baked_bytes + baked_size;

// Local macro: advance cursor by n bytes, abort if that would exceed end
#define ADVANCE(n)                                                             \
  if (cursor + (n) > end) {                                                    \
    fprintf(stderr, "[load_embed] not enough data at offset %td\n",            \
            cursor - baked_bytes);                                             \
    return false;                                                              \
  }

  // Read marker (1 byte)
  ADVANCE(1);
  char marker = (char)*cursor++;

  // Read label length (2 bytes) + label bytes
  ADVANCE(sizeof(uint16_t));
  uint16_t label_len;
  memcpy(&label_len, cursor, sizeof label_len);
  cursor += sizeof label_len;

  ADVANCE(label_len);
  char *label = nullptr;
  if (label_len) {
    label = malloc(label_len + 1);
    if (label == nullptr)
      return false;
    memcpy(label, cursor, label_len);
    label[label_len] = '\0';
  }
  cursor += label_len;

  // Read FAM size + allocate FlexText
  ADVANCE(1);
  uint8_t str_size = *cursor++;

  FlexText *ft = malloc(sizeof(FlexText) + str_size);
  if (ft == nullptr) {
    free(label);
    return false;
  }
  ft->marker = marker;
  ft->label = label;
  ft->str_size = str_size;

  ADVANCE(str_size);
  if (str_size)
    memcpy(ft->str_array, cursor, str_size);
  cursor += str_size;

  // Read NumericPack
  ADVANCE(sizeof(NumPack));
  NumPack numbers;
  memcpy(&numbers, cursor, sizeof(NumPack));

#undef ADVANCE

  rc->text = ft;
  rc->numbers = numbers;
  printf("[record_load_embed] OK — read from static array (no fopen).\n");
  return true;

#else
  // The file did not exist when the compiler processed this source.
  // Run the current executable once so save() creates "record.bin",
  // then recompile — #embed will find and bake it in.
  fprintf(stderr,
          "[record_load_embed] \"%s\" not found at compile time.\n"
          "  Step 1: run the current executable  ->  save() creates the file.\n"
          "  Step 2: recompile                   ->  #embed will pick it up.\n",
          BINARY_FILE);
  (void)rc;
  return false;
#endif
}

/* =========================================================================
 * record_free — releases memory allocated by load_classic() or load_embed()
 * ========================================================================= */
void record_free(Record *rec) {
  if (!rec->text)
    return;
  free((void *)rec->text->label); // string allocated separately
  free(rec->text);                // FlexText struct + FAM bytes
  rec->text = nullptr;
}
