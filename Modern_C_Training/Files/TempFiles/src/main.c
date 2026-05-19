#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int write_temp_file(const char *buffer, size_t *length);
const char *read_temp_file(int temp_file, size_t *length);

int main(int argc, char *argv[]) {

  printf("Program name: '%s'\n", argv[0]);
  printf("Arguments:\n");
  printf("  Quantity: %d\n", argc - 1);

  size_t length = 0;
  if (argc > 1) {
    printf("  Values  : ");
    for (int i = 1; i < argc; ++i) {
      printf("%s", argv[i]);
      printf("%s", i < argc - 1 ? ", " : "");
      length += strlen(argv[i]) + 1; // ' ' or '\0'
    }
    puts("");
  }

  if (length == 0) {
    printf("Nothing to do.\n");
    return EXIT_SUCCESS;
  }

  char *buffer = (char *)malloc(length);
  if (buffer == nullptr) {
    printf("Error on allocation!\n");
    return EXIT_FAILURE;
  }

  buffer[0] = '\0';
  for (int i = 1; i < argc; ++i) {
    strcat(buffer, argv[i]);
    if (i < argc - 1) {
      strcat(buffer, " ");
    }
  }

  // fd (File Descriptor)
  auto fd = write_temp_file(buffer, &length);
  if (fd == -1) {
    printf("Error on writing temporary file!\n");
    free(buffer);
    return EXIT_FAILURE;
  }

  const char *result = read_temp_file(fd, &length);
  if (result == nullptr) {
    printf("Error on reading temporary file!\n");
    free(buffer);
    return EXIT_FAILURE;
  }

  printf("Result from file: %s\n", result);

  free(buffer);
  free((void *)result);

  return EXIT_SUCCESS;
}

int write_temp_file(const char *buffer, size_t *length) {

  if (buffer == nullptr || buffer[0] == '\0') {
    printf("Invalid entry!\n");
    return -1;
  }

  char template[] = "./temporary_file.XXXXXX";
  int fd = mkstemp(template);
  if (fd == -1) {
    perror("mkstemp");
    return -1;
  }

  printf("Temporary file created: %s\n", template);

  // The file is deleted from the disk, but the file system remains valid until
  // we close it.
  unlink(template);

  *length = strlen(buffer);
  if (write(fd, buffer, *length) == -1) {
    perror("write");
    close(fd);
    return -1;
  }

  return fd;
}

const char *read_temp_file(int temp_file, size_t *length) {
  if (temp_file < 0 || length == nullptr || *length == 0)
    return "";

  int fd = temp_file;

  // Repositions the file pointer to the beginning.
  lseek(fd, 0, SEEK_SET);

  // Allocates extra space (+1) for the terminator character '\0'.
  char *buffer = (char *)malloc(*length + 1);
  if (buffer == nullptr) {
    close(fd);
    return nullptr;
  }

  ssize_t bytes_read = read(fd, buffer, *length);
  if (bytes_read == -1) {
    perror("read");
    free(buffer);
    close(fd);
    return nullptr;
  }

  // Ensures the string is properly terminated.
  buffer[bytes_read] = '\0';

  close(fd);

  return buffer;
}
