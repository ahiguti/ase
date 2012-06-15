
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GTCPP_AUTO_DIR_HPP
#define GTCPP_AUTO_DIR_HPP

#include <gtcpp/posix_error.hpp>
#include <boost/noncopyable.hpp>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

namespace gtcpp {

class auto_dir : private boost::noncopyable {

 public:

  auto_dir();
  ~auto_dir() throw ();
  void reset();
  void reset(::DIR *p);
  ::DIR *get() const { return ptr; }

 private:

  ::DIR *ptr;

};

int opendir(auto_dir& fd, const char *name, posix_error_callback& ec);
int readdir(const auto_dir& fd, ::dirent *& ent_r, posix_error_callback& ec);

int mkdir(const char *name, mode_t mode, posix_error_callback& ec);
int unlink(const char *name, posix_error_callback& ec);

};

#endif

