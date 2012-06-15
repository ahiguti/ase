
#ifndef GENTYPE_ASESTRUTIL_HPP
#define GENTYPE_ASESTRUTIL_HPP

#include <ase/ase.hpp>
#include <string.h>

inline const char *ase_strfind(const char *name, ase_size_type namelen, char c)
{
  return static_cast<const char *>(memchr(name, c, namelen));
}

inline const char *ase_strfind(const ase_string& str, char c)
{
  return static_cast<const char *>(memchr(str.data(), c, str.size()));
}

inline const char *ase_strrfind(const ase_string& str, char c)
{
  #ifdef __GNUC__
  return static_cast<const char *>(memrchr(str.data(), c, str.size()));
  #else
  const char *const p = str.data();
  const char *q = p + str.size();
  while (q != p) { if (*(--q) == c) { return q; } }
  return 0;
  #endif
}

ASELIB_DLLEXT void ase_basename(const ase_string& path, char c,
  ase_string& dn_r, ase_string& bn_r);
ASELIB_DLLEXT ase_string ase_file_ext(const ase_string& path);
ASELIB_DLLEXT ase_variant ase_parse_argv(const ase_string& ns, int argc,
  char **argv);

#endif

