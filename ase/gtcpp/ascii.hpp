
#ifndef GTCPP_ASCII_HPP
#define GTCPP_ASCII_HPP

#include <gtcpp/string_buffer.hpp>
#include <string>

namespace gtcpp {

void to_lower(string_buffer& buf, const char *start, const char *finish);
void to_upper(string_buffer& buf, const char *start, const char *finish);
std::string to_lower(const std::string& str);
std::string to_upper(const std::string& str);
std::string drop_nonalnum(const std::string& str);
void fold_space(string_buffer& buf, const char *start, const char *finish);

};

#endif

