
#ifndef GTCPP_HTML_HPP
#define GTCPP_HTML_HPP

#include <gtcpp/string_buffer.hpp>
#include <gtcpp/string_ref.hpp>

namespace gtcpp {

int html_get_entity_code(const char *name, int namelen);
void html_decode_entities(string_buffer& obuf, const char *start,
  const char *finish);
bool html_find_meta_charset(string_buffer& out, const char *start,
  const char *finish);

struct html_to_plain_options {
  bool show_urls;
  bool hide_linked_urls; /* <a href='http://REAL_URL'>http://HIDE_THIS</a> */
  bool fold_space;
  html_to_plain_options()
    : show_urls(false), hide_linked_urls(false), fold_space(false) { }
};

void html_to_plain(string_buffer& out, const char *start, const char *finish,
  const html_to_plain_options& opts);

};

#endif

