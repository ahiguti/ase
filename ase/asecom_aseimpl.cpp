
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <ase/aseflags.hpp>
#include <ase/asebindmethod.hpp>
#include <ase/aseexcept.hpp>

#include "asecom_util.hpp"
#include "asecom_asepr.hpp"
#include "asecom_aseimpl.hpp"

#define DBG(x)
#define DBG_DEL(x)
#define DBG_RC(x)
#define DBG_GET(x)

namespace asecom {

namespace {

struct ase_com_argshook : public ase_noncopyable {
  ase_com_argshook(VARIANTARG *as, ase_size_type l) : args(as), len(l) {
    for (ase_size_type i = 0; i < len; ++i) {
      VariantInit(args + i);
    }
  }
  ~ase_com_argshook() /* DCHK */ {
    for (ase_size_type i = 0; i < len; ++i) {
      VariantClear(args + i);
    }
  }
  VARIANTARG *args;
  ase_size_type len;
};

struct auto_excepinfo : public ase_noncopyable {
  auto_excepinfo() {
    std::memset(&value, 0, sizeof(value));
  }
  ~auto_excepinfo() /* DCHK */ {
    SysFreeString(value.bstrSource);
    SysFreeString(value.bstrDescription);
    SysFreeString(value.bstrHelpFile);
  }
  EXCEPINFO value;
};

DISPID
ase_com_idisp_getid(ase_com_script_engine *sc, IDispatch *idisp,
  const char *name, ase_size_type namelen)
{
  DISPID dispid = 0;
  CComBSTR wstr = ase_com_util::to_com_string(sc, name, namelen);
  OLECHAR *mname = wstr;
  HRESULT hr = idisp->GetIDsOfNames(IID_NULL, &mname, 1,
    LOCALE_SYSTEM_DEFAULT, &dispid);
  if (hr != S_OK) {
    ase_string s = "property not found: " + ase_string(name, namelen);
    ase_throw_illegal_operation(s.data());
  }
  return dispid;
}

ase_string
ase_com_error_string(HRESULT hr)
{
  const char *estr = 0;
  switch (hr) {
  case S_OK:
    estr = "ok";
    break;
  case E_INVALIDARG:
    estr = "invalidarg";
    break;
  case DISP_E_EXCEPTION:
    estr = "exception";
    break;
  case DISP_E_MEMBERNOTFOUND:
    estr = "membernotfound";
    break;
  case DISP_E_PARAMNOTOPTIONAL:
    estr = "paramnotoptional";
    break;
  case DISP_E_BADPARAMCOUNT:
    estr = "badparamcount";
    break;
  case DISP_E_BADVARTYPE:
    estr = "badvartype";
    break;
  case DISP_E_NONAMEDARGS:
    estr = "nonamedargs";
    break;
  case DISP_E_OVERFLOW:
    estr = "overflow";
    break;
  case DISP_E_TYPEMISMATCH:
    estr = "typemismatch";
    break;
  case DISP_E_UNKNOWNINTERFACE:
    estr = "unknowninterface";
    break;
  case DISP_E_UNKNOWNLCID:
    estr = "unknownlcid";
    break;
  }
  if (estr) {
    return ase_string(estr);
  }
  return ase_variant((ase_int)hr).get_string();
}

ase_variant
ase_com_idisp_invoke(ase_com_script_engine *sc, IDispatch *idisp,
  DISPID dispid, WORD flags, const ase_variant *args, ase_size_type nargs,
  HRESULT& err_r)
{
  err_r = false;
  auto_excepinfo exinfo;
  CComVariant result;
  unsigned int aerr = 0;
  DISPPARAMS params;
  DISPID namedargput = DISPID_PROPERTYPUT;
  std::memset(&params, 0, sizeof(params));
  params.cArgs = static_cast<unsigned int>(nargs);
  params.cNamedArgs = 0;
  params.rgvarg = static_cast<VARIANTARG *>(
    _alloca(nargs * sizeof(VARIANTARG)));
  ase_com_argshook(params.rgvarg, nargs);
  
  for (ase_size_type i = 0; i < nargs; ++i) {
    CComVariant v = ase_com_util::to_com_value(sc, args[i]);
    v.Detach(params.rgvarg + nargs - i - 1);
  }
  if ((flags & DISPATCH_PROPERTYPUT) != 0 && nargs == 1) {
    params.cNamedArgs = 1;
    params.rgdispidNamedArgs = &namedargput;
  }
  HRESULT hr = idisp->Invoke(dispid, IID_NULL, LOCALE_SYSTEM_DEFAULT, flags,
    &params, &result, &exinfo.value, &aerr);
  ASE_COM_VERBOSE_COMCALL(fprintf(stderr,
    "COM: method invoke dispid=%d hr=%x\n", dispid, hr));
  if (hr != S_OK) {
    ase_string exstr;
    if (hr == DISP_E_EXCEPTION) {
      exstr = "\tASECOM: "
        + ase_com_util::to_ase_string_from_bstr(exinfo.value.bstrSource)
        + ": "
        + ase_com_util::to_ase_string_from_bstr(exinfo.value.bstrDescription)
        ;
    } else {
      exstr = ase_com_error_string(hr);
    }
    err_r = hr;
    ASE_COM_VERBOSE_COMCALL(fprintf(stderr, "COM: exstr=%s\n", exstr.data()));
    return exstr.to_variant();
  } else {
    DBG(fprintf(stderr, "method invoke done\n"));
    ase_variant r = ase_com_util::to_ase_variant(sc, result);
    ASE_COM_VERBOSE_COMCALL(fprintf(stderr, "COM: vt=%d rval=%s\n",
      result.vt, r.get_string().data()));
    return r;
  }
}

}; // anonymous namespace

ase_com_ase_variant_impl::ase_com_ase_variant_impl(const init_arg& arg)
  : refcount(1), value(0), connections(0)
{
  value = arg.value;
  if (value) {
    value->AddRef();
  }
  ASE_DEBUG(ase_com_variant_impl_count_constr++);
}

ase_com_ase_variant_impl::~ase_com_ase_variant_impl() /* DCHK */
{
  dispose();
  ASE_DEBUG(ase_com_variant_impl_count_destr++);
}

void
ase_com_ase_variant_impl::add_ref(const ase_variant& self)
{
  ase_com_ase_variant_implnode *const selfp = get(self);
  DBG_RC(fprintf(stderr, "ASECOM addref comproxy %p %lu++\n",
    selfp, selfp->value.refcount));
  selfp->value.refcount++;
}

void
ase_com_ase_variant_impl::release(const ase_variant& self)
{
  ase_com_ase_variant_implnode *const selfp = get(self);
  DBG_RC(fprintf(stderr, "ASECOM release comproxy %p %lu--\n",
    selfp, selfp->value.refcount));
  if ((selfp->value.refcount--) == 1) {
    ase_variant::debug_on_destroy_object(self, "ComObject");
    DBG_DEL(fprintf(stderr, "ASECOM destroy comproxy %p\n", selfp));
    delete selfp;
  }
}

void
ase_com_ase_variant_impl::dispose()
{
  if (connections) {
    ase_size_type i;
    for (i = 0; i < connections->size(); ++i) {
      connection_type& c = (*connections)[i];
      if (c.source != 0) {
        try {
          ase_com_util::connection_unadvise(c.source, c.cookie);
        } catch (...) {
        }
      }
    }
    delete connections;
    connections = 0;
  }
  if (value) {
    value->Release();
    value = 0;
  }
}

void
ase_com_ase_variant_impl::connect(ase_com_script_engine *sc, IDispatch *sink,
  const ase_string& interface_name)
{
  if (connections == 0) {
    connections = new connections_type();
  }
  connections->push_back(connection_type());
  CComPtr<IConnectionPoint> cp;
  CComPtr<ITypeInfo> ti;
  IID iid = IID_IUnknown;
  if (interface_name.empty()) {
    iid = ase_com_util::get_default_source(value, cp, ti);
  } else {
    CComBSTR bname = ase_com_util::to_com_string(sc, interface_name.data(),
      interface_name.size());
    iid = ase_com_util::get_source(value, bname, cp, ti);
  }
  ase_com_aseproxy *ap = ase_com_util::get_aseproxy(sc, sink);
  if (ap == 0) {
    ase_throw_type_mismatch("ASECOM: sink is not an ASE object");
  }
  ap->value.set_event_sink_synchronized(iid, ti);
  DWORD cookie = ase_com_util::connection_advise(cp, sink);
  connections->back().source = cp;
  connections->back().cookie = cookie;
}

ase_int
ase_com_ase_variant_impl::get_method_id(const ase_variant& self,
  const char *name, ase_size_type namelen)
{
  ase_com_script_engine *sc = getctx(self);
  ase_com_ase_variant_implnode *selfp = get(self);
  IDispatch *idisp = selfp->value.get();
  if (idisp == 0) {
    return ase_method_id_not_found;
  }
  DISPID dispid = ase_com_idisp_getid(sc, idisp, name, namelen);
  return dispid;
}

ase_int
ase_com_ase_variant_impl::get_attributes(const ase_variant& self)
{
  return 0;
}

bool
ase_com_ase_variant_impl::get_boolean(const ase_variant& self)
{
  return false;
}

ase_int
ase_com_ase_variant_impl::get_int(const ase_variant& self)
{
  return 0;
}

ase_long
ase_com_ase_variant_impl::get_long(const ase_variant& self)
{
  return 0;
}

double
ase_com_ase_variant_impl::get_double(const ase_variant& self)
{
  return 0.0;
}

ase_string
ase_com_ase_variant_impl::get_string(const ase_variant& self)
{
  return ase_string("[ComObject]");
}

bool
ase_com_ase_variant_impl::is_same_object(const ase_variant& self,
  const ase_variant& value)
{
  ase_com_script_engine *sc = getctx(self);
  ase_com_ase_variant_implnode *selfp = get(self);
  IDispatch *idisp = selfp->value.get();
  ase_com_ase_variant_implnode *ovp = ase_com_util::get_comproxy(sc, value);
  IDispatch *idpo = ovp->value.get();
  return (idisp == idpo);
}

ase_variant
ase_com_ase_variant_impl::get_property(const ase_variant& self,
  const char *name, ase_size_type namelen)
{
  DBG_GET(fprintf(stderr, "COM: GetProp %s\n", name));
  ase_int id = get_method_id(self, name, namelen);
  if (id < 0) {
    DBG_GET(fprintf(stderr, "COM: GetProp %s notfound\n", name));
    return ase_variant();
  }
  DBG_GET(fprintf(stderr, "COM: GetProp %s id=%d\n", name, id));
  ase_com_script_engine *sc = getctx(self);
  ase_com_ase_variant_implnode *selfp = get(self);
  IDispatch *idisp = selfp->value.get();
  HRESULT err = 0;
  ase_variant r = ase_com_idisp_invoke(sc, idisp, id,
    DISPATCH_PROPERTYGET, 0, 0, err);
  DBG_GET(fprintf(stderr, "COM: GetProp %s id=%d err=%x\n", name, id, err));
  if (err == S_OK) {
    return r;
  }
  if (err == DISP_E_MEMBERNOTFOUND || err == DISP_E_BADPARAMCOUNT ||
    err == E_INVALIDARG) {
    return ase_new_variant_bind_method_id(self, id);
  }
  return r;
}

void
ase_com_ase_variant_impl::set_property(const ase_variant& self,
  const char *name, ase_size_type namelen, const ase_variant& value)
{
  ase_com_script_engine *sc = getctx(self);
  ase_com_ase_variant_implnode *selfp = get(self);
  IDispatch *idisp = selfp->value.get();
  if (idisp == 0) {
    return;
  }
  DISPID dispid = ase_com_idisp_getid(sc, idisp, name, namelen);
  HRESULT err = 0;
  ase_variant r = ase_com_idisp_invoke(sc, idisp, dispid,
    DISPATCH_PROPERTYPUT, &value, 1, err);
  if (err) {
    throw r;
  }
}

void
ase_com_ase_variant_impl::del_property(const ase_variant& self,
  const char *name, ase_size_type namelen)
{
}

ase_variant
ase_com_ase_variant_impl::get_element(const ase_variant& self, ase_int index)
{
  return ase_variant();
}

void
ase_com_ase_variant_impl::set_element(const ase_variant& self, ase_int index,
  const ase_variant& value)
{
}

ase_int
ase_com_ase_variant_impl::get_length(const ase_variant& self)
{
  return -1;
}

void
ase_com_ase_variant_impl::set_length(const ase_variant& self, ase_int len)
{
}

ase_variant
ase_com_ase_variant_impl::invoke_by_name(const ase_variant& self,
  const char *name, ase_size_type namelen, const ase_variant *args,
  ase_size_type nargs)
{
  ase_com_script_engine *sc = getctx(self);
  ase_com_ase_variant_implnode *selfp = get(self);
  IDispatch *idisp = selfp->value.get();
  if (idisp == 0) {
    return ase_variant();
  }
  DISPID dispid = ase_com_idisp_getid(sc, idisp, name, namelen);
  HRESULT err = 0;
  ase_variant r = ase_com_idisp_invoke(sc, idisp, dispid,
    DISPATCH_PROPERTYGET | DISPATCH_METHOD, args, nargs, err);
  if (err) {
    throw r;
  }
  return r;
}

ase_variant
ase_com_ase_variant_impl::invoke_by_id(const ase_variant& self, ase_int id,
  const ase_variant *args, ase_size_type nargs)
{
  ase_com_script_engine *sc = getctx(self);
  ase_com_ase_variant_implnode *selfp = get(self);
  IDispatch *idisp = selfp->value.get();
  if (idisp == 0) {
    return ase_variant();
  }
  DISPID dispid = id;
  HRESULT err = 0;
  ase_variant r = ase_com_idisp_invoke(sc, idisp, dispid,
    DISPATCH_PROPERTYGET | DISPATCH_METHOD, args, nargs, err);
  if (err) {
    throw r;
  }
  return r;
}

ase_variant
ase_com_ase_variant_impl::invoke_self(const ase_variant& self,
  const ase_variant *args, ase_size_type nargs)
{
  if (nargs < 1) {
    ase_throw_missing_arg("ASECOM: missing method name");
  }
  ase_string name = args[0].get_string();
  return invoke_by_name(self, name.data(), name.size(), args + 1, nargs - 1);
}

ase_variant
ase_com_ase_variant_impl::get_enumerator(const ase_variant& self)
{
  return ase_variant();
}

ase_string
ase_com_ase_variant_impl::enum_next(const ase_variant& self, bool& hasnext_r)
{
  hasnext_r = false;
  return ase_string();
}

}; // namespace asecom

