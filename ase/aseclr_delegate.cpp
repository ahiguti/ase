
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include "aseclr_delegate.hpp"
#include "aseclr_util.hpp"

#pragma warning(push)
#pragma warning(disable : 4091)
#include <msclr/lock.h>
#pragma warning(pop)

#define DBG(x)
#define DBG_FOBJ(x)

namespace aseclr {

using namespace System::Reflection;
using namespace System::Reflection::Emit;

System::Delegate ^create_typed_delegate(System::Type ^deltype,
  untyped_function_object ^fobj)
{
  MethodInfo ^delmi = deltype->GetMethod("Invoke");
  System::Type ^rettype = delmi->ReturnType;
  array<ParameterInfo ^> ^pinfos = delmi->GetParameters();
  int nargs = pinfos->Length;
  if (nargs > 0xfffeU) {
    throw gcnew System::OverflowException(
      "untyped_delegate: too many arguments");
  }
  array<System::Type ^> ^argtypes = gcnew array<System::Type ^>(nargs + 1);
  argtypes[0] = untyped_function_object::typeid;
  for (int i = 0; i < nargs; ++i) {
    argtypes[i + 1] = pinfos[i]->ParameterType;
    DBG(System::Console::WriteLine("argtype " + i + " " + argtypes[i + 1]));
  }
  DynamicMethod ^dm = gcnew DynamicMethod("", rettype, argtypes,
    untyped_function_object::typeid->Module);
  ILGenerator ^il = dm->GetILGenerator();
  LocalBuilder ^loc_argarr = il->DeclareLocal(
    array<System::Object ^>::typeid);
  {
    /* loc_argarr = gcnew array<Object ^>(nargs) */
    il->Emit(OpCodes::Ldc_I4, nargs);
    il->Emit(OpCodes::Newarr, System::Object::typeid);
    il->Emit(OpCodes::Stloc, loc_argarr);
  }
  DBG(System::Console::WriteLine("rettype=" + rettype));
  for (int i = 0; i < nargs; ++i) {
    /* loc_argarr[i] := ldarg[i + 1] */
    System::Type ^argtyp = argtypes[i + 1];
    System::UInt16 argidx = static_cast<System::UInt16>(i + 1);
    il->Emit(OpCodes::Ldloc, loc_argarr);
    il->Emit(OpCodes::Ldc_I4, i);
    il->Emit(OpCodes::Ldarg, argidx);
    DBG(System::Console::WriteLine("argtype=" + argtyp));
    if (argtyp->IsByRef || argtyp->IsPointer) {
      argtyp = argtyp->GetElementType();
      il->Emit(OpCodes::Ldobj, argtyp);
      DBG(System::Console::WriteLine("byref elemtype=" + argtyp));
    }
    if (argtyp->IsValueType) {
      il->Emit(OpCodes::Box, argtyp);
      DBG(System::Console::WriteLine("box elemtype=" + argtyp));
    }
    il->Emit(OpCodes::Stelem_Ref);
  }
  {
    /* fobj->invoke(loc_argarr) */
    il->Emit(OpCodes::Ldarg_0); /* fobj */
    il->Emit(OpCodes::Ldloc, loc_argarr);
    MethodInfo ^utmi = untyped_function_object::typeid->GetMethod("invoke");
    il->EmitCall(OpCodes::Callvirt, utmi, nullptr);
  }
  if (rettype == void::typeid) {
    il->Emit(OpCodes::Pop);
  } else {
    if (rettype->IsByRef || rettype->IsPointer) {
      throw gcnew System::NotSupportedException(
        "untyped_delegate: unsupported return type");
    }
    if (rettype->IsValueType) {
      DBG(System::Console::WriteLine("unbox rettype=" + rettype));
      il->Emit(OpCodes::Unbox_Any, rettype);
    }
  }
  il->Emit(OpCodes::Ret);
  return dm->CreateDelegate(deltype, fobj);
}

ref struct ase_clr_dynamic_funcobj : public untyped_function_object {

  ase_clr_dynamic_funcobj(ase_clr_script_engine *sc, const ase_variant& val)
    : script_engine(sc),
      value(new aseproxy_node(val, *sc->get_asepr_endnode())) {
    DBG_FOBJ(fprintf(stderr, "CLRFOBJ new %p\n", value));
  }
  ~ase_clr_dynamic_funcobj() {
    dispose_synchronized();
  }
  !ase_clr_dynamic_funcobj() {
    dispose_synchronized();
  }
  ase_variant get_synchronized() {
    msclr::lock lck(this);
    if (value) {
      return value->value.get();
    }
    return ase_variant();
  }
  void dispose_synchronized() {
    ase_variant v;
    {
      msclr::lock lck(this);
      if (value) {
        value->value.swap(v);
        delete value;
      }
      value = nullptr;
    }
    /* v is released here */
  }
  virtual System::Object ^invoke(array<System::Object ^> ^args) {
    try {
      int nargs = args->Length;
      std::vector<ase_variant> aargs(nargs);
      for (int i = 0; i < nargs; ++i) {
        aargs[i] = to_ase_value(script_engine, args[i]);
      }
      ase_variant obj = get_synchronized();
      ase_variant rval = obj.invoke_self(
        nargs > 0 ? (&aargs[0]) : 0, static_cast<ase_size_type>(nargs));
      return to_clr_value(script_engine, rval);
    } catch (const std::exception& e) {
      /* TODO: create wrapped exceptions */
      std::string wh = e.what();
      System::String ^mess = to_clr_string(wh.data(), wh.size());
      throw gcnew System::Exception(mess);
    } catch (const ase_variant& e) {
      ase_string wh = e.get_string();
      System::String ^mess = to_clr_string(wh.data(), wh.size());
      throw gcnew System::Exception(mess);
    }
    return nullptr;
  }

 private:
 
  ase_clr_script_engine *script_engine;
  aseproxy_node *value;

};

System::Delegate ^
create_typed_delegate(ase_clr_script_engine *sc, System::Type ^deltype,
  const ase_variant& value)
{
  ase_clr_dynamic_funcobj ^dm = gcnew ase_clr_dynamic_funcobj(sc, value);
  return create_typed_delegate(deltype, dm);
}


}; // namespace aseclr

