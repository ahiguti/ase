
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GTCPP_AUTO_PGCONN_HPP
#define GTCPP_AUTO_PGCONN_HPP

#include <boost/noncopyable.hpp>
#include <stdexcept>
#include <memory>
#include <libpq-fe.h>

namespace gtcpp {

class pg_error : public std::exception {
};

class pgconn_error : public pg_error {

 public:

  pgconn_error(PGconn *conn)
    : status_code(PQstatus(conn)),
      message(std::string("[PQERROR]") + PQerrorMessage(conn)) { }
  ~pgconn_error() throw () { }
  const char *what() const throw () { return message.c_str(); }

 private:

  ConnStatusType status_code;
  std::string message;

};

class pgresult_error : public pg_error {

 public:

  pgresult_error(PGresult *res)
    : status_code(PQresultStatus(res)),
      message(std::string("[PQRESULT]") + PQresultErrorMessage(res)) { }
  ~pgresult_error() throw () { }
  const char *what() const throw () { return message.c_str(); }

 private:

  ExecStatusType status_code;
  std::string message;

};

class auto_pgconn : private boost::noncopyable {

 public:

  auto_pgconn(const char *conninfo) : pgconn(PQconnectdb(conninfo)) {
    check_error();
  }
  ~auto_pgconn() {
    if (pgconn) {
      PQfinish(pgconn);
    }
  }
  void check_error() const {
    if (PQstatus(pgconn) != CONNECTION_OK) {
      throw pgconn_error(pgconn);
    }
  }
  operator PGconn *() { return pgconn; }
  
 private:

  PGconn *pgconn;

};

class auto_pgresult : private boost::noncopyable {

 public:
 
  auto_pgresult(PGconn *conn, const char *cmd)
    : pgresult(PQexec(conn, cmd)) { }
  ~auto_pgresult() {
    if (pgresult) {
      PQclear(pgresult);
    }
  }
  void check_error() const {
    const ExecStatusType s = PQresultStatus(pgresult);
    if (s != PGRES_COMMAND_OK && s != PGRES_TUPLES_OK) {
      throw pgresult_error(pgresult);
    }
  }
  operator PGresult *() { return pgresult; }

 private:

  PGresult *pgresult;
  
};

};

#endif

