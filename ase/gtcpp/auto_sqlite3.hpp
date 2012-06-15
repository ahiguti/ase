
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <gtcpp/string_util.hpp>
#include <boost/noncopyable.hpp>
#include <stdexcept>
#include <sqlite3.h>

#ifndef GTCPP_DBG_SQLITE3
#define GTCPP_DBG_SQLITE3(x)
#endif

namespace gtcpp {

class sqlite3_error : public std::exception {

 public:

  sqlite3_error(int ec, const char *es) : ecode(ec),
    estr(std::string("[SQLITE3][") + to_stdstring_decimal(ecode) + "] " + es) {
  }
  ~sqlite3_error() throw() { }
  const char *what() const throw() { return estr.c_str(); }

 private:

  int ecode;
  std::string estr;

};

class auto_sqlite3_str : private boost::noncopyable {

 public:

  auto_sqlite3_str(char *s) : str(s) { }
  ~auto_sqlite3_str() {
    if (str) {
      sqlite3_free(str);
    }
  }
  operator char *() { return str; }

 private:

  char *const str;

};

class auto_sqlite3 : private boost::noncopyable {

 public:

  auto_sqlite3(const char *filename) : db(0) {
    const int r = sqlite3_open(filename, &db);
    if (r != SQLITE_OK) {
      throw sqlite3_error(r,
	(std::string("failed to open ") + filename).c_str());
    }
  }
  ~auto_sqlite3() {
    this->close(false);
  }
  operator sqlite3 *() const { return db; }
  void close(bool thr = true) {
    if (db) {
      int r = 0;
      if ((r = sqlite3_close(db)) != SQLITE_OK && thr) {
	throw sqlite3_error(r, "failed to close");
      }
    }
    db = 0;
  }
  void execute(const char *sqlstr) {
    char *err = 0;
    GTCPP_DBG_SQLITE3(std::cerr << "EXEC: " << sqlstr << std::endl);
    const int r = sqlite3_exec(db, sqlstr, 0, 0, &err);
    auto_sqlite3_str aerr(err);
    if (r != SQLITE_OK) {
      throw sqlite3_error(r, err ? err : "unknown error");
    }
  }

 private:

  sqlite3 *db;

};

class auto_sqlite3_stmt : private boost::noncopyable {

 public:

  auto_sqlite3_stmt(sqlite3 *db, const char *str) : stmt(0) {
    int r = 0;
    if ((r = sqlite3_prepare_v2(db, str, -1, &stmt, 0)) != SQLITE_OK) {
      throw sqlite3_error(r,
	(std::string("sqlite3_prepare_v2 failed: ") + str).c_str());
    }
  }
  ~auto_sqlite3_stmt() {
    if (stmt) {
      int r = 0;
      if ((r = sqlite3_finalize(stmt)) != SQLITE_OK) {
	GTCPP_DBG_SQLITE3(std::cerr << "WARNING: sqlite3_finalize failed("
	  << r << ")" << std::endl);
      }
    }
  }
  operator sqlite3_stmt *() const { return stmt; }

 private:

  sqlite3_stmt *stmt;

};

class sqlite3_query : private boost::noncopyable {

 public:

  sqlite3_query(sqlite3_stmt *s) : stmt(s) { }
  ~sqlite3_query() {
    int r = 0;
    if ((r = sqlite3_reset(stmt)) != SQLITE_OK) {
      GTCPP_DBG_SQLITE3(std::cerr << "WARNING: sqlite3_reset failed("
	<< r << ")" << std::endl);
    }
  }
  void bind_null(int i) {
    int r = 0;
    if ((r = sqlite3_bind_null(stmt, i)) != SQLITE_OK) {
      throw sqlite3_error(r, "sqlite3_bind_null failed");
    }
  }
  void bind_blob(int i, const void *data, int sz) {
    int r = 0;
    if ((r = sqlite3_bind_blob(stmt, i, data, sz, 0)) != SQLITE_OK) {
      throw sqlite3_error(r, "sqlite3_bind_blob failed");
    }
  }
  void bind_text(int i, const char *data, int sz) {
    int r = 0;
    if ((r = sqlite3_bind_text(stmt, i, data, sz, 0)) != SQLITE_OK) {
      throw sqlite3_error(r, "sqlite3_bind_text failed");
    }
  }
  void bind_int(int i, int data) {
    int r = 0;
    if ((r = sqlite3_bind_int(stmt, i, data)) != SQLITE_OK) {
      throw sqlite3_error(r, "sqlite3_bind_int failed");
    }
  }
  void bind_int64(int i, sqlite_int64 data) {
    int r = 0;
    if ((r = sqlite3_bind_int64(stmt, i, data)) != SQLITE_OK) {
      throw sqlite3_error(r, "sqlite3_bind_int64 failed");
    }
  }
  void bind_double(int i, double data) {
    int r = 0;
    if ((r = sqlite3_bind_double(stmt, i, data)) != SQLITE_OK) {
      throw sqlite3_error(r, "sqlite3_bind_double failed");
    }
  }
  int step() {
    const int r = sqlite3_step(stmt);
    const bool v = (r == SQLITE_OK || r == SQLITE_ROW || r == SQLITE_DONE);
    if (!v) {
      throw sqlite3_error(r, "sqlite3_step failed");
    }
    return r;
  }

 private:

  sqlite3_stmt *const stmt;

};

};

