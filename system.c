#include "system.h"
#include "strings.h"


File *
get_stdout_fd()
{
  String *stdout_path = join(join(new_string("/proc/"), int_to_string(getpid())), new_string("/fd/2"));
  File *stdout = fopen(to_c_string(stdout_path), "a");

  return stdout;
}