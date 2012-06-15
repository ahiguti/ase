
#ifndef GTCPP_ESCAPE_HEX_HPP
#define GTCPP_ESCAPE_HEX_HPP

#include <gtcpp/string_buffer.hpp>

namespace gtcpp {

void escape_hexadecimal_control(string_buffer& buf, const char *start,
  const char *finish);

};

#endif

