
#ifndef GTCPP_QUOTEDPR_HPP
#define GTCPP_QUOTEDPR_HPP

#include <gtcpp/string_buffer.hpp>

namespace gtcpp {

size_t quoted_printable_decode(const char *start, const char *finish,
  string_buffer& buf_append);
  /* returns the number of ignored bytes */

};

#endif

