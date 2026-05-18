/*
 * main.c — C23 struct serialization demo
 *
 * Shows:
 *  - FlexText: struct with a Flexible Array Member (FAM)
 *  - NumPack:  struct with fixed-width integer and float fields
 *  - record_save():        field-by-field serialization via fwrite()
 *  - record_deserialize(): pure parsing over a byte buffer (no I/O)
 *  - record_load():        file → buffer → record_deserialize()
 *  - record_load_embed():  static #embed array → record_deserialize()
 *
 * #embed lifecycle
 * ────────────────
 *  Build 1  → BINARY_FILE absent → __has_embed() disables #embed;
 *             record_load_embed() prints instructions and returns false.
 *  Run 1    → record_save() writes BINARY_FILE to disk.
 *  Build 2  → compiler bakes the file into a static byte array.
 *  Run 2    → record_load_embed() reads the array without any fopen().
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ── Structs ─────────────────────────────────────────────────────────────── */

typedef struct {
  char marker;       /* single classification character   */
  const char *label; /* non-owning pointer to a string    */
  uint8_t str_size;  /* FAM payload length in bytes       */
  char str_array[];  /* FAM — Flexible Array Member       */
} FlexText;

typedef struct {
  uint8_t u8;   /* 0 – 255                          */
  uint16_t u16; /* 0 – 65 535                       */
  uint32_t u32; /* 0 – ~4 billion                   */
  uint64_t u64; /* 0 – 18 quintillion               */
  float f32;    /* ~7 significant decimal digits     */
  double f64;   /* ~15 significant decimal digits    */
} NumPack;

typedef struct {
  FlexText *text; /* heap-allocated (FAM requires dynamic sizing) */
  NumPack numbers;
} Record;

/* ── Constants ───────────────────────────────────────────────────────────── */

#define BINARY_FILE "../resources/record.bin"

/* ── Prototypes ──────────────────────────────────────────────────────────── */

FlexText *flexText_create(char marker, const char *label, const char *str_array,
                          uint8_t str_size);
void flexText_print(const FlexText *ft);
void numPack_print(const NumPack *np);
void record_print(const char *label, const Record *rec);
bool record_save(Record rc);
bool record_deserialize(const unsigned char *buf, size_t buf_size, Record *rc);
bool record_load(Record *rc);
bool record_load_embed(Record *rc);
void record_free(Record *rc);

/* ── main ────────────────────────────────────────────────────────────────── */

int main(void) {
  const char sample[] = "META_v1";

  FlexText *text =
      flexText_create('R', "Hello, C23", sample,
                      (uint8_t)(sizeof(sample) - 1) /* exclude '\0' */
      );
  if (text == nullptr) {
    fputs("Error: FlexText allocation failed.\n", stderr);
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
    free(text);
    return EXIT_FAILURE;
  }
  puts("");

  Record from_classic = {0};
  if (record_load(&from_classic)) {
    record_print("Loaded via fread()", &from_classic);
    record_free(&from_classic);
  }

  Record from_embed = {0};
  if (record_load_embed(&from_embed)) {
    record_print("Loaded via #embed (C23)", &from_embed);
    record_free(&from_embed);
  }

  free(text);
  return EXIT_SUCCESS;
}

/* ── flexText_create ─────────────────────────────────────────────────────── */
/*
 * Allocates a FlexText with a FAM sized to the actual content length.
 * strnlen caps the copy at str_size, preventing reads past the source buffer.
 */
FlexText *flexText_create(char marker, const char *label, const char *str_array,
                          uint8_t str_size) {

  uint8_t safe_size = 0;
  while (safe_size < str_size && str_array[safe_size] != '\0')
    safe_size++;

  FlexText *ft = malloc(sizeof(FlexText) + safe_size);
  if (ft == nullptr)
    return nullptr;

  if (str_array == nullptr || str_array[0] == '\0')
    return nullptr;

  if (label != nullptr && label[0] == '\0')
    label = nullptr;

  ft->marker = marker;
  ft->label = label;
  ft->str_size = safe_size;
  memcpy(ft->str_array, str_array, safe_size);

  return ft;
}

/* ── flexText_print ──────────────────────────────────────────────────────── */

void flexText_print(const FlexText *ft) {
  printf("FlexText {\n"
         "  marker      = '%c'\n"
         "  label       = '%s'\n"
         "  string size = %u byte(s)\n"
         "  string      = \"%.*s\"\n"
         "}\n",
         ft->marker, ft->label ? ft->label : "(null)", ft->str_size,
         (int)ft->str_size, ft->str_array);
}

/* ── numPack_print ───────────────────────────────────────────────────────── */

void numPack_print(const NumPack *np) {
  printf("NumericPack {\n"
         "  u8  = %u\n"
         "  u16 = %u\n"
         "  u32 = 0x%08X  (%u)\n"
         "  u64 = 0x%016llX  (%llu)\n"
         "  f32 = %.6f\n"
         "  f64 = %.15f\n"
         "}\n",
         np->u8, np->u16, np->u32, np->u32, (unsigned long long)np->u64,
         (unsigned long long)np->u64, (double)np->f32, np->f64);
}

/* ── record_print ────────────────────────────────────────────────────────── */

void record_print(const char *label, const Record *rec) {
  printf("=== %s ===\n", label);
  flexText_print(rec->text);
  numPack_print(&rec->numbers);
  puts("");
}

/* ── record_save ─────────────────────────────────────────────────────────── */
/*
 * Binary file layout (fields in this exact order):
 *
 *  ┌──────────────────────────────────────────────────────────────────┐
 *  │ [1 byte ] marker    — FlexText marker char                       │
 *  │ [2 bytes] label_len — label string length (no null terminator)   │
 *  │ [N bytes] label     — label characters, no null terminator       │
 *  │ [1 byte ] str_size  — FAM payload length                         │
 *  │ [M bytes] str_array — FAM content                                │
 *  │ [sizeof(NumPack)]   — flat dump of the NumPack struct            │
 *  └──────────────────────────────────────────────────────────────────┘
 */
bool record_save(Record rc) {
  FILE *file = fopen(BINARY_FILE, "wb");
  if (file == nullptr) {
    perror("save: fopen");
    return false;
  }

  FlexText *ft = rc.text;

  /* marker (1 byte) */
  if (fwrite(&ft->marker, 1, 1, file) != 1)
    goto write_error;

  /* label: length (2 bytes) + characters */
  uint16_t label_len = ft->label ? (uint16_t)strlen(ft->label) : 0;
  if (fwrite(&label_len, sizeof(label_len), 1, file) != 1)
    goto write_error;
  if (label_len && fwrite(ft->label, 1, label_len, file) != label_len)
    goto write_error;

  /* FAM: size (1 byte) + payload bytes */
  if (fwrite(&ft->str_size, 1, 1, file) != 1)
    goto write_error;
  if (ft->str_size &&
      fwrite(ft->str_array, 1, ft->str_size, file) != ft->str_size)
    goto write_error;

  /* NumPack: flat struct dump */
  if (fwrite(&rc.numbers, sizeof(NumPack), 1, file) != 1)
    goto write_error;

  fclose(file);
  printf("[save] \"%s\" written (%zu text bytes + %zu numeric bytes).\n",
         BINARY_FILE,
         (size_t)(1 + sizeof(uint16_t) + label_len + 1 + ft->str_size),
         sizeof(NumPack));
  return true;

write_error:
  perror("save: fwrite");
  fclose(file);
  return false;
}

/* ── record_deserialize ──────────────────────────────────────────────────── */
/*
 * Pure parsing over [buf, buf+buf_size). No I/O — works with any byte source.
 * Allocates FlexText on the heap; caller must release with record_free().
 * Returns false on truncated data or allocation failure; no resources are
 * leaked.
 *
 * ADVANCE(n): bounds-checks n bytes before the cursor moves. On failure it
 * jumps to `cleanup`, which frees any partially allocated resources. Note that
 * ownership of `label` transfers to `ft` only after both are allocated, so
 * freeing them independently at cleanup is always safe.
 */
bool record_deserialize(const unsigned char *buf, size_t buf_size, Record *rc) {
  const unsigned char *cursor = buf;
  const unsigned char *end = buf + buf_size;

  char *label = nullptr;
  FlexText *ft = nullptr;

#define ADVANCE(n)                                                             \
  do {                                                                         \
    if (cursor + (size_t)(n) > end) {                                          \
      fprintf(stderr, "[deserialize] truncated at offset %td (needed %zu)\n",  \
              cursor - buf, (size_t)(n));                                      \
      goto cleanup;                                                            \
    }                                                                          \
  } while (0)

  /* marker (1 byte) */
  ADVANCE(1);
  char marker = (char)*cursor++;

  /* label: length (2 bytes) + characters */
  ADVANCE(sizeof(uint16_t));
  uint16_t label_len;
  memcpy(&label_len, cursor, sizeof(label_len));
  cursor += sizeof(label_len);

  if (label_len) {
    ADVANCE(label_len);
    label = malloc(label_len + 1);
    if (label == nullptr)
      goto cleanup;
    memcpy(label, cursor, label_len);
    label[label_len] = '\0';
  }
  cursor += label_len;

  /* FAM: size (1 byte) + payload bytes */
  ADVANCE(1);
  uint8_t str_size = *cursor++;

  ft = malloc(sizeof(FlexText) + str_size);
  if (ft == nullptr)
    goto cleanup;

  ft->marker = marker;
  ft->label = label; /* ownership of label transfers to ft */
  ft->str_size = str_size;

  if (str_size) {
    ADVANCE(str_size);
    memcpy(ft->str_array, cursor, str_size);
  }
  cursor += str_size;

  /* NumPack: flat struct dump */
  ADVANCE(sizeof(NumPack));
  NumPack numbers;
  memcpy(&numbers, cursor, sizeof(NumPack));

#undef ADVANCE

  rc->text = ft;
  rc->numbers = numbers;
  return true;

cleanup:
  free(ft);
  free(label);
  return false;
}

/* ── record_load ─────────────────────────────────────────────────────────── */
/*
 * Byte source: reads BINARY_FILE into a temporary heap buffer, then delegates
 * all parsing to record_deserialize(). Caller must release with record_free().
 */
bool record_load(Record *rc) {
  FILE *file = fopen(BINARY_FILE, "rb");
  if (file == nullptr) {
    perror("load: fopen");
    return false;
  }

  if (fseek(file, 0, SEEK_END) != 0) {
    perror("load: fseek");
    fclose(file);
    return false;
  }
  long file_size = ftell(file);
  if (file_size < 0) {
    perror("load: ftell");
    fclose(file);
    return false;
  }
  rewind(file);

  unsigned char *buf = malloc((size_t)file_size);
  if (buf == nullptr) {
    perror("load: malloc");
    fclose(file);
    return false;
  }
  if (fread(buf, 1, (size_t)file_size, file) != (size_t)file_size) {
    perror("load: fread");
    free(buf);
    fclose(file);
    return false;
  }
  fclose(file);

  bool ok = record_deserialize(buf, (size_t)file_size, rc);
  free(buf);

  if (ok)
    printf("[record_load] OK — %ld bytes read via fread().\n", file_size);
  return ok;
}

/* ── record_load_embed ───────────────────────────────────────────────────── */
/*
 * Byte source: points record_deserialize() at the static array baked in by
 * #embed (C23) — no fopen() at runtime.
 *
 * __has_embed() checks at preprocessing time whether BINARY_FILE exists.
 * If it does, the compiler expands #embed into integer constants that
 * initialize baked_bytes[] inside the executable.
 *
 * See the #embed lifecycle note at the top of this file.
 */
bool record_load_embed(Record *rc) {
#if __has_embed(BINARY_FILE)

  static const unsigned char baked_bytes[] = {
#embed BINARY_FILE
  };

  const size_t baked_size = sizeof(baked_bytes);
  printf("[record_load_embed] %zu bytes baked in at compile time.\n",
         baked_size);

  bool ok = record_deserialize(baked_bytes, baked_size, rc);

  if (ok)
    printf("[record_load_embed] OK — parsed from static array.\n");
  return ok;

#else
  /* BINARY_FILE was absent when this translation unit was compiled.
   * Step 1: run the current executable  →  record_save() creates the file.
   * Step 2: recompile                   →  #embed will bake it in.        */
  fprintf(stderr,
          "[record_load_embed] \"%s\" not found at compile time.\n"
          "  Step 1: run this executable  ->  record_save() creates the file.\n"
          "  Step 2: recompile            ->  #embed will pick it up.\n",
          BINARY_FILE);
  (void)rc;
  return false;
#endif
}

/* ── record_free ─────────────────────────────────────────────────────────── */
/* Releases memory allocated by record_load() or record_load_embed(). */
void record_free(Record *rec) {
  if (!rec->text)
    return;
  free((void *)rec->text->label); /* separately allocated string */
  free(rec->text);                /* FlexText header + FAM bytes */
  rec->text = nullptr;
}
