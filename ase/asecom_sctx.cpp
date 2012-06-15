
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <ase/aseutf16.hpp>
#include <ase/asestrmap.hpp>
#include <ase/asefuncobj.hpp>
#include <ase/aseexcept.hpp>

#include "asecom_util.hpp"
#include "asecom_aseimpl.hpp"
#include "asecom_asepr.hpp"

#define DBG_IMPORT(x)
#define DBG_STACK(x)
#define DBG_CTX(x)
#define DBG_EVAL(x)
#define DBG(x)

namespace asecom {

namespace {

ase_variant
create_instance(ase_com_script_engine *sc, const char *name,
  ase_size_type namelen, bool getobj)
{
  CComPtr<IUnknown> unknown;
  DBG(fprintf(stderr, "Create: [%s]\n", name));
  ase_utf16_string wstr(name, namelen);
  HRESULT hr = 0;
  if (getobj) {
    HRESULT hr = CoGetObject(wstr.data(), NULL, IID_IUnknown,
      (void **)&unknown);
  } else {
    hr = unknown.CoCreateInstance(wstr.data(), NULL,
      CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER);
  }
  if (hr != S_OK) {
    DBG(fprintf(stderr, "CoCreateInstance failed"));
    ase_string s = "ASECOM: failed to create COM object: " +
      ase_string(name, namelen);
    ase_throw_invalid_arg(s.data());
  }
  CComPtr<IDispatch> cp;
  hr = unknown.QueryInterface(&cp);
  if (hr != S_OK) {
    ase_string s = "ASECOM: failed to get IDispatch: " +
      ase_string(name, namelen);
    ase_throw_invalid_arg(s.data());
  }
  DBG(fprintf(stderr, "IDispatch: created\n"));
  return ase_com_util::create_comproxy(sc, cp);
}

struct func_create_instance {
  func_create_instance(ase_com_script_engine *sc, bool getf)
    : script_engine(sc), getobj_flag(getf) { }
  ase_variant operator ()(const ase_variant *args, ase_size_type nargs) {
    ase_string s;
    if (nargs >= 1) {
      s = args[0].get_string();
    }
    return create_instance(script_engine, s.data(), s.size(), getobj_flag);
  }
  ase_com_script_engine *script_engine;
  bool getobj_flag;
};

struct func_connect {
  func_connect(ase_com_script_engine *sc) : script_engine(sc) { }
  ase_variant operator ()(const ase_variant *args, ase_size_type nargs) {
    if (nargs < 2) {
      ase_throw_missing_arg("ASECOM: Usage: Connect(comobject, sink)");
    }
    ase_com_ase_variant_implnode *cpr = ase_com_util::get_comproxy(
      script_engine, args[0]);
    ase_string interface_name;
    if (nargs >= 3) {
      interface_name = args[2].get_string();
    }
    if (cpr == 0) {
      ase_throw_type_mismatch("ASECOM: Usage: Connect(comobject, sink)");
    }
    CComVariant sink = ase_com_util::to_com_value(script_engine, args[1]);
    if (sink.vt != VT_DISPATCH) {
      ase_throw_type_mismatch("ASECOM: Usage: Connect(comobject, sink)");
    }
    cpr->value.connect(script_engine, sink.pdispVal, interface_name);
    return ase_variant();
  }
  ase_com_script_engine *script_engine;
};

struct func_dispatch_message {
  func_dispatch_message(ase_com_script_engine *sc)
    : script_engine(sc) { }
  ase_variant operator ()(const ase_variant *args, ase_size_type nargs) {
    ase_int tout = 0;
    if (nargs >= 1) {
      tout = args[0].get_int();
    }
    DWORD r = MsgWaitForMultipleObjects(0, NULL, FALSE,
      (nargs >= 1) ? tout : INFINITE, QS_ALLINPUT);
    if (r == WAIT_OBJECT_0) {
      MSG msg;
      if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        DispatchMessage(&msg);
      }
    }
    return ase_variant();
  }
  ase_com_script_engine *script_engine;
};

}; // anonymous namespace

ase_com_script_engine::ase_com_script_engine()
  : variant_vtable(ase_variant_impl<ase_com_ase_variant_impl>::vtable),
    refcount(1), com_endnode(0), ase_endnode(0),
    asepr_com_vtable_addr(0)
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
    ase_throw_error("ASECOM: failed to initialize com engine");
  }
}

ase_com_script_engine::~ase_com_script_engine() /* DCHK */
{
  cleanup();
}

void
ase_com_script_engine::destroy()
{
  delete this;
}

void
ase_com_script_engine::init()
{
  variant_vtable.impl_private_data = this;
  variant_vtable.vtopt = ase_vtoption_multithreaded;
  com_endnode = new ase_com_ase_variant_implnode(
    ase_com_ase_variant_impl::init_arg(this, 0));
  ase_endnode = new ase_com_aseproxy(
    ase_com_aseproxy::init_arg(this, ase_variant()));
  asepr_com_vtable_addr = ase_com_com_vtable(ase_endnode);
  comglobal = ase_new_string_map();
  comglobal.set("CreateObject", ase_new_function_object(
    func_create_instance(this, false)));
  comglobal.set("GetObject", ase_new_function_object(
    func_create_instance(this, true)));
  comglobal.set("Connect", ase_new_function_object(
    func_connect(this)));
  comglobal.set("DispatchMessage", ase_new_function_object(
    func_dispatch_message(this)));
  comglobal = ase_string_map_unmodifiable(comglobal);
}

void
ase_com_script_engine::cleanup()
{
  dispose_proxies();
  comglobal = ase_variant();
  delete ase_endnode;
  delete com_endnode;
}

void
ase_com_script_engine::child_thread_initialized()
{
  ase_com_util::init_child_thread();
}

void
ase_com_script_engine::child_thread_terminated()
{
  ase_com_util::uninit_child_thread();
}

void
ase_com_script_engine::dispose_proxies()
{
  DBG(fprintf(stderr, "com: disposeExternal\n"));
  if (com_endnode) {
    ase_com_ase_variant_implnode *p = com_endnode;
    do {
      p->value.dispose();
      p = p->get_next();
    } while (p != com_endnode);
  }
  if (ase_endnode) {
    ase_com_aseproxy_privnode *p = ase_endnode;
    do {
      p->value.dispose_synchronized(this);
      p = p->get_next();
    } while (p != ase_endnode);
  }
  DBG(fprintf(stderr, "com: disposeExternal done\n"));
}

const char *const *
ase_com_script_engine::get_global_names()
{
  static const char *const globals[] = { "COM", 0 };
  return globals;
}

ase_variant
ase_com_script_engine::get_global_object(const char *name)
{
  return comglobal;
}

void
ase_com_script_engine::import_ase_global(const ase_variant& val)
{
}

ase_variant
ase_com_script_engine::load_file(const char *glname, const char *fname)
{
  return ase_variant();
}

ase_script_engine *
ase_com_script_engine::get_real_engine()
{
  return this;
}

}; // namespace asecom

