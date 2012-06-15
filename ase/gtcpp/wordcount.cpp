
#include <gtcpp/buffered_file.hpp>
#include <gtcpp/auto_glob.hpp>
#include <gtcpp/posix_error.hpp>
#include <map>
#include <string>
#include <cctype>

using namespace gtcpp;

struct wordcount {

  typedef std::map<std::string, int> wordmap_type;
  wordmap_type wordmap;

  static std::string
  remove_punctuation(const std::string& s)
  {
    const char *p = s.data();
    const char *q = p + s.size();
    while (p != q && ispunct(p[0])) {
      ++p;
    }
    while (p != q && ispunct(q[-1])) {
      --q;
    }
    return std::string(p, q);
  }

  void
  append(buffered_file& bf)
  {
    posix_error_callback ec;
    while (true) {
      char *line = 0;
      ssize_t len = bf.read_line_lf(line, ec);
      if (len <= 0) {
	break;
      }
      std::string buffer(line, len);
      buffer += '\0';
      char *save = 0;
      const char *const delim = " \t\r\n[](){}";
      const char *tok = strtok_r(&buffer[0], delim, &save);
      while (tok) {
	std::string s(tok);
	s = remove_punctuation(s);
	if (!s.empty()) {
	  wordmap[s]++;
	}
	tok = strtok_r(0, delim, &save);
      }
    }
  }

  void dump() {
    typedef std::multimap<int, std::string> rmap_type;
    rmap_type rmap;
    for (wordmap_type::const_iterator i = wordmap.begin(); i != wordmap.end(); 
      ++i) {
      rmap.insert(std::make_pair(i->second, i->first));
    }
    for (rmap_type::const_iterator i = rmap.begin(); i != rmap.end(); ++i) {
      printf("%d %s\n", i->first, i->second.c_str());
    }
  }

};

int
main(int argc, char **argv)
{
  posix_error_callback ec;
  glob_error_callback gec;
  if (argc < 2) {
    printf("Usage: %s GLOB_PATTERN\n", argv[0]);
    return 2;
  }
  const char *const glob_pattern = argv[1];
  auto_glob gl;
  glob(gl, glob_pattern, 0, gec);
  wordcount wc;
  for (size_t i = 0; i < gl.get().gl_pathc; ++i) {
    const char *const fname = gl.get().gl_pathv[i];
    buffered_file bf;
    open(bf.get_file(), fname, O_RDONLY, ec);
    wc.append(bf);
  }
  wc.dump();
}

