

#include <gtcpp/html.hpp>
#include <gtcpp/algorithm.hpp>
#include <gtcpp/string_util.hpp>
#include <gtcpp/rfc822.hpp>
#include <gtcpp/aseutfconv.hpp>
#include <gtcpp/ascii.hpp>
#include <string>
#include <iostream>

#define DBG_HTML(x)

namespace gtcpp {

namespace {

struct html_entity {
  const char *name;
  int namelen;
  int code;
};

const html_entity html_entities[] = {
  { "AElig", 5, 198 },
  { "Aacute", 6, 193 },
  { "Acirc", 5, 194 },
  { "Agrave", 6, 192 },
  { "Alpha", 5, 913 },
  { "Aring", 5, 197 },
  { "Atilde", 6, 195 },
  { "Auml", 4, 196 },
  { "Beta", 4, 914 },
  { "Ccedil", 6, 199 },
  { "Chi", 3, 935 },
  { "Dagger", 6, 8225 },
  { "Delta", 5, 916 },
  { "ETH", 3, 208 },
  { "Eacute", 6, 201 },
  { "Ecirc", 5, 202 },
  { "Egrave", 6, 200 },
  { "Epsilon", 7, 917 },
  { "Eta", 3, 919 },
  { "Euml", 4, 203 },
  { "Gamma", 5, 915 },
  { "Iacute", 6, 205 },
  { "Icirc", 5, 206 },
  { "Igrave", 6, 204 },
  { "Iota", 4, 921 },
  { "Iuml", 4, 207 },
  { "Kappa", 5, 922 },
  { "Lambda", 6, 923 },
  { "Mu", 2, 924 },
  { "Ntilde", 6, 209 },
  { "Nu", 2, 925 },
  { "OElig", 5, 338 },
  { "Oacute", 6, 211 },
  { "Ocirc", 5, 212 },
  { "Ograve", 6, 210 },
  { "Omega", 5, 937 },
  { "Omicron", 7, 927 },
  { "Oslash", 6, 216 },
  { "Otilde", 6, 213 },
  { "Ouml", 4, 214 },
  { "Phi", 3, 934 },
  { "Pi", 2, 928 },
  { "Prime", 5, 8243 },
  { "Psi", 3, 936 },
  { "Rho", 3, 929 },
  { "Scaron", 6, 352 },
  { "Sigma", 5, 931 },
  { "THORN", 5, 222 },
  { "Tau", 3, 932 },
  { "Theta", 5, 920 },
  { "Uacute", 6, 218 },
  { "Ucirc", 5, 219 },
  { "Ugrave", 6, 217 },
  { "Upsilon", 7, 933 },
  { "Uuml", 4, 220 },
  { "Xi", 2, 926 },
  { "Yacute", 6, 221 },
  { "Yuml", 4, 376 },
  { "Zeta", 4, 918 },
  { "aacute", 6, 225 },
  { "acirc", 5, 226 },
  { "acute", 5, 180 },
  { "aelig", 5, 230 },
  { "agrave", 6, 224 },
  { "alefsym", 7, 8501 },
  { "alpha", 5, 945 },
  { "amp", 3, 38 },
  { "and", 3, 8743 },
  { "ang", 3, 8736 },
  { "aring", 5, 229 },
  { "asymp", 5, 8776 },
  { "atilde", 6, 227 },
  { "auml", 4, 228 },
  { "bdquo", 5, 8222 },
  { "beta", 4, 946 },
  { "brvbar", 6, 166 },
  { "bull", 4, 8226 },
  { "cap", 3, 8745 },
  { "ccedil", 6, 231 },
  { "cedil", 5, 184 },
  { "cent", 4, 162 },
  { "chi", 3, 967 },
  { "circ", 4, 710 },
  { "clubs", 5, 9827 },
  { "cong", 4, 8773 },
  { "copy", 4, 169 },
  { "crarr", 5, 8629 },
  { "cup", 3, 8746 },
  { "curren", 6, 164 },
  { "dArr", 4, 8659 },
  { "dagger", 6, 8224 },
  { "darr", 4, 8595 },
  { "deg", 3, 176 },
  { "delta", 5, 948 },
  { "diams", 5, 9830 },
  { "divide", 6, 247 },
  { "eacute", 6, 233 },
  { "ecirc", 5, 234 },
  { "egrave", 6, 232 },
  { "empty", 5, 8709 },
  { "emsp", 4, 8195 },
  { "ensp", 4, 8194 },
  { "epsilon", 7, 949 },
  { "equiv", 5, 8801 },
  { "eta", 3, 951 },
  { "eth", 3, 240 },
  { "euml", 4, 235 },
  { "euro", 4, 8364 },
  { "exist", 5, 8707 },
  { "fnof", 4, 402 },
  { "forall", 6, 8704 },
  { "frac12", 6, 189 },
  { "frac14", 6, 188 },
  { "frac34", 6, 190 },
  { "frasl", 5, 8260 },
  { "gamma", 5, 947 },
  { "ge", 2, 8805 },
  { "gt", 2, 62 },
  { "hArr", 4, 8660 },
  { "harr", 4, 8596 },
  { "hearts", 6, 9829 },
  { "hellip", 6, 8230 },
  { "iacute", 6, 237 },
  { "icirc", 5, 238 },
  { "iexcl", 5, 161 },
  { "igrave", 6, 236 },
  { "image", 5, 8465 },
  { "infin", 5, 8734 },
  { "int", 3, 8747 },
  { "iota", 4, 953 },
  { "iquest", 6, 191 },
  { "isin", 4, 8712 },
  { "iuml", 4, 239 },
  { "kappa", 5, 954 },
  { "lArr", 4, 8656 },
  { "lambda", 6, 955 },
  { "lang", 4, 9001 },
  { "laquo", 5, 171 },
  { "larr", 4, 8592 },
  { "lceil", 5, 8968 },
  { "ldquo", 5, 8220 },
  { "le", 2, 8804 },
  { "lfloor", 6, 8970 },
  { "lowast", 6, 8727 },
  { "loz", 3, 9674 },
  { "lrm", 3, 8206 },
  { "lsaquo", 6, 8249 },
  { "lsquo", 5, 8216 },
  { "lt", 2, 60 },
  { "macr", 4, 175 },
  { "mdash", 5, 8212 },
  { "micro", 5, 181 },
  { "middot", 6, 183 },
  { "minus", 5, 8722 },
  { "mu", 2, 956 },
  { "nabla", 5, 8711 },
  { "nbsp", 4, 160 },
  { "ndash", 5, 8211 },
  { "ne", 2, 8800 },
  { "ni", 2, 8715 },
  { "not", 3, 172 },
  { "notin", 5, 8713 },
  { "nsub", 4, 8836 },
  { "ntilde", 6, 241 },
  { "nu", 2, 957 },
  { "oacute", 6, 243 },
  { "ocirc", 5, 244 },
  { "oelig", 5, 339 },
  { "ograve", 6, 242 },
  { "oline", 5, 8254 },
  { "omega", 5, 969 },
  { "omicron", 7, 959 },
  { "oplus", 5, 8853 },
  { "or", 2, 8744 },
  { "ordf", 4, 170 },
  { "ordm", 4, 186 },
  { "oslash", 6, 248 },
  { "otilde", 6, 245 },
  { "otimes", 6, 8855 },
  { "ouml", 4, 246 },
  { "para", 4, 182 },
  { "part", 4, 8706 },
  { "permil", 6, 8240 },
  { "perp", 4, 8869 },
  { "phi", 3, 966 },
  { "pi", 2, 960 },
  { "piv", 3, 982 },
  { "plusmn", 6, 177 },
  { "pound", 5, 163 },
  { "prime", 5, 8242 },
  { "prod", 4, 8719 },
  { "prop", 4, 8733 },
  { "psi", 3, 968 },
  { "quot", 4, 34 },
  { "rArr", 4, 8658 },
  { "radic", 5, 8730 },
  { "rang", 4, 9002 },
  { "raquo", 5, 187 },
  { "rarr", 4, 8594 },
  { "rceil", 5, 8969 },
  { "rdquo", 5, 8221 },
  { "real", 4, 8476 },
  { "reg", 3, 174 },
  { "rfloor", 6, 8971 },
  { "rho", 3, 961 },
  { "rlm", 3, 8207 },
  { "rsaquo", 6, 8250 },
  { "rsquo", 5, 8217 },
  { "sbquo", 5, 8218 },
  { "scaron", 6, 353 },
  { "sdot", 4, 8901 },
  { "sect", 4, 167 },
  { "shy", 3, 173 },
  { "sigma", 5, 963 },
  { "sigmaf", 6, 962 },
  { "sim", 3, 8764 },
  { "spades", 6, 9824 },
  { "sub", 3, 8834 },
  { "sube", 4, 8838 },
  { "sum", 3, 8721 },
  { "sup", 3, 8835 },
  { "sup1", 4, 185 },
  { "sup2", 4, 178 },
  { "sup3", 4, 179 },
  { "supe", 4, 8839 },
  { "szlig", 5, 223 },
  { "tau", 3, 964 },
  { "there4", 6, 8756 },
  { "theta", 5, 952 },
  { "thetasym", 8, 977 },
  { "thinsp", 6, 8201 },
  { "thorn", 5, 254 },
  { "tilde", 5, 732 },
  { "times", 5, 215 },
  { "trade", 5, 8482 },
  { "uArr", 4, 8657 },
  { "uacute", 6, 250 },
  { "uarr", 4, 8593 },
  { "ucirc", 5, 251 },
  { "ugrave", 6, 249 },
  { "uml", 3, 168 },
  { "upsih", 5, 978 },
  { "upsilon", 7, 965 },
  { "uuml", 4, 252 },
  { "weierp", 6, 8472 },
  { "xi", 2, 958 },
  { "yacute", 6, 253 },
  { "yen", 3, 165 },
  { "yuml", 4, 255 },
  { "zeta", 4, 950 },
  { "zwj", 3, 8205 },
  { "zwnj", 4, 8204 },
};

struct compare_html_entity {
  int operator ()(const html_entity& x, const html_entity& y) const {
    return compare_mem(x.name, x.namelen, y.name, y.namelen);
  }
};

bool
html_is_space_or_ctrl(char c)
{
  return c >= 0 && c <= ' ';
}

bool
html_is_alpha(char c)
{
  return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

bool
html_is_number(char c)
{
  return c >= '0' && c <= '9';
}

bool
html_is_name_char(char c)
{
  return html_is_alpha(c) || html_is_number(c) ||
    c == '-' || c == '_' || c == ',' || c == '.';
}

bool
html_is_special(char c)
{
  switch (c) {
  case '/':
  case '<':
  case '>':
  case '&':
    return true;
  }
  return false;
}

bool
html_is_attrname_char(char c)
{
  /* very permissive */
  return !html_is_space_or_ctrl(c) && !html_is_special(c) && c != '=';
}

bool
html_is_attrvalue_noquote_char(char c)
{
  /* very permissive */
  return !html_is_space_or_ctrl(c) && c != '<' && c != '>';
}

void
html_skip_alnum(const char *& start, const char *finish)
{
  while (start < finish &&
    (html_is_alpha(start[0]) || html_is_number(start[0]))) {
    ++start;
  }
}

void
html_skip_semicolon(const char *& start, const char *finish)
{
  while (start < finish && start[0] == ';') {
    ++start;
  }
}

void
html_skip_space(const char *& start, const char *finish)
{
  while (start < finish && html_is_space_or_ctrl(start[0])) {
    ++start;
  }
}

void
html_skip_attrname(const char *& start, const char *finish)
{
  while (start < finish && html_is_attrname_char(start[0])) {
    ++start;
  }
}

char
html_skip_equal(const char *& start, const char *finish)
{
  if (start < finish && start[0] == '=') {
    ++start;
    return '=';
  }
  return '\0';
}

char
html_skip_quote(const char *& start, const char *finish)
{
  if (start < finish && (start[0] == '\'' || start[0] == '\"')) {
    const char c = start[0];
    ++start;
    return c;
  }
  return 0;
}

bool
html_skip_char(const char *& start, const char *finish, char c)
{
  if (start < finish && start[0] == c) {
    ++start;
    return true;
  }
  return false;
}

void
html_skip_attrvalue(const char *& start, const char *finish, char qc)
{
  if (qc) {
    const char *const qend = memchr_char(start, qc, finish - start);
    if (qend) {
      start = qend;
      return;
    }
  }
  while (start < finish && html_is_attrvalue_noquote_char(start[0])) {
    ++start;
  }
}

void
html_next_attribute(const char *& start, const char *finish,
  string_ref& name_r, string_ref& value_r)
{
  html_skip_space(start, finish);
  const char *const nb = start;
  html_skip_attrname(start, finish);
  const char *const ne = start;
  name_r.set(nb, ne);
  if (html_skip_equal(start, finish) == '=') {
    const char qc = html_skip_quote(start, finish);
    const char *const vb = start;
    html_skip_attrvalue(start, finish, qc);
    const char *const ve = start;
    if (qc) {
      html_skip_char(start, finish, qc);
    }
    value_r.set(vb, ve);
  } else {
    value_r.set(ne, ne);
  }
}

string_ref
html_find_attribute(const char *start, const char *finish,
  const string_ref& name)
{
  while (start < finish) {
    string_ref n, v;
    html_next_attribute(start, finish, n, v);
    if (n.empty()) {
      break;
    }
    if (!n.equals_strcase(name.begin(), name.end())) {
      continue;
    }
    return v;
  }
  return string_ref();
}

string_ref
html_skip_element_name(const char *& start, const char *finish)
{
  const char *const p = start;
  while (start < finish && !html_is_space_or_ctrl(start[0]) &&
    start[0] != '>') {
    ++start;
  }
  return string_ref(p, start);
}


string_ref
html_find_http_equiv_charset(const char *start, const char *finish)
{
  bool is_content_type = false;
  string_ref content_cs;
  while (true) {
    string_ref n, v;
    html_next_attribute(start, finish, n, v);
    DBG_HTML(std::cerr << "n=[" << n.to_stdstring() << "] v=[" <<
      v.to_stdstring() << "]" << std::endl);
    if (n.empty()) {
      break;
    }
    if (n.equals_strcase("http-equiv") && v.equals_strcase("content-type")) {
      is_content_type = true;
    }
    if (n.equals_strcase("content")) {
      string_ref ct, cs, bnd;
      rfc822_parse_content_type(v.begin(), v.end(), ct, cs, bnd);
      if (!cs.empty()) {
	content_cs = cs;
      }
    }
  }
  if (is_content_type && !content_cs.empty()) {
    return content_cs;
  }
  return string_ref();
}

void
append_u16ch(string_buffer& obuf, unsigned int code)
{
  char *const op = obuf.make_space(6);
  pmc_u8ch_t *const opu8 = reinterpret_cast<pmc_u8ch_t *>(op);
  const pmc_u16ch_t u16buf[1] = { code };
  const pmc_u16ch_t *u16pos = 0;
  pmc_u8ch_t *u8pos = 0;
  pmc_uconv_16to8(u16buf, u16buf + 1, opu8, opu8 + 6, &u16pos, &u8pos);
  obuf.append_done(u8pos - opu8);
}

};

void
html_to_plain(string_buffer& out, const char *start, const char *finish,
  const html_to_plain_options& opts)
{
  string_buffer plain;
  string_buffer& oref = opts.fold_space ? plain : out;
  while (start != finish) {
    const char *const lt = memchr_char(start, '<', finish - start);
    if (!lt) {
      html_decode_entities(oref, start, finish);
      break;
    }
    html_decode_entities(oref, start, lt);
    start = lt + 1;
    string_ref ename = html_skip_element_name(start, finish);
    DBG_HTML(std::cerr << "ename=[" << ename.to_stdstring() << "]"
      << std::endl);
    if (ename == "!--") {
      /* comment */
      const char *const cmtend = search_mem("-->", start, finish - start);
      if (!cmtend) {
	break;
      }
      start = cmtend + string_literal_length("-->");
      continue;
    }
    if (ename.equals_strcase("style")) {
      /* style */
      const char *const stend = search_strcase("</style>", start,
	finish - start);
      if (!stend) {
	break;
      }
      start = stend + string_literal_length("</style>");
      continue;
    }
    /* other tags */
    const char *const tokend = memchr_char(start, '>', finish - start);
    if (!tokend) {
      break;
    }
    if (opts.show_urls) {
      bool is_a_href = false;
      string_ref href;
      if (ename.equals_strcase("a")) {
	is_a_href = true;
	href = html_find_attribute(start, tokend, "href");
      } else if (ename.equals_strcase("img") ||
	ename.equals_strcase("iframe")) {
	href = html_find_attribute(start, tokend, "src");
      }
      if (!href.empty()) {
	oref.append_literal("[");
	oref.append(href.begin(), href.size());
	oref.append_literal("]");
      }
      if (is_a_href && opts.hide_linked_urls) {
	const char *const atxt = tokend + 1;
	const char *const atxtend = search_strcase("</a>", atxt,
	  finish - atxt);
	if (atxtend) {
	  const char *const ht = search_strcase("http://", atxt,
	    atxtend - atxt);
	  if (ht) {
	    /* hide this */
	    start = atxtend + string_literal_length("</a>");
	    continue;
	  }
	}
      }
    }
    start = tokend + 1;
  }
  if (opts.fold_space) {
    fold_space(out, plain.begin(), plain.end());
  }
}

bool
html_find_meta_charset(string_buffer& obuf, const char *start,
  const char *finish)
{
  /* the supplied string is possibly not in utf-8, so strict parsing
   * is futile. */
  while (start < finish) {
    const char *const meta = search_strcase("<meta", start, finish - start);
    if (meta == 0) {
      break;
    }
    start = meta + 5;
    if (start < finish && start[0] > ' ') {
      continue;
    }
    const char *const metae = memchr_char(start, '>', finish - start);
    if (metae == 0) {
      break;
    }
    const string_ref cs = html_find_http_equiv_charset(start, metae);
    if (!cs.empty()) {
      html_decode_entities(obuf, cs.begin(), cs.end());
      return true;
    }
    start = metae;
  }
  return false;
}

int
html_get_entity_code(const char *name, int namelen)
{
  const html_entity ent = { name, namelen, 0 };
  const size_t num_ents = sizeof(html_entities) / sizeof(html_entities[0]);
  const html_entity *const html_entities_end = html_entities + num_ents;
  const html_entity *const iter = gtcpp::binary_search(html_entities,
    html_entities_end, html_entities_end, ent, compare_html_entity());
  if (iter != html_entities_end) {
    return iter->code;
  }
  return -1;
}

void
html_decode_entities(string_buffer& obuf, const char *start,
  const char *finish)
{
  while (start < finish) {
    const char *const amp = memchr_char(start, '&', finish - start);
    if (amp == 0) {
      obuf.append(start, finish - start);
      break;
    }
    obuf.append(start, amp - start);
    start = amp + 1;
    bool suc = true;
    unsigned int code = 0;
    if (start < finish && start[0] == '#') {
      /* number */
      ++start;
      if (start < finish && start[0] == 'x') {
	/* hexadecimal */
	++start;
	const char *const nb = start;
	html_skip_alnum(start, finish);
	const char *const ne = start;
	suc = parse_hexadecimal(nb, ne, code);
      } else {
	/* decimal */
	const char *const nb = start;
	html_skip_alnum(start, finish);
	const char *const ne = start;
	suc = parse_decimal(nb, ne, code);
      }
    } else {
      /* name */
      const char *const nb = start;
      html_skip_alnum(start, finish);
      const char *const ne = start;
      if (ne - nb >= 100) {
	suc = false;
      } else {
	int c = html_get_entity_code(nb, ne - nb);
	suc = (c >= 0);
	code = static_cast<unsigned int>(c);
      }
    }
    if (suc && (code < 0xd000 || code >= 0xe000)) {
      html_skip_semicolon(start, finish);
      append_u16ch(obuf, code);
    } else {
      obuf.append(amp, start - amp);
    }
  }
}

};

