
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GENTYPE_ASECOM_ASEPR_HPP
#define GENTYPE_ASECOM_ASEPR_HPP

#include <ase/asemutex.hpp>

#include "asecom_sctx.hpp"
#include "asecom_comvtbl.h"

namespace asecom {

struct ase_com_aseproxy_priv : public ase_noncopyable {

 public:

  friend struct ase_com_script_engine;

  struct init_arg {
    init_arg(ase_com_script_engine *sc, const ase_variant& v)
      : script_engine(sc), valueref(v) { }
    ase_com_script_engine *const script_engine;
    const ase_variant& valueref;
  };

  ase_com_aseproxy_priv(const init_arg& ini);
  ~ase_com_aseproxy_priv(); /* DCHK */

  void dispose_synchronized(const ase_com_script_engine *sc);

  ase_com_script_engine *get_script_engine() const {
    return script_engine;
  }
  ase_variant get_value_synchronized() const {
    ase_mutex_guard<ase_fast_mutex> g(mutex);
    return value;
  }

  struct event_sink_type {
    IID iid;
    CComPtr<ITypeInfo> tinfo;
    std::map<DISPID, ase_string> to_str;
    std::map<ase_string, DISPID> to_id;
    event_sink_type(IID i, const CComPtr<ITypeInfo>& ti)
      : iid(i), tinfo(ti) { }
  };
  
  void set_event_sink_synchronized(IID iid, const CComPtr<ITypeInfo>& ti);
  
  IID get_event_sink_iid_synchronized() const {
    ase_mutex_guard<ase_fast_mutex> g(mutex);
    return event_sink->iid;
  }
  
  bool get_dispid_synchronized(const ase_string& name, DISPID& id_r);
  bool get_methodname_synchronized(DISPID id, ase_string& name_r);
  
 private:

  ase_com_script_engine *const script_engine;
  mutable ase_fast_mutex mutex;
    /* note that we must not call metamethods on 'value' with 'mutex' locked,
      or it causes a deadlock with callee's locks (ase_unified_engine::mutex
      for example). */
  ase_variant value;
  typedef std::vector<ase_string> ids_type;
  ids_type ids;
  bool used_as_unconnected : 1;
  std::auto_ptr<event_sink_type> event_sink;

};

/*
 * ase_com_aseproxy_priv      := ase_variant + context pointer
 * ase_com_aseproxy_privnode  := ase_com_aseproxy_priv + listnode info
 * ase_com_aseproxy           := ase_com_aseproxy_privnode + com_refcount
 *
 */

struct ase_com_aseproxy :
 public ase_com_aseproxy_privnode, public IDispatch {
  
  typedef ase_com_aseproxy_priv::init_arg init_arg;
  
  ase_com_aseproxy(const init_arg& ini);
  ase_com_aseproxy(const init_arg& ini, ase_com_aseproxy_privnode& node);
  ~ase_com_aseproxy(); /* DCHK */

  /* IUnknown */
  STDMETHODIMP QueryInterface(REFIID iid, void **ppvObject);
  ULONG STDMETHODCALLTYPE AddRef(void);
  ULONG STDMETHODCALLTYPE Release(void);

  /* IDispatch */
  STDMETHODIMP GetTypeInfoCount(unsigned int FAR *pctinfo);
  STDMETHODIMP GetTypeInfo(unsigned int iTInfo, LCID lcid,
    ITypeInfo FAR *FAR *ppTInfo);
  STDMETHODIMP GetIDsOfNames(REFIID riid, OLECHAR FAR *FAR *rgszNames,
    unsigned int cNames, LCID lcid, DISPID FAR *rgDispId);
  STDMETHODIMP Invoke(DISPID dispIdMember, REFIID riid, LCID lcid,
    WORD wFlags, DISPPARAMS FAR *pDispParams, VARIANT FAR *pVarResult,
    EXCEPINFO FAR *pExcepInfo, unsigned int FAR *puArgErr);

  ase_atomic_count com_refcount;

};

}; // namespace asecom

#endif

