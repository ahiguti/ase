
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#define _WIN32_DCOM
#include <objbase.h>

#include <ase/aseexcept.hpp>
#include <ase/aseutf16.hpp>
#include <ase/aseexcept.hpp>

#include "asecom_util.hpp"
#include "asecom_aseimpl.hpp"
#include "asecom_asepr.hpp"

#ifdef _DEBUG
#pragma comment(lib, "comsuppwd.lib")
#else
#pragma comment(lib, "comsuppw.lib")
#endif

#define DBG_TOASE(x)
#define DBG_COMPRX(x)
#define DBG_ASEPRX(x)
#define DBG_EXC(x)
#define DBG_MAP(x)
#define DBG_THR(x)

namespace asecom {

unsigned int ase_com_util::verbose = 0;

namespace {

DWORD tlskey = 0;

void
set_thr_initialized()
{
  TlsSetValue(tlskey, &set_thr_initialized);
}

void
reset_thr_initialized()
{
  TlsSetValue(tlskey, NULL);
}

bool
get_thr_initialized()
{
  return TlsGetValue(tlskey) != NULL;
}

HRESULT
init_com()
{
  /* return CoInitializeEx(NULL, COINIT_MULTITHREADED); */
  return CoInitialize(NULL);
}

}; // anonymous namespace

void
ase_com_util::init_main_thread()
{
  tlskey = TlsAlloc();
  if (tlskey == 0xFFFFFFFFUL) {
    ase_throw_error("ASECOM: TLS_OUT_OF_INDEX");
  }
  HRESULT hr = init_com();
  if (hr != S_OK && hr != S_FALSE) {
    TlsFree(tlskey);
    if (hr == RPC_E_CHANGED_MODE) {
      ase_throw_error(
	"ASECOM: main thread: initialized with a "
	"different concurrency model");
    }
    ase_throw_error("ASECOM: failed to initialize main thread");
  }
}

void
ase_com_util::uninit_main_thread()
{
  CoUninitialize();
  TlsFree(tlskey);
}

void
ase_com_util::init_child_thread()
{
  if (get_thr_initialized()) {
    return;
  }
  DBG_THR(fprintf(stderr, "coinit main\n"));
  HRESULT hr = init_com();
  if (hr != S_OK && hr != S_FALSE) {
    if (hr == RPC_E_CHANGED_MODE) {
      ase_throw_error(
	"ASECOM: main thread: initialized with a "
	"different concurrency model");
    }
    ase_throw_error("ASECOM: failed to initialize child thread");
  }
  set_thr_initialized();
}

void
ase_com_util::uninit_child_thread()
{
  if (get_thr_initialized()) {
    CoUninitialize();
  }
  reset_thr_initialized();
}

ase_string
ase_com_util::to_ase_string_from_bstr(BSTR str)
{
  wchar_t *const p = str;
  ase_size_type len = SysStringLen(str);
  return ase_string(reinterpret_cast<ase_ucchar *>(p), len);
}

ase_string
ase_com_util::to_ase_string_from_olestr(const OLECHAR *str)
{
  ase_size_type len = lstrlenW(str);
  return ase_string(reinterpret_cast<const ase_ucchar *>(str), len);
}

CComBSTR
ase_com_util::to_com_string(ase_com_script_engine *sc, const char *str,
  ase_size_type slen)
{
  ase_utf16_string s(str, slen);
  return CComBSTR(s.data());
}

CComBSTR
ase_com_util::to_com_string(ase_com_script_engine *sc, const ase_string& str)
{
  return to_com_string(sc, str.data(), str.size());
}

ase_variant
ase_com_util::to_ase_variant(ase_com_script_engine *sc, const VARIANT& val)
{
  switch (val.vt) {
  case VT_EMPTY:
  case VT_NULL:
    return ase_variant();
  case VT_BOOL:
    return ase_variant::create_boolean(val.boolVal != 0);
  case VT_INT:
    return ase_variant::create_int(val.intVal);
  case VT_UINT:
    return ase_variant::create_long(val.uintVal);
  case VT_I1:
    return ase_variant::create_int(val.cVal);
  case VT_I2:
    return ase_variant::create_int(val.iVal);
  case VT_I4:
    return ase_variant::create_long(val.lVal);
  case VT_I8:
    return ase_variant::create_long(val.llVal);
  case VT_UI1:
    return ase_variant::create_int(val.bVal);
  case VT_UI2:
    return ase_variant::create_int(val.uiVal);
  case VT_UI4:
    return ase_variant::create_long(val.ulVal);
  case VT_UI8:
    return ase_variant::create_long(val.ullVal);
  case VT_R4:
    return ase_variant::create_double(val.fltVal);
  case VT_R8:
    return ase_variant::create_double(val.dblVal);
  case VT_BSTR:
    {
      return to_ase_string_from_bstr(val.bstrVal).to_variant();
    }
  case VT_DISPATCH:
    {
      IDispatch *idisp = val.pdispVal;
      ase_com_aseproxy *asepr = get_aseproxy(sc, idisp);
      if (asepr != 0) {
        return asepr->value.get_value_synchronized();
      }
      return create_comproxy(sc, idisp);
    }
  default:
    break;
  }
  return ase_variant(); /* TODO: create wrapper */
}

CComVariant
ase_com_util::to_com_value(ase_com_script_engine *sc, const ase_variant& val)
{
  switch (val.get_type()) {
  case ase_vtype_void:
    return CComVariant();
  case ase_vtype_bool:
    return CComVariant(val.get_boolean());
  case ase_vtype_int:
    return CComVariant(val.get_int());
  case ase_vtype_long:
    {
      VARIANT v;
      v.vt = VT_I8;
      v.llVal = val.get_long();
      return v;
    }
  case ase_vtype_double:
    return CComVariant(val.get_double());
  case ase_vtype_string:
    return CComVariant(to_com_string(sc, val.get_string()));
  case ase_vtype_object:
    {
      if (val.get_impl_priv() == sc) {
        /* com proxy */
        ase_com_ase_variant_impl *cpr =
          static_cast<ase_com_ase_variant_impl *>(val.get_rep().p);
        IDispatch *idisp = cpr->get();
        return CComVariant(idisp);
      }
      return create_aseproxy(sc, val);
    }
  default:
    break;
  }
  return CComVariant();
}

ase_com_ase_variant_implnode *
ase_com_util::get_comproxy(ase_com_script_engine *sc, const ase_variant& val)
{
  if (val.get_impl_priv() == sc) {
    ase_com_ase_variant_implnode *cpr =
      static_cast<ase_com_ase_variant_implnode *>(val.get_rep().p);
    return cpr;
  }
  return 0;
}

ase_com_aseproxy *
ase_com_util::get_aseproxy(ase_com_script_engine *sc, IDispatch *idisp)
{
  if (ase_com_com_vtable(idisp) == sc->get_asepr_com_vtable_addr()) {
    ase_com_aseproxy *asepr = static_cast<ase_com_aseproxy *>(idisp);
    return asepr;
  }
  return 0;
}

ase_variant
ase_com_util::create_comproxy(ase_com_script_engine *sc, IDispatch *value)
{
  ase_com_ase_variant_implnode *p = new ase_com_ase_variant_implnode(
    ase_com_ase_variant_impl::init_arg(sc, value),
    *sc->get_com_endnode());
  DBG_COMPRX(fprintf(stderr, "ASECOM new comproxy %p\n", p));
  ase_variant rv = ase_variant::create_object(sc->get_variant_vtable(), p,
    "ComObject");
  return rv;
}

CComVariant
ase_com_util::create_aseproxy(ase_com_script_engine *sc,
  const ase_variant& value)
{
  CComVariant v;
  v.vt = VT_DISPATCH;
  v.pdispVal = new ase_com_aseproxy(
    ase_com_aseproxy::init_arg(sc, value),
    *sc->get_ase_endnode());
  DBG_ASEPRX(fprintf(stderr, "ASECOM new aseproxy %p\n", p));
  /* v.pdispVal has refcount == 1 and should not addref here */
  return v;
}

HRESULT
ase_com_util::handle_ase_exception(ase_com_script_engine *sc)
{
  try {
    ase_variant ex = ase_variant::create_from_active_exception();
    return E_UNEXPECTED;
  } catch (const std::bad_alloc&) {
    return E_OUTOFMEMORY;
  } catch (...) {
    return E_UNEXPECTED;
  }
}

ase_string
create_err_string(HRESULT hr, const char *mess)
{
  char buf[33];
  _snprintf_s(buf, 32, 32, "0x%x", hr);
  buf[32] = 0;
  return "ASECOM: " + ase_string(mess) + ": "
    + ase_string(buf, std::strlen(buf));
}

void
ase_com_util::check_com_error(IUnknown *iunk, const char *mess)
{
  if (!iunk) {
    ase_throw_runtime_exception(mess);
  }
}

void
ase_com_util::check_com_error(HRESULT hr, const char *mess)
{
  if (FAILED(hr)) {
    ase_throw_runtime_exception(create_err_string(hr, mess).data());
  }
}

struct auto_typeattr : private ase_noncopyable {
  auto_typeattr(ITypeInfo *ti) : tinfo(ti), value(0) {
    HRESULT hr = tinfo->GetTypeAttr(&value);
    ase_com_util::check_com_error(hr, "GetTypeAttr");
  }
  ~auto_typeattr() /* DCHK */ {
    if (value) {
      tinfo->ReleaseTypeAttr(value);
    }
  }
  TYPEATTR *get() const { return value; }
 private:
  CComPtr<ITypeInfo> tinfo;
  TYPEATTR *value;
};

struct auto_funcdesc : private ase_noncopyable {
  auto_funcdesc(ITypeInfo *ti, int i) : tinfo(ti), value(0) {
    HRESULT hr = tinfo->GetFuncDesc(i, &value);
    ase_com_util::check_com_error(hr, "GetFuncDesc");
  }
  ~auto_funcdesc() /* DCHK */ {
    if (value) {
      tinfo->ReleaseFuncDesc(value);
    }
  }
  FUNCDESC *get() const { return value; }
 private:
  CComPtr<ITypeInfo> tinfo;
  FUNCDESC *value;
};

struct auto_vardesc : private ase_noncopyable {
  auto_vardesc(ITypeInfo *ti, int i) : tinfo(ti), value(0) {
    HRESULT hr = tinfo->GetVarDesc(i, &value);
    ase_com_util::check_com_error(hr, "GetVarDesc");
  }
  ~auto_vardesc() /* DCHK */ {
    if (value) {
      tinfo->ReleaseVarDesc(value);
    }
  }
  VARDESC *get() const { return value; }
 private:
  CComPtr<ITypeInfo> tinfo;
  VARDESC *value;
};

IID
ase_com_util::find_iid(IUnknown *iunk, BSTR name, CComPtr<ITypeInfo>& tinfo_r)
{
  CComQIPtr<IDispatch> disp(iunk);
  check_com_error(disp, "IDispatch");
  CComPtr<ITypeLib> tlib;
  {
    CComPtr<ITypeInfo> ti;
    LCID lcid = LOCALE_SYSTEM_DEFAULT;
    check_com_error(disp->GetTypeInfo(0, lcid, &ti), "ITypeInfo");
    UINT idx = 0;
    check_com_error(ti->GetContainingTypeLib(&tlib, &idx), "ITypeLib");
  }
  UINT nicount = tlib->GetTypeInfoCount();
  for (UINT i = 0; i < nicount; ++i) {
    CComPtr<ITypeInfo> ti;
    check_com_error(tlib->GetTypeInfo(i, &ti), "GetTypeInfo");
    auto_typeattr ta(ti);
    if (ta.get()->typekind != TKIND_COCLASS) {
      continue;
    }
    for (UINT j = 0; j < ta.get()->cImplTypes; ++j) {
      HREFTYPE reftype = 0;
      check_com_error(ti->GetRefTypeOfImplType(j, &reftype),
        "GetRefTypeOfImplType");
      CComPtr<ITypeInfo> rti;
      check_com_error(ti->GetRefTypeInfo(reftype, &rti), "GetTypeInfo");
      CComBSTR bstrdoc;
      check_com_error(rti->GetDocumentation(MEMBERID_NIL, &bstrdoc,
        NULL, NULL, NULL), "GetDocumentation");
      ase_string sdoc = to_ase_string_from_bstr(bstrdoc);
      ase_string sname = to_ase_string_from_bstr(name);
      if (sdoc == sname) {
        auto_typeattr rta(rti);
        IID iid = rta.get()->guid;
        tinfo_r = rti;
        return iid;
      }
    }
  }
  ase_throw_invalid_arg("ASECOM: failed to find typeinfo");
  return IID();
}

IID
ase_com_util::get_source(IUnknown *iunk, BSTR name,
  CComPtr<IConnectionPoint>& cp_r, CComPtr<ITypeInfo>& tinfo_r)
{
  LCID lcid = LOCALE_SYSTEM_DEFAULT;
  ase_string s = to_ase_string_from_bstr(name);
  IID iid = IID_IUnknown;
  if (isalpha(s.data()[0])) {
    CComPtr<ITypeInfo> ti;
    iid = find_iid(iunk, name, ti);
  } else {
    check_com_error(IIDFromString(name, &iid), "IIDFromString");
  }
  /* get IConnectionPoint */
  CComQIPtr<IConnectionPointContainer, &IID_IConnectionPointContainer>
    cpc(iunk);
  check_com_error(cpc, "IConnectionPointContainer");
  CComPtr<IConnectionPoint> cp;
  cpc->FindConnectionPoint(iid, &cp);
  check_com_error(cp, "FindConnectionPoint");  
  /* get ITypeInfo */
  CComQIPtr<IDispatch, &IID_IDispatch> idisp(iunk);
  check_com_error(idisp, "IDispatch");
  CComPtr<ITypeInfo> iti;
  check_com_error(idisp->GetTypeInfo(0, lcid, &iti),
    "GetTypeInfo");
  CComPtr<ITypeLib> itl;
  UINT idx = 0;
  check_com_error(iti->GetContainingTypeLib(&itl, &idx),
    "GetContainingTypeLib");
  CComPtr<ITypeInfo> rti;
  check_com_error(itl->GetTypeInfoOfGuid(iid, &rti),
    "GetTypeInfoOfGuid");
  cp_r = cp;
  tinfo_r = rti;
  return iid;
}

IID
ase_com_util::get_default_source(IUnknown *iunk,
  CComPtr<IConnectionPoint>& cp_r, CComPtr<ITypeInfo>& tinfo_r)
{
  bool got_iid = false;
  CComPtr<IProvideClassInfo2> cinfo2;
  HRESULT hr = iunk->QueryInterface(IID_IProvideClassInfo2,
    (void **)&cinfo2);
  if (hr != S_OK && hr != E_NOINTERFACE) {
    check_com_error(hr, "IProvideClassInfo2");
  }
  IID iid = IID_IUnknown;
  if (hr == S_OK) {
    check_com_error(cinfo2->GetGUID(GUIDKIND_DEFAULT_SOURCE_DISP_IID,
      &iid), "GetGUID");
    got_iid = true;
  }
  CComQIPtr<IProvideClassInfo, &IID_IProvideClassInfo> cinfo(iunk);
  check_com_error(cinfo, "IProvideClassInfo");
  CComPtr<ITypeInfo> tinfo;
  check_com_error(cinfo->GetClassInfo(&tinfo), "GetClassInfo");
  auto_typeattr tattr(tinfo);
  int i = 0;
  for (i = 0; i < tattr.get()->cImplTypes; ++i) {
    int iflags = 0;
    check_com_error(tinfo->GetImplTypeFlags(i, &iflags), "GetImplTypeFlags");
    if (got_iid) {
      CComPtr<ITypeInfo> rtinfo;
      HREFTYPE hrefty = 0;
      check_com_error(tinfo->GetRefTypeInfo(hrefty, &rtinfo),
        "GetRefTypeInfo");
      auto_typeattr rtattr(rtinfo);
      IID riid = rtattr.get()->guid;
      if (IsEqualIID(iid, riid)) {
        break;
      }
    } else {
      if ((iflags & IMPLTYPEFLAG_FDEFAULT) != 0 &&
        (iflags & IMPLTYPEFLAG_FSOURCE) != 0) {
        break;
      }
    }
  }
  if (i == tattr.get()->cImplTypes) {
    ase_throw_invalid_arg("ASECOM: failed to get default event source");
  }
  HREFTYPE hrefty = 0;
  check_com_error(tinfo->GetRefTypeOfImplType(i, &hrefty),
    "GetRefTypeOfImpLType");
  CComPtr<ITypeInfo> rtinfo;
  check_com_error(tinfo->GetRefTypeInfo(hrefty, &rtinfo),
    "GetRefTypeInfo");
  auto_typeattr rtattr(rtinfo);
  iid = rtattr.get()->guid;
  CComQIPtr<IConnectionPointContainer, &IID_IConnectionPointContainer>
    cpc(iunk);
  check_com_error(cpc, "IConnectionPointContainer");
  CComPtr<IConnectionPoint> cp;
  cpc->FindConnectionPoint(iid, &cp);
  check_com_error(cp, "FindConnectionPoint");
  cp_r = cp;
  tinfo_r = rtinfo;
  return iid;
}

void
ase_com_util::create_idmap(ITypeInfo *ti,
  std::map<DISPID, ase_string>& to_str_r,
  std::map<ase_string, DISPID>& to_id_r)
{
  auto_typeattr tattr(ti);
  const TYPEATTR *const t = tattr.get();
  int i = 0;
  for (i = 0; i < t->cFuncs; ++i) {
    auto_funcdesc fdesc(ti, i);
    MEMBERID id = fdesc.get()->memid;
    CComBSTR bs;
    UINT c = 0;
    check_com_error(ti->GetNames(id, &bs, 1, &c), "GetNames");
    ase_string s = to_ase_string_from_bstr(bs);
    to_str_r[id] = s;
    to_id_r[s] = id;
    ASE_COM_VERBOSE_MAP(fprintf(stderr, "COM: IDMAP %u %s\n", id, s.data()));
  }
}

DWORD
ase_com_util::connection_advise(IConnectionPoint *cp, IUnknown *sink)
{
  DWORD cookie = 0;
  check_com_error(cp->Advise(sink, &cookie), "Advise");
  return cookie;
}

void
ase_com_util::connection_unadvise(IConnectionPoint *cp, DWORD cookie)
{
  check_com_error(cp->Unadvise(cookie), "Unadvise");
}

}; // namespace asecom

