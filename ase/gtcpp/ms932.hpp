
#ifndef GTCPP_MS932_HPP
#define GTCPP_MS932_HPP

#include <gtcpp/string_buffer.hpp>

namespace gtcpp {

const char *iso2022jp_to_ms932(string_buffer& buf, const char *start,
  const char *finish);

};

#endif

