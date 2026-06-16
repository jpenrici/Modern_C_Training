// Reference: https://perldoc.perl.org/perlembed
// src/main.c

#define PERL_NO_GET_CONTEXT
#include <EXTERN.h> // EXTCONST and EXT
#include <perl.h>

static PerlInterpreter *my_perl = nullptr;

int main(int argc, char **argv) {

  char *perl_args[] = {"", "perl/showtime.pl", nullptr};
  int perl_argc = 2;

  char *args[] = {nullptr};
  PERL_SYS_INIT3(&argc, &argv, nullptr);

  my_perl = perl_alloc();
  if (!my_perl) {
    perror("Fatal: perl_alloc() failed!");
    perl_free(my_perl);
    PERL_SYS_TERM();
    return EXIT_FAILURE;
  }

  perl_construct(my_perl);

  if (perl_parse(my_perl, nullptr, perl_argc, perl_args, nullptr) != 0) {
    perror("Fatal: perl_parse() failed!");
    perl_free(my_perl);
    PERL_SYS_TERM();
    return EXIT_FAILURE;
  }

  perl_run(my_perl);

  PL_exit_flags |= PERL_EXIT_DESTRUCT_END;

  call_argv("showtime", G_DISCARD | G_NOARGS, args);

  perl_destruct(my_perl);
  perl_free(my_perl);
  PERL_SYS_TERM();
}
