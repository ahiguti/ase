
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include "asecom_util.hpp"
#include "asecom_asepr.hpp"

#include <ase/aseexcept.hpp>

#define DBG(x)
#define DBG_DEL(x)
#define DBG_ARR(x)
#define DBG_COM(x)

namespace asecom {

ase_com_aseproxy_priv::ase_com_aseproxy_priv(const init_arg& ini)
  : script_engine(ini.script_engine), value(ini.valueref),
    used_as_unconnected(false)
{
  DBG(fprintf(stderr, "ASEPROXY constr %p\n", this));
  ASE_DEBUG(ase_com_asepr_count_constr++);
}

ase_com_aseproxy_priv::~ase_com_aseproxy_priv() /* DCHK */
{
  DBG(fprintf(stderr, "ASEPROXY destr %p\n", this));
  ASE_DEBUG(ase_com_asepr_count_destr++);
}

void
ase_com_aseproxy_priv::dispose_synchronized(const ase_com_script_engine *sc)
{
  ase_variant v;
  {
    ase_mutex_guard<ase_fast_mutex> g(mutex);
    value.swap(v);
  }
  /* v is released here */
}

bool
ase_com_aseproxy_priv::get_dispid_synchronized(const ase_string& name,
  DISPID& id_r)
{
  ase_mutex_guard<ase_fast_mutex> g(mutex);
  if (event_sink.get()) {
    std::map<ase_string, DISPID>::const_iterator iter =
      event_sink->to_id.find(name);
    if (iter == event_sink->to_id.end()) {
      return false;
    }
    id_r = iter->second;
    return true;
  }
  ase_size_type i = 0;
  for (i = 0; i < ids.size(); ++i) {
    if (ids[i] == name) {
      id_r = static_cast<DISPID>(i);
      return true;
    }
  }
  used_as_unconnected = true;
  ids.push_back(name);
  id_r = static_cast<DISPID>(i);
  return true;
}

bool
ase_com_aseproxy_priv::get_methodname_synchronized(DISPID id,
  ase_string& name_r)
{
  ase_mutex_guard<ase_fast_mutex> g(mutex);
  if (event_sink.get()) {
    std::map<DISPID, ase_string>::const_iterator iter =
      event_sink->to_str.find(id);
    if (iter == event_sink->to_str.end()) {
      return false;
    }
    name_r = iter->second;
    return true;
  }
  if (id < 0) {
    return false;
  }
  ase_size_type i = static_cast<ase_size_type>(id);
  if (i >= ids.size()) {
    return false;
  }
  name_r = ids[i];
  return true;
}

void
ase_com_aseproxy_priv::set_event_sink_synchronized(IID iid,
  const CComPtr<ITypeInfo>& ti)
{
  ase_mutex_guard<ase_fast_mutex> g(mutex);
  if (event_sink.get()) {
    ase_throw_illegal_operation("ASECOM: object is already connected");
  }
  if (used_as_unconnected) {
    ase_throw_illegal_operation(
      "ASECOM: already used as an unconnected object");
  }
  event_sink.reset(new event_sink_type(iid, ti));
  ase_com_util::create_idmap(ti, event_sink->to_str, event_sink->to_id);
}

ase_com_aseproxy::ase_com_aseproxy(const init_arg& ini)
  : ase_com_aseproxy_privnode(ini), com_refcount(1)
{
}

ase_com_aseproxy::ase_com_aseproxy(const init_arg& ini,
  ase_com_aseproxy_privnode& node)
  : ase_com_aseproxy_privnode(ini, node), com_refcount(1)
{
}

ase_com_aseproxy::~ase_com_aseproxy() /* DCHK */
{
}

STDMETHODIMP
ase_com_aseproxy::QueryInterface(REFIID iid, void **ppvObject)
{
  DBG_COM(fprintf(stderr, "wrap: %p QI\n", this));
  if (ppvObject == NULL) {
    return E_POINTER;
  }
  if (IsEqualIID(iid, IID_IUnknown)) {
    IUnknown *pu = this;
    pu->AddRef();
    *ppvObject = pu;
    return S_OK;
  } else if (IsEqualIID(iid, IID_IDispatch)) {
    IDispatch *pd = this;
    pd->AddRef();
    *ppvObject = pd;
    return S_OK;
  } else {
    IID sinkiid = value.get_event_sink_iid_synchronized();
    if (IsEqualIID(iid, sinkiid)) {
      IDispatch *pd = this;
      pd->AddRef();
      *ppvObject = pd;
      return S_OK;
    }
  }
  *ppvObject = 0;
  return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE
ase_com_aseproxy::AddRef(void)
{
  DBG_COM(fprintf(stderr, "wrap: %p add_ref\n", this));
  com_refcount++;
  return com_refcount;
}

ULONG STDMETHODCALLTYPE
ase_com_aseproxy::Release(void)
{
  DBG_COM(fprintf(stderr, "wrap: %p release\n", this));
  if ((com_refcount--) == 1) {
    DBG_DEL(fprintf(stderr, "ASECOM destroy aseproxy %p\n", this));
    delete this;
    return 0;
  }
  return com_refcount;
}

STDMETHODIMP
ase_com_aseproxy::GetTypeInfoCount(unsigned int FAR *pctinfo)
{
  DBG_COM(fprintf(stderr, "wrap: %p GetTypeInfoCount\n", this));
  if (pctinfo == NULL) {
    return E_POINTER;
  }
  *pctinfo = 0;
  return S_OK;
}

STDMETHODIMP
ase_com_aseproxy::GetTypeInfo(unsigned int iTInfo, LCID lcid,
  ITypeInfo FAR *FAR *ppTInfo)
{
  DBG_COM(fprintf(stderr, "wrap: %p GetTypeInfo %p\n", this, ppTInfo));
  if (ppTInfo == NULL) {
    return E_POINTER;
  }
  *ppTInfo = NULL;
  return DISP_E_BADINDEX;
}

STDMETHODIMP
ase_com_aseproxy::GetIDsOfNames(REFIID riid, OLECHAR FAR *FAR *rgszNames,
  unsigned int cNames, LCID lcid, DISPID FAR *rgDispId)
{
  DBG_COM(fprintf(stderr, "wrap: %p GetIDsOfNames\n", this));
  try {
    DBG(aioport::log() << "wrap: GetIDsOfNames count=" << cNames);
    if (cNames == 0) {
      return S_OK;
    }
    if (rgszNames == NULL || rgDispId == NULL) {
      return E_INVALIDARG;
    }
    for (unsigned int i = 0; i < cNames; ++i) {
      ase_string s = ase_com_util::to_ase_string_from_olestr(rgszNames[i]);
      DBG_COM(fprintf(stderr, "wrap: %p GetIDsOfNames %s\n", this,
	s.data()));
      DISPID id = 0;
      if (!value.get_dispid_synchronized(s, id)) {
        return DISP_E_UNKNOWNNAME;
      }
      rgDispId[i] = id;
    }
  } catch (...) {
    return ase_com_util::handle_ase_exception(value.get_script_engine());
  }
  return S_OK;
}

STDMETHODIMP
ase_com_aseproxy::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid,
  WORD wFlags, DISPPARAMS FAR *pDispParams, VARIANT FAR *pVarResult,
  EXCEPINFO FAR *pExcepInfo, unsigned int FAR *puArgErr)
{
  ASE_COM_VERBOSE_ASECALL(fprintf(stderr, "COM: %p invoke\n", this));
  try {
    if (dispIdMember < 0) {
      return DISP_E_MEMBERNOTFOUND;
    }
    if (puArgErr) {
      *puArgErr = 0; /* initialize error argpos */
    }
    ase_string name;
    int argc = 0;
    if (pDispParams) {
      argc = pDispParams->cArgs;
    }
    std::vector<ase_variant> args(argc);
    for (int i = 0; i < argc; ++i) {
      args[i] = ase_com_util::to_ase_variant(value.get_script_engine(),
        pDispParams->rgvarg[i]);
    }
    if (!value.get_methodname_synchronized(dispIdMember, name)) {
      return DISP_E_MEMBERNOTFOUND;
    }
    ASE_COM_VERBOSE_ASECALL(fprintf(stderr, "COM: %p invoke name=%s\n", this,
      name.data()));
    ase_variant aseval = value.get_value_synchronized();
    const ase_variant *argsp = argc ? (&args[0]) : 0;
    ase_variant r = aseval.invoke_by_name(name.data(), name.size(), argsp,
      static_cast<ase_size_type>(argc));
    ASE_COM_VERBOSE_ASECALL(fprintf(stderr, "COM: %p invoke r=%s\n", this,
      r.get_string().data()));
    if (pVarResult) {
      CComVariant rval = ase_com_util::to_com_value(
	value.get_script_engine(), r);
      rval.Detach(pVarResult);
    }
    return S_OK;
  } catch (...) {
    return ase_com_util::handle_ase_exception(value.get_script_engine());
  }
}

}; // namespace asecom

