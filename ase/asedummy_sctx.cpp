
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <ase/aseexcept.hpp>

#include "asedummy_util.hpp"
#include "asedummy_aseimpl.hpp"
#include "asedummy_asepr.hpp"

#define DBG(x)

namespace asedummy {

ase_dummy_script_engine::ase_dummy_script_engine(ase_unified_engine_ref& ue)
  : uengine(ue), uemutex(ue.get_mutex()),
    variant_vtable(ase_variant_impl<ase_dummy_ase_variant_impl>::vtable),
    refcount(1), dummy_endnode(0), ase_endnode(0)
{
  try {
    init();
  } catch (const std::bad_alloc&) {
    cleanup();
    throw;
  } catch (...) {
    cleanup();
    /* we must not rethrow the thrown object because it may refer to
      'this'. */
    ase_throw_error("ASEDUMMY: failed to initialize dummy engine");
  }
}

ase_dummy_script_engine::~ase_dummy_script_engine() /* DCHK */
{
  cleanup();
}

void
ase_dummy_script_engine::init()
{
  variant_vtable.impl_private_data = this;
  dummy_endnode = new ase_dummy_ase_variant_implnode(
    ase_dummy_ase_variant_impl::init_arg(this, ase_dummy_value()));
  ase_endnode = new ase_dummy_aseproxy_privnode(
    ase_dummy_aseproxy_priv::init_arg(this, ase_variant()));
}

void
ase_dummy_script_engine::cleanup()
{
  dispose_proxies();
  dummy_global = ase_variant();
  delete ase_endnode;
  delete dummy_endnode;
}

void
ase_dummy_script_engine::destroy()
{
  delete this;
}

void
ase_dummy_script_engine::child_thread_initialized()
{
}

void
ase_dummy_script_engine::child_thread_terminated()
{
}

void
ase_dummy_script_engine::force_gc()
{
}

void
ase_dummy_script_engine::wait_threads()
{
}

void
ase_dummy_script_engine::dispose_proxies()
{
  DBG(fprintf(stderr, "dummy: dispose\n"));
  if (dummy_endnode) {
    ase_dummy_ase_variant_implnode *p = dummy_endnode;
    do {
      p->value.dispose(this);
      p = p->get_next();
    } while (p != dummy_endnode);
  }
  if (ase_endnode) {
    ase_dummy_aseproxy_privnode *p = ase_endnode;
    do {
      p->value.dispose(this);
      p = p->get_next();
    } while (p != ase_endnode);
  }
  DBG(fprintf(stderr, "dummy: dispose done\n"));
}

const char *const *
ase_dummy_script_engine::get_global_names()
{
  static const char *const globals[] = { "Dummy", 0 };
  return globals;
}

ase_variant
ase_dummy_script_engine::get_global_object(const char *name)
{
  callfromase_guard g(this);
  return dummy_global;
}

void
ase_dummy_script_engine::import_ase_global(const ase_variant& val)
{
  callfromase_guard g(this);
}

ase_variant
ase_dummy_script_engine::load_file(const char *glname, const char *filename)
{
  callfromase_guard g(this);
  return ase_variant();
}

ase_script_engine *
ase_dummy_script_engine::get_real_engine()
{
  return this;
}

}; // namespace asedummy

