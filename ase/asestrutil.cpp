
#include <ase/asestrutil.hpp>
#include <ase/asestrmap.hpp>

void
ase_basename(const ase_string& path, char c, ase_string& dn_r,
  ase_string& bn_r)
{
  const char *const sl = ase_strrfind(path, c);
  if (sl) {
    const char *const p = path.data();
    dn_r = ase_string(p, sl - p);
    bn_r = ase_string(sl + 1, (p + path.size()) - (sl - 1));
  } else {
    dn_r = ase_string();
    bn_r = ase_string();
  }
}

ase_string
ase_file_ext(const ase_string& path)
{
  const char *const dot = ase_strrfind(path, '.');
  if (dot) {
    return ase_string(dot + 1, (path.data() + path.size()) - (dot - 1));
  }
  return ase_string();
}

ase_variant
ase_parse_argv(const ase_string& ns, int argc, char **argv)
{
  ase_variant r = ase_new_string_map();
  for (int i = 0; i < argc; ++i) {
    const char *const s = argv[i];
    const char *const s_end = s + std::strlen(s);
    /* nspace:key1=val1,key2=val2,... */
    const char *c = static_cast<const char *>(std::memchr(s, ':', s_end - s));
    if (c == 0 || ase_string(s, c - s) != ns) {
      continue;
    }
    ++c;
    while (c != s_end) {
      const char *p = static_cast<const char *>(std::memchr(c, '=',
	s_end - c));
      if (p == 0) {
	continue;
      }
      const ase_string key(c, p - c);
      ++p;
      const char *q = static_cast<const char *>(std::memchr(p, ',',
	s_end - p));
      if (q == 0) {
	q = s_end;
      }
      const ase_string val(p, q - p);
      r.set_property(key, val.to_variant());
      if (q != s_end) {
	++q;
      }
      c = q;
    }
  }
  return r;
}

