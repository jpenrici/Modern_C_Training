/*
 * main.c — Word Counter via Embedded Perl
 * Usage: ./word_counter <path/to/file.txt> [path/to/counter.pl]
 */

#define PERL_NO_GET_CONTEXT
#include <EXTERN.h> // EXTCONST and EXT
#include <perl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INJECT_LEN 1024
#define MAX_CMD_LEN 768

static PerlInterpreter *my_perl = nullptr;

/*
 * embed_run_counter
 *
 * Sets @ARGV = (file_path) in Perl, then runs the counter script.
 * Reads back $main::output and returns it as a heap string (caller frees).
 * Returns nullptr on error.
 */
[[nodiscard]] static char *embed_run_counter(const char *script_path,
                                             const char *file_path) {
  // Pass file_path to Perl via @ARGV
  char inject[MAX_INJECT_LEN];
  int n = snprintf(inject, sizeof inject, "@ARGV = ('%s'); $main::output = '';",
                   file_path);

  if (n < 0 || (size_t)n >= sizeof inject) {
    fprintf(stderr, "Error: file path too long.\n");
    return nullptr;
  }

  eval_pv(inject, TRUE);

  if (SvTRUE(ERRSV)) {
    fprintf(stderr, "Perl injection error: %s\n", SvPV_nolen(ERRSV));
    return nullptr;
  }

  // Run the counter script
  char cmd[MAX_CMD_LEN];
  n = snprintf(cmd, sizeof cmd, "do '%s'; die $@ if $@;", script_path);

  if (n < 0 || (size_t)n >= sizeof cmd) {
    fprintf(stderr, "Error: script path too long.\n");
    return nullptr;
  }

  eval_pv(cmd, TRUE);

  if (SvTRUE(ERRSV)) {
    fprintf(stderr, "Perl runtime error:\n%s\n", SvPV_nolen(ERRSV));
    return nullptr;
  }

  // Read $main::output back into C
  SV *output_sv = get_sv("main::output", 0);

  if (!output_sv || !SvOK(output_sv)) {
    fprintf(stderr, "Error: $main::output was not set by the Perl script.\n");
    return nullptr;
  }

  STRLEN len;
  const char *result = SvPV(output_sv, len);

  char *copy = (char *)malloc(len + 1);
  if (!copy) {
    fprintf(stderr, "Error: out of memory.\n");
    return nullptr;
  }

  memcpy(copy, result, len + 1);

  return copy;
}

/*
 * resolve_script_path
 *
 * Returns a pointer to a static buffer containing the script path
 * prefixed with "./" if no directory separator is present, so that
 * Perl's `do` can find it without @INC lookup.
 */
static const char *resolve_script_path(const char *path) {
  static char resolved[MAX_CMD_LEN];

  if (path[0] == '/' || (path[0] == '.' && path[1] == '/')) {
    return path;
  }

  int n = snprintf(resolved, sizeof resolved, "./%s", path);
  if (n < 0 || (size_t)n >= sizeof resolved) {
    return path; /* fallback */
  }

  return resolved;
}

int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "Usage  : %s <text-file> [counter.pl]\n", argv[0]);
    fprintf(stderr, "Example: %s sample.txt\n", argv[0]);
    return EXIT_FAILURE;
  }

  const char *file_path = argv[1];
  const char *script_path =
      resolve_script_path((argc >= 3) ? argv[2] : "perl/counter.pl");

  printf("=== C23 + libperl Word Counter ===\n");
  printf("Script : %s\n", script_path);
  printf("Target : %s\n\n", file_path);

  // Bootstrap the Perl interpreter
  PERL_SYS_INIT3(&argc, &argv, nullptr);

  my_perl = perl_alloc();
  if (!my_perl) {
    fprintf(stderr, "Fatal: perl_alloc() failed.\n");
    return EXIT_FAILURE;
  }

  perl_construct(my_perl);

  char *perl_argv[] = {(char *)"perl", (char *)"-e", (char *)"0"};
  int perl_argc = 3;

  if (perl_parse(my_perl, nullptr, perl_argc, perl_argv, nullptr) != 0) {
    fprintf(stderr, "Fatal: perl_parse() failed.\n");
    perl_free(my_perl);
    PERL_SYS_TERM();
    return EXIT_FAILURE;
  }

  perl_run(my_perl);

  // Delegate all work to Perl
  char *report = embed_run_counter(script_path, file_path);

  // C prints the result
  if (report) {
    puts(report);
    free(report);
  }

  // Clean Perl teardown
  PL_perl_destruct_level = 1;
  perl_destruct(my_perl);
  perl_free(my_perl);
  PERL_SYS_TERM();

  printf("Interpreter shut down cleanly.\n");

  return report ? EXIT_SUCCESS : EXIT_FAILURE;
}
