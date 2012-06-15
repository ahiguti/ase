
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GENTYPE_ASELIST_HPP
#define GENTYPE_ASELIST_HPP

#include <ase/asemutex.hpp>

template <typename T>
class ase_list_node : public ase_noncopyable {

 public:

  template <typename INI>
  ase_list_node(const INI& ini)
    : value(ini), next(0), prev(0), mutex(new ase_fast_mutex()) {
    next = this;
    prev = this;
  }
  ase_list_node()
    : value(), next(0), prev(0), mutex(new ase_fast_mutex()) {
    next = this;
    prev = this;
  }
  template <typename INI> 
  ase_list_node(const INI& ini, ase_list_node& x)
    : value(ini), next(0), prev(0), mutex(x.mutex) {
    ase_mutex_guard<ase_fast_mutex> g(*mutex);
    ase_list_node *nprev = x.prev;
    ase_list_node *nnext = &x;
    this->next = nnext;
    this->prev = nprev;
    nprev->next = this;
    nnext->prev = this;
  }
  ~ase_list_node() /* DCHK */ {
    {
      ase_mutex_guard<ase_fast_mutex> g(*mutex);
      next->prev = prev;
      prev->next = next;
    }
    if (this == next) {
      delete mutex;
    }
  }
  template <typename F> void for_each_unlocked(F f) {
    ase_list_node *p = this;
    do  {
      f(p->value);
      p = p->next;
    } while (p != this);
  }
  ase_list_node *get_next() const { return next; }
  ase_list_node *get_prev() const { return prev; }
  ase_fast_mutex& get_shared_mutex() { return *mutex; }

 public:

  T value;

 private:

  ase_list_node *next;
  ase_list_node *prev;
  ase_fast_mutex *mutex; /* shared */

};

#endif

