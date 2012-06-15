
#include <gtcpp/unistd.hpp>
#include <gtcpp/auto_file.hpp>
#include <unistd.h>
#include <errno.h>

namespace gtcpp {

std::string
gethostname()
{
  char buf[1025];
  const int r = ::gethostname(buf, sizeof(buf));
  if (r < 0) {
    posix_error_callback ec;
    ec.on_posix_error(errno, __PRETTY_FUNCTION__);
    return std::string(); /* not reached */
  }
  return std::string(buf);
}

int
stat(const char *filename, struct stat *sbuf, posix_error_callback& ec)
{
  const int r = ::stat(filename, sbuf);
  GTCPP_CHECK_POSIX_ERROR_NEGATIVE(r, ec);
}

pid_t
fork(posix_error_callback& ec)
{
  const pid_t r = ::fork();
  GTCPP_CHECK_POSIX_ERROR_NEGATIVE(r, ec);
}

int
setpgid(pid_t pid, pid_t pgid, posix_error_callback& ec)
{
  const int r = ::setpgid(pid, pgid);
  GTCPP_CHECK_POSIX_ERROR_NEGATIVE(r, ec);
}

pid_t
getpgid(pid_t pid, posix_error_callback& ec)
{
  const pid_t r = ::getpgid(pid);
  GTCPP_CHECK_POSIX_ERROR_NEGATIVE(r, ec);
}

};

