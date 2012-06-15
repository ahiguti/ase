
#ifndef GTCPP_BASE64_HPP
#define GTCPP_BASE64_HPP

#include <gtcpp/string_buffer.hpp>

namespace gtcpp {

size_t base64_decode(const char *start, const char *finish,
  string_buffer& buf_append);
  /* returns the number of ignored bytes */

};

#endif

