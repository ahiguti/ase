
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <gtcpp/string_util.hpp>
#include <gtcpp/horspool.hpp>
#include <gtcpp/ascii.hpp>
#include <gtcpp/ms932.hpp>
#include <gtcpp/rfc822.hpp>
#include <gtcpp/base64.hpp>
#include <gtcpp/quotedpr.hpp>
#include <gtcpp/html.hpp>
#include <gtcpp/posix_error.hpp>
#include <gtcpp/auto_dir.hpp>
#include <gtcpp/file_util.hpp>
#include <gtcpp/auto_iconv.hpp>
#include <gtcpp/buffered_file.hpp>
#include <gtcpp/auto_glob.hpp>
#include <gtcpp/kr_varlen.hpp>
#include <gtcpp/rfc822_util.hpp>
#include <errno.h>
#include <iostream>
#include <vector>
#include <set>
#include <cctype>

#define DBG(x)
#define DBG_CS(x)
#define DBG_HTML(x)

using namespace gtcpp;

namespace {

namespace krgrep {
// FIXME: duplicated. copied from krgrep.cpp.

struct kr_param : public kr_varlen_default_param {
};
typedef kr_varlen<kr_param> kr_type;

struct found_fobj {
  found_fobj(const std::string& fn, const string_buffer& haystack)
    : filename(fn), haystack_ref(haystack) { }
  bool operator ()(size_t offset, const std::string& ndl) {
    printf("%s:%zu %s\n", filename.c_str(), offset, ndl.c_str());
    return false; /* continue */
  }
  std::string filename;
  const string_buffer& haystack_ref;
};

}; // krgrep

void
mail2plain_main(int argc, char **argv)
{
  if (argc < 2) {
    throw std::runtime_error(
      "usage: mail2plain [-html][-fold][-noconv][-urls][-doms][-grep PATFILE] "
      "FILENAME [OUTFILENAME]");
  }
  posix_error_callback ec;
  glob_error_callback gec;
  std::string infile, outfile;
  rfc822_to_plain_options opts;
  bool extract_urls = false;
  bool extract_doms = false;
  bool foldsp = false;
  bool verbose = false;
  std::string grep_patfile;
  for (int i = 1; i < argc; ++i) {
    const std::string k(argv[i]);
    if (k == "-html") {
      opts.html_to_plain = false;
    } else if (k == "-fold") {
      foldsp = true;
    } else if (k == "-noconv") {
      opts.to_utf8 = false;
    } else if (k == "-urls") {
      extract_urls = true;
    } else if (k == "-doms") {
      extract_doms = true;
    } else if (k == "-v") {
      verbose = true;
    } else if (k == "-grep" && i + 1 < argc) {
      ++i;
      grep_patfile = argv[i];
    } else if (infile.empty()) {
      infile = k;
    } else if (outfile.empty()) {
      outfile = k;
    }
  }

  /* krgrep */
  typedef std::set<std::string> wdset_type;
  wdset_type wdset;
  if (!grep_patfile.empty()) {
    read_string_set(grep_patfile.c_str(), wdset, ec);
  }
  krgrep::kr_type kr(8, wdset.size() * 2);
  {
    string_buffer sb;
    for (wdset_type::const_iterator i = wdset.begin(); i != wdset.end(); ++i) {
      fold_space(sb, i->data(), i->data() + i->size());
      const std::string s(sb.begin(), sb.end());
      if (verbose) {
	printf("key %s\n", s.c_str());
      }
      kr.register_needle(s);
      sb.reset();
    }
  }

  /* mail2plain */
  auto_glob gl;
  glob(gl, infile.c_str(), 0, gec);
  for (size_t j = 0; j < gl.get().gl_pathc; ++j) {
    const std::string fn(gl.get().gl_pathv[j]);
    string_buffer inbuf, bufu8;
    try {
      read_file_contents(fn.c_str(), inbuf, ec);
    } catch (posix_error& pe) {
      fprintf(stderr, "%s: %s\n", fn.c_str(), pe.what());
      continue;
    }
    rfc822_to_plain(bufu8, inbuf.begin(), inbuf.end(), opts);
    inbuf.reset();
    if (foldsp || !grep_patfile.empty()) {
      string_buffer buffold;
      fold_space(buffold, bufu8.begin(), bufu8.end());
      bufu8.swap(buffold);
    }
    if (extract_urls || extract_doms) {
      std::vector<std::string> urls;
      rfc822_extract_urls(urls, bufu8.begin(), bufu8.end(), false,
	extract_doms);
      for (size_t k = 0; k < urls.size(); ++k) {
	printf("%s\n", urls[k].c_str());
      }
    } else if (!grep_patfile.empty()) {
      if (verbose) {
	printf("[%s]\n", std::string(bufu8.begin(), bufu8.end()).c_str());
      }
      krgrep::found_fobj fobj(fn, bufu8);
      kr.search(fobj, bufu8.begin(), bufu8.size());
    } else if (outfile.empty()) {
      fwrite(bufu8.begin(), bufu8.size(), 1, stdout);
    } else {
      write_file_contents(outfile.c_str(),
	string_ref(bufu8.begin(), bufu8.end()), false, ec);
    }
  }
}

};

int
main(int argc, char **argv)
{
  try {
    mail2plain_main(argc, argv);
  } catch (const std::exception& e) {
    std::cerr << "uncaught exception: " << e.what() << std::endl;
    return 1;
  }
  return 0;
}

