
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <ase/aseio.hpp>
#include <ase/asearray.hpp>
#include <ase/aseexcept.hpp>

#include <fstream>
#include <sstream>

#ifdef __GNUC__

#include <stdexcept>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <pthread.h>
#include <glob.h>

namespace {

struct auto_dir : public ase_noncopyable {
  auto_dir(DIR *p) : dir(p) { }
  ~auto_dir() /* DCHK */ {
    if (dir) {
      while (true) {
	errno = 0;
	if (::closedir(dir) != 0) {
	  /* closedir can return EINTR */
	  if (errno == EINTR) {
	    continue;
	  }
	}
	break;
      }
    }
  }
  DIR *get() const { return dir; }
 private:
  DIR *dir;
};

struct auto_glob : public ase_noncopyable {
  auto_glob(const char *pattern, int flags) : err(0) {
    err = glob(pattern, flags, 0, &gl);
  }
  ~auto_glob() {
    globfree(&gl);
  }
  const glob_t& get() const { return gl; }
  int get_error() const { return err; }
 private:
  glob_t gl;
  int err;
};

struct ase_unrecoverable_error : std::exception {

  ase_unrecoverable_error(int e, const char *filename, int line,
    const char *funcname) {
    std::stringstream ss;
    ss << "ase_unrecoverable_error: errno=" << e << " at " << filename << ":"
      << line << ", " << funcname;
    mess = ss.str();
    mess_cstr = mess.c_str();
  }
  ~ase_unrecoverable_error() throw () { }
  const char *what() const throw () { return mess_cstr; }

 private:

  std::string mess;
  const char *mess_cstr;

};

// FIXME: test
void
check_unrecoverable_error(const char *filename, int line,
  const char *funcname)
{
  int e = errno;
  if (e == ENOMEM || e == EIO) {
    throw ase_unrecoverable_error(e, filename, line, funcname);
  }
}

#define CHECK_UNRECOVERABLE_ERROR() \
  check_unrecoverable_error(__FILE__, __LINE__, __PRETTY_FUNCTION__)

}; // anonymous namespace

// FIXME: check_unrecoverable_error for win32

ase_string
ase_getenv_mtunsafe(const char *key)
{
  char *c = std::getenv(key);
  return c ? ase_string(c) : ase_string();
}

ase_variant
ase_read_directory(const ase_string& dpath)
{
  std::vector<ase_string> ents;
  auto_dir d(::opendir(dpath.data()));
  if (d.get() == NULL) {
    CHECK_UNRECOVERABLE_ERROR();
    return ase_variant();
  }
  ase_variant r = ase_new_array();
  struct dirent *ent;
  ase_size_type n = 0;
  while (true) {
    errno = 0;
    ent = ::readdir(d.get());
    if (ent == 0) {
      if (errno != 0) {
	CHECK_UNRECOVERABLE_ERROR();
      }
      break;
    }
    if (ent->d_name[0] == '.') {
      /* skip dot files */
    }
    ase_string s(ent->d_name, std::strlen(ent->d_name));
    r.set_element(n++, s.to_variant());
  }
  return r;
}

ase_variant
ase_glob(const ase_string& pattern)
{
  std::vector<ase_string> ents;
  auto_glob gl(pattern.data(), GLOB_ERR | GLOB_NOESCAPE);
  int e = gl.get_error();
  if (e != 0) {
    if (e == GLOB_NOSPACE) {
      throw std::bad_alloc();
    }
    return ase_variant();
  }
  ase_variant r = ase_new_array();
  const glob_t& g = gl.get();
  for (size_t i = 0; i < g.gl_pathc; ++i) {
    const char *p = g.gl_pathv[i];
    ase_string s(p);
    r.set_element(i, s.to_variant());
  }
  return r;
}

bool
ase_file_exists(const ase_string& fname)
{
  int ac = ::access(fname.data(), R_OK);
  if (ac != 0) {
    CHECK_UNRECOVERABLE_ERROR();
    return false;
  }
  return true;
}

bool
ase_newer_than(const ase_string& f1, const ase_string& f2)
{
  struct stat sb1;
  struct stat sb2;
  /* stat f1 first */
  if (::stat(f1.data(), &sb1) != 0) {
    CHECK_UNRECOVERABLE_ERROR();
    return false;
  }
  if (::stat(f2.data(), &sb2) != 0) {
    CHECK_UNRECOVERABLE_ERROR();
    return false;
  }
  return (sb1.st_mtime > sb2.st_mtime);
}

bool
ase_change_directory(const ase_string& dname)
{
  int r = ::chdir(dname.data());
  if (r != 0) {
    CHECK_UNRECOVERABLE_ERROR();
    return false;
  }
  return true;
}

ase_string
ase_getcwd()
{
  long pm = _POSIX_PATH_MAX;
  std::vector<char> buf(static_cast<size_t>(pm));
  char *cwd = ::getcwd(&buf[0], static_cast<size_t>(pm));
  if (!cwd) {
    CHECK_UNRECOVERABLE_ERROR();
    ase_throw_io_error("getcwd failed");
  }
  return ase_string(cwd);
}

void *
ase_get_current_thread_id()
{
  return (void *)(pthread_self());
}

#else

#include <ase/aseutf16.hpp>

#include <windows.h>

namespace {

struct auto_dir {
  auto_dir(HANDLE h) : hnd(h) { }
  ~auto_dir() /* DCHK */ { if (hnd != INVALID_HANDLE_VALUE) FindClose(hnd); }
  HANDLE get() const { return hnd; }
 private:
  auto_dir(const auto_dir&);
  auto_dir& operator =(const auto_dir&);
  HANDLE hnd;
};

ase_variant
find_files(const ase_string& pattern)
{
  ase_utf16_string wpattern(pattern);
  WIN32_FIND_DATAW fdata;
  auto_dir d(FindFirstFileW(wpattern.data(), &fdata));
  if (d.get() == INVALID_HANDLE_VALUE) {
    return ase_variant();
  }
  ase_variant r = ase_new_array();
  ase_int n = 0;
  do {
    const ase_ucchar *fw = fdata.cFileName;
    ase_size_type fwlen = wcslen(fdata.cFileName);
    ase_string s(fw, fwlen);
    if (s.size() > 0 && s.data()[0] == '.') {
      /* skip dot files */
      continue;
    }
    r.set(n++, s.to_variant());
  } while (FindNextFileW(d.get(), &fdata));
  return r;
}

}; // anonymous namespace

ase_string
ase_getenv_mtunsafe(const char *key)
{
  #if _MSC_VER >= 1400
  std::vector<char> buf(32);
  while (true) {
    size_t reqlen = 0;
    errno_t e = ::getenv_s(&reqlen, &buf[0], buf.size(), key);
    if (e == 0) {
      if (reqlen == 0) {
	return ase_string();
      }
      buf.push_back('\0'); /* buf should be already nul-terminated though. */
      return ase_string(&buf[0]);
    }
    buf.resize(reqlen);
  }
  #else
  char *c = std::getenv(key);
  return c ? ase_string(c) : ase_string();
  #endif
}

ase_variant
ase_read_directory(const ase_string& dpath)
{
  return find_files(dpath + "\\*");
}

ase_variant
ase_glob(const ase_string& pattern)
{
  return find_files(pattern);
}

bool
ase_file_exists(const ase_string& fname)
{
  ase_utf16_string wpath(fname);
  WIN32_FIND_DATAW fdata;
  auto_dir d(FindFirstFileW(wpath.data(), &fdata));
  return (d.get() != INVALID_HANDLE_VALUE);
}

bool
ase_newer_than(const ase_string& f1, const ase_string& f2)
{
  ase_utf16_string wf1(f1);
  ase_utf16_string wf2(f2);
  WIN32_FIND_DATAW fd1, fd2;
  auto_dir d1(FindFirstFileW(wf1.data(), &fd1));
  if (d1.get() == INVALID_HANDLE_VALUE) {
    return false;
  }
  auto_dir d2(FindFirstFileW(wf2.data(), &fd2));
  if (d2.get() == INVALID_HANDLE_VALUE) {
    return false;
  }
  return (CompareFileTime(&fd1.ftLastWriteTime,
    &fd2.ftLastWriteTime) > 0);
}

bool
ase_change_directory(const ase_string& dname)
{
  ase_utf16_string wpath(dname);
  return (SetCurrentDirectoryW(wpath.data()) != 0);
}

ase_string
ase_getcwd()
{
  DWORD wblen = GetCurrentDirectoryW(0, NULL);
  if (wblen == 0) {
    ase_throw_io_error("GetCurrentDirectoryW(len) failed");
  }
  std::vector<WCHAR> wbuf(wblen);
  WCHAR *const wstr = &wbuf[0];
  if (GetCurrentDirectoryW(wblen, wstr) == 0) {
    ase_throw_io_error("GetCurrentDirectoryW(buf) failed");
  }
  DWORD wstr_len = wblen - 1;
  ase_string r(wstr, wstr_len);
  return r;
}

void *
ase_get_current_thread_id()
{
  #pragma warning(push)
  #pragma warning(disable:4312)
  return reinterpret_cast<void *>(GetCurrentThreadId());
  #pragma warning(pop)
}

#endif

/* common */

ase_string
ase_load_file(const ase_string& fname)
{
  std::vector<char> buffer;
  const char *fnstr = fname.data();
  std::ifstream fs(fnstr);
  if (!fs) {
    ase_string s = ase_string("failed to open: ") + ase_string(fnstr);
    ase_throw_io_error(s.data());
  }
  char ch;
  while (!fs.fail()) {
    /* TODO: slow */
    ch = fs.get();
    buffer.push_back(ch);
  }
  fs.close();
  if (!fs.eof()) {
    ase_string s = ase_string("failed to read: ") + ase_string(fnstr);
    ase_throw_io_error(s.data());
  }
  ase_string s(&buffer[0], buffer.size() - 1);
  return s;
}

