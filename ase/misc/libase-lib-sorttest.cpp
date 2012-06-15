
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <ase/asecpnt.hpp>
#include <iostream>
#include <pthread.h>

#include <cstdlib>
#include <time.h>

#define DBG(x) x

namespace {

struct sorttest_klass {

  sorttest_klass(ASE_Component::Library& lib)
    : libref(lib) {
    DBG(std::cout << "create klass " << this << std::endl);
    globals = lib.GetGlobals();
  }
  ~sorttest_klass() {
    DBG(std::cout << "delete klass " << this << std::endl);
  }
  ase_variant InvokeClassMethod(ase_int id, const ase_variant *args,
    ase_size_type nargs) {
    if (id != 0) {
      return ase_variant();
    }
    ase_variant selfclass = libref.GetClass(ase_string("Main"));
    ase_variant Comparable = globals.get("Java").get("java.lang.Comparable");
    ase_variant Arrays = globals.get("Java").get("java.util.Arrays");
    const int len = 20;
    ase_variant arr = Comparable.minvoke("NewArray", len);
    for (int i = 0; i < len; ++i) {
      DBG(std::cout << "i=" << i << std::endl);
      int v = std::rand() % 1000;
      ase_variant elem = selfclass.invoke(v);
      DBG(std::cout << "elem" << std::endl);
      ase_variant prox = Comparable.invoke(elem);
      DBG(std::cout << "prox" << std::endl);
      arr.set(i, prox);
    }
    Arrays.minvoke("sort", arr);
    for (int i = 0; i < len; ++i) {
      ase_long v = arr.get(i).minvoke("getValue").get_long();
      std::cout << "i=" << i << " v=" << v << std::endl;
    }
    return ase_variant();
  }

  ASE_Component::Library& libref;
  ase_variant globals;

  static const char *const ClassMethods[];

};

struct sorttest {

  sorttest(const sorttest_klass& klass, const ase_variant *args,
    ase_size_type nargs) : value(0) {
    if (nargs > 0) {
      value = args[0].get_long();
    }
    DBG(std::cout << "create instance " << this << std::endl);
  }
  ~sorttest() {
    DBG(std::cout << "del instance " << this << std::endl);
  }
  ase_variant InvokeInstanceMethod(const sorttest_klass& klass,
    ase_int id, const ase_variant *args, ase_size_type nargs) {
    switch (id) {
    case 0:
      /* getValue */
      return ase_variant(value);
    case 1:
      /* compareTo */
      {
	ase_long xv = 0;
	if (nargs > 0) {
	  xv = args[0].minvoke("getValue").get_long();
	}
	std::cout << "compareTo: " << value  << " " << xv << std::endl;
	if (value < xv) {
	  return ase_variant(-1);
	}
	if (value > xv) {
	  return ase_variant(1);
	}
	return ase_variant(0);
      }
    default:
      break;
    }
    return ase_variant();
  }

  static const char *const InstanceMethods[];

 private:

  ase_long value;

};

const char *const sorttest_klass::ClassMethods[] = {
  "Main",
};
const char *const sorttest::InstanceMethods[] = {
  "getValue", "compareTo",
};

}; // anonymous namespace

extern "C" void
ASE_NewVariantFactory(ASE_VariantFactoryPtr& ap_r)
{
  static const ASE_Component::LibraryEntry ents[] = {
    { "Main", ASE_Component::NewClass<sorttest_klass, sorttest, true> },
  };
  std::srand(::time(0));
  ASE_Component::NewLibraryFactory(ents, ap_r);
}

