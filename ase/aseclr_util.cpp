
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include "aseclr_util.hpp"
#include "aseclr_clrpr.hpp"
#include "aseclr_dynproxy.hpp"
#include "aseclr_delegate.hpp"
#include "aseclr_wrapper.hpp"

#include <ase/asebindmethod.hpp>
#include <ase/aseutf16.hpp>
#include <ase/asecast.hpp>
#include <ase/aseexcept.hpp>

#define DBG(x)
#define DBG_RC(x)
#define DBG_DEL(x)
#define DBG_INV(x)
#define DBG_DPR(x)

namespace aseclr {

ase_string
to_ase_string(System::String ^str)
{
  cli::pin_ptr<const wchar_t> cs = PtrToStringChars(str);
  ase_size_type ncs = str->Length;
  const wchar_t *wcs = cs;
  const ase_ucchar *ucs = reinterpret_cast<const ase_ucchar *>(wcs);
  return ase_string(ucs, ncs);
}

System::String ^
to_clr_string(const char *str, ase_size_type len)
{
  ase_utf16_string wstr(str, len);
  wchar_t *wcs = wstr.DataWr();
  int nwcs = ase_numeric_cast<int>(wstr.size());
  return gcnew System::String(wcs, 0, nwcs);
}

System::String ^
to_clr_string(const ase_string& str)
{
  return to_clr_string(str.data(), str.size());
}

ase_variant
to_ase_value(ase_clr_script_engine *sc, System::Object ^obj)
{
  if (obj == nullptr) {
    return ase_variant();
  }
  System::Type ^typ = obj->GetType();
  if (typ->IsPrimitive) {
    /* Boolean Byte SByte Int16 UInt16 Int32 UInt32 */
    /* Int64 UInt64 Char Single Double */
    if (typ == System::Boolean::typeid) {
      return ase_variant::create_boolean(cli::safe_cast<System::Boolean>(obj));
    }
    if (typ == System::Byte::typeid || typ == System::SByte::typeid ||
        typ == System::Int16::typeid || typ == System::UInt16::typeid ||
        typ == System::Int32::typeid || typ == System::Char::typeid) {
      return ase_variant::create_int(cli::safe_cast<System::Int32>(obj));
    }
    if (typ == System::UInt32::typeid || typ == System::Int64::typeid ||
        typ == System::UInt64::typeid) {
      return ase_variant::create_long(cli::safe_cast<System::Int64>(obj));
    }
    return ase_variant::create_double(cli::safe_cast<System::Double>(obj));
  } else {
    if (typ == System::Void::typeid) {
      return ase_variant();
    }
    if (typ == System::String::typeid) {
      return to_ase_string(cli::safe_cast<System::String ^>(obj)).to_variant();
    }
    return create_clrproxy(sc, obj);
  }
}

System::Object ^
to_clr_number(double val)
{
  /* try lossless conv to int */
  System::Int32 ival = static_cast<System::Int32>(val);
  double dval = static_cast<double>(ival);
  if (dval == val) {
    return ival;
  }
  System::Int64 lval = static_cast<System::Int64>(val);
  dval = static_cast<double>(lval);
  if (dval == val) {
    return lval;
  }
  return val;
}

System::Object ^
to_clr_value(ase_clr_script_engine *sc, const ase_variant& val)
{
  System::Object ^obj = extract_clrproxy(val);
  if (obj != nullptr) {
    ValueWrapper ^vw = dynamic_cast<ValueWrapper ^>(obj);
    if (vw != nullptr) {
      return vw->value;
    }
    return obj;
  }
  switch (val.get_type()) {
  case ase_vtype_void:
    return nullptr;
  case ase_vtype_bool:
    return val.get_boolean();
  case ase_vtype_int:
    return val.get_int();
  case ase_vtype_long:
    return val.get_long();
  case ase_vtype_double:
    return to_clr_number(val.get_double());
  case ase_vtype_string:
    return to_clr_string(val.get_string());
  default:
    break;
  }
  ase_throw_type_mismatch("ASECLR: not a CLR object");
  return nullptr;
}

array<System::Object ^> ^
to_clr_value_array(ase_clr_script_engine *sc, const ase_variant *arr,
  ase_size_type alen)
{
  int len = ase_numeric_cast<int>(alen);
  array<System::Object ^> ^a = gcnew array<System::Object ^>(len);
  for (int i = 0; i < len; ++i) {
    a[i] = to_clr_value(sc, arr[i]);
  }
  return a;
}

void
throw_ase_exception(ase_clr_script_engine *sc, System::Exception ^e)
{
  /* TODO: if e is outofmemory? */
  throw to_ase_value(sc, e);
}

ase_variant
get_executing_assembly(ase_clr_script_engine *sc)
{
  System::Object ^a = System::Reflection::Assembly::GetExecutingAssembly();
  return to_ase_value(sc, a);
}

ase_variant
get_mscorlib_assembly(ase_clr_script_engine *sc)
{
  System::Object ^a =
    System::Reflection::Assembly::GetAssembly(System::Object::typeid);
  return to_ase_value(sc, a);
}

ase_variant
create_delegate(ase_clr_script_engine *sc, const ase_variant& dobj,
  const ase_variant& value)
{
  System::Object ^obj = extract_clrproxy(dobj);
  if (obj == nullptr) {
    ase_throw_type_mismatch("ASECLR: not a CLR object");
  }
  System::Type ^detyp = dynamic_cast<System::Type ^>(obj);
  if (detyp == nullptr) {
    ase_throw_type_mismatch("ASECLR: not a type");
  }
  System::Delegate ^handler = create_typed_delegate(sc, detyp, value);
  return to_ase_value(sc, handler);
}

ase_variant
add_event_handler(ase_clr_script_engine *sc, const ase_variant& v,
  const ase_string& name, const ase_variant& handler)
{
  System::Object ^obj = extract_clrproxy(v);
  if (obj == nullptr) {
    ase_throw_type_mismatch("ASECLR: not a CLR object");
  }
  System::Type ^typ = obj->GetType();
  System::String ^ename = to_clr_string(name);
  System::Reflection::EventInfo ^ei = typ->GetEvent(ename);
  if (ei == nullptr) {
    ase_string s = "ASECLR: event not found: " + name;
    ase_throw_invalid_arg(s.data());
  }
  System::Delegate ^hobj = create_typed_delegate(sc, ei->EventHandlerType,
    handler);
  ei->AddEventHandler(obj, hobj);
  return to_ase_value(sc, hobj);
}

void
remove_event_handler(ase_clr_script_engine *sc, const ase_variant& v,
  const ase_string& name, const ase_variant& handler)
{
  System::Object ^obj = extract_clrproxy(v);
  if (obj == nullptr) {
    ase_throw_type_mismatch("ASECLR: not a CLR object");
  }
  System::Type ^typ = obj->GetType();
  System::String ^ename = to_clr_string(name);
  System::Reflection::EventInfo ^ei = typ->GetEvent(ename);
  if (ei == nullptr) {
    ase_string s = "ASECLR: event not found: " + name;
    ase_throw_invalid_arg(s.data());
  }
  System::Object ^hobj = extract_clrproxy(v);
  System::Delegate ^hdel = dynamic_cast<System::Delegate ^>(hobj);
  if (hdel == nullptr) {
    ase_throw_type_mismatch("ASECLR: not a dynamic delegate object");
  }
  ei->RemoveEventHandler(obj, hdel);
}

}; // namespace aseclr

