
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <gtcpp/auto_glob.hpp>
#include <gtcpp/posix_error.hpp>
#include <gtcpp/auto_dir.hpp>
#include <gtcpp/string_util.hpp>
#include <gtcpp/buffered_file.hpp>
#include <errno.h>
#include <iostream>
#include <vector>

namespace {

using namespace gtcpp;

class error_callback :
  public posix_error_callback,
  public glob_error_callback {
};

// FIXME: remove
#if 0
class error_callback_hookisdir : public error_callback {
  int on_posix_error(int e, const char *f) {
    if (e == EISDIR) {
      return to_negative(e);
    }
    throw_posix_error(e, f);
  }
};
#endif

void
mailfmt_main(int argc, char **argv)
{
  if (argc < 3) {
    throw std::runtime_error("usage: mailfmt GLOB_PATTERN OUTPUT_DIR");
  }
  error_callback ec;
  posix_error_callback_nothrow ecnthr;
  std::string glob_pattern(argv[1]);
  std::string outdir(argv[2]);
  auto_glob gl;
  glob(gl, glob_pattern.c_str(), 0, ec);
  std::vector<std::string> fnlist;
  for (size_t i = 0; i < gl.get().gl_pathc; ++i) {
    fnlist.push_back(std::string(gl.get().gl_pathv[i]));
  }
  mkdir(outdir.c_str(), 0755, ec);
  for (size_t i = 0; i < fnlist.size(); ++i) {
    const std::string& s = fnlist[i];
    // string_buffer sb;
    buffered_file bf;
    const int e = open(bf.get_file(), s.c_str(), O_RDONLY, ecnthr);
    if (e < 0) {
      if (ecnthr.get_last_posix_error() == EISDIR) {
        continue;
      } else {
        ecnthr.throw_last_posix_error();
      }
    }
    while (true) {
      char *line = 0;
      ssize_t len = bf.read_line_lf(line, ec);
      if (len <= 1 || (len == 2 && line[0] == '\r')) {
	/* found an empty line or eof */
	break;
      }
    }
    const size_t body_begin_offset = bf.get_cursor_offset();
    char *bytes = 0;
    while (bf.read_bytes(1024, bytes, ec) > 0) {
      /* do nothing */
    }
    const char *const hdr_begin = bf.get_buffer().begin();
    const char *const body_begin = bf.get_buffer().begin() + body_begin_offset;
    const char *const body_end = bf.get_buffer().end();
    std::string cntstr;
    {
      char buf[1024];
      snprintf(buf, sizeof(buf), "%zu", i + 1);
      cntstr = std::string(buf);
    }
    const std::string fn_hdr = outdir + "/" + cntstr + "h";
    const std::string fn_bdy = outdir + "/" + cntstr + "b";
    auto_file wf;
    creat(wf, fn_hdr.c_str(), 0644, ec);
    write(wf, hdr_begin, body_begin - hdr_begin, ec);
    creat(wf, fn_bdy.c_str(), 0644, ec);
    write(wf, body_begin, body_end - body_begin, ec);
  }
}

};

int
main(int argc, char **argv)
{
  try {
    mailfmt_main(argc, argv);
  } catch (const std::exception& e) {
    std::cerr << "uncaught exception: " << e.what() << std::endl;
    return 1;
  }
  return 0;
}

