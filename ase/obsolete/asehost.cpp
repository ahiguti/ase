
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <cstring>
#include <iostream>
#include <map>
#include <stdexcept>

#include <ase/asehost.hpp>
#include <ase/aseatomic.hpp>
#include <ase/asestrmap.hpp>

#define DBG(x)
#define DBG_WEAK(x)

struct ASE_HostService::impl_type {

  impl_type() : hostlibs_fixed(false) { }

  typedef std::map<ase_string, ASE_VariantFactory *> hostlibs_type;
  hostlibs_type hostlibs;
  bool hostlibs_fixed;

  struct ase_host_script_engine;

};

struct ASE_HostService::impl_type::ase_host_script_engine
  : public ase_script_engine {

  ase_host_script_engine(impl_type *impl, ase_unified_engine_ref& ue) {
    egs = ase_new_string_map();
    egs_unmod = ase_string_map_unmodifiable(egs);
    host = ase_new_string_map();
    host_unmod = ase_string_map_unmodifiable(host);
    egs.set("Host", host_unmod);
    impl_type::hostlibs_type::const_iterator i;
    for (i = impl->hostlibs.begin(); i != impl->hostlibs.end(); ++i) {
      ASE_VariantFactory *vf = i->second;
      ase_variant v = vf->NewVariant(ue, egs_unmod);
      DBG(fprintf(stderr, "host setprop %s %s\n", i->first.c_str(),
	v.get_string().data()));
      ase_string s(i->first.data(), i->first.size());
      host.set_property(s, v);
    }
  }
  void destroy() {
    delete this;
  }
  void child_thread_initialized() {
  }
  void child_thread_terminated() {
  }
  void ForceGC() {
  }
  void wait_threads() {
  }
  void dispose_proxies() {
    egs = ase_variant();
    egs_unmod = ase_variant();
  }
  const char *const *get_global_names() {
    static const char *const globals[] = { "Host", 0 };
    return globals;
  }
  ase_variant get_global_object(const char *name) {
    ase_string s(name);
    if (s == "Host") {
      return host_unmod;
    }
    return ase_variant();
  }
  void import_global(const char *name, const ase_variant& val) {
    egs.set_property(name, std::strlen(name), val);
  }
  ase_variant LoadFile(const char *glname, const char *filename) {
    return ase_variant();
  }

 private:

  ase_variant egs;
  ase_variant egs_unmod;
  ase_variant host;
  ase_variant host_unmod;

};

ASE_HostService::ASE_HostService()
  : impl(new impl_type())
{
}

ASE_HostService::~ASE_HostService() /* DCHK */
{
  for (impl_type::hostlibs_type::iterator i = impl->hostlibs.begin();
    i != impl->hostlibs.end(); ++i) {
    i->second->destroy();
  }
  impl->hostlibs.clear();
  delete impl;
}

void
ASE_HostService::destroy()
{
  delete this;
}

void
ASE_HostService::new_script_engine(ase_unified_engine_ref& ue,
  ase_script_engine_ptr& ap_r)
{
  impl->hostlibs_fixed = true;
  ap_r.reset(new impl_type::ase_host_script_engine(impl, ue));
}

void
ASE_HostService::RegisterLibrary(const ase_string& name,
  ASE_NewVariantFactoryFunc *func)
{
  if (impl->hostlibs_fixed) {
    throw std::logic_error(
      "HostRegisterLibrary: host service is started already");
  }
  ASE_VariantFactoryPtr avf;
  func(avf);
  impl->hostlibs[name] = avf.get();
  avf.release();
}

