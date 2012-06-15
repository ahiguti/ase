
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GENTYPE_ASERUNTIME_HPP
#define GENTYPE_ASERUNTIME_HPP

#include <ase/ase.hpp>

class ase_unified_engine_ref;
class ase_unified_engine;
class ase_environment;

struct ase_script_engine : public ase_noncopyable {

  ASELIB_DLLEXT ase_script_engine();
  virtual void destroy() = 0;
  virtual void child_thread_initialized() = 0;
  virtual void child_thread_terminated() = 0;
  virtual void force_gc() = 0;
  virtual void wait_threads() = 0;
  virtual void dispose_proxies() = 0;
  virtual const char *const *get_global_names() = 0;
  virtual ase_variant get_global_object(const char *name) = 0;
  virtual void import_ase_global(const ase_variant& val) = 0;
  virtual ase_variant load_file(const char *glname, const char *filename) = 0;
  virtual ase_script_engine *get_real_engine() = 0;

 protected:

  ASELIB_DLLEXT virtual ~ase_script_engine();

};

typedef ase_auto_ptr<ase_script_engine> ase_script_engine_ptr;

struct ase_script_engine_factory : public ase_noncopyable {

  ASELIB_DLLEXT ase_script_engine_factory();
  virtual void destroy() = 0;
  virtual void new_script_engine(ase_unified_engine_ref& ue,
    ase_script_engine_ptr& ap_r) = 0;

 protected:

  ASELIB_DLLEXT virtual ~ase_script_engine_factory();

};

typedef ase_auto_ptr<ase_script_engine_factory> ase_script_engine_factory_ptr;
typedef void ase_new_script_engine_factory_func(ase_environment& env,
  ase_script_engine_factory_ptr& ap_r);

class ase_environment : public ase_noncopyable {

 public:

  virtual const char *const *get_path() const = 0;
  virtual void *get_preexisting_engine(const char *name) const = 0;

 protected:

  ASELIB_DLLEXT ase_environment();
  ASELIB_DLLEXT virtual ~ase_environment();

};

class ase_unified_engine_ref : public ase_noncopyable {

 public:

  virtual const char *const *get_path() = 0;
  virtual ase_recursive_mutex& get_mutex() = 0;
  virtual void child_thread_initialized() = 0;
  virtual void child_thread_terminated() = 0;
  virtual void force_gc() = 0;
  virtual ase_variant load_file(const ase_string& fname) = 0;

 protected:

  ASELIB_DLLEXT ase_unified_engine_ref();
  ASELIB_DLLEXT virtual ~ase_unified_engine_ref();

};

struct ase_preexisting_engine_info {
  const char *name;
  void *pointer;
};

class ase_runtime : public ase_environment {

 public:

  friend class ase_unified_engine;

  ASELIB_DLLEXT ase_runtime(const char *execpath, int argc, char **argv,
    const ase_preexisting_engine_info *pe = 0, size_t num_pe = 0);
  ASELIB_DLLEXT ~ase_runtime(); /* DCHK */

  ASELIB_DLLEXT static ase_string get_lib_exec(const char *execpath);
  ASELIB_DLLEXT const char *const *get_path() const;
  ASELIB_DLLEXT void *get_preexisting_engine(const char *name) const;

  ASELIB_DLLEXT static void show_statistics();

 private:

  struct impl_type;
  impl_type *impl;

};

class ase_unified_engine : public ase_unified_engine_ref {

 private:

  ASELIB_DLLEXT const char *const *get_path();
  ASELIB_DLLEXT ase_recursive_mutex& get_mutex();
  ASELIB_DLLEXT void child_thread_initialized();
  ASELIB_DLLEXT void child_thread_terminated();
  ASELIB_DLLEXT void force_gc();

 public:

  ASELIB_DLLEXT ase_unified_engine(ase_runtime& cnf);
  ASELIB_DLLEXT ~ase_unified_engine();
  ASELIB_DLLEXT ase_variant load_file(const ase_string& filename);
  ASELIB_DLLEXT void wait_threads(); /* not mt-safe */
  ASELIB_DLLEXT ase_variant get_ase_global_object();
  ASELIB_DLLEXT ase_script_engine *get_real_engine(const char *name);


 private:

  struct impl_type;
  impl_type *impl;

};

#endif

