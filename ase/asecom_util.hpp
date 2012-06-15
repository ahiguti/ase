
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GENTYPE_ASECOM_UTIL_HPP
#define GENTYPE_ASECOM_UTIL_HPP

#ifdef _DEBUG
#define _ATL_DEBUG_INTERFACES
#define _ATL_DEBUG_QI
#endif

#include <atlbase.h>
#include <comutil.h>
#include <atlcomcli.h>
#include <oleauto.h>

#include "asecom_sctx.hpp"


#define ASE_COM_VERBOSE_MAP(x) \
  if ((asecom::ase_com_util::verbose &   1) != 0) { x; };
#define ASE_COM_VERBOSE_COMCALL(x) \
  if ((asecom::ase_com_util::verbose &   2) != 0) { x; };
#define ASE_COM_VERBOSE_ASECALL(x) \
  if ((asecom::ase_com_util::verbose &   4) != 0) { x; };

namespace asecom {

struct ase_com_util {

  static unsigned int verbose;

  static void init_main_thread();
  static void uninit_main_thread();
  static void init_child_thread();
  static void uninit_child_thread();

  static ase_variant create_comproxy(ase_com_script_engine *sc,
    IDispatch *val);
  static CComVariant create_aseproxy(ase_com_script_engine *sc,
    const ase_variant& value);

  static ase_string to_ase_string_from_bstr(BSTR str);
  static ase_string to_ase_string_from_olestr(const OLECHAR *str);
  static CComBSTR to_com_string(ase_com_script_engine *sc,
    const char *str, ase_size_type slen);
  static CComBSTR to_com_string(ase_com_script_engine *sc,
    const ase_string& str);
  static ase_variant to_ase_variant(ase_com_script_engine *sc,
    const VARIANT& val);
  static CComVariant to_com_value(ase_com_script_engine *sc,
    const ase_variant& val);
  static ase_com_ase_variant_implnode *get_comproxy(
    ase_com_script_engine *sc, const ase_variant& val);
  static ase_com_aseproxy *get_aseproxy(ase_com_script_engine *sc,
    IDispatch *idisp);

  static HRESULT handle_ase_exception(ase_com_script_engine *sc);

  static void check_com_error(IUnknown *iunk, const char *mess);
  static void check_com_error(HRESULT hr, const char *mess);
  static IID find_iid(IUnknown *iunk, BSTR name, CComPtr<ITypeInfo>& tinfo_r);
  static IID get_source(IUnknown *iunk, BSTR name,
    CComPtr<IConnectionPoint>& cp_r, CComPtr<ITypeInfo>& tinfo_r);
  static IID get_default_source(IUnknown *iunk,
    CComPtr<IConnectionPoint>& cp_r, CComPtr<ITypeInfo>& tinfo_r);
  static void create_idmap(ITypeInfo *ti,
    std::map<DISPID, ase_string>& to_str_r,
    std::map<ase_string, DISPID>& to_id_r);
  static DWORD connection_advise(IConnectionPoint *cp, IUnknown *sink);
  static void connection_unadvise(IConnectionPoint *cp, DWORD cookie);

};

}; // namespace asecom

#endif

